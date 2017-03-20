#!/usr/bin/python3

from PyQt5.QtCore import QObject, pyqtSlot, pyqtSignal


class PhotoItem(QObject):
    selectionChanged = pyqtSignal(bool)

    def __init__(self, fileName, seriesUuid, parent=None):
        super(PhotoItem, self).__init__(parent)

        self.fileName = fileName
        self.seriesUuid = seriesUuid
        self.selected = None

    def isSelected(self):
        if self.selected:
            return True
        return False

    @pyqtSlot()
    def select(self):
        self.selected = True
        self.selectionChanged.emit(True)

    @pyqtSlot()
    def discard(self):
        self.selected = False
        self.selectionChanged.emit(False)

    @pyqtSlot()
    def toggleSelection(self):
        self.discard() if self.isSelected() else self.select()

