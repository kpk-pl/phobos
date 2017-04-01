#!/usr/bin/python3


def asQSize(attribute):
    from PyQt5.QtCore import QSize

    value = attribute.get()

    if isinstance(value, dict):
        if 'width' not in value or 'height' not in value:
            raise AttributeError()
        return QSize(value['width'], value['height'])
    elif isinstance(value, list):
        if len(value) != 2:
            raise AttributeError()
        return QSize(value[0], value[1])
    else:
        raise AttributeError()


def asQColor(attribute):
    from PyQt5.QtGui import QColor

    value = attribute.get()

    if isinstance(value, str):
        return QColor(value)
    else:
        raise AttributeError()
