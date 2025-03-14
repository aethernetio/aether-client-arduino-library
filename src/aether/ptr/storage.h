/*
 * Copyright 2024 Aethernet Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AETHER_PTR_STORAGE_H_
#define AETHER_PTR_STORAGE_H_

#include <new>
#include <cstdint>

#include "aether/common.h"

namespace ae {
template <typename T>
class Storage {
 public:
  Storage() noexcept = default;

  AE_CLASS_NO_COPY_MOVE(Storage);

  [[nodiscard]] T const* ptr() const noexcept {
    return std::launder(reinterpret_cast<T const*>(data_));
  }
  [[nodiscard]] T* ptr() noexcept {
    return std::launder(reinterpret_cast<T*>(data_));
  }

 private:
  alignas(alignof(T)) std::uint8_t data_[sizeof(T)];
};
}  // namespace ae

#endif  // AETHER_PTR_STORAGE_H_
