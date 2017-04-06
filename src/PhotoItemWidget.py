#!/usr/bin/python3

from PyQt5.QtCore import Qt, QSize, QPoint, QUuid, QEvent, QRectF, pyqtSlot, pyqtSignal
import PyQt5.QtGui as QtGui
from PyQt5.QtWidgets import QMenu
from ImageWidget import ImageWidget
from PhotoItem import PhotoItemState
import Config
import Exceptions


def _getColorIcon(filePath, size, color, opacity):
    pixmap = QtGui.QIcon(filePath).pixmap(size)
    if pixmap.isNull():
        raise Exceptions.CannotLoadMediaException(filePath)

    painter = QtGui.QPainter(pixmap)
    painter.setOpacity(opacity)
    painter.setRenderHint(QtGui.QPainter.Antialiasing)
    painter.setCompositionMode(QtGui.QPainter.CompositionMode_SourceIn)
    painter.setBrush(color)
    painter.setPen(color)

    painter.drawRect(pixmap.rect())
    painter.end()

    return pixmap


def _getIconToPaint(configTable, availableSize):
    prcSize = configTable.get_or("sizePercent", 0.2)
    iconSize = QSize(availableSize.width() * prcSize, availableSize.height() * prcSize)
    color = QtGui.QColor(configTable.get_or("color", "black"))
    if not color.isValid():
        color = Qt.black
    opacity = configTable.get_or("opacity", 0.5)

    pixmap = _getColorIcon(configTable.get("path"), iconSize, color, opacity)
    return pixmap


def _isPower2(num):
    return ((num & (num - 1)) == 0) and num != 0


def _histogramIcon(hist):
    configTable = Config.Table("photoItemWidget.histogram")
    width = configTable.get_or("width", 32)
    width = width if _isPower2(width) else 32
    height = configTable.get_or("height", width)
    height = height if _isPower2(height) else width

    assert _isPower2(len(hist))
    mult = int(len(hist)/width)
    acch = [sum(hist[i*mult : (i+1)*mult]) for i in range(width)]
    maxh = max(acch)
    acch = [x/maxh for x in acch]

    pixmap = QtGui.QPixmap(width, height)
    fillColor = QtGui.QColor(configTable.get_or("fillColor", "transparent"))
    if not fillColor.isValid():
        fillColor = Qt.transparent
    pixmap.fill(fillColor)

    painter = QtGui.QPainter(pixmap)
    histColor = QtGui.QColor(configTable.get_or("color", "black"))
    if not histColor.isValid():
        histColor = Qt.black
    painter.setPen(histColor)
    painter.setOpacity(configTable.get_or("opacity", 1.0))

    h = pixmap.height()-1
    for i in range(len(acch)):
        painter.drawLine(i, h, i, (1-acch[i])*h)

    painter.end()
    return pixmap


class PhotoItemWidget(ImageWidget):
    BORDER_WIDTH = Config.get_or("photoItemWidget.border", "width", 2)
    BORDER_COLORS_IN_STATES = {PhotoItemState.UNKNOWN:
                                   Config.asQColor("photoItemWidget.border", "colorUnknown", Qt.lightGray),
                               PhotoItemState.SELECTED:
                                   Config.asQColor("photoItemWidget.border", "colorSelected", Qt.green),
                               PhotoItemState.DISCARDED:
                                   Config.asQColor("photoItemWidget.border", "colorDiscarded", Qt.red)}

    QUALITY_TEXT_FORMATSTR = "%." + str(Config.get_or("photoItemWidget.qualityText", "decimalPlaces", 0)) + "f%%"

    openInSeries = pyqtSignal(QUuid)

    def __init__(self, photoItem, preloadPixmap=None, parent=None, addons=[]):
        super(PhotoItemWidget, self).__init__(preloadPixmap, parent=parent)

        self.setFocusPolicy(Qt.StrongFocus)
        self.installEventFilter(self)

        self.photoItem = photoItem
        self.enabledAddons = addons

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
            self.photoItem.toggleSelection()
        else:
            super(PhotoItemWidget, self).keyPressEvent(event)

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

        if "focus" in self.enabledAddons and self.hasFocus():
            self._paintFocusMark(painter, availableSize, pixmap.size())

        quality, best = self._getPhotoItemQuality()
        if "score" in self.enabledAddons and quality is not None:
            self._paintQualityText(painter, pixmap.size(), quality)

        if "best" in self.enabledAddons and best:
            self._paintBestMark(painter, availableSize)

        if "histogram" in self.enabledAddons:
            if self.photoItem.metrics is not None and self.photoItem.metrics.hist is not None:
                hp = _histogramIcon(self.photoItem.metrics.hist)
                painter.drawPixmap(pixmap.width() - self.BORDER_WIDTH - 7 - hp.width(),
                                   pixmap.height() - self.BORDER_WIDTH - 7 - hp.height(),
                                   hp)

        painter.end()
        return pixmap

    def _paintQualityText(self, painter, pixmapSize, quality):
        painter.save()

        painter.setOpacity(Config.get_or("photoItemWidget.qualityText", "opacity", 1))
        painter.setPen(Config.asQColor("photoItemWidget.qualityText", "color", Qt.black))

        try:
            painter.setFont(Config.asQFont("photoItemWidget.qualityText", "font"))
        except AttributeError:
            pass

        padding = Config.get_or("photoItemWidget.qualityText", "padding", 7)
        textRect = QRectF(self.BORDER_WIDTH + padding,
                          pixmapSize.height() - self.BORDER_WIDTH - padding - painter.font().pointSize(),
                          1, 1)

        painter.drawText(textRect, Qt.AlignLeft | Qt.TextSingleLine | Qt.TextDontClip,
                         self.QUALITY_TEXT_FORMATSTR % (quality*100))

        painter.restore()

    def _paintFocusMark(self, painter, availableSize, pixmapSize):
        configTable = Config.Table("photoItemWidget.focusIcon")
        padding = configTable.get_or("padding", 7)
        focusPixmap = _getIconToPaint(configTable, availableSize)

        painter.drawPixmap(pixmapSize.width() - self.BORDER_WIDTH - padding - focusPixmap.width(),
                           self.BORDER_WIDTH + padding,
                           focusPixmap)

    def _paintBestMark(self, painter, availableSize):
        configTable = Config.Table("photoItemWidget.bestMarkIcon")
        padding = configTable.get_or("padding", 7)
        bestMarkPixmap = _getIconToPaint(configTable, availableSize)
        painter.drawPixmap(self.BORDER_WIDTH + padding, self.BORDER_WIDTH + padding, bestMarkPixmap)

    def _getPhotoItemQuality(self):
        metrics = self.photoItem.metrics
        if metrics is None or metrics.seriesAggregated is None:
            return None, False
        return metrics.quality(), metrics.bestQuality

    def _connectSignals(self):
        self.clicked.connect(self.photoItem.toggleSelection)
        self.photoItem.stateChanged.connect(self.update)
