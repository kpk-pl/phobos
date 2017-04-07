#!/usr/bin/python3

from PyQt5.QtCore import QSize, pyqtSignal, pyqtSlot
from PyQt5.QtWidgets import QWidget
from PhotoItemWidget import PhotoItemWidget
from Exceptions import CannotReadImageException
import ImageOperations
import Config
import abc


def _clearLayout(layout):
    while True:
        item = layout.takeAt(0)
        if item is None:
            break

        wgt = item.widget()
        if wgt:
            wgt.setParent(None)

        lt = item.layout()
        if lt:
            _clearLayout(lt)


class SeriesViewBase(QWidget):
    switchView = pyqtSignal(str)
    nextSeries = pyqtSignal()
    prevSeries = pyqtSignal()

    def __init__(self, parent=None):
        super(SeriesViewBase, self).__init__(parent)
        self._preloadPixmap = None

    def showSeries(self, series):
        self.clear()

        for photoItem in series:
            try:
                preload = photoItem.pixmap if photoItem.pixmap is not None else self._getPreloadPixmap()
                widget = PhotoItemWidget(photoItem, preloadPixmap=preload,
                                         addons=Config.get_or("seriesView", "enabledAddons", []))
            except CannotReadImageException as e:
                print("TODO: cannot load image exception " + str(e))
            else:
                self._addPhotoItemToLayout(widget)
                photoItem.loadPhoto(Config.asQSize('seriesView', 'maxPixmapSize', QSize(1920, 1080)),
                                    widget.setImagePixmap)

    @abc.abstractmethod
    def clear(self):
        pass

    @pyqtSlot()
    def backToSeries(self):
        self.clear()
        self.returnFromView.emit()

    @abc.abstractmethod
    def _addPhotoItemToLayout(self, photoItem):
        pass

    def _getPreloadPixmap(self):
        if self._preloadPixmap is None:
            self._preloadPixmap = ImageOperations.buildPreloadPixmap(
                Config.asQSize('seriesView', 'maxPixmapSize', QSize(1920, 1080)))
        return self._preloadPixmap
