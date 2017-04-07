#!/usr/bin/python3

from enum import IntEnum
from PyQt5.QtCore import Qt, QSize
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QSlider
import Config
import ImageProcessing


class IconButton(QPushButton):
    BUTTON_ICON_SIZE = Config.asQSize("navigationBar", "buttonSize", QSize(40, 40))
    CONTENT_MARGIN = Config.get_or("navigationBar", "buttonMargin", 5)

    def __init__(self, icon, parent=None):
        super(IconButton, self).__init__(icon, "", parent)
        self.setIconSize(self.BUTTON_ICON_SIZE)
        self.setContentsMargins(self.CONTENT_MARGIN, self.CONTENT_MARGIN, self.CONTENT_MARGIN, self.CONTENT_MARGIN)

    def hasHeightForWidth(self):
        return True

    def heightForWidth(self, width):
        return width

    def sizeHint(self):
        iconSize = self.BUTTON_ICON_SIZE
        return QSize(iconSize.width() + 2*self.CONTENT_MARGIN, iconSize.height() + 2*self.CONTENT_MARGIN)


class NavigationCapability(IntEnum):
    NONE = 0
    BACK_TO_SERIES = 1
    NUM_SERIES = 2
    ONE_SERIES = 4
    PREV = 8
    NEXT = 16
    SLIDER = 32


class NavigationBar(QWidget):
    def __init__(self, capabilities, parent=None):
        super(NavigationBar, self).__init__(parent)

        self.showAllSeries = None
        self.showOneSeries = None
        self.showNumSeries = None
        self.slider = None
        self.prev = None
        self.next = None

        self._configTable = Config.Table("navigationBar")

        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(self._configTable.get_or("spacing", 2))

        if capabilities & NavigationCapability.BACK_TO_SERIES:
            iconPixmap = ImageProcessing.coloredPixmap(self._configTable.get("allSeriesIcon"), QSize(64, 64), color=Config.asQColor(self._configTable, "iconColor", Qt.black))
            self.showAllSeries = IconButton(QtGui.QIcon(iconPixmap))
            layout.addWidget(self.showAllSeries)

        if capabilities & NavigationCapability.NUM_SERIES:
            iconPixmap = ImageProcessing.coloredPixmap(self._configTable.get("numSeriesIcon"), QSize(64, 64), color=Config.asQColor(self._configTable, "iconColor", Qt.black))
            self.showNumSeries = IconButton(QtGui.QIcon(iconPixmap))
            layout.addWidget(self.showNumSeries)

        if capabilities & NavigationCapability.ONE_SERIES:
            iconPixmap = ImageProcessing.coloredPixmap(self._configTable.get("oneSeriesIcon"), QSize(64, 64), color=Config.asQColor(self._configTable, "iconColor", Qt.black))
            self.showOneSeries = IconButton(QtGui.QIcon(iconPixmap))
            layout.addWidget(self.showOneSeries)

        if capabilities & NavigationCapability.SLIDER:
            self.slider = QSlider(Qt.Horizontal)
            self.slider.setMaximum(100)
            self.slider.setValue(100)

            spacingLeft = QWidget()
            spacingLeft.setMinimumWidth(15)
            layout.addWidget(spacingLeft)
            layout.addWidget(self.slider)

            spacingRight = QWidget()
            spacingRight.setMinimumWidth(15)
            layout.addWidget(spacingRight)
        else:
            layout.addStretch()

        if capabilities & NavigationCapability.PREV:
            self.prev = IconButton(QtGui.QIcon(Config.get("navigationBar", "prevIcon")))
            layout.addWidget(self.prev)

        if capabilities & NavigationCapability.NEXT:
            self.next = IconButton(QtGui.QIcon(Config.get("navigationBar", "nextIcon")))
            layout.addWidget(self.next)

        self.setLayout(layout)
