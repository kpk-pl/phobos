#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint, QUuid, QEvent, QRectF, pyqtSlot, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QMenu
from ImageWidget import ImageWidget
from PhotoItem import PhotoItemState
import Config


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
    BORDER_WIDTH = Config.get("photoItemWidget.border", "width")
    BORDER_COLORS_IN_STATES = {PhotoItemState.UNKNOWN: Config.asQColor("photoItemWidget.border", "colorUnknown"),
                               PhotoItemState.SELECTED: Config.asQColor("photoItemWidget.border", "colorSelected"),
                               PhotoItemState.DISCARDED: Config.asQColor("photoItemWidget.border", "colorDiscarded")}

    FOCUS_ICON_PADDING = Config.get("photoItemWidget.focusIcon", "padding")
    FOCUS_ICON_OPACITY = Config.get("photoItemWidget.focusIcon", "opacity")
    FOCUS_ICON_COLOR = Config.asQColor("photoItemWidget.focusIcon", "color")
    FOCUS_ICON_SIZE_PERCENT = Config.get("photoItemWidget.focusIcon", "sizePercent")

    QUALITY_TEXT_FORMATSTR = "%." + str(Config.get("photoItemWidget.qualityText", "decimalPlaces")) + "f%%"

    openInSeries = pyqtSignal(QUuid)

    def __init__(self, photoItem, preloadPixmap=None, parent=None):
        super(PhotoItemWidget, self).__init__(preloadPixmap, parent=parent)

        self.setFocusPolicy(Qt.StrongFocus)
        self.installEventFilter(self)

        self.photoItem = photoItem

        self._connectSignals()

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

    def focusOutEvent(self, event):
        self.update()

    def eventFilter(self, obj, event):
        if obj == self and event.type() == QEvent.FocusIn:
            if event.reason() == Qt.PopupFocusReason or event.reason() == Qt.ActiveWindowFocusReason:
                return True
        return False

    def keyPressEvent(self, event):
        if event.key() in [Qt.Key_Return, Qt.Key_Enter]:
            event.accept()
            self.photoItem.toggleSelection()

    def _renderedPixmap(self):
        size = self.size()
        availableSize = QSize(size.width() - 2*self.BORDER_WIDTH, size.height() - 2*self.BORDER_WIDTH)
        scaledImage = self.scaledPixmap(availableSize)

        pixmapSize = QSize(scaledImage.size().width() + 2*self.BORDER_WIDTH,
                           scaledImage.size().height() + 2*self.BORDER_WIDTH)
        pixmap = QtGui.QPixmap(pixmapSize)
        pixmap.fill(self.BORDER_COLORS_IN_STATES[self.photoItem.state])
        painter = QtGui.QPainter(pixmap)

        painter.drawPixmap((pixmap.width() - scaledImage.width()) / 2,
                           (pixmap.height() - scaledImage.height()) / 2,
                           scaledImage)

        if self.hasFocus():
            self._paintFocusMark(painter, availableSize, pixmap.size())

        quality, best = self._getPhotoItemQuality()
        if quality is not None:
            self._paintQualityText(painter, pixmap.size(), quality)

        if best:
            self._paintBestMark(painter, availableSize)

        painter.end()
        return pixmap

    def _paintQualityText(self, painter, pixmapSize, quality):
        painter.save()

        painter.setOpacity(Config.get("photoItemWidget.qualityText", "opacity"))
        painter.setPen(Config.asQColor("photoItemWidget.qualityText", "color"))
        painter.setFont(Config.asQFont("photoItemWidget.qualityText", "font"))

        padding = Config.get("photoItemWidget.qualityText", "padding")
        textRect = QRectF(self.BORDER_WIDTH + padding,
                          pixmapSize.height() - self.BORDER_WIDTH - padding - painter.font().pointSize(),
                          1, 1)

        painter.drawText(textRect, Qt.AlignLeft | Qt.TextSingleLine | Qt.TextDontClip,
                         self.QUALITY_TEXT_FORMATSTR % (quality*100))

        painter.restore()

    def _paintFocusMark(self, painter, availableSize, pixmapSize):
        focusIcon = QtGui.QIcon(Config.get("photoItemWidget.focusIcon", "path"))
        iconSize = QSize(availableSize.width() * self.FOCUS_ICON_SIZE_PERCENT,
                         availableSize.height() * self.FOCUS_ICON_SIZE_PERCENT)
        focusPixmap = _getColorIcon(focusIcon, iconSize, self.FOCUS_ICON_COLOR, self.FOCUS_ICON_OPACITY)

        painter.drawPixmap(pixmapSize.width() - self.BORDER_WIDTH - self.FOCUS_ICON_PADDING - focusPixmap.width(),
                           self.BORDER_WIDTH + self.FOCUS_ICON_PADDING,
                           focusPixmap)

    def _paintBestMark(self, painter, availableSize):
        icon = QtGui.QIcon(Config.get("photoItemWidget.bestMarkIcon", "path"))
        prcSize = Config.get("photoItemWidget.bestMarkIcon", "sizePercent")
        padding = Config.get("photoItemWidget.bestMarkIcon", "padding")
        iconSize = QSize(availableSize.width() * prcSize,
                         availableSize.height() * prcSize)
        bestMarkPixmap = _getColorIcon(icon, iconSize,
                                       Config.asQColor("photoItemWidget.bestMarkIcon", "color"),
                                       Config.get("photoItemWidget.bestMarkIcon", "opacity"))

        painter.drawPixmap(self.BORDER_WIDTH + padding, self.BORDER_WIDTH + padding, bestMarkPixmap)

    def _getPhotoItemQuality(self):
        metrics = self.photoItem.metrics
        if metrics is None or metrics.seriesAggregated is None:
            return None, False
        return metrics.seriesAggregated.quality(), metrics.seriesAggregated.bestQuality

    @pyqtSlot()
    def temp(self):
        self.update()

    def _connectSignals(self):
        self.clicked.connect(self.photoItem.toggleSelection)
        self.photoItem.stateChanged.connect(self.temp)
