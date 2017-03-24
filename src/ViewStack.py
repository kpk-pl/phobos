from PyQt5.QtCore import QUuid, pyqtSlot
from PyQt5.QtWidgets import QStackedWidget
from AllSeriesView import AllSeriesView
from SeriesRowView import SeriesRowView
from PhotoItem import PhotoItem
from PhotoContainers import PhotoSeries, PhotoSeriesSet


class ViewStack(QStackedWidget):
    def __init__(self, parent=None):
        super(ViewStack, self).__init__(parent)

        self.series = PhotoSeriesSet()

        self.allSeriesView = AllSeriesView()
        self.seriesRowView = SeriesRowView()

        self.addWidget(self.allSeriesView)
        self.addWidget(self.seriesRowView)

        self._connectSignals()

    def addPhotos(self, photos):
        series = self._createSeries(photos)
        for s in series:
            self.series.addSeries(s)

        self.allSeriesView.addPhotoSeries(series)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid):
        phSeries = self.series.findSeries(seriesUuid)
        assert phSeries is not None
        self.seriesRowView.showSeries(phSeries)
        self.setCurrentWidget(self.seriesRowView)

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

    def _connectSignals(self):
        self.allSeriesView.openInSeries.connect(self.openInSeries)
