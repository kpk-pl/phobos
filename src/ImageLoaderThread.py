#!/usr/bin/env python

from PyQt5.QtCore import pyqtSignal, QRunnable, QObject
from PyQt5.QtGui import QImage, QPixmap
import ImageOperations

class LoaderSignals(QObject):
    pixmapReady = pyqtSignal(QPixmap)


#def _loadWithOpenCVToQT(filename):
#    cvImage = cv2.imread(filename)
#    return QPixmap.fromImage(ImageOperations.convCvToImage(cvImage))


class ImageLoaderThread(QRunnable):
    def __init__(self, fileName):
        super(ImageLoaderThread, self).__init__()
        self.signals = LoaderSignals()
        self.fileToLoad = fileName

    def run(self):
        pixmap = ImageOperations.readPixmapFromFile(self.fileToLoad)
        self.signals.pixmapReady.emit(pixmap)

