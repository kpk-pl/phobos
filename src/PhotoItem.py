#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint, pyqtSlot
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QMenu
from ImageWidget import ImageWidget
from Exceptions import CannotReadImageException


class PhotoItem(ImageWidget):
    BORDER_COLOR_UNKNOWN = QtGui.QColor(Qt.darkGray)
    BORDER_COLOR_SELECTED = QtGui.QColor(Qt.green)
    BORDER_COLOR_DISCARDED = QtGui.QColor(Qt.red)

    def __init__(self, fileName, parent=None):
        super(PhotoItem, self).__init__(PhotoItem._readImageFromFile(fileName), parent)

        self.fileName = fileName
        self._borderWidth = 2
        self._borderColor = PhotoItem.BORDER_COLOR_UNKNOWN
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
        self.action = menu.addAction("Discard") if self.isSelected() else menu.addAction("Select")
        self.action.triggered.connect(self.toggleSelection)
        menu.exec_(self.mapToGlobal(QPoint(event.x(), event.y())))

    def _renderedPixmap(self):
        size = self.size()
        availableSize = QSize(size.width() - 2*self._borderWidth, size.height() - 2*self._borderWidth)
        scaledImage = self.imagePixmap.scaled(availableSize, Qt.KeepAspectRatio, Qt.SmoothTransformation)

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

    @staticmethod
    def _readImageFromFile(fileName):
        reader = QtGui.QImageReader(fileName)
        reader.setAutoTransform(True)
        reader.setAutoDetectImageFormat(True)

        image = reader.read()
        if not image:
            raise CannotReadImageException()

        return image
