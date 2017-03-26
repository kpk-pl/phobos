#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint, QUuid, QEvent, pyqtSlot, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QMenu
from ImageWidget import ImageWidget
from PhotoItem import PhotoItemState


def _getColorIcon(icon, size, color, opacity=1.0):
    pixmap = icon.pixmap(size)

    painter = QtGui.QPainter(pixmap)
    painter.setOpacity(opacity)
    painter.setRenderHint(QtGui.QPainter.Antialiasing)
    painter.setCompositionMode(QtGui.QPainter.CompositionMode_SourceIn)
    painter.setBrush(color)
    painter.setPen(color)

    painter.drawRect(pixmap.rect())
    painter.end()

    return pixmap


class PhotoItemWidget(ImageWidget):
    BORDER_COLOR_UNKNOWN = QtGui.QColor(Qt.darkGray)
    BORDER_COLOR_SELECTED = QtGui.QColor(Qt.green)
    BORDER_COLOR_DISCARDED = QtGui.QColor(Qt.red)
    BORDER_WIDTH = 2
    FOCUS_ICON_PADDING = 7
    FOCUS_ICON_OPACITY = 0.5
    FOCUS_ICON_COLOR = QtGui.QColor("orange")
    FOCUS_ICON_SIZE_PERCENT = 0.15

    openInSeries = pyqtSignal(QUuid)

    def __init__(self, photoItem, maxSize=None, preloadPixmap=None, parent=None):
        super(PhotoItemWidget, self).__init__(photoItem.fileName, maxSize=maxSize, preloadPixmap=preloadPixmap, parent=parent)

        self.setFocusPolicy(Qt.StrongFocus)
        self.installEventFilter(self)

        self.photoItem = photoItem
        self._borderWidth = self.BORDER_WIDTH
        self._updateBorder(self.photoItem.state)

        self._connectSignals()

    @pyqtSlot(PhotoItemState)
    def photoItemSelectionChanged(self, state):
        self._updateBorder(state)
        self.update()

    def contextMenuEvent(self, event):
        menu = QMenu()

        toggleAction = menu.addAction("Discard") if self.photoItem.isSelected() else menu.addAction("Select")
        toggleAction.triggered.connect(self.photoItem.toggleSelection)

        menu.addSeparator()

        viewSeries = menu.addAction("View series")
        viewSeries.triggered.connect(lambda: self.openInSeries.emit(self.photoItem.seriesUuid))

        menu.exec_(self.mapToGlobal(QPoint(event.x(), event.y())))

    def focusInEvent(self, event):
        self.update()
        print("FOCUS IN:  " + str(self.photoItem.fileName))

    def focusOutEvent(self, event):
        self.update()
        print("FOCUS OUT: " + str(self.photoItem.fileName))

    def eventFilter(self, obj, event):
        if obj == self and event.type() == QEvent.FocusIn:
            if event.reason() == Qt.PopupFocusReason or event.reason() == Qt.ActiveWindowFocusReason:
                return True
        return False

    def _updateBorder(self, state):
        if state == PhotoItemState.SELECTED:
            self._borderColor = self.BORDER_COLOR_SELECTED
        elif state == PhotoItemState.DISCARDED:
            self._borderColor = self.BORDER_COLOR_DISCARDED
        else:
            self._borderColor = self.BORDER_COLOR_UNKNOWN

    def _renderedPixmap(self):
        size = self.size()
        availableSize = QSize(size.width() - 2*self._borderWidth, size.height() - 2*self._borderWidth)
        scaledImage = self.scaledPixmap(availableSize)

        pixmapSize = QSize(scaledImage.size().width() + 2*self._borderWidth,
                           scaledImage.size().height() + 2*self._borderWidth)
        pixmap = QtGui.QPixmap(pixmapSize)
        pixmap.fill(self._borderColor)
        painter = QtGui.QPainter(pixmap)

        painter.drawPixmap((pixmap.width() - scaledImage.width()) / 2,
                           (pixmap.height() - scaledImage.height()) / 2,
                           scaledImage)

        if self.hasFocus():
            focusIcon = QtGui.QIcon("../icon/focus.png")
            iconSize = QSize(availableSize.width()*self.FOCUS_ICON_SIZE_PERCENT,
                             availableSize.height()*self.FOCUS_ICON_SIZE_PERCENT)
            focusPixmap = _getColorIcon(focusIcon, iconSize, self.FOCUS_ICON_COLOR, self.FOCUS_ICON_OPACITY)

            painter.drawPixmap(pixmap.width() - self._borderWidth - self.FOCUS_ICON_PADDING - focusPixmap.width(),
                               self._borderWidth + self.FOCUS_ICON_PADDING,
                               focusPixmap)

        painter.end()
        return pixmap

    def _connectSignals(self):
        self.clicked.connect(self.photoItem.toggleSelection)
        self.photoItem.selectionChanged.connect(self.photoItemSelectionChanged)
