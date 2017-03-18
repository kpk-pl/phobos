#!/usr/bin/python3

from PyQt5.QtCore import Qt
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import (QWidget, QHBoxLayout)
from ImageWidget import ImageWidget


class WorkArea(QWidget):
    def __init__(self):
        super(WorkArea, self).__init__()

        layout = QHBoxLayout()
        self.setLayout(layout)

    def addPhotos(self, photos):
        for fileName in photos:
            photo = WorkArea._readPhotoFromFile(fileName)
            if not photo:
                continue  # TODO display warning!

            widget = ImageWidget(photo)
            self.layout().addWidget(widget)

    def _readPhotoFromFile(fileName):
        reader = QtGui.QImageReader(fileName)
        reader.setAutoTransform(True)
        reader.setAutoDetectImageFormat(True)

        return reader.read()
