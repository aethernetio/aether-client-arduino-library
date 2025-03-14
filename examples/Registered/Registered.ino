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

#include "aether_lib.h"

// This sets Arduino Stack Size - comment this line to use default 8K stack size
SET_LOOP_TASK_STACK_SIZE(16 * 1024);  // 16KB

static constexpr int kWaitTime = 1;
static constexpr int kWaitUntil = 5;

namespace ae::registered {
constexpr ae::SafeStreamConfig kSafeStreamConfig{
  std::numeric_limits<std::uint16_t>::max(),                // buffer_capacity
  (std::numeric_limits<std::uint16_t>::max() / 2) - 1,      // window_size
  (std::numeric_limits<std::uint16_t>::max() / 2) - 1 - 1,  // max_data_size
  10,                                                       // max_repeat_count
  std::chrono::milliseconds{ 600 },                         // wait_confirm_timeout
  {},                                                       // send_confirm_timeout
  std::chrono::milliseconds{ 400 },                         // send_repeat_timeout
};

/**
 * \brief This is the main action to perform all busyness logic here.
 */
class RegisteredAction : public Action<RegisteredAction> {
  enum class State : std::uint8_t {
    kLoad,
    kConfigureSender,
    kSendMessages,
    kWaitDone,
    kResult,
    kError,
  };

public:
  explicit RegisteredAction(Ptr<AetherApp> const &aether_app)
    : Action{ *aether_app }, aether_{ aether_app->aether() },
      state_{ State::kLoad }, messages_{},
      state_changed_{ state_.changed_event().Subscribe(
        [this](auto) {
          Action::Trigger();
        }) } {
    AE_TELED_INFO("Registered test");
  }

  TimePoint Update(TimePoint current_time) override {
    if (state_.changed()) {
      switch (state_.Acquire()) {
        case State::kLoad:
          LoadClients();
          break;
        case State::kConfigureSender:
          ConfigureSender();
          break;
        case State::kSendMessages:
          SendMessages(current_time);
          break;
        case State::kWaitDone:
          break;
        case State::kResult:
          Action::Result(*this);
          return current_time;
        case State::kError:
          Action::Error(*this);
          return current_time;
      }
    }
    // wait till all sent messages received and confirmed
    if (state_.get() == State::kWaitDone) {
      AE_TELED_DEBUG("Wait done receive_count {}, confirm_count {}",
                     receive_count_, confirm_count_);
      if ((receive_count_ == messages_.size()) && (confirm_count_ == messages_.size())) {
        state_ = State::kResult;
      }
      // if no any events happens wake up after 1 second
      return current_time + std::chrono::seconds{ 1 };
    }
    return current_time;
  }

private:
  /**
   * \brief Perform a client registration.
   * We need a two clients for this test.
   */
  void LoadClients() {
    AE_TELED_INFO("Testing loaded clients");
    for (std::size_t i{ 0 }; i < aether_->clients().size(); i++) {
      auto msg_str =
        std::string("Test message for client ") + std::to_string(i);
      messages_.insert(messages_.begin() + i, msg_str);
    }
    state_ = State::kConfigureSender;
  }

  /**
   * \brief Make required preparation to send messages.
   * Create a sender to receiver stream.
   * Subscribe to receiving message event for confirmations \see
   * ConfigureReceiver.
   */
  void ConfigureSender() {
    std::uint8_t clients_cnt{ 0 };

    AE_TELED_INFO("Sender configuration");
    confirm_count_ = 0;
    assert(aether_->clients().size() > 0);

    for (auto const &client : aether_->clients()) {
      auto stream = make_unique<P2pSafeStream>(
        *aether_->action_processor, kSafeStreamConfig,
        make_unique<P2pStream>(*aether_->action_processor, client,
                               client->uid(), StreamId{ clients_cnt }));
      sender_streams_.emplace_back(std::move(stream));
      sender_message_subscriptions_.Push(
        sender_streams_[clients_cnt]->in().out_data_event().Subscribe(
          [&](auto const &data) {
            auto str_response = std::string(
              reinterpret_cast<const char *>(data.data()), data.size());
            AE_TELED_DEBUG("Received a response [{}], confirm_count {}",
                           str_response, confirm_count_);
            confirm_count_++;
          }));
      receiver_message_subscriptions_.Push(
        sender_streams_[clients_cnt]->in().out_data_event().Subscribe(
          [&](auto const &data) {
            auto str_msg = std::string(
              reinterpret_cast<const char *>(data.data()), data.size());
            AE_TELED_DEBUG("Received a message [{}]", str_msg);
            auto confirm_msg = std::string{ "confirmed " } + str_msg;
            auto response_action =
              sender_streams_[receive_count_++]->in().Write(
                { confirm_msg.data(),
                  confirm_msg.data() + confirm_msg.size() },
                ae::Now());
            response_subscriptions_.Push(
              response_action->SubscribeOnError([&](auto const &) {
                AE_TELED_ERROR("Send response failed");
                state_ = State::kError;
              }));
          }));
      clients_cnt++;
    }

    state_ = State::kSendMessages;
  }

