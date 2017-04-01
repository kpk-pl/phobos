#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, pyqtSignal, pyqtSlot, QEvent
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QScrollArea, QFrame
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar, NavigationCapability
from Exceptions import CannotReadImageException
import ImageOperations
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


class SeriesRowView(QWidget):
    MAX_IMAGE_SIZE = Config.asQSize('seriesRowView', 'maxPixmapSize')

    returnFromView = pyqtSignal()
    nextSeries = pyqtSignal()
    prevSeries = pyqtSignal()

    def __init__(self, parent=None):
        super(SeriesRowView, self).__init__(parent)
        self.navigationBar = NavigationBar(NavigationCapability.BACK_TO_SERIES | NavigationCapability.SLIDER |
                                           NavigationCapability.PREV | NavigationCapability.NEXT)

        self.scroll = HorizontalImageScrollArea()
        self.scroll.layout.setContentsMargins(0, 0, 0, 0)

        layout = QVBoxLayout()
        layout.addWidget(self.navigationBar)
        layout.addWidget(self.scroll, 100)
        layout.addStretch(0)

        self.setLayout(layout)
        self._connectSignals()

        self._preloadPixmap = None

    def _getPreloadPixmap(self):
        if self._preloadPixmap is None:
            self._preloadPixmap = ImageOperations.buildPreloadPixmap(self.MAX_IMAGE_SIZE)
        return self._preloadPixmap

    def showSeries(self, series):
        self.clear()

        for photoItem in series:
            try:
                preload = photoItem.pixmap if photoItem.pixmap is not None else self._getPreloadPixmap()
                widget = PhotoItemWidget(photoItem, preloadPixmap=preload)
            except CannotReadImageException as e:
                print("TODO: cannot load image exception " + str(e))
            else:
                self.scroll.layout.addWidget(widget)
                photoItem.loadPhoto(self.MAX_IMAGE_SIZE, widget.setImagePixmap)

        self.scroll.layout.itemAt(0).widget().setFocus()

    def clear(self):
        _clearLayout(self.scroll.layout)
        self.update()
        self.scroll.horizontalScrollBar().setValue(0)

    @pyqtSlot()
    def backToSeries(self):
        self.clear()
        self.returnFromView.emit()

    @pyqtSlot(int)
    def _resizeImages(self, percent):
        self.layout().setStretch(1, percent)
        self.layout().setStretch(2, 100-percent)

    def _connectSignals(self):
        self.navigationBar.backToSeries.clicked.connect(self.backToSeries)
        self.navigationBar.slider.valueChanged.connect(self._resizeImages)
        self.navigationBar.prev.clicked.connect(self.prevSeries)
        self.navigationBar.next.clicked.connect(self.nextSeries)
