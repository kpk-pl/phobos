#!/usr/bin/python3


class CannotReadImageException(Exception):
    def __init__(self, fileName):
        super(CannotReadImageException, self).__init__(self)
        self.fileName = fileName

    def __str__(self):
        return "Cannot read image from file" + self.fileName
