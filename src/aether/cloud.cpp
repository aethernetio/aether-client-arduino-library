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

#include "aether/cloud.h"

#include <utility>

#include "aether/tele/tele.h"

namespace ae {

#ifdef AE_DISTILLATION
Cloud::Cloud(Domain* domain) : Obj{domain} {}
#endif  // AE_DISTILLATION

void Cloud::AddServer(Server::ptr const& server) {
  servers_.push_back(server);
  servers_.back().SetFlags(ObjFlags::kUnloadedByDefault);
}

void Cloud::LoadServer(Server::ptr& server) {
  if (!server) {
    domain_->LoadRoot(server);
    assert(server);
  }
}

void Cloud::set_adapter(Adapter::ptr const& adapter) { adapter_ = adapter; }
Adapter::ptr& Cloud::adapter() { return adapter_; }

std::vector<Server::ptr>& Cloud::servers() { return servers_; }

}  // namespace ae
