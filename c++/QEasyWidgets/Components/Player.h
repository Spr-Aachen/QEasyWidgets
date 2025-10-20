#ifndef QEASYWIDGETS_PLAYER_H
#define QEASYWIDGETS_PLAYER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

#include "Button.h"
#include "Slider.h"


namespace QEW {

/**
 * @brief Media player widget with play/pause controls and progress slider
 */
class MediaPlayerBase : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPlayerBase(QWidget *parent = nullptr);
    ~MediaPlayerBase() override = default;

    QMediaPlayer *mediaPlayer() const;

    void setSource(const QUrl &url);
    void play();
    void pause();
    void stop();

private slots:
    void onPlayClicked();
    void onPauseClicked();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onSliderValueChanged(int value);

private:
    void init();
    void setupUI();
    void setupConnections();

    QStackedWidget *m_stackedWidget;
    ButtonBase *m_playButton;
    ButtonBase *m_pauseButton;
    SliderBase *m_slider;

    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;

    QHBoxLayout *m_layout;
};

} // namespace QEW

#endif // QEASYWIDGETS_PLAYER_H
