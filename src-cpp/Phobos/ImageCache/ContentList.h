#ifndef IMAGECACHE_CONTENTLIST_H
#define IMAGECACHE_CONTENTLIST_H

#include <QString>
#include <list>
#include <utility>

namespace phobos { namespace icache {

class ContentList
{
public:
  using SizeT = unsigned long long;

  explicit ContentList() = default;

  SizeT size() const;
  bool empty() const;

  SizeT insert(QString const& item, SizeT const itemSize);
  SizeT remove(QString const& item);
  QString pop_front();
  bool exists(QString const& item) const;

private:
  using ListT = std::list<std::pair<QString, SizeT>>;
  SizeT remove(ListT::iterator const it);

  ListT _content;
  SizeT _size = 0;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_CONTENTLIST_H
