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

#include "aether/client_connections/cloud_cache.h"

namespace ae {
std::optional<std::reference_wrapper<Cloud::ptr>> CloudCache::GetCache(
    Uid uid) {
  auto it = clouds_.find(uid);
  if (it == std::end(clouds_)) {
    return {};
  }
  return std::ref(it->second);
}

void CloudCache::AddCloud(Uid uid, Cloud::ptr cloud) {
  clouds_[uid] = std::move(cloud);
}

}  // namespace ae
