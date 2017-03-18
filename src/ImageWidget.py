#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel


class ClickableLabel(QLabel):
    clicked = pyqtSignal()

    def __init__(self, parent=None):
        super(ClickableLabel, self).__init__(parent)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.clicked.emit()


class ImageWidget(ClickableLabel):
    def __init__(self, img, parent=None):
        super(ImageWidget, self).__init__(parent)
        self.imagePixmap = QtGui.QPixmap.fromImage(img)

    def paintEvent(self, event):
        size = self.size()
        point = QPoint(0, 0)
        scaledPix = self._renderedPixmap()

        # start painting the label from left upper corner
        point.setX((size.width() - scaledPix.width())/2)
        point.setY((size.height() - scaledPix.height())/2)
        QtGui.QPainter(self).drawPixmap(point, scaledPix)

    def _renderedPixmap(self):
        size = self.size()
        scaledPix = self.imagePixmap.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        return scaledPix
