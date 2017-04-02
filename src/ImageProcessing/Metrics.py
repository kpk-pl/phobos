#!/usr/bin/python3


class SeriesMetrics:
    blurSobel = None
    blurLaplace = None
    blurLaplaceMod = None

    bestQuality = False

    def blur(self):
        if self.blurSobel is None or self.blurLaplace is None or self.blurLaplaceMod is None:
            return None
        return self.blurSobel * self.blurLaplace * self.blurLaplaceMod

    def quality(self):
        return self.blur()


class Metrics:
    blurSobel = 0
    blurLaplace = 0
    blurLaplaceMod = 0

    seriesAggregated = None

    def fillAggregates(self, blurMax):
        self.seriesAggregated = SeriesMetrics()
        self.seriesAggregated.blurSobel = self.blurSobel / blurMax.blurSobel
        self.seriesAggregated.blurLaplace = self.blurLaplace / blurMax.blurLaplace
        self.seriesAggregated.blurLaplaceMod = self.blurLaplaceMod / blurMax.blurLaplaceMod
