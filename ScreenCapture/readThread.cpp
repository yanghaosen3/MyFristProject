#include "readThread.h"
#include "VideoDecode.h"
#include "VideoCodec.h"
#include <QEventLoop>
#include <QTimer>

extern "C"
{
#include "libavcodec/avcodec.h"
}

static void sleepMesc(int msec)
{
	if (msec <= 0) return;
	QEventLoop loop;		//定义一个新的事件循环
	QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
	loop.exec();
}

ReadThread::ReadThread()
{
	m_spVideoDecode = std::make_unique<VideoDecode>();
	m_spVideoCodec = std::make_unique<VideoCodec>();
	qRegisterMetaType<PlayState>("PlayState");
}

ReadThread::~ReadThread()
{
}

void ReadThread::setReadFilePath(const QString& str)
{
	m_savePath = str;
}

void ReadThread::open(const QString& str)
{
	if (this->isRunning())
		return;

	emit this->start();
}

void ReadThread::close()
{
	m_bPlay = false;
}

void ReadThread::run()
{
	if (m_savePath.isEmpty())
		return;

	bool ret = m_spVideoDecode->init();
	if (!ret)
		return;

	m_bPlay = m_spVideoCodec->open(m_spVideoDecode->getCodecContext(), m_savePath, m_spVideoDecode->getAvFrameRate());
	if (m_bPlay)
		emit updatePlayState(play);
	
	while (m_bPlay)
	{
		AVFrame* pFrame = m_spVideoDecode->read();
		if (pFrame)
		{
			m_spVideoCodec->write(pFrame);
		}
		else
		{
			if (m_spVideoDecode->isEnd())
				break;
			sleepMesc(1);
		}
	}

	m_spVideoDecode->close();
	m_spVideoCodec->close();
	emit updatePlayState(end);
}
