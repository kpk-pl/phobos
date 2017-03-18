#!/usr/bin/python3

from PyQt5.QtWidgets import QWidget, QHBoxLayout


class PhotoSeries(QWidget):
    SPACING_BETWEEN_PHOTOS = 3

    def __init__(self, parent=None):
        super(PhotoSeries, self).__init__(parent)

        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(self.SPACING_BETWEEN_PHOTOS)
        self.setLayout(layout)

    def addPhoto(self, photo):
        self.layout().addWidget(photo)

    def count(self):
        return self.layout().count()

    def selectedCount(self):
        count = 0
        for i in range(self.layout().count()):
            if self.layout().itemAt(i).isSelected():
                count += 1
        return count
