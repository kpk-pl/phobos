#include <algorithm>
#include "Widgets/PhotoItem/Addon.h"

namespace phobos { namespace widgets { namespace pitem {

Addons::Addons(std::vector<std::string> const& specifiers)
{
    for (auto const& str : specifiers)
    {
        if (str == "focus")
            addons.push_back(AddonType::FOCUS_IND);
        else if (str == "best")
            addons.push_back(AddonType::BEST_IND);
        else if (str == "score")
            addons.push_back(AddonType::SCORE_NUM);
        else if (str == "histogram")
            addons.push_back(AddonType::HISTOGRAM);
        else if (str == "ord")
            addons.push_back(AddonType::ORD_NUM);
    }
    std::sort(addons.begin(), addons.end());
    addons.erase(std::unique(addons.begin(), addons.end()), addons.end());
}

bool Addons::has(AddonType type) const
{
    return std::binary_search(addons.begin(), addons.end(), type);
}

}}} // namespace phobos::widgets::pitem
