#!/usr/bin/python3

import pytoml as toml


with open('Config/config.toml', 'rb') as f:
    _rawToml = toml.load(f)


from Config.Property import Property
from Config.Table import Table
from Config.Config import Config


def get(tableName, key):
    return Property(tableName, key).get()


def asQSize(tableName, key):
    from Config.Types import asQSize
    return asQSize(Property(tableName, key))


def asQColor(tableName, key):
    from Config.Types import asQColor
    return asQColor(Property(tableName, key))


def asQFont(tableName, key):
    from Config.Types import asQFont
    return asQFont(Property(tableName, key))
