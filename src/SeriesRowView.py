#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, pyqtSignal, pyqtSlot, QEvent
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QScrollArea, QFrame, QSizePolicy
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar
from Exceptions import CannotReadImageException


def _clearLayout(layout):
    while True:
        item = layout.itemAt(0)
        if item is None:
            break

        wgt = item.widget()
        if wgt:
            del wgt

        lt = item.layout()
        if lt:
            _clearLayout(lt)

        del item


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
    def __init__(self, parent=None):
        super(SeriesRowView, self).__init__(parent)

        self.navigationBar = NavigationBar()

        self.scroll = HorizontalImageScrollArea()
        self.scroll.layout.setContentsMargins(0, 0, 0, 0)

        layout = QVBoxLayout()
        layout.addWidget(self.navigationBar)
        layout.addWidget(self.scroll)

        self.setLayout(layout)

    def showSeries(self, series):
        self.clear()
        size = QSize(1920, 1080)

        for photoItem in series.photoItems:
            try:
                widget = PhotoItemWidget(photoItem, size)
            except CannotReadImageException as e:
                print("TODO: cannot load image exception " + str(e))
            else:
                self.scroll.layout.addWidget(widget)

    def clear(self):
        _clearLayout(self.scroll.layout)
