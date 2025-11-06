#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QAudioOutput>
#include <QTime>
#include <QTimer>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), darkMode(true)
{
    setWindowTitle("🎬 EpiKodi Deluxe");
    resize(1300, 850);

    applyDarkTheme();

    // === Player ===
    player = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);

    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);

    // === Playlist ===
    playlistWidget = new QListWidget(this);
    playlistWidget->setStyleSheet(
        "QListWidget { background-color: #1E1E1E; color: #FFF; border: none; font-size: 15px; }"
        "QListWidget::item:selected { background-color: #FFA500; color: black; }"
    );

    playlistManager = new PlaylistManager(playlistWidget, this);
    connect(playlistManager, &PlaylistManager::mediaChanged,
            this, &MainWindow::playFromPlaylist);

    // === Layout principal ===
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(playlistWidget);
    splitter->addWidget(videoWidget);

    // === Boîte d’informations média ===
    infoBox = new QTextEdit(this);
    infoBox->setReadOnly(true);
    infoBox->setMinimumWidth(300);
    infoBox->setStyleSheet(
        "QTextEdit { background-color: #1A1A1A; color: #00FFAA; "
        "border: 1px solid #333; font-family: Consolas; font-size: 13px; }"
    );
    splitter->addWidget(infoBox);
    splitter->setStretchFactor(1, 5);
    splitter->setStretchFactor(2, 2);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(splitter);

    // === Control bar moderne ===
    QWidget *controls = new QWidget(this);
    controls->setFixedHeight(this->height() * 0.11);
    controls->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2B2B2B, stop:1 #1E1E1E);"
        "border-top: 1px solid #444;"
    );

    QHBoxLayout *controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setSpacing(15);
    controlsLayout->setContentsMargins(25, 5, 25, 5);

    auto makeButton = [&](QString emoji) {
        QPushButton *btn = new QPushButton(emoji, this);
        btn->setFixedSize(50, 50);
        btn->setStyleSheet(
            "QPushButton { color: #FFF; background-color: #333; border-radius: 25px; font-size: 20px; }"
            "QPushButton:hover { background-color: #FFA500; color: black; }"
        );
        return btn;
    };

    QPushButton *prevButton = makeButton("⏮");
    QPushButton *playButton = makeButton("▶");
    QPushButton *pauseButton = makeButton("⏸");
    QPushButton *stopButton = makeButton("⏹");
    QPushButton *nextButton = makeButton("⏭");

    // === Volume ===
    isMuted = false;
    lastVolume = 50;

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(lastVolume);
    volumeSlider->setFixedWidth(120);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: #444; border-radius: 3px; }"
        "QSlider::sub-page:horizontal { background: #FFA500; border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 12px; background: #FFF; border-radius: 6px; margin: -3px 0; }"
    );

    volumeButton = makeButton("🔊");
    volumeButton->setFixedSize(45, 45);

    controlsLayout->addStretch();
    controlsLayout->addWidget(prevButton);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addWidget(nextButton);
    controlsLayout->addStretch();
    controlsLayout->addWidget(volumeButton);
    controlsLayout->addWidget(volumeSlider);

    mainLayout->addWidget(controls);

    // === Barre de progression tout en bas ===
    QWidget *progressWidget = new QWidget(this);
    progressWidget->setFixedHeight(40);
    progressWidget->setStyleSheet("background-color: #202020;");

    QHBoxLayout *progressLayout = new QHBoxLayout(progressWidget);
    progressLayout->setContentsMargins(20, 0, 20, 0);

    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setRange(0, 0);
    progressSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 6px; background: #444; border-radius: 3px; }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFA500, stop:1 #FF5F00); border-radius: 3px; }"
        "QSlider::handle:horizontal { background: #FFF; border-radius: 7px; width: 14px; margin: -4px 0; }"
    );

    timeLabel = new QLabel("00:00 / 00:00", this);
    timeLabel->setStyleSheet("color: #DDD; font-weight: bold; font-size: 14px;");

    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(timeLabel);
    mainLayout->addWidget(progressWidget);

    setCentralWidget(central);

    // === Menu ===
    QMenu *fileMenu = menuBar()->addMenu("📁 Fichier");
    QAction *openAction = new QAction("Ouvrir un média", this);
    QAction *exitAction = new QAction("Quitter", this);
    QAction *themeAction = new QAction("Changer de thème", this);

    fileMenu->addAction(openAction);
    fileMenu->addAction(themeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApp);
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);

    // === Connexions ===
    connect(prevButton, &QPushButton::clicked, playlistManager, &PlaylistManager::previous);
    connect(nextButton, &QPushButton::clicked, playlistManager, &PlaylistManager::next);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playMedia);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pauseMedia);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopMedia);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(volumeButton, &QPushButton::clicked, this, &MainWindow::toggleMute);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
}

