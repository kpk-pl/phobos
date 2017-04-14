#include <QHBoxLayout>
#include <QIcon>
#include <QSize>
#include "NavigationBar.h"
#include "ConfigExtension.h"
#include "ImageProcessing/ColoredPixmap.h"

namespace phobos {

namespace {
    QIcon makeIcon(std::string const& configName)
    {
        std::string const path = config::qualified("navigationBar."+configName, std::string{});
        QColor const color = config::qColor("navigationBar.iconColor", Qt::black);
        return iprocess::coloredPixmap(path, QSize(64, 64), color, 1.0);
    }

    class IconButton : public QPushButton
    {
    public:
        IconButton(QIcon const& icon, QWidget* parent = nullptr) :
            QPushButton(icon, "", parent),
            margin(config::qualified("navigationBar.buttonMargin", 5u))
        {
            setIconSize(config::qSize("navigationBar.buttonSize", QSize(40, 40)));
            setContentsMargins(margin, margin, margin, margin);
        }

        QSize sizeHint() const override
        {
            QSize iSize = iconSize();
            return QSize(iSize.width() + 2*margin, iSize.height() + 2*margin);
        }
    private:
        std::size_t const margin;
    };
} // unnamed namespace

NavigationBar::NavigationBar(int capabilities) :
    _leftButton(nullptr), _rightButton(nullptr),
    _allSeries(nullptr), _numSeries(nullptr), _oneSeries(nullptr),
    _slider(nullptr)
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(config::qualified("navigationBar.spacing", 2u));
    setLayout(layout);

    if (capabilities & Capability::ALL_SERIES)
    {
        _allSeries = new IconButton(makeIcon("allSeriesIcon"));
        layout->addWidget(_allSeries);
    }

    if (capabilities & Capability::NUM_SERIES)
    {
        _numSeries = new IconButton(makeIcon("numSeriesIcon"));
        layout->addWidget(_numSeries);
    }

    if (capabilities & Capability::ONE_SERIES)
    {
        _oneSeries = new IconButton(makeIcon("oneSeriesIcon"));
        layout->addWidget(_oneSeries);
    }

    if (capabilities & Capability::SLIDER)
    {
        _slider = new QSlider(Qt::Horizontal);
        _slider->setMaximum(100);
        _slider->setValue(100);

        QWidget* lSpacing = new QWidget();
        lSpacing->setMinimumWidth(15);
        QWidget* rSpacing = new QWidget();
        rSpacing->setMinimumWidth(15);

        layout->addWidget(lSpacing);
        layout->addWidget(_slider);
        layout->addWidget(rSpacing);
    }
    else
    {
        layout->addStretch();
    }

    if (capabilities & Capability::LEFT)
    {
        std::string const fileName = config::qualified("navigationBar.prevIcon", std::string{});
        _leftButton = new IconButton(QIcon(fileName.c_str()));
        layout->addWidget(_leftButton);
    }

    if (capabilities & Capability::RIGHT)
    {
        std::string const fileName = config::qualified("navigationBar.nextIcon", std::string{});
        _rightButton = new IconButton(QIcon(fileName.c_str()));
        layout->addWidget(_rightButton);
    }
}

} // namespace phobos
