#pragma once

#include <deque>
#include <functional>

struct DeletionQueue
{
  void push_function(std::function<void()>&& func) { deletors.push_back(func); }

  void flush()
  {
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)();
    }

    deletors.clear();
  }

  std::deque<std::function<void()>> deletors;
};