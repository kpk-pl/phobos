#!/usr/bin/env python

from PyQt5.QtCore import pyqtSignal, QRunnable, QObject
import PyQt5.QtGui as QtGui
import ImageOperations


class LoaderSignals(QObject):
    pixmapReady = pyqtSignal(QtGui.QPixmap)


class ImageLoaderThread(QRunnable):
    def __init__(self, fileName):
        super(ImageLoaderThread, self).__init__()
        self.signals = LoaderSignals()
        self.fileToLoad = fileName

    def run(self):
        pixmap = ImageOperations.readImageFromFile(self.fileToLoad)
        self.signals.pixmapReady.emit(pixmap)
        pass
