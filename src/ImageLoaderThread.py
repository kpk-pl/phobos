#!/usr/bin/env python

from PyQt5.QtCore import pyqtSignal, QRunnable, QObject, QSize
from PyQt5.QtGui import QPixmap
import ImageOperations
import ImageProcessing
import Config
import cv2


class LoaderSignals(QObject):
    pixmapReady = pyqtSignal(QPixmap)
    metricsReady = pyqtSignal(ImageProcessing.Metrics)


class ImageLoaderThread(QRunnable):
    def __init__(self, fileName, requestedPixmapSizes, calculateMetrics=False):
        super(ImageLoaderThread, self).__init__()
        self.signals = LoaderSignals()
        self.fileToLoad = fileName
        self.requestedPixmapSizes = requestedPixmapSizes
        self.calculateMetrics = calculateMetrics

    def run(self):
        cvImage = cv2.imread(self.fileToLoad)
        self._emitLoadedSignal(cvImage)

        if self.calculateMetrics:
            cvImage = self._prepareImageForMetrics(cvImage)  # done here to definitely lose old cvImage (RAM usage)
            self._runMetrics(cvImage)

    def _runMetrics(self, cvImage):
        metrics = ImageProcessing.Metrics()
        metrics.blurSobel = ImageOperations.blurrinessSobel(cvImage)
        metrics.blurLaplace = ImageOperations.blurrinessLaplace(cvImage)
        metrics.blurLaplaceMod = ImageOperations.blurinessLaplaceMod(cvImage)
        self.signals.metricsReady.emit(metrics)

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

    @staticmethod
    def _prepareImageForMetrics(cvImage):
        if Config.get_or("imageLoaderThread", "processInGrayscale", True):
            cvImage = cv2.cvtColor(cvImage, cv2.COLOR_BGR2GRAY)

        height, width = cvImage.shape[:2]
        maxSize = Config.asQSize("imageLoaderThread", "processingSize", QSize(1920, 1080))
        scale = 1.0 / max(width / maxSize.width(), height / maxSize.height())
        cvImage = cv2.resize(cvImage, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)

        return cvImage
