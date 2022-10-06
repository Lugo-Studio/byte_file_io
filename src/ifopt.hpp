//
// Created by galex on 10/6/2022.
//

#pragma once

#include <optional>

namespace fx {
  template<typename T, typename Callback>
  auto if_opt(std::optional<T>&& option, Callback&& true_branch) -> bool
  {
    if (option.has_value()) {
      std::forward<decltype(true_branch)>(true_branch)(std::forward<decltype(option.value())>(option.value()));
    }
    return option.has_value();
  }
} // fx