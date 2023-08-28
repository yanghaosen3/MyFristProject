#include "ScreenCaptureWidget.h"
#include "QHBoxLayout"
#include "QPushButton"
#include "readThread.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QApplication>
#include <QFileDialog>
ScreenCaptureWidget::ScreenCaptureWidget(QWidget *parent)
    : QWidget(parent)
{
    m_spReadThread = std::make_unique<ReadThread>();
    this->setWindowTitle(QString::fromLocal8Bit("qt录屏器"));
    QHBoxLayout* pHLayout = new QHBoxLayout(this);
    m_pPushButton = new QPushButton(QString::fromLocal8Bit("开始录制"));
    m_pTimeEdit = new QTimeEdit();
    m_pTimeEdit->setAlignment(Qt::AlignCenter);
    m_pTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_pTimeEdit->setDisplayFormat(QApplication::translate("Widget", "HH:mm:ss", nullptr));
	pHLayout->addWidget(m_pTimeEdit);
    pHLayout->addWidget(m_pPushButton);
    bool ret = QObject::connect(m_pPushButton, &QPushButton::pressed, this, &ScreenCaptureWidget::onStartRecord);
    ret = QObject::connect(m_spReadThread.get(), &ReadThread::updatePlayState, this, &ScreenCaptureWidget::onUpdatePalyState);
    ret = QObject::connect(&m_timer, &QTimer::timeout, this, &ScreenCaptureWidget::onTimerOut);
    this->setFixedSize(QSize(300, 60));
}

ScreenCaptureWidget::~ScreenCaptureWidget()
{
}

void ScreenCaptureWidget::onStartRecord()
{
    if (m_pPushButton->text() == QString::fromLocal8Bit("开始录制"))
    {
		QString saveUrl = setSavePath();
		if (saveUrl.isEmpty())
			return;

		m_spReadThread->setReadFilePath(saveUrl);
		m_spReadThread->open(saveUrl);
    }
    else
    {
        m_spReadThread->close();
    }
}

QString ScreenCaptureWidget::setSavePath()
{
	QString strDefault = QString("%1/%2.mp4").arg(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss"));

	QString strPath = QFileDialog::getSaveFileName(this, "视频保存到~", strDefault,
		"常用视频文件 (*.mp4 *.avi *.mov *.wmv *.flv *.h264 *.h265);;"
		"其它文件格式 (*)");

    return strPath;
}

void ScreenCaptureWidget::onUpdatePalyState(ReadThread::PlayState state)
{
    if (state == ReadThread::play)
    {
        this->setWindowTitle(QString::fromLocal8Bit("正在录制"));
        m_pPushButton->setText(QString::fromLocal8Bit("停止录制"));
        m_timer.start(1000);
        m_pTimeEdit->setTime(QTime(0, 0, 0, 0));
    }
    else
    {
		m_pPushButton->setText(QString::fromLocal8Bit("开始录制"));
        m_timer.stop();
    }
}

void ScreenCaptureWidget::onTimerOut()
{
    m_pTimeEdit->setTime(m_pTimeEdit->time().addSecs(1));
}