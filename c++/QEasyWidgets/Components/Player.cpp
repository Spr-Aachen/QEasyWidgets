#include "Player.h"

#include <QUrl>

#include "../Common/Icon.h"

/**
 * MediaPlayerBase implementation
 */

MediaPlayerBase::MediaPlayerBase(QWidget *parent)
    : QWidget(parent)
    , m_stackedWidget(nullptr)
    , m_playButton(nullptr)
    , m_pauseButton(nullptr)
    , m_slider(nullptr)
    , m_mediaPlayer(nullptr)
    , m_audioOutput(nullptr)
    , m_layout(nullptr) {
    init();
}

QMediaPlayer *MediaPlayerBase::mediaPlayer() const {
    return m_mediaPlayer;
}

void MediaPlayerBase::setSource(const QUrl &url) {
    if (m_mediaPlayer) {
        m_mediaPlayer->setSource(url);
    }
}

void MediaPlayerBase::play() {
    if (m_mediaPlayer) {
        m_mediaPlayer->play();
        m_stackedWidget->setCurrentWidget(m_pauseButton);
    }
}

void MediaPlayerBase::pause() {
    if (m_mediaPlayer) {
        m_mediaPlayer->pause();
        m_stackedWidget->setCurrentWidget(m_playButton);
    }
}

void MediaPlayerBase::stop() {
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        m_stackedWidget->setCurrentWidget(m_playButton);
    }
}

void MediaPlayerBase::init() {
    setupUI();
    setupConnections();

    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
}

void MediaPlayerBase::setupUI() {
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(12);
    m_layout->setContentsMargins(21, 12, 21, 12);

    // Play/Pause button stack
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setMaximumSize(36, 36);
    m_stackedWidget->setContentsMargins(0, 0, 0, 0);

    m_playButton = new ButtonBase(this);
    m_playButton->setIcon(IconBase::Play);
    m_playButton->setBorderless(true);
    m_playButton->setTransparent(true);

    m_pauseButton = new ButtonBase(this);
    m_pauseButton->setIcon(IconBase::Pause);
    m_pauseButton->setBorderless(true);
    m_pauseButton->setTransparent(true);

    m_stackedWidget->addWidget(m_playButton);
    m_stackedWidget->addWidget(m_pauseButton);
    m_stackedWidget->setCurrentWidget(m_playButton);

    // Progress slider
    m_slider = new SliderBase(Qt::Horizontal, this);

    m_layout->addWidget(m_stackedWidget, 1);
    m_layout->addWidget(m_slider, 5);
}

void MediaPlayerBase::setupConnections() {
    connect(m_playButton, &QPushButton::clicked, this, &MediaPlayerBase::onPlayClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &MediaPlayerBase::onPauseClicked);

    connect(m_slider, QOverload<int>::of(&SliderBase::valueChanged),
            this, &MediaPlayerBase::onSliderValueChanged);
}

void MediaPlayerBase::onPlayClicked() {
    play();
}

void MediaPlayerBase::onPauseClicked() {
    pause();
}

void MediaPlayerBase::onPositionChanged(qint64 position) {
    if (m_mediaPlayer && m_mediaPlayer->duration() > 0) {
        int value = static_cast<int>((position * 100) / m_mediaPlayer->duration());
        m_slider->blockSignals(true);
        m_slider->setValue(value);
        m_slider->blockSignals(false);
    }
}

void MediaPlayerBase::onDurationChanged(qint64 duration) {
    Q_UNUSED(duration);
    // Duration changed, can update slider range if needed
}

void MediaPlayerBase::onSliderValueChanged(int value) {
    if (m_mediaPlayer && m_mediaPlayer->duration() > 0) {
        qint64 position = (value * m_mediaPlayer->duration()) / 100;
        m_mediaPlayer->setPosition(position);
    }
}