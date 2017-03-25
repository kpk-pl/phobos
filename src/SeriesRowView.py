#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, pyqtSignal, pyqtSlot, QEvent
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QScrollArea, QFrame
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar, NavigationCapability
from Exceptions import CannotReadImageException


def _clearLayout(layout):
    while True:
        item = layout.takeAt(0)
        if item is None:
            break

        wgt = item.widget()
        if wgt:
            del wgt

        lt = item.layout()
        if lt:
            _clearLayout(lt)

        del item


def _buildPreloadPixmap():
    imagePixmap = QtGui.QPixmap(QSize(320, 240))
    imagePixmap.fill(QtGui.QColor(Qt.lightGray))
    return imagePixmap

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
    returnFromView = pyqtSignal()

    def __init__(self, parent=None):
        super(SeriesRowView, self).__init__(parent)

        self.navigationBar = NavigationBar(NavigationCapability.BACK_TO_SERIES)

        self.scroll = HorizontalImageScrollArea()
        self.scroll.layout.setContentsMargins(0, 0, 0, 0)

        layout = QVBoxLayout()
        layout.addWidget(self.navigationBar)
        layout.addWidget(self.scroll)

        self.setLayout(layout)

        self._connectSignals()

    def showSeries(self, series, pixmaps=[]):
        self.clear()

        if len(pixmaps) < len(series):
            preload = _buildPreloadPixmap()
            pixmaps.extend([preload for _ in range(len(series)-len(pixmaps))])

        maxSize = QSize(1920, 1080)

        for photoItem, preload in zip(series, pixmaps):
            try:
                widget = PhotoItemWidget(photoItem, maxSize=maxSize, preloadPixmap=preload)
            except CannotReadImageException as e:
                print("TODO: cannot load image exception " + str(e))
            else:
                self.scroll.layout.addWidget(widget)

    def clear(self):
        _clearLayout(self.scroll.layout)

    @pyqtSlot()
    def backToSeries(self):
        self.clear()
        self.returnFromView.emit()

    def _connectSignals(self):
        self.navigationBar.backToSeries.clicked.connect(self.backToSeries)
