/******************************************************************************
 * @�ļ���     ScreenCaptureWidget.h
 * @����       ¼�������࣬����¼�ƿ���
 * @������     yhs
 * @����       897111137@qq.com
 * @ʱ��       2023/8/13
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