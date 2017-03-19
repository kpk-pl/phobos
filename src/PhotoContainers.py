#!/usr/bin/python3

from PyQt5.QtCore import QUuid


class PhotoSeries(object):
    def __init__(self):
        super(PhotoSeries, self).__init__()

        self.uuid = QUuid.createUuid()
        self.photos = []

    def addPhoto(self, photo):
        self.photos.append(photo)

    def __len__(self):
        return len(self.photos)


class PhotoSeriesSet(object):
    def __init__(self):
        super(PhotoSeriesSet, self).__init__()

        self.series = []

    def addSeries(self, series):
        self.series.append(series)

    def findSeries(self, seriesUuid):
        for s in self.series:
            if s.uuid == seriesUuid:
                return s
        return None
