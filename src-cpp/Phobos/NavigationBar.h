#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>

namespace phobos {

class NavigationBar : public QWidget
{
public:
    enum Capability
    {
        NONE = 0,
        ALL_SERIES = 1,
        NUM_SERIES = 2,
        ONE_SERIES = 4,
        LEFT = 8,
        RIGHT = 16,
        SLIDER = 32
    };

    explicit NavigationBar(int capabilities = Capability::NONE);

    QPushButton* leftButton() const { return _leftButton; }
    QPushButton* rightButton() const { return _rightButton; }
    QPushButton* allSeriesButton() const { return _allSeries; }
    QPushButton* numSeriesButton() const { return _numSeries; }
    QPushButton* oneSeriesButton() const { return _oneSeries; }
    QSlider* slider() const { return _slider; }

private:
    QPushButton *_leftButton, *_rightButton;
    QPushButton *_allSeries, *_numSeries, *_oneSeries;
    QSlider *_slider;
};

} // namespace phobos

#endif // NAVIGATIONBAR_H
