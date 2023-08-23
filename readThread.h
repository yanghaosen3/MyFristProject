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
class ReadThread : public QThread
{
	Q_OBJECT
public:
	ReadThread();
	~ReadThread();
	void setReadFilePath(const QString& str);
	void open(const QString& str);

private:
	void run() override;

private:
	QString m_savePath;
};
#endif // !_SCREENCAP_TEADTHREAD_H