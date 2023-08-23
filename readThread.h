/******************************************************************************
 * @�ļ���     readThread.h
 * @����       ��ȡ��Ƶͼ����̣߳����߳��н�����Ƶ
 * @������     yhs
 * @����       897111137@qq.com
 * @ʱ��       2023/8/23
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