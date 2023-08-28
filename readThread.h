/******************************************************************************
 * @文件名     readThread.h
 * @功能       读取视频图像的线程，在线程中解码视频
 * @开发者     yhs
 * @邮箱       897111137@qq.com
 * @时间       2023/8/23
 *****************************************************************************/
#ifndef _SCREENCAP_TEADTHREAD_H
#define _SCREENCAP_TEADTHREAD_H

#include <QThread>
class VideoDecode;
class VideoCodec;
class ReadThread : public QThread
{
	Q_OBJECT
public:
	enum PlayState
	{
		play,
		end
	};
	ReadThread();
	~ReadThread();
	void setReadFilePath(const QString& str);
	void open(const QString& str);
	void close();
signals:
	void updatePlayState(PlayState state);

private:
	void run() override;

private:
	std::unique_ptr<VideoDecode> m_spVideoDecode;		
	std::unique_ptr<VideoCodec> m_spVideoCodec;
	QString m_savePath;
	bool m_bPlay = false;
};
#endif // !_SCREENCAP_TEADTHREAD_H