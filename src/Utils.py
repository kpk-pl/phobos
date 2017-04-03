#!/usr/bin/python3

from PyQt5.QtWidgets import QApplication
from PhotoItemWidget import PhotoItemWidget


def focusedPhotoItem():
    focusWidget = QApplication.focusWidget()
    if focusWidget is not None and isinstance(focusWidget, PhotoItemWidget):
        return focusWidget
    return None
