#!/usr/bin/python3

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QImage, QImageReader, QPixmap, QColor
from Exceptions import CannotReadImageException
import numpy as np
import cv2


def readImageFromFile(fileName):
    reader = QImageReader(fileName)
    reader.setAutoTransform(True)
    reader.setAutoDetectImageFormat(True)

    image = reader.read()
    if not image:
        raise CannotReadImageException()

    return image


def readPixmapFromFile(fileName):
    return QPixmap.fromImage(readImageFromFile(fileName))


def scaleImage(image, size):
    return image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)


def buildPreloadPixmap(size):
    imagePixmap = QPixmap(size)
    imagePixmap.fill(QColor(Qt.lightGray))
    return imagePixmap


def convCvToImage(cvImage):
    height, width = cvImage.shape[:2]
    if len(cvImage.shape) == 2:
        return QImage(cvImage, width, height, width, QImage.Format_Grayscale8)
    else:
        bytesPerLine = cvImage.shape[2] * width
        cvImage = cv2.cvtColor(cvImage, cv2.COLOR_BGR2RGB)
        return QImage(cvImage, width, height, bytesPerLine, QImage.Format_RGB888)


def convImageToCv(qImage):
    swapped = qImage.convertToFormat(QImage.Format_RGB888).rgbSwapped()
    bits = swapped.bits()
    bits.setsize(swapped.byteCount())

    return np.array(bits).reshape(swapped.height(), swapped.width(), 3)


# Returns smaller values for blurry images
# depth of cv2.CV_32F allocates a lot of memory
def blurrinessSobel(cvImage, depth=-1):
    sobelNormX = cv2.norm(cv2.Sobel(cvImage, depth, 1, 0))
    sobelNormY = cv2.norm(cv2.Sobel(cvImage, depth, 0, 1))

    height, width = cvImage.shape[:2]
    area = height * width

    sumSq = sobelNormX * sobelNormX + sobelNormY * sobelNormY
    return sumSq / area
    #return 1.0/(sumSq / area + 1e-6)


# Returns smaller values for blurry images
def blurrinessLaplace(cvImage, depth=-1):
    laplVar = np.var(cv2.Laplacian(cvImage, depth))
    return laplVar


# Returns smaller values for blurry images
# Modified from http://stackoverflow.com/questions/7765810/is-there-a-way-to-detect-if-an-image-is-blurry/7768918#7768918
def blurinessLaplaceMod(cvImage, depth=-1):
    kernelX = np.array([-1, 2, -1], dtype=np.float64)
    kernelY = cv2.getGaussianKernel(3, -1)

    lxAbsMean = cv2.mean(np.abs(cv2.sepFilter2D(cvImage, depth, kernelX, kernelY)))[0]
    lyAbsMean = cv2.mean(np.abs(cv2.sepFilter2D(cvImage, depth, kernelY, kernelX)))[0]

    return lxAbsMean + lyAbsMean


def normalizedHistogramAndContrast(cvImage):
    hist = cv2.calcHist([cvImage], [0], None, [256], [0, 256])
    stddev = np.std(hist)
    m = max(hist)[0]
    hist = [x[0]/m for x in hist]

    return hist, stddev


# Substracts median-blurred image from itself and calculates average pixel value
# The ideal result is close to 0
def noiseMeasure(cvImage, medianSize=5):
    result = cv2.medianBlur(cvImage, medianSize)
    result = cvImage - result
    return np.average(result)/255
