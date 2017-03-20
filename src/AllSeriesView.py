#!/usr/bin/python3

from PyQt5.QtCore import Qt, QUuid, pyqtSlot
from PyQt5.QtWidgets import QWidget, QGridLayout
from PhotoItemWidget import PhotoItemWidget
from PhotoItem import PhotoItem
from Exceptions import CannotReadImageException
from PhotoContainers import PhotoSeries, PhotoSeriesSet


class AllSeriesView(QWidget):
    SPACING_BETWEEN_SERIES = 15
    SPACING_BETWEEN_PHOTOS = 3

    def __init__(self):
        super(AllSeriesView, self).__init__()

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
            for col in range(len(s.photoItems)):
                try:
                    photoItemWidget = PhotoItemWidget(s.photoItems[col])
                except CannotReadImageException as e:
                    print("TODO: cannot load image exception " + str(e))
                else:
                    photoItemWidget.openInSeries.connect(self.openInSeries)
                    self.layout().addWidget(photoItemWidget, row, col)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid):
        phSeries = self.series.findSeries(seriesUuid)
        assert phSeries is not None

        print("Showing series")

    def _createSeries(self, photos):
        series = []
        currentSeries = PhotoSeries()

        for fileName in photos:
            phitem = PhotoItem(fileName, currentSeries.uuid)
            currentSeries.addPhotoItem(phitem)
            if len(currentSeries) == 7:
                series.append(currentSeries)
                currentSeries = PhotoSeries()

        if len(currentSeries):
            series.append(currentSeries)

        return series
