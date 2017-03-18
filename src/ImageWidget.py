#!/usr/bin/python3

from PyQt5.QtCore import Qt, QPoint
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QLabel


class ImageWidget(QLabel):
    def __init__(self, img, parent=None):
        super(QLabel, self).__init__(parent)
        self.imagePixmap = QtGui.QPixmap.fromImage(img)

    def paintEvent(self, event):
        size = self.size()
        point = QPoint(0, 0)
        scaledPix = self.renderedPixmap()

        # start painting the label from left upper corner
        point.setX((size.width() - scaledPix.width())/2)
        point.setY((size.height() - scaledPix.height())/2)
        QtGui.QPainter(self).drawPixmap(point, scaledPix)

    def renderedPixmap(self):
        size = self.size()
        scaledPix = self.imagePixmap.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        return scaledPix
