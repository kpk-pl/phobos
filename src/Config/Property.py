#!/usr/bin/python3

from Config import _rawToml


class Property:
    def __init__(self, tableName, key):
        self.tableName = tableName
        self.key = key

    def get(self):
        rawDict = self._getTable()

        if self.key not in rawDict:
            raise AttributeError()
        return rawDict[self.key]

    def get_or(self, defaultValue):
        rawDict = self._getTable()

        if self.key not in rawDict:
            return defaultValue
        return rawDict[self.key]

    def _getTable(self):
        tables = self.tableName.split('.')
        rawDict = _rawToml

        for table in tables:
            if table not in rawDict:
                return None
            rawDict = rawDict[table]

        return rawDict