#!/usr/bin/python3

from PyQt5.QtCore import Qt
import PyQt5.QtGui as QtGui
from Exceptions import CannotReadImageException


def readImageFromFile(fileName):
    reader = QtGui.QImageReader(fileName)
    reader.setAutoTransform(True)
    reader.setAutoDetectImageFormat(True)

    image = reader.read()
    if not image:
        raise CannotReadImageException()

    return QtGui.QPixmap.fromImage(image)


def scaleImage(image, size):
    return image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
