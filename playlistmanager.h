#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QListWidget>

class PlaylistManager : public QObject {
    Q_OBJECT
public:
    explicit PlaylistManager(QListWidget *listWidget, QObject *parent = nullptr);

    void addMedia(const QString &filePath);
    QString currentMedia() const;
    void next();
    void previous();

signals:
    void mediaChanged(const QString &filePath);

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);

private:
    QListWidget *playlistWidget;
    int currentIndex;
    QStringList mediaList;
};

#endif // PLAYLISTMANAGER_H