MainWindow::~MainWindow() {}

// === Slots ===
void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier média");
    if (!fileName.isEmpty())
        playlistManager->addMedia(fileName);
}

void MainWindow::exitApp() { close(); }
void MainWindow::playMedia() { player->play(); }
void MainWindow::pauseMedia() { player->pause(); }
void MainWindow::stopMedia() { player->stop(); }

void MainWindow::playFromPlaylist(const QString &filePath) {
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();

    // Attendre un peu pour laisser Qt lire les métadonnées
    QTimer::singleShot(1000, this, &MainWindow::showMediaInfo);
}

// === Progress bar ===
void MainWindow::updatePosition(qint64 position) {
    progressSlider->setValue(static_cast<int>(position));
    QTime current(0, 0), total(0, 0);
    current = current.addMSecs(position);
    total = total.addMSecs(player->duration());
    QString format = "mm:ss";
    timeLabel->setText(current.toString(format) + " / " + total.toString(format));
}

void MainWindow::updateDuration(qint64 duration) {
    progressSlider->setRange(0, static_cast<int>(duration));
}

void MainWindow::setPosition(int position) { player->setPosition(position); }

// === Volume ===
void MainWindow::changeVolume(int value) {
    if (player->audioOutput()) {
        player->audioOutput()->setVolume(value / 100.0);
        if (value == 0) {
            volumeButton->setText("🔇");
            isMuted = true;
        } else {
            volumeButton->setText("🔊");
            isMuted = false;
            lastVolume = value;
        }
    }
}

void MainWindow::toggleMute() {
    if (!player->audioOutput()) return;

    if (isMuted) {
        isMuted = false;
        volumeSlider->setValue(lastVolume);
        player->audioOutput()->setVolume(lastVolume / 100.0);
        volumeButton->setText("🔊");
    } else {
        isMuted = true;
        lastVolume = volumeSlider->value();
        player->audioOutput()->setVolume(0.0);
        volumeSlider->setValue(0);
        volumeButton->setText("🔇");
    }
}

// === Métadonnées ===
void MainWindow::showMediaInfo() {
    infoBox->clear();

    QMediaMetaData meta = player->metaData();
    QString info;

    auto safe = [&](QMediaMetaData::Key key, const QString &fallback = "Inconnu") {
        QVariant val = meta.value(key);
        return val.isValid() && !val.toString().isEmpty() ? val.toString() : fallback;
    };

    info += "🎵 Titre : " + safe(QMediaMetaData::Title) + "\n";
    info += "🎞️ Codec vidéo : " + safe(QMediaMetaData::VideoCodec) + "\n";
    info += "🎧 Codec audio : " + safe(QMediaMetaData::AudioCodec) + "\n";

    QVariant res = meta.value(QMediaMetaData::Resolution);
    if (res.isValid()) {
        QSize s = res.toSize();
        info += QString("📐 Résolution : %1 x %2\n").arg(s.width()).arg(s.height());
    } else {
        info += "📐 Résolution : Inconnue\n";
    }

    qint64 dur = player->duration();
    if (dur > 0) {
        int sec = dur / 1000;
        int min = sec / 60;
        sec %= 60;
        info += QString("⏱ Durée : %1:%2\n")
                    .arg(min, 2, 10, QLatin1Char('0'))
                    .arg(sec, 2, 10, QLatin1Char('0'));
    }

    if (info.trimmed().isEmpty())
        info = "⚠️ Aucune métadonnée trouvée.\nCertaines infos requièrent FFmpeg.";

    infoBox->setPlainText(info);
}


// === Thèmes ===
void MainWindow::toggleTheme() {
    darkMode = !darkMode;
    darkMode ? applyDarkTheme() : applyLightTheme();
}

void MainWindow::applyDarkTheme() {
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(30, 30, 30));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Button, QColor(45, 45, 45));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::Base, QColor(20, 20, 20));
    pal.setColor(QPalette::Text, Qt::white);
    setPalette(pal);
}

void MainWindow::applyLightTheme() {
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Button, QColor(220, 220, 220));
    pal.setColor(QPalette::ButtonText, Qt::black);
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::Text, Qt::black);
    setPalette(pal);
}
