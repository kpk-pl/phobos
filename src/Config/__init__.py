#!/usr/bin/python3

import pytoml as toml


with open('./Config/config.toml', 'rb') as f:
    _rawToml = toml.load(f)


from Config.Property import Property
from Config.Table import Table
from Config.Config import Config


def get(tableName, key):
    return Property(tableName, key).get()


def get_or(tableName, key, defaultValue):
    return Property(tableName, key).get_or(defaultValue)


def asQSize(tableName, key, defaultValue=None):
    from Config.Types import asQSize
    prop = Property(tableName, key)
    if prop.hasValue():
        return asQSize(prop)
    elif defaultValue is None:
        raise AttributeError()
    return defaultValue


def asQColor(table, key, defaultValue=None):
    from Config.Types import asQColor
    if isinstance(table, str):
        prop = Property(table, key)
    else:  # Table
        prop = table[key]
    if prop.hasValue():
        return asQColor(prop)
    elif defaultValue is None:
        raise AttributeError()
    return defaultValue


def asQFont(tableName, key, defaultValue=None):
    from Config.Types import asQFont
    prop = Property(tableName, key)
    if prop.hasValue():
        return asQFont(prop)
    elif defaultValue is None:
        raise AttributeError()
    return defaultValue
