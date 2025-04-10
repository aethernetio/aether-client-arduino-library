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

#ifndef AETHER_ACTIONS_ACTION_CONTEXT_H_
#define AETHER_ACTIONS_ACTION_CONTEXT_H_

#include "aether/actions/action_registry.h"
#include "aether/actions/action_trigger.h"
#include "aether/actions/action_processor.h"

namespace ae {
class ActionContext {
 public:
  ActionContext(ActionProcessor& processor);

  ActionTrigger& get_trigger();
  ActionRegistry& get_registry();

 private:
  ActionProcessor* processor_;
};
}  // namespace ae

#endif  // AETHER_ACTIONS_ACTION_CONTEXT_H_
