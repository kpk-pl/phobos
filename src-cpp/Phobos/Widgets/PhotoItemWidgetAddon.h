#ifndef WIDGETS_PHOTOITEMWIDGETADDON_H
#define WIDGETS_PHOTOITEMWIDGETADDON_H

#include <string>
#include <vector>

namespace phobos { namespace widgets {

enum class PhotoItemWidgetAddonType
{
    FOCUS_IND,
    BEST_IND,
    SCORE_NUM,
    ORD_NUM,
    HISTOGRAM
};

struct PhotoItemWidgetAddons
{
public:
    explicit PhotoItemWidgetAddons(std::vector<std::string> const& specifiers);

    bool has(PhotoItemWidgetAddonType type) const;

    std::vector<PhotoItemWidgetAddonType> addons;
};

}} // namespace phobos::widgets

#endif // WIDGETS_PHOTOITEMWIDGETADDON_H
