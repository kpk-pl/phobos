#!/usr/bin/python3

from PyQt5.QtCore import QUuid, QObject, pyqtSignal, pyqtSlot
from PhotoItem import PhotoItem


class PhotoSeries(QObject):
    def __init__(self, args):
        super(PhotoSeries, self).__init__()

        self.uuid = QUuid.createUuid()
        self.photoItems = []
        self.addPhotoItems(args)

    def addPhotoItem(self, photoItem):
        if isinstance(photoItem, str):
            photoItem = PhotoItem(photoItem, self.uuid)

        assert(photoItem.seriesUuid == self.uuid)
        photoItem.metricsChanged.connect(self._metricCalculated)
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

    @pyqtSlot()
    def _metricCalculated(self):
        if not self._allMetricsAvailable():
            return

        # Todo find the best and calculate overall relative metric

    def _allMetricsAvailable(self):
        for item in self.photoItems:
            if item.metrics is None:
                return False
        return True


def _creation_date(path):
    import platform
    import os
    # http://stackoverflow.com/questions/237079/how-to-get-file-creation-modification-date-times-in-python
    if platform.system() == 'Windows':
        return os.path.getctime(path)
    else:
        stat = os.stat(path)
        try:
            return stat.st_birthtime
        except AttributeError:
            # We're probably on Linux. No easy way to get creation dates here,
            # so we'll settle for when its content was last modified.
            return stat.st_mtime


def _divideIntoSeries(photos, cdate_thresh):
    attrs = [(file, _creation_date(file)) for file in photos]
    attrs = sorted(attrs, key=lambda x: x[1])  # sort on creation data

    result = []

    curr_list = []
    for item in attrs:
        if len(curr_list) == 0:
            curr_list.append(item)
        else:
            if item[1] - curr_list[-1][1] > cdate_thresh:
                result.append([i[0] for i in curr_list])
                curr_list = []

            curr_list.append(item)

    if len(curr_list) > 0:
        result.append([i[0] for i in curr_list])

    return result


def _createSeries(photos):
    photosInSeries = _divideIntoSeries(photos, 2)

    result = []
    for s in photosInSeries:
        series = PhotoSeries(s)
        result.append(series)

    return result


class PhotoSeriesSet(QObject):
    newSeries = pyqtSignal(PhotoSeries)

    def __init__(self):
        super(PhotoSeriesSet, self).__init__()
        self.series = []

    def __getitem__(self, key):
        return self.series[key]

    def __len__(self):
        return len(self.series)

    def addPhotos(self, photos):
        series = _createSeries(photos)
        for s in series:
            self.series.append(s)
            self.newSeries.emit(s)

    def findSeries(self, seriesUuid, offset=0):
        for idx in range(len(self.series)):
            if self.series[idx].uuid == seriesUuid:
                return self.series[(idx+offset) % len(self.series)]
        return None
