#include "VideoCodec.h"
#include "erroroutput.h"
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

VideoCodec::VideoCodec()
{
}

VideoCodec::~VideoCodec()
{
	free();
}

bool VideoCodec::open(AVCodecContext* pAvCodecContext, const QString& path, const QPoint& size)
{
	if (!pAvCodecContext || path.isEmpty())
		return false;

	int ret = avformat_alloc_output_context2(&m_pFormatContext, nullptr, nullptr, path.toStdString().data());
	check_ret(ret, this->free());

	ret = avio_open(&m_pFormatContext->pb, path.toStdString().data(), AVIO_FLAG_WRITE);
	check_ret(ret, this->free());

	const AVCodec* pAvCodec = avcodec_find_encoder(m_pFormatContext->oformat->video_codec);
	check_p(pAvCodec, this->free());

	m_pCodecContext = avcodec_alloc_context3(pAvCodec);
	check_p(m_pCodecContext, this->free());

	m_pCodecContext->width = pAvCodecContext->width;
	m_pCodecContext->height = pAvCodecContext->height;
	m_pCodecContext->pix_fmt = pAvCodec->pix_fmts[0];
	m_pCodecContext->time_base = { size.y(), size.x() };
	m_pCodecContext->framerate = { size.x(), size.y() };
	m_pCodecContext->bit_rate = 1000000;
	m_pCodecContext->gop_size = 12;
	m_pCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ret = avcodec_open2(m_pCodecContext, nullptr, nullptr);
	check_ret(ret, this->free());

	m_pVideoStream = avformat_new_stream(m_pFormatContext, nullptr);
	check_p(m_pVideoStream, this->free());

	ret = avcodec_parameters_from_context(m_pVideoStream->codecpar, m_pCodecContext);
	check_ret(ret, this->free());

	ret = avformat_write_header(m_pFormatContext, nullptr);
	check_ret(ret, this->free());
	m_bWriteToHeader = true;

	m_pPacket = av_packet_alloc();
	check_p(m_pPacket, this->free());

	m_pFrame = av_frame_alloc();
	check_p(m_pFrame, this->free());

	m_pFrame->format = pAvCodec->pix_fmts[0];
	return true;
}

void VideoCodec::write(AVFrame* pFrame)
{
	QMutexLocker locker(&m_mutex);
	if (!m_pPacket || !pFrame)
		return;

	if (!swsFormat(pFrame))
		return;

	if (m_pFrame)
	{
		m_pFrame->pts = m_frameIdex;
		m_frameIdex++;
	}

	avcodec_send_frame(m_pCodecContext, m_pFrame);
	while (true)
	{
		int ret = avcodec_receive_packet(m_pCodecContext, m_pPacket);
		if (ret < 0)
			break;

		av_packet_rescale_ts(m_pPacket, m_pCodecContext->time_base, m_pVideoStream->time_base);
		av_write_frame(m_pFormatContext, m_pPacket);
		av_packet_unref(m_pPacket);
	}
}

bool VideoCodec::swsFormat(AVFrame* pFrame)
{
	if (!pFrame || pFrame->width < 0 || pFrame->height < 0)
		return false;

	if (!m_pSwsContext)
	{
		m_pSwsContext = sws_getCachedContext(m_pSwsContext, pFrame->width, pFrame->height,
			static_cast<AVPixelFormat>(pFrame->format), pFrame->width, pFrame->height,
			static_cast<AVPixelFormat>(m_pFrame->format), SWS_BILINEAR, nullptr, nullptr, nullptr);

		if (!m_pSwsContext)
		{
			av_frame_unref(pFrame);
			return false;
		}

		if (m_pFrame)
		{
			m_pFrame->width = pFrame->width;
			m_pFrame->height = pFrame->height;
			av_frame_get_buffer(m_pFrame, 3 * 8);
		}
	}

	if (m_pFrame->width <= 0 || m_pFrame->height <= 0)
		return false;

	bool ret = sws_scale(m_pSwsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, m_pFrame->data, m_pFrame->linesize);
	av_frame_unref(pFrame);
	return ret;
}

void VideoCodec::free()
{
	if (m_pFormatContext)
	{
		avformat_free_context(m_pFormatContext);
		m_pFormatContext = nullptr;
		m_pVideoStream = nullptr;
	}	

	if (m_pCodecContext)
	{
		avcodec_free_context(&m_pCodecContext);
		m_pCodecContext = nullptr;
	}

	if (m_pPacket)
	{
		av_packet_free(&m_pPacket);
		m_pPacket = nullptr;
	}

	if (m_pFrame)
	{
		av_frame_free(&m_pFrame);
		m_pFrame = nullptr;
	}

	if (m_pSwsContext)
	{
		sws_freeContext(m_pSwsContext);
		m_pSwsContext = nullptr;
	}

	m_frameIdex = 0;
}

void VideoCodec::close()
{
	write(nullptr);
	do
	{
		if (m_pFormatContext)
		{
			if (m_bWriteToHeader)
			{
				m_bWriteToHeader = false;
				int ret = av_write_trailer(m_pFormatContext);
				if (ret < 0)
					break;
			}

			avio_close(m_pFormatContext->pb);
		}
	} while (false);

	free();
}