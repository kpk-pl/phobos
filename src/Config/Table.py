#!/usr/bin/python3

from Config import _rawToml
from Config.Property import Property


class Table:
    def __init__(self, tableName):
        self.tableName = tableName

    def __len__(self):
        if self.tableName not in _rawToml:
            return 0
        return len(_rawToml[self.tableName])

    def __getitem__(self, key):
        return Property(self.tableName, key)

    def get(self, key):
        return Property(self.tableName, key).get()

    def get_or(self, key, defaultValue):
        return Property(self.tableName, key).get_or(defaultValue)
