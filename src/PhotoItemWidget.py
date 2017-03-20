#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint, QUuid, pyqtSlot, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QMenu
from ImageWidget import ImageWidget


class PhotoItemWidget(ImageWidget):
    BORDER_COLOR_UNKNOWN = QtGui.QColor(Qt.darkGray)
    BORDER_COLOR_SELECTED = QtGui.QColor(Qt.green)
    BORDER_COLOR_DISCARDED = QtGui.QColor(Qt.red)
    BORDER_WIDTH = 2

    openInSeries = pyqtSignal(QUuid)

    def __init__(self, fileName, seriesUuid, parent=None):
        super(PhotoItemWidget, self).__init__(fileName, parent)

        self.seriesUuid = seriesUuid

        self._borderWidth = self.BORDER_WIDTH
        self._borderColor = self.BORDER_COLOR_UNKNOWN
        self._selected = None

        self._connectSignals()

    @pyqtSlot()
    def select(self):
        self._selected = True
        self._borderColor = self.BORDER_COLOR_SELECTED
        self.repaint()

    @pyqtSlot()
    def discard(self):
        self._selected = False
        self._borderColor = self.BORDER_COLOR_DISCARDED
        self.repaint()

    @pyqtSlot()
    def toggleSelection(self):
        self.discard() if self.isSelected() else self.select()

    def isSelected(self):
        if self._selected:
            return True
        return False

    def contextMenuEvent(self, event):
        menu = QMenu()

        toggleAction = menu.addAction("Discard") if self.isSelected() else menu.addAction("Select")
        toggleAction.triggered.connect(self.toggleSelection)

        menu.addSeparator()

        viewSeries = menu.addAction("View series")
        viewSeries.triggered.connect(lambda: self.openInSeries.emit(self.seriesUuid))

        menu.exec_(self.mapToGlobal(QPoint(event.x(), event.y())))

    def _renderedPixmap(self):
        size = self.size()
        availableSize = QSize(size.width() - 2*self._borderWidth, size.height() - 2*self._borderWidth)
        scaledImage = self.scaledPixmap(availableSize)

        pixmapSize = QSize(scaledImage.size().width() + 2*self._borderWidth,
                           scaledImage.size().height() + 2*self._borderWidth)
        pixmap = QtGui.QPixmap(pixmapSize)
        pixmap.fill(self._borderColor)
        painter = QtGui.QPainter(pixmap)

        centerPoint = QPoint((pixmap.width() - scaledImage.width()) / 2,
                             (pixmap.height() - scaledImage.height()) / 2)
        painter.drawPixmap(centerPoint, scaledImage)

        painter.end()
        return pixmap

    def _connectSignals(self):
        self.clicked.connect(self.toggleSelection)


