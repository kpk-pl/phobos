#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, QSize, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel, QSizePolicy
from Exceptions import CannotReadImageException


class ClickableLabel(QLabel):
    clicked = pyqtSignal()

    def __init__(self, parent=None):
        super(ClickableLabel, self).__init__(parent)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.clicked.emit()


def sizeFits(small, big):
    return small.width() <= big.width() and small.height() <= big.height()


def scaleImage(image, size):
    return image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)


class ImageWidget(ClickableLabel):
    MINI_PIXMAP_SIZE = QSize(320, 240)

    def __init__(self, fileName, maxSize=None, parent=None):
        super(ImageWidget, self).__init__(parent)

        self.fileName = fileName
        self.maxSize = maxSize if maxSize is not None else self.MINI_PIXMAP_SIZE

        fullPixmap = QtGui.QPixmap.fromImage(self._readImageFromFile(self.fileName))
        self._fullPixmapSize = fullPixmap.size()
        self.setGeometry(0, 0, self._fullPixmapSize.width(), self._fullPixmapSize.height())
        self._imagePixmap = scaleImage(fullPixmap, self.maxSize)

        self.setScaledContents(False)
        self.setSizePolicy(QSizePolicy.Maximum, QSizePolicy.Maximum)
        self.setStyleSheet("border:1px solid red;")

#    def setPixmap(self, pixmap):
#        self._imagePixmap = pixmap
#        super(ImageWidget, self).setPixmap(self._renderedPixmap())

    def getPixmap(self):
        return self._imagePixmap

    def scaledPixmap(self, size):
        return scaleImage(self._imagePixmap, size)

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
        return self._fullPixmapSize

    def _renderedPixmap(self):
        return self.scaledPixmap(self.size())

    @staticmethod
    def _readImageFromFile(fileName):
        reader = QtGui.QImageReader(fileName)
        reader.setAutoTransform(True)
        reader.setAutoDetectImageFormat(True)

        image = reader.read()
        if not image:
            raise CannotReadImageException()

        return image
