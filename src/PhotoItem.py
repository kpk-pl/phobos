#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QBoxLayout
from ImageWidget import ImageWidget
from Exceptions import CannotReadImageException


class PhotoItem(ImageWidget):
    def __init__(self, fileName, parent=None):
        super(PhotoItem, self).__init__(PhotoItem._readImageFromFile(fileName), parent)

        self.fileName = fileName
        self._borderWidth = 1
        self._borderColor = QtGui.QColor(Qt.green)

    def renderedPixmap(self):
        size = self.size()
        availableSize = QSize(size.width() - 2*self._borderWidth, size.height() - 2*self._borderWidth)
        scaledImage = self.imagePixmap.scaled(availableSize, Qt.KeepAspectRatio, Qt.SmoothTransformation)

        pixmapSize = QSize(scaledImage.size().width() + 2*self._borderWidth,
                           scaledImage.size().height() + 2*self._borderWidth)
        pixmap = QtGui.QPixmap(pixmapSize)
        pixmap.fill(self._borderColor)
        painter = QtGui.QPainter(pixmap)

        centerPoint = QPoint((pixmap.width() - scaledImage.width()) / 2,
                             (pixmap.height() - scaledImage.height()) / 2)
        painter.drawPixmap(centerPoint, scaledImage)

        painter.end()
        return pixmap

    @staticmethod
    def _readImageFromFile(fileName):
        reader = QtGui.QImageReader(fileName)
        reader.setAutoTransform(True)
        reader.setAutoDetectImageFormat(True)

        image = reader.read()
        if not image:
            raise CannotReadImageException()

        return image
