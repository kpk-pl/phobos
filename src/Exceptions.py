#!/usr/bin/python3


class CannotReadImageException(Exception):
    def __init__(self, fileName):
        super(CannotReadImageException, self).__init__()
        self.fileName = fileName

    def __str__(self):
        return "Cannot read image from file " + self.fileName


class FileNotFoundException(Exception):
    def __init__(self, fileName):
        super(FileNotFoundException, self).__init__()
        self.fileName = fileName

    def __str__(self):
        return 'File "' + self.fileName + '" not found but required.'


class CannotLoadMediaException(Exception):
    def __init__(self, fileName):
        super(CannotLoadMediaException, self).__init__()
        self.fileName = fileName

    def __str__(self):
        return "Cannot load media from file " + self.fileName
