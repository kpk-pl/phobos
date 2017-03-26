#!/usr/bin/python3

from PyQt5.QtCore import QUuid
from PhotoItem import PhotoItem


class PhotoSeries(object):
    def __init__(self, args):
        super(PhotoSeries, self).__init__()

        self.uuid = QUuid.createUuid()
        self.photoItems = []
        self.addPhotoItems(args)

    def addPhotoItem(self, photoItem):
        if isinstance(photoItem, str):
            self.photoItems.append(PhotoItem(photoItem, self.uuid))
        elif isinstance(photoItem, PhotoItem):
            self.photoItems.append(photoItem)

    def addPhotoItems(self, items):
        for it in items:
            self.addPhotoItem(it)

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

    def __len__(self):
        return len(self.series)

    def addSeries(self, series):
        self.series.append(series)

    def findSeries(self, seriesUuid, offset=0):
        for idx in range(len(self.series)):
            if self.series[idx].uuid == seriesUuid:
                return self.series[(idx+offset) % len(self.series)]
        return None
