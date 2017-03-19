#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, QSize, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel
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
    MINI_PIXMAP_SIZE = QSize(160, 120)
    MEDIUM_PIXMAP_SIZE = QSize(640, 480)

    def __init__(self, fileName, parent=None):
        super(ImageWidget, self).__init__(parent)

        self.fileName = fileName

        fullPixmap = QtGui.QPixmap.fromImage(self._readImageFromFile(self.fileName))
        self._imagePixmap = scaleImage(fullPixmap, self.MINI_PIXMAP_SIZE)

    def getPixmap(self):
        return self._imagePixmap

    def scaledPixmap(self, size):
        if not sizeFits(size, self._imagePixmap.size()):  # requested size if bigger than current pixmap
            fullPixmap = QtGui.QPixmap.fromImage(self._readImageFromFile(self.fileName))

            # TODO: load new pixmapx in separate thread
            # now resizing window which causes pixmap recalculation is blocking
            # small pixmap should be returned but bigger calculated and then widget should be updated
            if sizeFits(size, self.MEDIUM_PIXMAP_SIZE):
                self._imagePixmap = scaleImage(fullPixmap, self.MEDIUM_PIXMAP_SIZE)
            else:
                self._imagePixmap = fullPixmap

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

    def hasHeightForWidth(self):
        return self._imagePixmap is not None

    def sizeHint(self):
        return self._imagePixmap.size()

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
