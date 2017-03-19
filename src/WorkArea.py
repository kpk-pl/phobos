#!/usr/bin/python3

from PyQt5.QtCore import Qt, QUuid, pyqtSlot
from PyQt5.QtWidgets import QWidget, QGridLayout
from PhotoItem import PhotoItem
from Exceptions import CannotReadImageException
from PhotoContainers import PhotoSeries, PhotoSeriesSet


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

        self.series = PhotoSeriesSet()

    # TODO: addPhotos is blocking, so all photos are drawn at once
    #       Fit this so that each photo or each series is painted separately
    #       And updates the screen
    #       Or create a dialog window to show loading process in percent
    def addPhotos(self, photos):
        series = self._createSeries(photos)

        for s in series:
            self.series.addSeries(s)

            row = self.layout().rowCount()
            for col in range(len(s.photos)):
                self.layout().addWidget(s.photos[col], row, col)

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
