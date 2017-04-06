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
        self.cvImage = None

    def run(self):
        self.cvImage = cv2.imread(self.fileToLoad)
        self._emitLoadedSignal(self.cvImage)

        if self.calculateMetrics:
            self._runMetrics()

    def _runMetrics(self):
        height, width = self.cvImage.shape[:2]
        maxSize = Config.asQSize("imageLoaderThread", "processingSize", QSize(1920, 1080))
        scale = 1.0 / max(width / maxSize.width(), height / maxSize.height())
        self.cvImage = cv2.resize(self.cvImage, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)

        metrics = ImageProcessing.Metrics()

        #self.cvImage = cv2.cvtColor(self.cvImage, cv2.COLOR_BGR2HSV)
        #metrics.hist = ImageOperations.normalizedHistogram(self.cvImage[:, :, 2])

        self.cvImage = cv2.cvtColor(self.cvImage, cv2.COLOR_BGR2GRAY)

        metrics.hist, metrics.contrast = ImageOperations.normalizedHistogramAndContrast(self.cvImage)
        metrics.noise = ImageOperations.noiseMeasure(self.cvImage, Config.get_or("imageLoaderThread", "noiseMedianSize", 3))

        metrics.blurSobel = ImageOperations.blurrinessSobel(self.cvImage)
        metrics.blurLaplace = ImageOperations.blurrinessLaplace(self.cvImage)
        metrics.blurLaplaceMod = ImageOperations.blurinessLaplaceMod(self.cvImage)

        self.cvImage = None
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
