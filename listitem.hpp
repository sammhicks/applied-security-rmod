#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

class ListItem {
public:
  typedef uint16_t T;

  T value;

  ListItem *previous;
  ListItem *next;

  ListItem() : ListItem(T(), nullptr, nullptr) {}

  ListItem(T value, ListItem *previous, ListItem *next)
      : value(value), previous(previous), next(next) {
    if (previous != nullptr) {
      previous->next = this;
    }

    if (next != nullptr) {
      next->previous = this;
    }
  }

  ~ListItem() {
    ListItem *old_previous = previous;
    ListItem *old_next = next;

    if (previous != nullptr) {
      previous->next = old_next;
    }

    if (next != nullptr) {
      next->previous = old_previous;
    }
  }
};
