#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidget>
#include "playlistmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void openFile();
    void exitApp();
    void playMedia();
    void pauseMedia();
    void stopMedia();
    void toggleTheme();
    void playFromPlaylist(const QString &filePath);

private:
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QListWidget *playlistWidget;
    PlaylistManager *playlistManager;
    bool darkMode;

    void applyDarkTheme();
    void applyLightTheme();
};

#endif // MAINWINDOW_H
