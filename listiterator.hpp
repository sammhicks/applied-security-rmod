#pragma once

#include <iterator>
#include <stdexcept>

#include "listitem.hpp"

template <bool is_forwards, typename RT>
class ListIterator
    : public std::iterator<std::bidirectional_iterator_tag, ListItem::T> {
private:
  ListItem *item;
  ptrdiff_t position;

  void do_move(bool my_forwards) {
    if (my_forwards == is_forwards) {
      if (item->next == nullptr) {
        throw std::range_error("cannot increment past end of list!");
      }
      item = item->next;
      ++position;
    } else {
      if (item->previous == nullptr) {
        throw std::range_error("cannot increment past end of list!");
      }
      item = item->previous;
      --position;
    }
  }

public:
  ListIterator<is_forwards, RT>(ListItem *item, ptrdiff_t position)
      : item(item), position(position){};

  ListIterator<is_forwards, RT> &operator++() {
    do_move(true);
    return *this;
  }

  ListIterator<is_forwards, RT> operator++(int) {
    ListIterator<is_forwards, RT> tmp(*this);
    do_move(true);
    return tmp;
  }

  ListIterator<is_forwards, RT> &operator--() {
    do_move(false);
    return *this;
  }

  ListIterator<is_forwards, RT> operator--(int) {
    ListIterator<is_forwards, RT> tmp(*this);
    do_move(false);
    return tmp;
  }

  template <typename OT>
  bool operator==(const ListIterator<is_forwards, OT> &rhs) const {
    return item == rhs.item;
  }

  template <typename OT>
  bool operator!=(const ListIterator<is_forwards, OT> &rhs) const {
    return item != rhs.item;
  }

  template <typename OT>
  bool operator<(const ListIterator<is_forwards, OT> &rhs) const {
    return position < rhs.position;
  }

  template <typename OT>
  bool operator<=(const ListIterator<is_forwards, OT> &rhs) const {
    return position <= rhs.position;
  }

  template <typename OT>
  bool operator>(const ListIterator<is_forwards, OT> &rhs) const {
    return position > rhs.position;
  }

  template <typename OT>
  bool operator>=(const ListIterator<is_forwards, OT> &rhs) const {
    return position >= rhs.position;
  }

  template <typename OT>
  ptrdiff_t operator-(const ListIterator<is_forwards, OT> &rhs) const {
    return position - rhs.position;
  }

  RT &operator*() {
    if ((item->previous == nullptr) || (item->next == nullptr)) {
      throw std::range_error("Cannot resolve iterator! Off end!");
    } else {
      return item->value;
    }
  }

  const ListItem::T &operator*() const {
    return const_cast<const ListItem::T &>(item->value);
  }

  operator ListIterator<is_forwards, const ListItem::T>() {
    return ListIterator<is_forwards, const ListItem::T>(item, position);
  }
};
