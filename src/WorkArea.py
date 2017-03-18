#!/usr/bin/python3

from PyQt5.QtCore import Qt
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import (QWidget, QHBoxLayout)
from PhotoItem import PhotoItem
from Exceptions import CannotReadImageException


class WorkArea(QWidget):
    def __init__(self):
        super(WorkArea, self).__init__()

        layout = QHBoxLayout()
        self.setLayout(layout)

    def addPhotos(self, photos):
        for fileName in photos:
            try:
                widget = PhotoItem(fileName)
                self.layout().addWidget(widget)
            except CannotReadImageException as e:
                pass  # display warning with str(e)
