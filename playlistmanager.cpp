#include "playlistmanager.h"
#include <QListWidgetItem>

PlaylistManager::PlaylistManager(QListWidget *listWidget, QObject *parent)
    : QObject(parent), playlistWidget(listWidget), currentIndex(-1)
{
    connect(playlistWidget, &QListWidget::itemDoubleClicked,
            this, &PlaylistManager::onItemDoubleClicked);
}

void PlaylistManager::addMedia(const QString &filePath) {
    mediaList.append(filePath);
    playlistWidget->addItem(filePath);

    // Si c’est le premier fichier ajouté, on le sélectionne
    if (currentIndex == -1) {
        currentIndex = 0;
        playlistWidget->setCurrentRow(0);
        emit mediaChanged(mediaList[currentIndex]);
    }
}

QString PlaylistManager::currentMedia() const {
    if (currentIndex >= 0 && currentIndex < mediaList.size())
        return mediaList[currentIndex];
    return QString();
}

void PlaylistManager::next() {
    if (mediaList.isEmpty()) return;
    currentIndex = (currentIndex + 1) % mediaList.size();
    playlistWidget->setCurrentRow(currentIndex);
    emit mediaChanged(mediaList[currentIndex]);
}

void PlaylistManager::previous() {
    if (mediaList.isEmpty()) return;
    currentIndex = (currentIndex - 1 + mediaList.size()) % mediaList.size();
    playlistWidget->setCurrentRow(currentIndex);
    emit mediaChanged(mediaList[currentIndex]);
}

void PlaylistManager::onItemDoubleClicked(QListWidgetItem *item) {
    currentIndex = playlistWidget->row(item);
    emit mediaChanged(mediaList[currentIndex]);
}
