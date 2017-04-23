#ifndef VIEWDESCRIPTION_H
#define VIEWDESCRIPTION_H

#include <QUuid>
#include <QMetaType>
#include <memory>
#include <boost/optional.hpp>

namespace phobos
{
    enum class ViewType
    {
        CURRENT,
        ALL_SERIES,
        ANY_SINGLE_SERIES,
        ROW_SINGLE_SERIES,
        NUM_SINGLE_SERIES
    };

    struct ViewDescription
    {
        static std::shared_ptr<ViewDescription> make(ViewType type,
                                                     boost::optional<QUuid> uuid = boost::none,
                                                     boost::optional<int> offset = boost::none)
        {
            ViewDescription desc{type, uuid, offset};
            return std::make_shared<ViewDescription>(std::move(desc));
        }

        ViewType type;
        boost::optional<QUuid> seriesUuid;
        boost::optional<int> seriesOffset;
    };

    typedef std::shared_ptr<ViewDescription> ViewDescriptionPtr;
} // namespace phobos

Q_DECLARE_METATYPE(phobos::ViewDescriptionPtr)

#endif // VIEWDESCRIPTION_H
