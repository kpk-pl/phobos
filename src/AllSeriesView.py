#!/usr/bin/python3

from PyQt5.QtCore import Qt, QObject, QUuid, QSize, QEvent, pyqtSignal, pyqtSlot
from PyQt5.QtWidgets import QWidget, QGridLayout, QVBoxLayout, QScrollArea, QFrame
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar, NavigationCapability
from Exceptions import CannotReadImageException
from PhotoContainers import PhotoSeries
import ImageOperations
import Config
import Utils


class ArrowFilter(QObject):
    def __init__(self, parent=None):
        super(ArrowFilter, self).__init__(parent)

    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyPress:
            if event.key() in [Qt.Key_Down, Qt.Key_Up, Qt.Key_Left, Qt.Key_Right]:
                event.ignore()
                return True
        return super(ArrowFilter, self).eventFilter(obj, event)


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

    def eventFilter(self, obj, event):
        print(event.type())
        return False

    def keyPressEvent(self, event):
        if event.type() == QEvent.KeyPress and event.key() in [Qt.Key_Left, Qt.Key_Right, Qt.Key_Up, Qt.Key_Down]:
            focusCoord = self._getFocusGridCoordinates()
            if focusCoord is not None:
                self._focusNextInGrid(focusCoord, event.key())
            elif self._grid.count() > 0:
                self._grid.itemAt(0).widget().setFocus()

        super(AllSeriesView, self).keyPressEvent(event)

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

    def _getFocusGridCoordinates(self):
        focusItem = Utils.focusedPhotoItem()
        if focusItem is None:
            return None

        focusSeries = focusItem.photoItem.seriesUuid
        assert focusSeries in self.seriesUuidToRow, "Found item from unregistered series"

        focusRow = self.seriesUuidToRow[focusSeries]
        for i in range(self._grid.columnCount()):
            wgt = self._grid.itemAtPosition(focusRow, i)
            if wgt is None or wgt.widget() is None:
                continue
            if wgt.widget() == focusItem:
                return focusRow, i

        assert False, "Item from existing series not found in grid. ALl items should be showed."

    def _focusNextInGrid(self, currentCoord, keyDirection):
        row, col = currentCoord
        maxRow = self._grid.rowCount()-1
        if keyDirection == Qt.Key_Right:
            proposals = [(row, col+1), (row+1, 0), (0, 0)]
        elif keyDirection == Qt.Key_Down:
            proposals = [(row+1, col), (row+1, None), (0, col), (0, None)]
        elif keyDirection == Qt.Key_Left:
            proposals = [(row, col-1), (row-1, None), (maxRow, None)]
        elif keyDirection == Qt.Key_Up:
            proposals = [(row-1, col), (row-1, None), (maxRow, col), (maxRow, None)]

        target = self._findValidProposal(proposals)
        self._grid.itemAtPosition(*target).widget().setFocus()

    def _findValidProposal(self, proposals):
        assert self._grid.count() > 0, "Should never be called on empty grid"

        for row, col in proposals:
            if row < 0 or row >= self._grid.rowCount() or (col is not None and col < 0):
                continue
            if col is None:
                for i in range(self._grid.columnCount()+1):
                    if self._grid.itemAtPosition(row, i) is None:
                        if i != 0:
                            return row, i-1
                        break
            else:
                if self._grid.itemAtPosition(row, col) is not None:
                    return row, col

        # Should never happen if proposals are correctly constructed.
        # On each proposal list should be at least one (0,0) or (0, None). When grid has at least one
        # correctly positioned element at (0,0) it should be found.
        assert False, "No valid proposals found"

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
        scroll.installEventFilter(ArrowFilter(scroll))
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setWidget(scrollWidget)

        hlayout = QVBoxLayout()
        hlayout.addWidget(self.navigationBar)
        hlayout.addWidget(scroll)

        self.setLayout(hlayout)
