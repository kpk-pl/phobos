#!/usr/bin/env python

from PyQt5.QtCore import pyqtSignal, QRunnable, QObject, QSize
from PyQt5.QtGui import QPixmap
import ImageOperations
import cv2

class LoaderSignals(QObject):
    pixmapReady = pyqtSignal(QPixmap)


def _getImageForProcessing(cvImage):
    result = cv2.cvtColor(cvImage, cv2.COLOR_BGR2GRAY)

    height, width = result.shape
    maxSize = (1920, 1080)
    scale = 1.0/max(width/maxSize[0], height/maxSize[1])
    result = cv2.resize(result, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)

    return result


def _calcMetrics(cvImage):
    cvProcess = _getImageForProcessing(cvImage)
    blurSobel = ImageOperations.blurrinessSobel(cvProcess)
    blurLapl = ImageOperations.blurrinessLaplace(cvProcess)
    blurLaplMod = ImageOperations.blurinessLaplaceMod(cvProcess)


class ImageLoaderThread(QRunnable):
    def __init__(self, fileName, requestedPixmapSizes):
        super(ImageLoaderThread, self).__init__()
        self.signals = LoaderSignals()
        self.fileToLoad = fileName
        self.requestedPixmapSizes = requestedPixmapSizes

    def run(self):
        cvImage = cv2.imread(self.fileToLoad)
        self._emitLoadedSignal(cvImage)
        _calcMetrics(cvImage)

    def _emitLoadedSignal(self, cvImage):
        pixmap = QPixmap.fromImage(ImageOperations.convCvToImage(cvImage))
        pixmap = ImageOperations.scaleImage(pixmap, self._biggestClosestSize(pixmap.size()))
        self.signals.pixmapReady.emit(pixmap)

    def _biggestClosestSize(self, pixmapSize):
        scaledSizes = []
        pixels = []

        for size in self.requestedPixmapSizes:
            width = min(size.width(), pixmapSize.width())
            height = pixmapSize.height() * width / pixmapSize.width()
            if height > size.height():
                height = size.height()
                width = pixmapSize.width() * height / pixmapSize.height()

            scaledSizes.append(QSize(width, height))
            pixels.append(width*height)

        return scaledSizes[pixels.index(max(pixels))]
