#!/usr/bin/python3

from Config import _rawToml


class Property:
    def __init__(self, tableName, key):
        self.tableName = tableName
        self.key = key

    def get(self):
        tables = self.tableName.split('.')
        rawDict = _rawToml

        for table in tables:
            if table not in rawDict:
                raise AttributeError()
            rawDict = rawDict[table]

        if self.key not in rawDict:
            raise AttributeError()
        return rawDict[self.key]
