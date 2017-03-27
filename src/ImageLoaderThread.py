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


def _calcMetrics(filename, cvImage):
    cvProcess = _getImageForProcessing(cvImage)
    blurSobel = ImageOperations.blurrinessSobel(cvProcess)
    blurLapl = ImageOperations.blurrinessLaplace(cvProcess)
    blurLaplMod = ImageOperations.blurinessLaplaceMod(cvProcess)
    print(filename + " blurSobel=" + str(blurSobel) + " blurLapl=" + str(blurLapl) + " blurMod=" + str(blurLaplMod))


def _loadWithOpenCVToQT(filename):
    cvImage = cv2.imread(filename)
    _calcMetrics(filename, cvImage)

    return QPixmap.fromImage(ImageOperations.convCvToImage(cvImage))


class ImageLoaderThread(QRunnable):
    def __init__(self, fileName):
        super(ImageLoaderThread, self).__init__()
        self.signals = LoaderSignals()
        self.fileToLoad = fileName

    def run(self):
        pixmap = _loadWithOpenCVToQT(self.fileToLoad)
        self.signals.pixmapReady.emit(pixmap)

