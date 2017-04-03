#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, pyqtSignal, pyqtSlot
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel
import ImageOperations


class ClickableLabel(QLabel):
    clicked = pyqtSignal()

    def __init__(self, parent=None):
        super(ClickableLabel, self).__init__(parent)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.clicked.emit()


class ImageWidget(ClickableLabel):
    def __init__(self, pixmap, parent=None):
        super(ImageWidget, self).__init__(parent)

        self.setScaledContents(False)
        self._imagePixmap = pixmap
        self.setImagePixmap(pixmap)

    @pyqtSlot(QtGui.QPixmap)
    def setImagePixmap(self, pixmap):
        self._imagePixmap = pixmap
        #self.setGeometry(0, 0, self._imagePixmap.width(), self._imagePixmap.height())
        self.updateGeometry()
        self.update()

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
        return self._imagePixmap.size()

    def _renderedPixmap(self):
        return self.scaledPixmap(self.size())
