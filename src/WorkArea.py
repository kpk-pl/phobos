#!/usr/bin/python3

from PyQt5.QtCore import Qt, QUuid, pyqtSlot
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QGridLayout
from PhotoItem import PhotoItem
from Exceptions import CannotReadImageException


class PhotoSeries(object):
    def __init__(self):
        super(PhotoSeries, self).__init__()

        self.uuid = QUuid.createUuid()
        self.photos = []

    def addPhoto(self, photo):
        self.photos.append(photo)

    def __len__(self):
        return len(self.photos)


class PhotoSet(object):
    def __init__(self):
        super(PhotoSet, self).__init__()

        self.series = []

    def addSeries(self, series):
        self.series.append(series)

    def findSeries(self, seriesUuid):
        for s in self.series:
            if s.uuid == seriesUuid:
                return s
        return None


class WorkArea(QWidget):
    SPACING_BETWEEN_SERIES = 15
    SPACING_BETWEEN_PHOTOS = 3

    def __init__(self):
        super(WorkArea, self).__init__()

        layout = QGridLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setHorizontalSpacing(self.SPACING_BETWEEN_PHOTOS)
        layout.setVerticalSpacing(self.SPACING_BETWEEN_SERIES)
        self.setLayout(layout)

        self.series = PhotoSet()

    # TODO: addPhotos is blocking, so all photos are drawn at once
    #       Fit this so that each photo or each series is painted separately
    #       And updates the screen
    #       Or create a dialog window to show loading process in percent
    def addPhotos(self, photos):
        series = self._createSeries(photos)

        for s in series:
            row = self.layout().rowCount()
            for col in range(len(s.photos)):
                self.layout().addWidget(s.photos[col], row, col)
            self.series.addSeries(s)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid):
        phSeries = self.series.findSeries(seriesUuid)
        assert phSeries is not None

        print("Showing series")


    def _createSeries(self, photos):
        series = []
        currentSeries = PhotoSeries()

        for fileName in photos:
            try:
                phitem = PhotoItem(fileName, currentSeries.uuid)
            except CannotReadImageException as e:
                print("TODO: display warning with str(e) " + str(e))
            else:
                phitem.openInSeries.connect(self.openInSeries)
                currentSeries.addPhoto(phitem)
                if len(currentSeries) == 7:
                    series.append(currentSeries)
                    currentSeries = PhotoSeries()

        if len(currentSeries):
            series.append(currentSeries)

        return series
