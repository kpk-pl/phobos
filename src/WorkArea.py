#!/usr/bin/python3

from PyQt5.QtCore import Qt
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QSizePolicy
from PhotoItem import PhotoItem
from Exceptions import CannotReadImageException
from PhotoSeries import PhotoSeries


class WorkArea(QWidget):
    SPACING_BETWEEN_SERIES = 15

    def __init__(self):
        super(WorkArea, self).__init__()

        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(self.SPACING_BETWEEN_SERIES)
        self.setLayout(layout)

    # TODO: addPhotos is blocking, so all photos are drawn at once
    #       Fit this so that each photo or each series is painted separately
    #       And updates the screen
    #       Or create a dialog window to show loading process in percent
    def addPhotos(self, photos):
        count = 0
        series = PhotoSeries()

        for fileName in photos:
            try:
                phitem = PhotoItem(fileName)
            except CannotReadImageException as e:
                print("TODO: display warning with str(e) " + str(e))
            else:
                series.addPhoto(phitem)
                count += 1
                if count == 7:
                    self.layout().addWidget(series)
                    series = PhotoSeries()
                    count = 0

        if series.count():
            self.layout().addWidget(series)