  /**
   * \brief Send all messages at once.
   */
  void SendMessages(TimePoint current_time) {
    std::uint8_t messages_cnt{ 0 };

    AE_TELED_INFO("Send messages");

    for (auto const &sender_stream : sender_streams_) {
      auto msg = messages_[messages_cnt++];
      AE_TELED_DEBUG("Sending message {}", msg);
      auto send_action = sender_stream->in().Write(
        DataBuffer{ std::begin(msg), std::end(msg) }, current_time);
      send_subscriptions_.Push(send_action->SubscribeOnError([&](auto const &) {
        AE_TELED_ERROR("Send message failed");
        state_ = State::kError;
      }));
    }

    state_ = State::kWaitDone;
  }

  Aether::ptr aether_;

  std::vector<std::unique_ptr<ae::P2pSafeStream>> sender_streams_{};

  std::size_t clients_registered_;
  std::size_t receive_count_{ 0 };
  std::size_t confirm_count_{ 0 };

  MultiSubscription registration_subscriptions_;
  MultiSubscription receiver_message_subscriptions_;
  MultiSubscription response_subscriptions_;
  MultiSubscription sender_message_subscriptions_;
  MultiSubscription send_subscriptions_;
  StateMachine<State> state_;
  std::vector<std::string> messages_;
  Subscription state_changed_;
};

}  // namespace ae::registered

void AetherRegisteredExample();

static ae::Ptr<ae::AetherApp> aether_app{};
static ae::Subscription success{};
static ae::Subscription failed{};
static std::unique_ptr<ae::registered::RegisteredAction> registered_action{};

///
///\brief Test function.
///\param[in] void
///\return void
///
void AetherRegisteredExample(void) {
  /**
   * Construct a main aether application class.
   * It's include a Domain and Aether instances accessible by getter methods.
   * It has Update, WaitUntil, Exit, IsExit, ExitCode methods to integrate it in
   * your update loop.
   * Also it has action context protocol implementation \see Action.
   * To configure its creation \see AetherAppConstructor.
   */
  aether_app = ae::AetherApp::Construct(ae::AetherAppConstructor{
#if !AE_SUPPORT_REGISTRATION
    []() {
      auto fs =
        ae::make_unique<ae::FileSystemHeaderFacility>(std::string(""));
      return fs;
    }
#endif  // AE_SUPPORT_REGISTRATION
  });

  registered_action =
    ae::make_unique<ae::registered::RegisteredAction>(aether_app);

  success = registered_action->SubscribeOnResult(
    [&](auto const &) {
      aether_app->Exit(0);
    });
  failed = registered_action->SubscribeOnError(
    [&](auto const &) {
      aether_app->Exit(1);
    });
}

void setup() {
  Serial.begin(115200);

  Serial.printf("Arduino Stack was set to %d bytes",
                getArduinoLoopTaskStackSize());
  Serial.println();

  AetherRegisteredExample();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (aether_app->IsExited()) {
    Serial.printf("Exit error code: %d", aether_app->ExitCode());
    Serial.println();
    while (1) {
    };
  }

  auto current_time = ae::Now();
  auto next_time = aether_app->Update(current_time);
  aether_app->WaitUntil(
    std::min(next_time, current_time + std::chrono::seconds{ kWaitUntil }));
  Serial.printf("Arduino Stack was set to %d bytes",
                getArduinoLoopTaskStackSize());
  Serial.println();
}
