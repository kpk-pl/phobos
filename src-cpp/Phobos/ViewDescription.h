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
    NUM_SINGLE_SERIES,
    LABORATORY
  };

  using ViewDescriptionPtr = std::shared_ptr<struct ViewDescription>;

  struct ViewDescription
  {
    static ViewDescriptionPtr switchTo(ViewType type, boost::optional<QUuid> uuid = boost::none, int seriesOffset = 0, int photoOffset = 0)
    {
      return std::make_shared<ViewDescription>(ViewDescription{type, uuid, seriesOffset, photoOffset});
    }

    static ViewDescriptionPtr moveNextSeries()
    {
      return std::make_shared<ViewDescription>(ViewDescription{ViewType::CURRENT, boost::none, 1, 0});
    }

    static ViewDescriptionPtr movePreviousSeries()
    {
      return std::make_shared<ViewDescription>(ViewDescription{ViewType::CURRENT, boost::none, -1, 0});
    }

    static ViewDescriptionPtr moveNextPhoto()
    {
      return std::make_shared<ViewDescription>(ViewDescription{ViewType::CURRENT, boost::none, 0, 1});
    }

    static ViewDescriptionPtr movePreviousPhoto()
    {
      return std::make_shared<ViewDescription>(ViewDescription{ViewType::CURRENT, boost::none, 0, -1});
    }

    ViewType type;
    boost::optional<QUuid> seriesUuid;
    int seriesOffset;
    int photoOffset;
  };

} // namespace phobos

Q_DECLARE_METATYPE(phobos::ViewDescriptionPtr)

#endif // VIEWDESCRIPTION_H
