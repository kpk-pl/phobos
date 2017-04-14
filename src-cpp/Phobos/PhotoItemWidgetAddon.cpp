#include <algorithm>
#include "PhotoItemWidgetAddon.h"

namespace phobos {

PhotoItemWidgetAddons::PhotoItemWidgetAddons(std::vector<std::string> const& specifiers)
{
    for (auto const& str : specifiers)
    {
        if (str == "focus")
            addons.push_back(PhotoItemWidgetAddonType::FOCUS_IND);
        else if (str == "best")
            addons.push_back(PhotoItemWidgetAddonType::BEST_IND);
        else if (str == "score")
            addons.push_back(PhotoItemWidgetAddonType::SCORE_NUM);
        else if (str == "histogram")
            addons.push_back(PhotoItemWidgetAddonType::HISTOGRAM);
    }
    std::sort(addons.begin(), addons.end());
    addons.erase(std::unique(addons.begin(), addons.end()), addons.end());
}

bool PhotoItemWidgetAddons::has(PhotoItemWidgetAddonType type) const
{
    return std::binary_search(addons.begin(), addons.end(), type);
}

} // namespace phobos
