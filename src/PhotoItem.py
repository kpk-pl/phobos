#!/usr/bin/python3

from enum import Enum
from PyQt5.QtCore import QObject, pyqtSlot, pyqtSignal


class PhotoItemState(Enum):
    UNKNOWN = 0
    SELECTED = 1
    DISCARDED = 2


class PhotoItem(QObject):
    selectionChanged = pyqtSignal(PhotoItemState)

    def __init__(self, fileName, seriesUuid, parent=None):
        super(PhotoItem, self).__init__(parent)

        self.fileName = fileName
        self.seriesUuid = seriesUuid
        self.state = PhotoItemState.UNKNOWN

    def isSelected(self):
        return self.state == PhotoItemState.SELECTED

    @pyqtSlot()
    def select(self):
        self.state = PhotoItemState.SELECTED
        self.selectionChanged.emit(self.state)

    @pyqtSlot()
    def discard(self):
        self.state = PhotoItemState.DISCARDED
        self.selectionChanged.emit(self.state)

    @pyqtSlot()
    def toggleSelection(self):
        self.discard() if self.isSelected() else self.select()

