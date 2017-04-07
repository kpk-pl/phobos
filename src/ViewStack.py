#!/usr/bin/python3

from PyQt5.QtCore import QUuid, pyqtSlot
from PyQt5.QtWidgets import QStackedWidget
from AllSeriesView import AllSeriesView
from SeriesRowView import SeriesRowView
from SeriesNumView import SeriesNumView
from PhotoContainers import PhotoSeriesSet
import Utils
import Config


class ViewStack(QStackedWidget):
    def __init__(self, parent=None):
        super(ViewStack, self).__init__(parent)

        self.series = PhotoSeriesSet()
        self.currentSeriesInView = None

        self._setupUi()
        self._connectSignals()

        if Config.get_or("seriesView.num", "default", False):
            self._currentSeriesWidget = self.seriesNumView
        else:
            self._currentSeriesWidget = self.seriesRowView

    def addPhotos(self, photos):
        self.series.addPhotos(photos)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid, offset=0):
        phSeries = self.series.findSeries(seriesUuid, offset)
        assert phSeries is not None

        self.currentSeriesInView = phSeries.uuid
        self._currentSeriesWidget.showSeries(phSeries)
        self.setCurrentWidget(self._currentSeriesWidget)

    @pyqtSlot()
    def showAllSeries(self):
        self.currentSeriesInView = None
        self.setCurrentWidget(self.allSeriesView)

    @pyqtSlot(str)
    def switchView(self, specificator):
        if specificator == "showAllSeries":
            self.showAllSeries()
        elif specificator == "showNumSeries":
            self._currentSeriesWidget = self.seriesNumView
            if self.currentSeriesInView is None:
                self.showOneSeries()
            else:
                self.openInSeries(self.currentSeriesInView)
        elif specificator == "showOneSeries":
            self._currentSeriesWidget = self.seriesRowView
            if self.currentSeriesInView is None:
                self.showOneSeries()
            else:
                self.openInSeries(self.currentSeriesInView)

    @pyqtSlot()
    def showOneSeries(self):
        if self.currentSeriesInView is not None:
            return

        focusWidget = self._focusedPhotoItem()
        if focusWidget is not None:
            self.openInSeries(focusWidget.photoItem.seriesUuid)
        elif len(self.series) > 0:
            self.openInSeries(self.series[0].uuid)

    @pyqtSlot()
    def showNextSeries(self):
        self._showOffsetSeries(1)

    @pyqtSlot()
    def showPrevSeries(self):
        self._showOffsetSeries(-1)

    def _showOffsetSeries(self, offset):
        if self.currentSeriesInView is not None:
            self.openInSeries(self.currentSeriesInView, offset)
        else:
            focusWidget = Utils.focusedPhotoItem()
            if focusWidget is not None:
                nextSeries = self.series.findSeries(focusWidget.photoItem.seriesUuid, offset)
                self.allSeriesView.focusSeries(nextSeries.uuid)
            else:
                self.allSeriesView.focusSeries()

    def _setupUi(self):
        self.allSeriesView = AllSeriesView()
        self.seriesRowView = SeriesRowView()
        self.seriesNumView = SeriesNumView()

        self.addWidget(self.allSeriesView)
        self.addWidget(self.seriesRowView)
        self.addWidget(self.seriesNumView)

    def _connectSignals(self):
        self.allSeriesView.openInSeries.connect(self.openInSeries)
        self.series.newSeries.connect(self.allSeriesView.addPhotoSeries)

        self.seriesRowView.switchView.connect(self.switchView)
        self.seriesRowView.nextSeries.connect(self.showNextSeries)
        self.seriesRowView.prevSeries.connect(self.showPrevSeries)

        self.seriesNumView.switchView.connect(self.switchView)
        self.seriesNumView.nextSeries.connect(self.showNextSeries)
        self.seriesNumView.prevSeries.connect(self.showPrevSeries)
