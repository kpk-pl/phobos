#ifndef PHOTOITEMWIDGETADDON_H
#define PHOTOITEMWIDGETADDON_H

#include <string>
#include <vector>

namespace phobos {

enum class PhotoItemWidgetAddonType
{
    FOCUS_IND,
    BEST_IND,
    SCORE_NUM,
    HISTOGRAM
};

struct PhotoItemWidgetAddons
{
public:
    explicit PhotoItemWidgetAddons(std::vector<std::string> const& specifiers);

    bool has(PhotoItemWidgetAddonType type) const;

    std::vector<PhotoItemWidgetAddonType> addons;
};

} // namespace phobos

#endif // PHOTOITEMWIDGETADDON_H
