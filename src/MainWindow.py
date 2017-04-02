#!/usr/bin/python3

from PyQt5.QtCore import QStandardPaths, QDir
from PyQt5.QtGui import QImageReader
from PyQt5.QtWidgets import QAction, QMainWindow, QFileDialog
from ViewStack import ViewStack
import Config


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()

        self._setUpCentralWidget()
        self._createActions()
        self._createMenus()

        self.setWindowTitle(Config.get_or("mainWindow", "title", "Phobos"))
        self.setMinimumSize(Config.asQSize("mainWindow", "minimumSize"))
        self.resize(Config.asQSize("mainWindow", "defaultSize"))

        self._firstLoadDialog = True
        self._loadedImages = []

    def loadPhotos(self):
        dialog = self._createLoadImagesDialog()
        if not dialog.exec():
            return

        self.processNewPhotos(dialog.selectedFiles())

    def processNewPhotos(self, photos):
        selectedFiles = [x for x in photos if x not in self._loadedImages]
        # TODO: create list of rejected files and display it in dialog
        if selectedFiles:
            self._loadedImages += selectedFiles
            self.workArea.addPhotos(selectedFiles)

    def _createActions(self):
        self._loadPhotosAction = QAction("&Load photos", self, shortcut="Ctrl+L",
                                         statusTip="Load new photos", triggered=self.loadPhotos)

        self._exitAction = QAction("&Exit", self, shortcut="Ctrl+Q",
                                   statusTip="Exit the application", triggered=self.close)

        self._showAllSeriesView = QAction("&All series", self, shortcut="Ctrl+A",
                                          statusTip="Show all series in one view",
                                          triggered=self.workArea.showAllSeries)

        self._showOneSeriesView = QAction("&One series", self, shortcut="Ctrl+O",
                                          statusTip="Show one series on a single page",
                                          triggered=self.workArea.showOneSeries)

        self._showNextSeriesInView = QAction("&Next series", self, shortcut="Ctrl+N",
                                             statusTip="Jump to next series",
                                             triggered=self.workArea.showNextSeries)

        self._showPrevSeriesInView = QAction("&Prev series", self, shortcut="Ctrl+P",
                                             statusTip="Jump to previous series",
                                             triggered=self.workArea.showPrevSeries)

    def _createMenus(self):
        self._fileMenu = self.menuBar().addMenu("&File")
        self._fileMenu.addAction(self._loadPhotosAction)
        self._fileMenu.addSeparator()
        self._fileMenu.addAction(self._exitAction)

        self._viewMenu = self.menuBar().addMenu("&View")
        self._viewMenu.addAction(self._showAllSeriesView)
        self._viewMenu.addAction(self._showOneSeriesView)
        self._viewMenu.addSeparator()
        self._viewMenu.addAction(self._showNextSeriesInView)
        self._viewMenu.addAction(self._showPrevSeriesInView)

    def _setUpCentralWidget(self):
        self.workArea = ViewStack()
        self.setCentralWidget(self.workArea)

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
