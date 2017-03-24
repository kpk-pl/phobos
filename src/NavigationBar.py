#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton, QSizePolicy


class IconButton(QPushButton):
    BUTTON_ICON_SIZE = QSize(40, 40)

    def __init__(self, icon, parent=None):
        super(IconButton, self).__init__(icon, "", parent)
        self.setIconSize(self.BUTTON_ICON_SIZE)


class NavigationBar(QWidget):
    SPACING_BETWEEN_BUTTONS = 2

    def __init__(self, parent=None):
        super(NavigationBar, self).__init__(parent)

        self.next = IconButton(QtGui.QIcon("../icon/right.png"))
        self.prev = IconButton(QtGui.QIcon("../icon/left.png"))
        self.toggle = IconButton(QtGui.QIcon("../icon/select.png"))
        self.toggle.setDisabled(True)

        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(self.SPACING_BETWEEN_BUTTONS)

        layout.addStretch()
        layout.addWidget(self.prev)
        layout.addWidget(self.toggle)
        layout.addWidget(self.next)

        self.setLayout(layout)
