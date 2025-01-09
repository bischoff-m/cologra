#pragma once

#include <fmt/core.h>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>

using namespace indicators;

template <typename T> class ProgressBarIterator {
  using Iterator = typename std::map<std::string, T>::iterator;

private:
  Iterator _it, _end;
  BlockProgressBar bar;
  int i = 0;

public:
  ProgressBarIterator(Iterator begin, Iterator end)
      : _it(begin), _end(end),
        bar(option::MaxProgress{std::distance(begin, end)}) {
    show_console_cursor(false);
  }

  ~ProgressBarIterator() {
    bar.mark_as_completed();
    show_console_cursor(true);
  }

  Iterator begin() { return _it; }

  Iterator end() { return _end; }

  void finish() {
    bar.mark_as_completed();
    show_console_cursor(true);
  }

  std::pair<const std::string, T> &next() {
    auto &pair = *_it;
    bar.set_option(option::PostfixText{fmt::format(
        " {}/{} {}", i + 1, i + std::distance(_it, _end), pair.first)});
    bar.tick();
    ++_it;
    ++i;
    return pair;
  }
};
