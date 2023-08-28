#include "VideoDecode.h"
#include "erroroutput.h"

extern "C" {        // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavdevice/avdevice.h" 
}
							
VideoDecode::VideoDecode()
{
	initFFmpeg();
}

VideoDecode::~VideoDecode()
{
	free();
}

void VideoDecode::close()
{
	clear();
	free();
	m_videIndex = 0;
	m_avFrameTate = QPoint(0, 0);
	m_size = QSize(0, 0);
}

void VideoDecode::clear()
{
	if (m_pAvFormatContext && m_pAvFormatContext->pb)
		avio_flush(m_pAvFormatContext->pb);

	if (m_pAvFormatContext)
		avformat_flush(m_pAvFormatContext);
}

bool VideoDecode::init()
{
	AVDictionary* dict = nullptr;
	int ret = av_dict_set(&dict, "framerate", "20", 0);
	ret = av_dict_set(&dict, "draw_mouse", "1", 0);
	ret = avformat_open_input(&m_pAvFormatContext, "desktop", m_pAvInputFormat, &dict);
	if (dict)
		av_dict_free(&dict);

	check_ret(ret, this->free());

	ret = avformat_find_stream_info(m_pAvFormatContext, nullptr);
	check_ret(ret, this->free());

	m_videIndex = av_find_best_stream(m_pAvFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	check_ret(m_videIndex, this->free());

	AVStream* pVideStream = m_pAvFormatContext->streams[m_videIndex];
	check_p(pVideStream, this->free());

	m_avFrameTate.setX(pVideStream->avg_frame_rate.num);
	m_avFrameTate.setY(pVideStream->avg_frame_rate.den);
	m_size.setWidth(pVideStream->codecpar->width);
	m_size.setHeight(pVideStream->codecpar->height);

	const AVCodec* codec = avcodec_find_decoder(pVideStream->codecpar->codec_id);
	check_p(codec, this->free());

	m_pAvCodecContext = avcodec_alloc_context3(codec);
	check_p(m_pAvCodecContext, this->free());

	ret = avcodec_parameters_to_context(m_pAvCodecContext, pVideStream->codecpar);
	check_ret(ret, this->free());

	m_pAvCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;
	m_pAvCodecContext->thread_count = 8;

	ret = avcodec_open2(m_pAvCodecContext, nullptr, nullptr);
	check_ret(ret, this->free());

	m_pFrame = av_frame_alloc();
	check_p(m_pFrame, this->free());

	m_pPacket = av_packet_alloc();
	check_p(m_pPacket, this->free());
	m_bEnd = false;
	return true;
}

AVFrame* VideoDecode::read()
{
	if (!m_pAvFormatContext)
		return nullptr;

	int readRet = av_read_frame(m_pAvFormatContext, m_pPacket);
	if (readRet < 0)
	{
		avcodec_send_packet(m_pAvCodecContext, m_pPacket);
	}
	else
	{
		if (m_pPacket->stream_index == m_videIndex)
			avcodec_send_packet(m_pAvCodecContext, m_pPacket);
	}

	av_packet_unref(m_pPacket);
	av_frame_unref(m_pFrame);
	int ret = avcodec_receive_frame(m_pAvCodecContext, m_pFrame);
	if (ret < 0)
	{
		av_frame_unref(m_pFrame);
		if (readRet < 0)
		{
			m_bEnd = true;
		}
		return nullptr;
	}

	return m_pFrame;
}

bool VideoDecode::isEnd()
{
	return m_bEnd;
}

void VideoDecode::free()
{
	if (m_pAvCodecContext)
	{
		avcodec_free_context(&m_pAvCodecContext);
		m_pAvCodecContext = nullptr;
	}

	if (m_pAvFormatContext)
	{
		avformat_close_input(&m_pAvFormatContext);
		m_pAvFormatContext = nullptr;
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

}

void VideoDecode::initFFmpeg()
{
	avformat_network_init();

	avdevice_register_all();

	m_pAvInputFormat = av_find_input_format("gdigrab");
}

AVCodecContext* VideoDecode::getCodecContext()
{
	return m_pAvCodecContext;
}

QPoint VideoDecode::getAvFrameRate() const
{
	return m_avFrameTate;
}

QSize VideoDecode::getSize() const
{
	return m_size;
}