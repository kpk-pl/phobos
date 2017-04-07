#!/usr/bin/python3

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QHBoxLayout, QVBoxLayout
from NavigationBar import NavigationBar, NavigationCapability
from SeriesViewBase import SeriesViewBase
import Config


def _clearLayout(layout):
    while True:
        item = layout.takeAt(0)
        if item is None:
            break

        wgt = item.widget()
        if wgt:
            wgt.setParent(None)

        lt = item.layout()
        if lt:
            _clearLayout(lt)


class SeriesNumView(SeriesViewBase):
    def __init__(self, parent=None):
        super(SeriesNumView, self).__init__(parent)
        self.navigationBar = NavigationBar(NavigationCapability.BACK_TO_SERIES | NavigationCapability.ONE_SERIES |
                                           NavigationCapability.PREV | NavigationCapability.NEXT)
        self.visibleItems = Config.get_or("seriesView.num", "visibleItems", 2)
        self.photoItems = []
        self.currentItem = 0

        self.layout = QHBoxLayout()

        vlayout = QVBoxLayout()
        vlayout.addWidget(self.navigationBar)
        vlayout.addLayout(self.layout)
        vlayout.addStretch()

        self.setLayout(vlayout)
        self._connectSignals()

    def showSeries(self, series):
        super(SeriesNumView, self).showSeries(series)
        self.layout.itemAt(0).widget().setFocus()

    def clear(self):
        _clearLayout(self.layout)
        self.currentItem = 0
        self.photoItems = []
        self.update()

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Left:
            self._showPrevItem()
        if event.key() == Qt.Key_Right:
            self._showNextItem()
        super(SeriesNumView, self).keyPressEvent(event)

    def _showPrevItem(self):
        if self.currentItem > 0:
            self.currentItem -= 1
            self._setCurrentView()

    def _showNextItem(self):
        if self.currentItem < len(self.photoItems)-1:
            self.currentItem += 1
            self._setCurrentView()

    def _setCurrentView(self):
        _clearLayout(self.layout)

        startShow = max(self.currentItem - int((self.visibleItems - 1)/2), 0)
        endShow = min(startShow + self.visibleItems, len(self.photoItems))
        if endShow - startShow < self.visibleItems:
            startShow = max(endShow - self.visibleItems, 0)

        for i in range(startShow, endShow):
            self.layout.addWidget(self.photoItems[i])

        self.layout.itemAt(self.currentItem - startShow).widget().setFocus()

    def _addPhotoItemToLayout(self, photoItem):
        self.photoItems.append(photoItem)
        if self.layout.count() < self.visibleItems:
            self.layout.addWidget(photoItem)

    def _connectSignals(self):
        self.navigationBar.showAllSeries.clicked.connect(lambda: self.switchView.emit("showAllSeries"))
        self.navigationBar.showOneSeries.clicked.connect(lambda: self.switchView.emit("showOneSeries"))
        self.navigationBar.prev.clicked.connect(self.prevSeries)
        self.navigationBar.next.clicked.connect(self.nextSeries)
