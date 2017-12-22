#ifndef UTILS_CIRCULATOR_H
#define UTILS_CIRCULATOR_H

namespace phobos { namespace utils {

template<typename BidirectionalIterator>
class Circulator
{
public:
  using ReferencedType = decltype(*std::declval<BidirectionalIterator>());

  Circulator(BidirectionalIterator const begin,
             BidirectionalIterator const end,
             BidirectionalIterator const pos) :
    begin(begin), end(end), pos(pos)
  {}

  bool operator==(Circulator const& rhs) const
  {
    return rhs.pos == pos;
  }

  bool operator!=(Circulator const& rhs) const
  {
    return rhs.pos != pos;
  }

  ReferencedType& operator*()
  {
    return *pos;
  }

  ReferencedType const& operator*() const
  {
    return *pos;
  }

  operator BidirectionalIterator() const
  {
    return pos;
  }

  Circulator& operator++()
  {
    if (begin != end && pos != end)
      pos++;
    if (pos == end)
      pos = begin;
    return *this;
  }

  Circulator& operator--()
  {
    if (pos == begin)
      pos = end;
    if (begin != end)
      pos--;
    return *this;
  }

  Circulator& operator+=(int const distance)
  {
    if (distance >= 0)
      for (int i = 0; i < distance; ++i)
        ++(*this);
    else
      for (int i = 0; i > distance; --i)
        --(*this);
    return *this;
  }

  Circulator operator+(int const distance)
  {
    Circulator result = *this;
    result += distance;
    return result;
  }

private:
  BidirectionalIterator begin;
  BidirectionalIterator end;
  BidirectionalIterator pos;
};

template<typename BidirectionalIterator>
Circulator<BidirectionalIterator> makeCirculator(BidirectionalIterator const begin,
                                                 BidirectionalIterator const end,
                                                 BidirectionalIterator const pos)
{
  return Circulator<BidirectionalIterator>(begin, end, pos);
}

}} // namespace phobos::utils

#endif // UTILS_CIRCULATOR_H
