#!/usr/bin/python3


class SeriesMetrics:
    blurSobelPrc = None
    blurLaplacePrc = None
    blurLaplaceModPrc = None
    noisePrc = None
    contrastPrc = None

    def blur(self):
        result = 1.0
        if self.blurSobelPrc is not None:
            result *= self.blurSobelPrc
        if self.blurLaplacePrc is not None:
            result *= self.blurLaplacePrc
        if self.blurLaplaceModPrc is not None:
            result *= self.blurLaplaceModPrc
        return result

    def quality(self):
        blur = self.blur()
        result = 1.0
        if blur is not None:
            result *= blur
        if self.noisePrc is not None:
            result *= self.noisePrc
        if self.contrastPrc is not None:
            result *= self.contrastPrc
        return result

    def clone(self):
        result = SeriesMetrics()
        result.blurSobelPrc = self.blurSobelPrc
        result.blurLaplacePrc = self.blurLaplacePrc
        result.blurLaplaceModPrc = self.blurLaplaceModPrc
        result.noisePrc = self.noisePrc
        result.contrastPrc = self.contrastPrc
        return result


class Metrics:
    blurSobel = None
    blurLaplace = None
    blurLaplaceMod = None
    hist = None
    contrast = None
    noise = None

    seriesAggregated = None
    bestQuality = False

    def quality(self):
        result = self.seriesAggregated.quality() if self.seriesAggregated is not None else 1.0
        return result

    def clone(self):
        result = Metrics()
        result.blurSobel = self.blurSobel
        result.blurLaplace = self.blurLaplace
        result.blurLaplaceMod = self.blurLaplaceMod
        result.contrast = self.contrast
        result.noise = self.noise
        result.hist = self.hist
        result.seriesAggregated = self.seriesAggregated.clone() if self.seriesAggregated else None
        result.bestQuality = self.bestQuality
        return result


def _calcMaxMetric(metrics):
    maxMet = Metrics()
    maxMet.blurSobel = max([item.blurSobel for item in metrics])
    maxMet.blurLaplace = max([item.blurLaplace for item in metrics])
    maxMet.blurLaplaceMod = max([item.blurLaplaceMod for item in metrics])
    maxMet.noise = max([item.noise for item in metrics])
    maxMet.contrast = max([item.contrast for item in metrics])
    return maxMet


def generateAggregateMetrics(metrics):
    maxMet = _calcMaxMetric(metrics)
    result = []

    for item in metrics:
        newItem = item.clone()
        newItem.seriesAggregated = SeriesMetrics()
        newItem.seriesAggregated.blurSobelPrc = item.blurSobel / maxMet.blurSobel
        newItem.seriesAggregated.blurLaplacePrc = item.blurLaplace / maxMet.blurLaplace
        newItem.seriesAggregated.blurLaplaceModPrc = item.blurLaplaceMod / maxMet.blurLaplaceMod
        newItem.seriesAggregated.noisePrc = item.noise / maxMet.noise
        newItem.seriesAggregated.contrastPrc = item.contrast / maxMet.contrast
        result.append(newItem)

    bestItem = max(result, key=lambda i: i.quality())
    bestItem.bestQuality = True

    return result
