#include "readThread.h"

ReadThread::ReadThread()
{
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

void ReadThread::run()
{
	if (m_savePath.isEmpty())
		return;


}
