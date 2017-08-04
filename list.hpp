#pragma once

#include <utility>

#include "listiterator.hpp"

using std::runtime_error;

template <typename T> T *check_ptr(T *ptr) {
  if (ptr == nullptr) {
    runtime_error("Memory allocation error!");
  }

  return ptr;
}

class List {
public:
  typedef uint16_t T;

private:
  ListItem *start_item;
  ListItem *end_item;
  size_t length;

public:
  typedef ListIterator<true, T> iterator;
  typedef ListIterator<true, const T> const_iterator;
  typedef ListIterator<false, T> reverse_iterator;
  typedef ListIterator<false, const T> const_reverse_iterator;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  List()
      : start_item(check_ptr(new ListItem())),
        end_item(check_ptr(new ListItem())), length(0) {
    start_item->next = end_item;
    end_item->previous = start_item;
  };

  List(const List &other) : List() { copy_from(other); }

  List(List &&other) : List() { move_from(other); }

  ~List() {
    clear();
    delete start_item;
    delete end_item;
  }

  List &operator=(const List &other) {
    copy_from(other);
    return *this;
  }

  List &operator=(List &&other) {
    copy_from(other);
    return *this;
  }

  bool empty() const { return length == 0; }

  size_type size() const { return length; }

  void clear() {
    while (!empty()) {
      pop_front();
    }
  }

  iterator begin() { return iterator(start_item->next, 0); }
  iterator end() { return iterator(end_item, length); }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

  const_iterator cbegin() const { return const_iterator(start_item->next, 0); }
  const_iterator cend() const { return const_iterator(end_item, length); }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end_item->previous, length - 1);
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(start_item, -1);
  }

  T &front() { return start_item->next->value; }
  const T &front() const { return start_item->next->value; }

  T &back() { return end_item->previous->value; }
  const T &back() const { return end_item->previous->value; }

  void push_front(const T &val) {
    check_ptr(new ListItem(val, start_item, start_item->next));
    ++length;
  }

  void push_back(const T &val) {
    check_ptr(new ListItem(val, end_item->previous, end_item));

    ++length;
  }

  T pop_front() {
    if (length == 0) {
      throw runtime_error("Popping empty list!");
    } else {
      --length;

      T result = start_item->next->value;
      delete start_item->next;
      return result;
    }
  }

  T pop_back() {
    if (length == 0) {
      throw runtime_error("Popping empty list!");
    } else {
      --length;

      T result = end_item->previous->value;
      delete end_item->previous;
      return result;
    }
  }

  void move_from(List &other) {
    std::swap(start_item, other.start_item);
    std::swap(end_item, other.end_item);
    std::swap(length, other.length);
  }

  void copy_from(const List &other) {
    clear();
    for (const auto &element : other) {
      push_back(element);
    }
  }
};
