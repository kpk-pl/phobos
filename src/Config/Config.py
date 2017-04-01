#!/usr/bin/python3

from Config import _rawToml
from Config.Property import Property
from Config.Table import Table


class Config:
    def __len__(self):
        return len(_rawToml)

    def __getitem__(self, tableName):
        return Table(tableName)

    @staticmethod
    def get(tableName, key):
        return Property(tableName, key)
