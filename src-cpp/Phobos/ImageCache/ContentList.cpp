#include "ImageCache/ContentList.h"
#include <algorithm>
#include <cassert>

namespace phobos { namespace icache {
namespace {
  struct NameComp
  {
    QString const& name;
    bool operator()(std::pair<QString, unsigned long long> const& p)
    {
      return p.first == name;
    }
  };
} // unnamed namespace

auto ContentList::size() const -> SizeT
{
  return _size;
}

bool ContentList::empty() const
{
  return _content.empty();
}

auto ContentList::insert(QString const& item, SizeT const itemSize) -> SizeT
{
  if (exists(item))
    return _size;

  _content.emplace_back(item, itemSize);
  _size += itemSize;
  return _size;
}

auto ContentList::remove(QString const& item) -> SizeT
{
  auto it = std::find_if(_content.begin(), _content.end(), NameComp{item});
  if (it == _content.end())
    return _size;

  return remove(it);
}

QString ContentList::pop_front()
{
  assert(!_content.empty());

  QString item = _content.front().first;
  remove(_content.begin());
  return item;
}

bool ContentList::exists(QString const& item) const
{
  return std::find_if(_content.begin(), _content.end(), NameComp{item}) != _content.end();
}

auto ContentList::remove(ListT::iterator const it) -> SizeT
{
  assert(it != _content.end());
  _size -= it->second;
  _content.erase(it);
  return _size;
}

}} // namespace phobos::icache
