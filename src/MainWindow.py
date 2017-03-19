#!/usr/bin/python3

from PyQt5.QtCore import Qt, QStandardPaths, QDir
from PyQt5.QtGui import QImageReader
from PyQt5.QtWidgets import QAction, QMainWindow, QWidget, QFileDialog, QVBoxLayout, QScrollArea, QFrame
from AllSeriesView import AllSeriesView
from NavigationBar import NavigationBar


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()

        self._createActions()
        self._createMenus()
        self._setUpCentralWidget()

        self.setWindowTitle("Phobos")
        self.setMinimumSize(480, 360)
        self.resize(1024, 768)

        self._firstLoadDialog = True
        self._loadedImages = []

    def loadPhotos(self):
        dialog = self._createLoadImagesDialog()
        if not dialog.exec():
            return

        selectedFiles = [x for x in dialog.selectedFiles() if x not in self._loadedImages]
        # TODO: create list of rejected files and display it in dialog
        if selectedFiles:
            self._loadedImages += selectedFiles
            self.workArea.addPhotos(selectedFiles)

    def _createActions(self):
        self._loadPhotosAction = QAction("&Load photos", self, shortcut="Ctrl+L",
                                         statusTip="Load new photos", triggered=self.loadPhotos)

        self._exitAction = QAction("&Exit", self, shortcut="Ctrl+Q",
                                   statusTip="Exit the application", triggered=self.close)

    def _createMenus(self):
        self._fileMenu = self.menuBar().addMenu("&File")
        self._fileMenu.addAction(self._loadPhotosAction)
        self._fileMenu.addSeparator()
        self._fileMenu.addAction(self._exitAction)

    def _setUpCentralWidget(self):
        self.workArea = AllSeriesView()
        self.navigation = NavigationBar()

        vlayout = QVBoxLayout()
        vlayout.addWidget(self.navigation)
        vlayout.addWidget(self.workArea)
        vlayout.addStretch()

        scrollObject = QWidget()
        scrollObject.setLayout(vlayout)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setWidget(scrollObject)

        self.setCentralWidget(scroll)

    def _createLoadImagesDialog(self):
        dialog = QFileDialog(self, "Load photos")

        if self._firstLoadDialog:
            self._firstLoadDialog = False
            locations = QStandardPaths.standardLocations(QStandardPaths.PicturesLocation)
            dialog.setDirectory(locations[-1] if locations else QDir.currentPath())

        supportedTypes = []
        for mtype in QImageReader.supportedMimeTypes():
            supportedTypes.append(str(mtype))
        supportedTypes.sort()
        dialog.setMimeTypeFilters(supportedTypes)

        dialog.setFileMode(QFileDialog.ExistingFiles)
        return dialog
