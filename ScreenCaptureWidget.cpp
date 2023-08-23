#include "ScreenCaptureWidget.h"
#include "QHBoxLayout"
#include "QPushButton"
#include "readThread.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>
ScreenCaptureWidget::ScreenCaptureWidget(QWidget *parent)
    : QWidget(parent)
{
    m_spReadThread = std::make_unique<ReadThread>();
    this->setWindowTitle(QString::fromLocal8Bit("qt¼����"));
    QHBoxLayout* pHLayout = new QHBoxLayout(this);
    QPushButton* pStartButton = new QPushButton(QString::fromLocal8Bit("��ʼ¼��"));
    QPushButton* pEndButton = new QPushButton(QString::fromLocal8Bit("����¼��"));
    pHLayout->addWidget(pStartButton);
    pHLayout->addWidget(pEndButton);
    QObject::connect(pStartButton, &QPushButton::pressed, this, &ScreenCaptureWidget::onStartRecord);
	QObject::connect(pEndButton, &QPushButton::pressed, this, &ScreenCaptureWidget::onEndRecord);
}

ScreenCaptureWidget::~ScreenCaptureWidget()
{
}

void ScreenCaptureWidget::onStartRecord()
{
    if (m_readState == Start)
        return;

    QString saveUrl = setSavePath();
    if (saveUrl.isEmpty())
        return;

    m_spReadThread->setReadFilePath(saveUrl);
    m_spReadThread->open(saveUrl);
    m_readState = Start;
}

void ScreenCaptureWidget::onEndRecord()
{
    m_readState = end;
}

QString ScreenCaptureWidget::setSavePath()
{
	QString strDefault = QString("%1/%2.mp4").arg(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss"));

	QString strPath = QFileDialog::getSaveFileName(this, "��Ƶ���浽~", strDefault,
		"������Ƶ�ļ� (*.mp4 *.avi *.mov *.wmv *.flv *.h264 *.h265);;"
		"�����ļ���ʽ (*)");

    return strPath;
}