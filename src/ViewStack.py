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
        self.currentSeriesInView = None

        self.allSeriesView = AllSeriesView()
        self.seriesRowView = SeriesRowView()

        self.addWidget(self.allSeriesView)
        self.addWidget(self.seriesRowView)

        #self.allSeriesView.setFocus()

        self._connectSignals()

    def addPhotos(self, photos):
        series = self._createSeries(photos)
        for s in series:
            self.series.addSeries(s)

        self.allSeriesView.addPhotoSeries(series)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid, offset=0):
        phSeries = self.series.findSeries(seriesUuid, offset)
        assert phSeries is not None

        self.currentSeriesInView = phSeries.uuid
        photoPixmaps = self.allSeriesView.getPixmapsForSeries(self.currentSeriesInView)
        self.seriesRowView.showSeries(phSeries, photoPixmaps)
        self.setCurrentWidget(self.seriesRowView)

    @pyqtSlot()
    def returnFromView(self):
        self.currentSeriesInView = None
        self.setCurrentWidget(self.allSeriesView)

    @pyqtSlot()
    def openNextSeries(self):
        self.openInSeries(self.currentSeriesInView, 1)

    @pyqtSlot()
    def openPrevSeries(self):
        self.openInSeries(self.currentSeriesInView, -1)

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
        self.seriesRowView.returnFromView.connect(self.returnFromView)
        self.seriesRowView.nextSeries.connect(self.openNextSeries)
        self.seriesRowView.prevSeries.connect(self.openPrevSeries)
