#!/usr/bin/python3

from enum import IntFlag
from PyQt5.QtCore import Qt, QSize
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton


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


class NavigationBar(QWidget):
    SPACING_BETWEEN_BUTTONS = 2

    def __init__(self, capabilities, parent=None):
        super(NavigationBar, self).__init__(parent)

        #self.next = IconButton(QtGui.QIcon("../icon/next.png"))
        #self.prev = IconButton(QtGui.QIcon("../icon/prev.png"))
        #self.toggle = IconButton(QtGui.QIcon("../icon/check.png"))
        self.backToSeries = IconButton(QtGui.QIcon("../icon/series.png")) if (capabilities & NavigationCapability.BACK_TO_SERIES) else None


        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(self.SPACING_BETWEEN_BUTTONS)

        layout.addWidget(self.backToSeries)
        layout.addStretch()
        #layout.addWidget(self.prev)
        #layout.addWidget(self.toggle)
        #layout.addWidget(self.next)

        self.setLayout(layout)
