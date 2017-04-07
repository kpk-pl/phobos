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

    def _updateView(self):
        if self.currentSeriesInView is None:
            self.setCurrentWidget(self.allSeriesView)
        else:
            self.setCurrentWidget(self._currentSeriesWidget)

    @pyqtSlot(QUuid)
    def switchSeries(self, seriesUuid, offset=0): # openInSeries
        phSeries = self.series.findSeries(seriesUuid, offset)
        assert phSeries is not None

        self.currentSeriesInView = phSeries.uuid
        self._currentSeriesWidget.showSeries(phSeries)
        self._updateView()

    def _findSeriesToSwitch(self):
        if self.currentSeriesInView is None:
            focusWidget = Utils.focusedPhotoItem()
            if focusWidget is not None:
                return focusWidget.photoItem.seriesUuid
            elif len(self.series) > 0:
                return self.series[0].uuid
            else:
                return None
        else:
            return self.currentSeriesInView

    @pyqtSlot(str)
    def switchView(self, specificator):
        if specificator == "showAllSeries":
            self.currentSeriesInView = None
            self._updateView()
        elif specificator in ["showNumSeries", "showOneSeries"]:
            toSwitch = self._findSeriesToSwitch()
            if toSwitch is None:
                return

            if specificator == "showNumSeries":
                if self._currentSeriesWidget == self.seriesNumView and toSwitch == self.currentSeriesInView:
                    return
                if self._currentSeriesWidget != self.seriesNumView:
                    # move elements to another View
                    self._currentSeriesWidget = self.seriesNumView
            else:
                if self._currentSeriesWidget == self.seriesRowView and toSwitch == self.currentSeriesInView:
                    return
                if self._currentSeriesWidget != self.seriesRowView:
                    # move elements to another View
                    self._currentSeriesWidget = self.seriesRowView

            self.switchSeries(toSwitch)

    @pyqtSlot()
    def showNextSeries(self):
        self._showOffsetSeries(1)

    @pyqtSlot()
    def showPrevSeries(self):
        self._showOffsetSeries(-1)

    def _showOffsetSeries(self, offset):
        if self.currentSeriesInView is not None:
            self.switchSeries(self.currentSeriesInView, offset)
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
        self.allSeriesView.openInSeries.connect(self.switchSeries)
        self.series.newSeries.connect(self.allSeriesView.addPhotoSeries)

        self.seriesRowView.switchView.connect(self.switchView)
        self.seriesRowView.nextSeries.connect(self.showNextSeries)
        self.seriesRowView.prevSeries.connect(self.showPrevSeries)

        self.seriesNumView.switchView.connect(self.switchView)
        self.seriesNumView.nextSeries.connect(self.showNextSeries)
        self.seriesNumView.prevSeries.connect(self.showPrevSeries)
