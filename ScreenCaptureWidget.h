/******************************************************************************
 * @文件名     ScreenCaptureWidget.h
 * @功能       录屏界面类，用于录制控制
 * @开发者     yhs
 * @邮箱       897111137@qq.com
 * @时间       2023/8/13
 *****************************************************************************/
#ifndef _SCREENCAP_SCREENCAPTUREWIDGET_H
#define _SCREENCAP_SCREENCAPTUREWIDGET_H
#include <QWidget>
#include "memory"
class ReadThread;
class ScreenCaptureWidget : public QWidget
{
    Q_OBJECT
public:
    ScreenCaptureWidget(QWidget *parent = nullptr);
    ~ScreenCaptureWidget(); 

    enum RecoderState
    {
        ready = 0,
        Start = 1,
        end = 2
    };

private slots:
    void onStartRecord();
    void onEndRecord();

private:
    QString setSavePath();

private:
    std::unique_ptr<ReadThread> m_spReadThread;
    RecoderState m_readState = ready;
};
#endif // !_SCREENCAP_SCREENCAPTUREWIDGET_H