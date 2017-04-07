#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, pyqtSlot, QEvent
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QScrollArea, QFrame
from NavigationBar import NavigationBar, NavigationCapability
from SeriesViewBase import SeriesViewBase


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


class HorizontalImageScrollArea(QScrollArea):
    def __init__(self, parent=None):
        super(HorizontalImageScrollArea, self).__init__(parent)
        self.setWidgetResizable(True)
        self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.setFrameShape(QFrame.NoFrame)

        self.layout = QHBoxLayout()

        scroll = QWidget()
        scroll.setLayout(self.layout)
        self.setWidget(scroll)

    def eventFilter(self, obj, event):
        if obj == self.widget() and event.type() == QEvent.Resize:
            self.widget().resize(self.calcNewSize())
            return True

        return super(HorizontalImageScrollArea, self).eventFilter(obj, event)

    def calcNewSize(self):
        height = self.viewport().height()

        layoutMargins = self.layout.contentsMargins()
        heightForCalc = height - layoutMargins.top() - layoutMargins.bottom()

        width = self.calcWidthForHeight(heightForCalc)
        return QSize(width, height)

    def calcWidthForHeight(self, height):
        width = 0
        for wgt in range(self.layout.count()):
            width += self.layout.itemAt(wgt).widget().widthForHeight(height)

        if self.layout.count() > 1:
            width += self.layout.spacing() * (self.layout.count()-1)
        return width


class SeriesRowView(SeriesViewBase):
    def __init__(self, parent=None):
        super(SeriesRowView, self).__init__(parent)
        self.navigationBar = NavigationBar(NavigationCapability.BACK_TO_SERIES | NavigationCapability.SLIDER |
                                           NavigationCapability.NUM_SERIES |
                                           NavigationCapability.PREV | NavigationCapability.NEXT)

        self.scroll = HorizontalImageScrollArea()
        self.scroll.layout.setContentsMargins(0, 0, 0, 0)

        layout = QVBoxLayout()
        layout.addWidget(self.navigationBar)
        layout.addWidget(self.scroll, 100)
        layout.addStretch(0)

        self.setLayout(layout)
        self._connectSignals()

    def showSeries(self, series):
        super(SeriesRowView, self).showSeries(series)
        self.scroll.layout.itemAt(0).widget().setFocus()

    def clear(self):
        _clearLayout(self.scroll.layout)
        self.update()
        self.scroll.horizontalScrollBar().setValue(0)

    def _addPhotoItemToLayout(self, photoItem):
        self.scroll.layout.addWidget(photoItem)

    @pyqtSlot(int)
    def _resizeImages(self, percent):
        self.layout().setStretch(1, percent)
        self.layout().setStretch(2, 100-percent)

    def _connectSignals(self):
        self.navigationBar.showAllSeries.clicked.connect(lambda: self.switchView.emit("showAllSeries"))
        self.navigationBar.showNumSeries.clicked.connect(lambda: self.switchView.emit("showNumSeries"))
        self.navigationBar.slider.valueChanged.connect(self._resizeImages)
        self.navigationBar.prev.clicked.connect(self.prevSeries)
        self.navigationBar.next.clicked.connect(self.nextSeries)
