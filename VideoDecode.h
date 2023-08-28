/******************************************************************************
 * @文件名     readThread.h
 * @功能       视频解码类，在这个类中调用ffmpeg打开视频进行解码
 * @开发者     yhs
 * @邮箱       897111137@qq.com
 * @时间       2023/8/23
 *****************************************************************************/
#ifndef _SCREENCAP_VIDEODECODEC_H
#define _SCREENCAP_VIDEODECODEC_H
#include "memory"
#include <QPoint>
#include <QSize>
class AVFormatContext;
class AVCodecContext;
class AVInputFormat;
class AVFrame;
class AVPacket;
class VideoDecode
{
public:
	VideoDecode();
	~VideoDecode();
	bool init();
	AVFrame* read();
	bool isEnd();
	void close();
	void free();
	AVCodecContext* getCodecContext();
	QPoint getAvFrameRate() const;
	QSize getSize() const;


private:
	void initFFmpeg();
	void clear();
private:
	QPoint m_avFrameTate;
	QSize m_size;
	const AVInputFormat* m_pAvInputFormat = nullptr;			//输入设备
	AVFormatContext* m_pAvFormatContext = nullptr;				//解封装上下文
	AVCodecContext* m_pAvCodecContext = nullptr;				//解码器上下文
	AVPacket* m_pPacket = nullptr;								//数据包
	AVFrame* m_pFrame = nullptr;								//解码后的视频帧
	int m_videIndex = -1;										//视频流索引
	bool m_bEnd = false;
};
#endif // !_SCREENCAP_VIDEODECODEC_H