#!/usr/bin/python3


class SeriesMetrics:
    blurSobelPrc = None
    blurLaplacePrc = None
    blurLaplaceModPrc = None

    bestQuality = False

    def blur(self):
        if self.blurSobelPrc is None or self.blurLaplacePrc is None or self.blurLaplaceModPrc is None:
            return None
        return self.blurSobelPrc * self.blurLaplacePrc * self.blurLaplaceModPrc

    def quality(self):
        return self.blur()

    def clone(self):
        result = SeriesMetrics()
        result.blurSobelPrc = self.blurSobelPrc
        result.blurLaplacePrc = self.blurLaplacePrc
        result.blurLaplaceModPrc = self.blurLaplaceModPrc
        result.bestQuality = self.bestQuality
        return result


class Metrics:
    blurSobel = 0
    blurLaplace = 0
    blurLaplaceMod = 0

    seriesAggregated = None

    def clone(self):
        result = Metrics()
        result.blurSobel = self.blurSobel
        result.blurLaplace = self.blurLaplace
        result.blurLaplaceMod = self.blurLaplaceMod
        result.seriesAggregated = self.seriesAggregated.clone() if self.seriesAggregated else None
        return result


def _calcMaxMetric(metrics):
    maxBlur = Metrics()
    maxBlur.blurSobel = max([item.blurSobel for item in metrics])
    maxBlur.blurLaplace = max([item.blurLaplace for item in metrics])
    maxBlur.blurLaplaceMod = max([item.blurLaplaceMod for item in metrics])
    return maxBlur


def generateAggregateMetrics(metrics):
    blurMax = _calcMaxMetric(metrics)
    result = []

    for item in metrics:
        aggreg = SeriesMetrics()
        aggreg.blurSobelPrc = item.blurSobel / blurMax.blurSobel
        aggreg.blurLaplacePrc = item.blurLaplace / blurMax.blurLaplace
        aggreg.blurLaplaceModPrc = item.blurLaplaceMod / blurMax.blurLaplaceMod
        result.append(aggreg)

    bestItem = max(result, key=lambda i: i.quality())
    bestItem.bestQuality = True

    return result
