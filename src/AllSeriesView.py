#!/usr/bin/python3

from PyQt5.QtCore import Qt, QUuid, QSize, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QWidget, QGridLayout, QVBoxLayout, QScrollArea, QFrame
from PhotoItemWidget import PhotoItemWidget
from NavigationBar import NavigationBar, NavigationCapability
from Exceptions import CannotReadImageException


def _buildPreloadPixmap():
    imagePixmap = QtGui.QPixmap(QSize(320, 240))
    imagePixmap.fill(QtGui.QColor(Qt.lightGray))
    return imagePixmap


class AllSeriesView(QWidget):
    SPACING_BETWEEN_SERIES = 15
    SPACING_BETWEEN_PHOTOS = 3
    PHOTOITEM_PIXMAP_SIZE = QSize(320, 240)

    openInSeries = pyqtSignal(QUuid)

    def __init__(self):
        super(AllSeriesView, self).__init__()

        self.navigationBar = NavigationBar(NavigationCapability.NONE)

        self._grid = QGridLayout()
        self._grid.setContentsMargins(0, 0, 0, 0)
        self._grid.setHorizontalSpacing(self.SPACING_BETWEEN_PHOTOS)
        self._grid.setVerticalSpacing(self.SPACING_BETWEEN_SERIES)

        scrollLayout = QVBoxLayout()
        scrollLayout.setContentsMargins(0, 0, 0, 0)
        scrollLayout.addLayout(self._grid)
        scrollLayout.addStretch()

        scrollWidget = QWidget()
        scrollWidget.setLayout(scrollLayout)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setWidget(scrollWidget)

        hlayout = QVBoxLayout()
        hlayout.addWidget(self.navigationBar)
        hlayout.addWidget(scroll)

        self.setLayout(hlayout)

    def addPhotoSeries(self, series):
        preload = _buildPreloadPixmap()
        for s in series:
            row = self._grid.rowCount()
            for col in range(len(s.photoItems)):
                try:
                    photoItemWidget = PhotoItemWidget(s.photoItems[col],
                                                      maxSize=self.PHOTOITEM_PIXMAP_SIZE,
                                                      preloadPixmap=preload)
                except CannotReadImageException as e:
                    print("TODO: cannot load image exception " + str(e))
                else:
                    photoItemWidget.openInSeries.connect(self.openInSeries)
                    self._grid.addWidget(photoItemWidget, row, col)

