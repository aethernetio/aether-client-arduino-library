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

#ifndef EXAMPLES_KEY_LED_KEY_LED_NIX_H_
#define EXAMPLES_KEY_LED_KEY_LED_NIX_H_

#if (defined(__linux__) || defined(__unix__) || defined(__FreeBSD__))
#  define KEY_LED_NIX

#  include <cstdint>

#  include "aether/actions/action.h"
#  include "aether/actions/action_context.h"

const int SCROLL_LOCK = 1, CAPSLOCK = 2, NUMLOCK = 16;

#  define LED_PIN SCROLL_LOCK
#  define BUT_PIN XK_Shift_L
#  define BUT_MASK 0xFFFF

namespace ae {
class KeyLedNix : public Action<KeyLedNix> {
 public:
  explicit KeyLedNix(ActionContext action_context);
  ~KeyLedNix() override;
  TimePoint Update(TimePoint current_time) override;
  bool GetKey(void);
  void SetLed(bool led_state);

 private:
  void InitIOPriv(void);
  bool GetKeyPriv(uint16_t key_pin, uint16_t key_mask);
  void SetLedPriv(uint16_t led_pin, bool led_state);

  TimePoint prev_time_;
  TimePoint::duration key_timeout_;
  bool key_state_{false};
};
}  // namespace ae

#endif
#endif  // EXAMPLES_KEY_LED_KEY_LED_NIX_H_
