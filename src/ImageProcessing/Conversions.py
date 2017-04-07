#!/usr/bin/python3

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPainter, QPixmap


def coloredPixmap(filePath, size, color=Qt.black, opacity=1):
    pixmap = QPixmap(filePath).scaled(size, aspectRatioMode=Qt.KeepAspectRatio, transformMode=Qt.SmoothTransformation)
    if pixmap.isNull():
        pixmap = QPixmap(size)

    painter = QPainter(pixmap)
    painter.setOpacity(opacity)
    painter.setRenderHint(QPainter.Antialiasing)
    painter.setCompositionMode(QPainter.CompositionMode_SourceIn)
    painter.setBrush(color)
    painter.setPen(color)

    painter.drawRect(pixmap.rect())
    painter.end()

    return pixmap
