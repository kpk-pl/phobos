#!/usr/bin/python3

from PyQt5.QtCore import QUuid


class PhotoSeries(object):
    def __init__(self):
        super(PhotoSeries, self).__init__()

        self.uuid = QUuid.createUuid()
        self.photoItems = []

    def addPhotoItem(self, photoItem):
        self.photoItems.append(photoItem)

    def __len__(self):
        return len(self.photoItems)

    def __getitem__(self, key):
        return self.photoItems[key]

    def __iter__(self):
        return iter(self.photoItems)


class PhotoSeriesSet(object):
    def __init__(self):
        super(PhotoSeriesSet, self).__init__()

        self.series = []

    def __getitem__(self, key):
        return self.series[key]

    def addSeries(self, series):
        self.series.append(series)

    def findSeries(self, seriesUuid, offset=0):
        for idx in range(len(self.series)):
            if self.series[idx].uuid == seriesUuid:
                return self.series[(idx+offset) % len(self.series)]
        return None
