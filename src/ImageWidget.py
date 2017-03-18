#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, QSize, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel


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

    def __init__(self, img, parent=None):
        super(ImageWidget, self).__init__(parent)

        self._imagePixmap = QtGui.QPixmap.fromImage(img)

        self._mediumPixmap = None
        if not sizeFits(self._imagePixmap, self.MEDIUM_PIXMAP_SIZE):
            self._mediumPixmap = scaleImage(self._imagePixmap, self.MEDIUM_PIXMAP_SIZE)

        self._miniPixmap = None
        if not sizeFits(self._imagePixmap, self.MINI_PIXMAP_SIZE):
            self._miniPixmap = scaleImage(self._imagePixmap, self.MINI_PIXMAP_SIZE)

    def getPixmap(self):
        return self._imagePixmap

    def scaledPixmap(self, size):
        if self._miniPixmap is not None and sizeFits(size, self.MINI_PIXMAP_SIZE):
            return scaleImage(self._miniPixmap, size)
        if self._mediumPixmap is not None and sizeFits(size, self.MEDIUM_PIXMAP_SIZE):
            return scaleImage(self._mediumPixmap, size)
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
        return self._imagePixmap.height()*width/self._imagePixmap.width()

    def hasHeightForWidth(self):
        return True

    def sizeHint(self):
        return self._imagePixmap.size()

    def _renderedPixmap(self):
        return self.scaledPixmap(self.size())

