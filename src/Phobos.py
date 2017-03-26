#!/usr/bin/python3

import sys
from PyQt5.QtWidgets import QApplication
from MainWindow import MainWindow

if __name__ == '__main__':
    app = QApplication(sys.argv)

    mainWindow = MainWindow()
    mainWindow.show()

    # TESTING CODE
    mainWindow.processNewPhotos(['C:/Users/krzysiek/Pictures/C3283124.jpg', 'C:/Users/krzysiek/Pictures/C3283125.jpg', 'C:/Users/krzysiek/Pictures/C3283126.jpg', 'C:/Users/krzysiek/Pictures/C3283127.jpg', 'C:/Users/krzysiek/Pictures/C3283128.jpg', 'C:/Users/krzysiek/Pictures/C3283129.jpg', 'C:/Users/krzysiek/Pictures/C3283130.jpg'])
    mainWindow.processNewPhotos(['C:/Users/krzysiek/Pictures/C3293131.jpg', 'C:/Users/krzysiek/Pictures/C3293132.jpg', 'C:/Users/krzysiek/Pictures/C3293133.jpg', 'C:/Users/krzysiek/Pictures/C3293134.jpg', 'C:/Users/krzysiek/Pictures/C3293135.jpg', 'C:/Users/krzysiek/Pictures/C3293136.jpg', 'C:/Users/krzysiek/Pictures/C3293137.jpg'])
    mainWindow.workArea.openInSeries(mainWindow.workArea.series[0].uuid)
    mainWindow.workArea.seriesRowView.clear()
    mainWindow.workArea.openInSeries(mainWindow.workArea.series[1].uuid)

    sys.exit(app.exec_())
