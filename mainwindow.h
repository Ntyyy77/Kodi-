#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "playlistmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void exitApp();
    void playMedia();
    void pauseMedia();
    void stopMedia();
    void playFromPlaylist(const QString &filePath);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void setPosition(int position);
    void changeVolume(int value);
    void toggleMute();
    void toggleTheme();
    void showMediaInfo();  // ✅ nouvelle fonction

private:
    void applyDarkTheme();
    void applyLightTheme();

    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QListWidget *playlistWidget;
    PlaylistManager *playlistManager;
    QSlider *progressSlider;
    QSlider *volumeSlider;
    QPushButton *volumeButton;
    QLabel *timeLabel;
    QTextEdit *infoBox;  // ✅ zone d’infos à droite

    bool darkMode;
    bool isMuted;
    int lastVolume;
};

#endif // MAINWINDOW_H
