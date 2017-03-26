#!/usr/bin/python3

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QImage, QImageReader, QPixmap
from Exceptions import CannotReadImageException


def readImageFromFile(fileName):
    reader = QImageReader(fileName)
    reader.setAutoTransform(True)
    reader.setAutoDetectImageFormat(True)

    image = reader.read()
    if not image:
        raise CannotReadImageException()

    return image


def readPixmapFromFile(fileName):
    return QPixmap.fromImage(readImageFromFile(fileName))


def scaleImage(image, size):
    return image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)


def convCvToImage(cvImage):
    import cv2

    height, width, byteValue = cvImage.shape
    bytesPerLine = byteValue * width

    cv2.cvtColor(cvImage, cv2.COLOR_BGR2RGB, cvImage)
    return QImage(cvImage, width, height, bytesPerLine, QImage.Format_RGB888)


def convImageToCv(qImage):
    import numpy as np

    swapped = qImage.convertToFormat(QImage.Format_RGB888).rgbSwapped()
    bits = swapped.bits()
    bits.setsize(swapped.byteCount())

    return np.array(bits).reshape(swapped.height(), swapped.width(), 3)
