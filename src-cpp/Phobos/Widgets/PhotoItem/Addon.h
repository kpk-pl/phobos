#ifndef WIDGETS_PHOTOITEM_ADDON_H
#define WIDGETS_PHOTOITEM_ADDON_H

#include <string>
#include <vector>

namespace phobos { namespace widgets { namespace pitem {

enum class AddonType
{
    FOCUS_IND,
    BEST_IND,
    SCORE_NUM,
    ORD_NUM,
    HISTOGRAM
};

struct Addons
{
public:
    explicit Addons(std::vector<std::string> const& specifiers);

    bool has(AddonType type) const;

    std::vector<AddonType> addons;
};

}}} // namespace phobos::widgets::pitem

#endif // WIDGETS_PHOTOITEM_ADDON_H
