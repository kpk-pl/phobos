#!/usr/bin/python3

from enum import IntFlag
from PyQt5.QtCore import Qt, QSize
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QSlider
import Config


class IconButton(QPushButton):
    BUTTON_ICON_SIZE = QSize(40, 40)
    CONTENT_MARGIN = 5

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


class NavigationCapability(IntFlag):
    NONE = 0
    BACK_TO_SERIES = 1
    PREV = 2
    NEXT = 4
    SLIDER = 8


class NavigationBar(QWidget):
    def __init__(self, capabilities, parent=None):
        super(NavigationBar, self).__init__(parent)

        self.next = IconButton(QtGui.QIcon(Config.get("navigationBar", "nextIcon"))) \
            if (capabilities & NavigationCapability.NEXT) else None

        self.prev = IconButton(QtGui.QIcon(Config.get("navigationBar", "prevIcon"))) \
            if (capabilities & NavigationCapability.PREV) else None

        #self.toggle = IconButton(QtGui.QIcon("../icon/check.png"))
        self.backToSeries = IconButton(QtGui.QIcon(Config.get("navigationBar", "backToSeriesIcon"))) \
            if (capabilities & NavigationCapability.BACK_TO_SERIES) else None

        self.slider = QSlider(Qt.Horizontal) if (capabilities & NavigationCapability.SLIDER) else None

        if self.slider:
            self.slider.setMaximum(100)
            self.slider.setValue(100)

        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(Config.get("navigationBar", "spacing"))

        layout.addWidget(self.backToSeries)

        if self.slider:
            spacingLeft = QWidget()
            spacingLeft.setMinimumWidth(15)
            layout.addWidget(spacingLeft)

            layout.addWidget(self.slider)

            spacingRight = QWidget()
            spacingRight.setMinimumWidth(15)
            layout.addWidget(spacingRight)
        else:
            layout.addStretch()

        layout.addWidget(self.prev)
        #layout.addWidget(self.toggle)
        layout.addWidget(self.next)

        self.setLayout(layout)
