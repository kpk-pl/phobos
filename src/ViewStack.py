from PyQt5.QtCore import QUuid, pyqtSlot
from PyQt5.QtWidgets import QStackedWidget, QApplication
from AllSeriesView import AllSeriesView
from SeriesRowView import SeriesRowView
from PhotoItemWidget import PhotoItemWidget
from PhotoContainers import PhotoSeriesSet


class ViewStack(QStackedWidget):
    def __init__(self, parent=None):
        super(ViewStack, self).__init__(parent)

        self.series = PhotoSeriesSet()
        self.currentSeriesInView = None

        self._setupUi()
        self._connectSignals()

    def addPhotos(self, photos):
        self.series.addPhotos(photos)

    @pyqtSlot(QUuid)
    def openInSeries(self, seriesUuid, offset=0):
        phSeries = self.series.findSeries(seriesUuid, offset)
        assert phSeries is not None
        assert phSeries.uuid == seriesUuid

        self.currentSeriesInView = phSeries.uuid
        self.seriesRowView.showSeries(phSeries)
        self.setCurrentWidget(self.seriesRowView)

    @pyqtSlot()
    def showAllSeries(self):
        self.currentSeriesInView = None
        self.setCurrentWidget(self.allSeriesView)

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
            focusWidget = self._focusedPhotoItem()
            if focusWidget is not None:
                nextSeries = self.series.findSeries(focusWidget.photoItem.seriesUuid, offset)
                self.allSeriesView.focusSeries(nextSeries.uuid)
            else:
                self.allSeriesView.focusSeries()

    @staticmethod
    def _focusedPhotoItem():
        focusWidget = QApplication.focusWidget()
        if focusWidget is not None and isinstance(focusWidget, PhotoItemWidget):
            return focusWidget
        return None

    def _setupUi(self):
        self.allSeriesView = AllSeriesView()
        self.seriesRowView = SeriesRowView()

        self.addWidget(self.allSeriesView)
        self.addWidget(self.seriesRowView)

    def _connectSignals(self):
        self.allSeriesView.openInSeries.connect(self.openInSeries)
        self.series.newSeries.connect(self.allSeriesView.addPhotoSeries)

        self.seriesRowView.returnFromView.connect(self.showAllSeries)
        self.seriesRowView.nextSeries.connect(self.showNextSeries)
        self.seriesRowView.prevSeries.connect(self.showPrevSeries)
