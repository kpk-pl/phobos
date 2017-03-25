#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, QThreadPool, pyqtSignal, pyqtSlot
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel
import ImageOperations
from ImageLoaderThread import ImageLoaderThread


class ClickableLabel(QLabel):
    clicked = pyqtSignal()

    def __init__(self, parent=None):
        super(ClickableLabel, self).__init__(parent)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.clicked.emit()


class ImageWidget(ClickableLabel):
    def __init__(self, fileName, maxSize=None, preloadPixmap=None, parent=None):
        super(ImageWidget, self).__init__(parent)

        self.fileName = fileName
        self.maxSize = maxSize

        self.setScaledContents(False)

        if preloadPixmap:
            self._setPixmap(preloadPixmap)
            loaderTask = ImageLoaderThread(self.fileName)
            loaderTask.signals.pixmapReady.connect(self._setPixmap, Qt.QueuedConnection)
            QThreadPool.globalInstance().start(loaderTask)
        else:
            fullPixmap = ImageOperations.readImageFromFile(self.fileName)
            self._setPixmap(fullPixmap)

    @pyqtSlot(QtGui.QPixmap)
    def _setPixmap(self, pixmap):
        self.setDisabled(True)
        self._fullPixmapSize = pixmap.size()
        self._imagePixmap = ImageOperations.scaleImage(pixmap, self.sizeHint())
        #self.setGeometry(0, 0, self._imagePixmap.width(), self._imagePixmap.height())
        self.updateGeometry()
        self.setDisabled(False)

    def getPixmap(self):
        return self._imagePixmap

    def scaledPixmap(self, size):
        return ImageOperations.scaleImage(self._imagePixmap, size)

    def paintEvent(self, event):
        size = self.size()
        point = QPoint(0, 0)
        scaledPix = self._renderedPixmap()

        # start painting the label from left upper corner
        point.setX((size.width() - scaledPix.width())/2)
        point.setY((size.height() - scaledPix.height())/2)
        QtGui.QPainter(self).drawPixmap(point, scaledPix)

    def heightForWidth(self, width):
        return self._imagePixmap.height() * width / self._imagePixmap.width() if self._imagePixmap.width() else 0

    def widthForHeight(self, height):
        return self._imagePixmap.width() * height / self._imagePixmap.height() if self._imagePixmap.height() else 0

    def hasHeightForWidth(self):
        return self._imagePixmap is not None

    def sizeHint(self):
        return self.maxSize if self.maxSize is not None else self._fullPixmapSize

    def _renderedPixmap(self):
        return self.scaledPixmap(self.size())
