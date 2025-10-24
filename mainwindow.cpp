#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), darkMode(true)
{
    setWindowTitle("EpiKodi");
    resize(1200, 700);

    applyDarkTheme();

    // Lecteur vidéo avec audio
    player = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);
    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);

    // Playlist
    playlistWidget = new QListWidget(this);
    playlistManager = new PlaylistManager(playlistWidget, this);
    connect(playlistManager, &PlaylistManager::mediaChanged,
            this, &MainWindow::playFromPlaylist);

    // Layout principal : splitter vertical
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(playlistWidget);
    splitter->addWidget(videoWidget);
    splitter->setStretchFactor(1, 4);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(splitter);

    // Contrôles (10% de la hauteur)
    QWidget *controls = new QWidget(this);
    controls->setFixedHeight(this->height() * 0.10); // 10% de la hauteur
    QHBoxLayout *controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setSpacing(10);
    controlsLayout->setContentsMargins(0, 5, 0, 5);

    QPushButton *prevButton = new QPushButton("⏮ Previous", this);
    QPushButton *playButton = new QPushButton("▶ Play", this);
    QPushButton *pauseButton = new QPushButton("⏸ Pause", this);
    QPushButton *stopButton = new QPushButton("⏹ Stop", this);
    QPushButton *nextButton = new QPushButton("⏭ Next", this);

    // Tailles réduites
    QSize btnSize(90, 30);
    prevButton->setFixedSize(btnSize);
    playButton->setFixedSize(btnSize);
    pauseButton->setFixedSize(btnSize);
    stopButton->setFixedSize(btnSize);
    nextButton->setFixedSize(btnSize);

    controlsLayout->addStretch();
    controlsLayout->addWidget(prevButton);
    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addWidget(nextButton);
    controlsLayout->addStretch();

    mainLayout->addWidget(controls);
    setCentralWidget(central);

    // Connexions boutons
    connect(prevButton, &QPushButton::clicked, playlistManager, &PlaylistManager::previous);
    connect(nextButton, &QPushButton::clicked, playlistManager, &PlaylistManager::next);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playMedia);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pauseMedia);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopMedia);

    // Barre de menu
    QMenu *fileMenu = menuBar()->addMenu("Fichier");

    QAction *openAction = new QAction("Ouvrir", this);
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);

    QAction *exitAction = new QAction("Quitter", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApp);
    fileMenu->addAction(exitAction);

    QAction *themeAction = new QAction("Changer le mode", this);
    themeAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    fileMenu->addAction(themeAction);
}

MainWindow::~MainWindow() {}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier média");
    if (!fileName.isEmpty()) {
        playlistManager->addMedia(fileName);
    }
}

void MainWindow::exitApp() { close(); }

void MainWindow::playMedia() { player->play(); }
void MainWindow::pauseMedia() { player->pause(); }
void MainWindow::stopMedia() { player->stop(); }

void MainWindow::playFromPlaylist(const QString &filePath) {
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
}

void MainWindow::toggleTheme() {
    if(darkMode) {
        applyLightTheme();
        darkMode = false;
    } else {
        applyDarkTheme();
        darkMode = true;
    }
}

void MainWindow::applyDarkTheme() {
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(30, 30, 30));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Button, QColor(50, 50, 50));
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
