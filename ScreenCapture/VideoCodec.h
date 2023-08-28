/******************************************************************************
 * @文件名     readThread.h
 * @功能       视频的编码保存类，将AVFrame图像进行格式转换，然后编码保存到视频文件中
 * @开发者     yhs
 * @邮箱       897111137@qq.com
 * @时间       2023/8/23
 *****************************************************************************/
#ifndef _SCREENCAP_VIDEOCODEC_H
#define _SCREENCAP_VIDEOCODEC_H
#include <QString>
#include <QPoint>
#include <QMutex>
class AVCodecContext;
class AVFormatContext;
class SwsContext;
class AVStream;
class AVPacket;
class AVFrame;
class VideoCodec
{
public:
	VideoCodec();
	~VideoCodec();
	bool open(AVCodecContext* pAvCodecContext, const QString& path, const QPoint& size);
	void free();
	void close();
	void write(AVFrame* pFrame);

private:
	bool swsFormat(AVFrame* pFrame);
private:
	AVFormatContext* m_pFormatContext = nullptr;
	AVCodecContext* m_pCodecContext = nullptr;
	SwsContext* m_pSwsContext = nullptr;
	AVStream* m_pVideoStream = nullptr;
	AVPacket* m_pPacket = nullptr;
	AVFrame* m_pFrame = nullptr;
	QMutex	m_mutex;
	int m_frameIdex = 0;
	bool m_bWriteToHeader = false;
};
#endif // !_SCREENCAP_VIDEOCODEC_H