#!/usr/bin/python3

from PyQt5.QtCore import QUuid, QSize, pyqtSignal, pyqtSlot
from PyQt5.QtWidgets import QWidget, QGridLayout, QVBoxLayout, QScrollArea, QFrame
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar, NavigationCapability
from Exceptions import CannotReadImageException
from PhotoContainers import PhotoSeries
import ImageOperations
import Config


class AllSeriesView(QWidget):
    openInSeries = pyqtSignal(QUuid)

    def __init__(self):
        super(AllSeriesView, self).__init__()

        self._setupUi()
        self.seriesUuidToRow = {}
        self._preloadPixmap = None

    def numberOfSeries(self):
        return len(self.seriesUuidToRow)

    def focusSeries(self, seriesUuid=None):
        if seriesUuid is None or seriesUuid not in self.seriesUuidToRow:
            if self.numberOfSeries() > 0:
                self._grid.itemAtPosition(0, 0).widget().setFocus()
        else:
            self._grid.itemAtPosition(self.seriesUuidToRow[seriesUuid], 0).widget().setFocus()

    @pyqtSlot(PhotoSeries)
    def addPhotoSeries(self, series):
        if self._preloadPixmap is None:
            self._preloadPixmap = ImageOperations.buildPreloadPixmap(
                Config.asQSize("allSeriesView", "pixmapSize", QSize(320, 240)))

        row = self.numberOfSeries()
        self.seriesUuidToRow[series.uuid] = row

        for col in range(len(series.photoItems)):
            try:
                photoItemWidget = PhotoItemWidget(series[col], preloadPixmap=self._preloadPixmap)
            except CannotReadImageException as e:
                print("TODO: cannot load image exception " + str(e))
            else:
                photoItemWidget.openInSeries.connect(self.openInSeries)
                self._grid.addWidget(photoItemWidget, row, col)

                series[col].loadPhoto(Config.asQSize("allSeriesView", "pixmapSize", QSize(320, 240)),
                                      photoItemWidget.setImagePixmap)

    def _setupUi(self):
        self.navigationBar = NavigationBar(NavigationCapability.NONE)

        self._grid = QGridLayout()
        self._grid.setContentsMargins(0, 0, 0, 0)
        self._grid.setHorizontalSpacing(Config.get_or("allSeriesView", "photosSpacing", 3))
        self._grid.setVerticalSpacing(Config.get_or("allSeriesView", "seriesSpacing", 15))

        scrollLayout = QVBoxLayout()
        scrollLayout.setContentsMargins(0, 0, 0, 0)
        scrollLayout.addLayout(self._grid)
        scrollLayout.addStretch()

        scrollWidget = QWidget()
        scrollWidget.setLayout(scrollLayout)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setWidget(scrollWidget)

        hlayout = QVBoxLayout()
        hlayout.addWidget(self.navigationBar)
        hlayout.addWidget(scroll)

        self.setLayout(hlayout)
