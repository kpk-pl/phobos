#!/usr/bin/python3

from enum import Enum
from PyQt5.QtCore import Qt, QObject, QThreadPool, QSize, pyqtSlot, pyqtSignal
from PyQt5.QtGui import QPixmap
from ImageLoaderThread import ImageLoaderThread
import ImageOperations


class PhotoItemState(Enum):
    UNKNOWN = 0
    SELECTED = 1
    DISCARDED = 2


def _sizeFits(smaller, bigger):
    return smaller.width() <= bigger.width() and smaller.height() <= bigger.height()


class PhotoItem(QObject):
    PHOTOITEM_PIXMAP_SIZE = QSize(320, 240)
    selectionChanged = pyqtSignal(PhotoItemState)

    def __init__(self, fileName, seriesUuid, parent=None):
        super(PhotoItem, self).__init__(parent)

        self.fileName = fileName
        self.seriesUuid = seriesUuid
        self.state = PhotoItemState.UNKNOWN
        self.pixmap = None

    def loadPhoto(self, size, onLoadFun):
        # there already is a pixmap smaller or equal to requested
        if self.pixmap is not None and _sizeFits(size, self.pixmap.size()):
            onLoadFun(self.pixmap)
        else:
            loaderTask = ImageLoaderThread(self.fileName, [self.PHOTOITEM_PIXMAP_SIZE, size])

            loaderTask.signals.pixmapReady.connect(onLoadFun, Qt.QueuedConnection)

            if self.pixmap is None:
                loaderTask.signals.pixmapReady.connect(self._loadedPhoto, Qt.QueuedConnection)

            QThreadPool.globalInstance().start(loaderTask)

    def isSelected(self):
        return self.state == PhotoItemState.SELECTED

    @pyqtSlot()
    def select(self):
        self.state = PhotoItemState.SELECTED
        self.selectionChanged.emit(self.state)

    @pyqtSlot()
    def discard(self):
        self.state = PhotoItemState.DISCARDED
        self.selectionChanged.emit(self.state)

    @pyqtSlot()
    def toggleSelection(self):
        self.discard() if self.isSelected() else self.select()

    @pyqtSlot(QPixmap)
    def _loadedPhoto(self, pixmap):
        if not _sizeFits(pixmap, self.PHOTOITEM_PIXMAP_SIZE):
            self.pixmap = ImageOperations.scaleImage(pixmap, self.PHOTOITEM_PIXMAP_SIZE)
        else:
            self.pixmap = pixmap
