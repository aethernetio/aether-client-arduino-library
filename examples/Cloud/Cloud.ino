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

#define CLOUD_TEST_MODEM 0
#define CLOUD_TEST_ESP_WIFI 1

// This sets Arduino Stack Size - comment this line to use default 8K stack size
SET_LOOP_TASK_STACK_SIZE(16 * 1024); // 16KB

namespace ae::cloud_test {
static constexpr int kWaitTime = 1;
static constexpr int kWaitUntil = 5;

static constexpr std::string_view kWifiSsid = "Test1234";
static constexpr std::string_view kWifiPass = "Test1234";

static constexpr std::string_view kSerialPortModem =
    "UART0"; // Modem serial port
SerialInit serial_init_modem = {std::string(kSerialPortModem),
                                kBaudRate::kBaudRate115200};

ae::ModemInit const modem_init{
    serial_init_modem,            // Serial port
    {},                           // Power save parameters
    {},                           // Base station
    {1, 1, 1, 1},                 // Pin code
    false,                        // Use pin
    ae::kModemMode::kModeNbIot,   // Modem mode
    "00001",                      // Operator code
    "",                           // Operator long name
    "internet",                   // APN
    "user",                       // APN user
    "password",                   // APN pass
    ae::kAuthType::kAuthTypeNone, // Auth type
    false,                        // Use auth
    "",                           // Auth user
    "",                           // Auth pass
    "",                           // SSL cert
    false                         // Use SSL
};

constexpr ae::SafeStreamConfig kSafeStreamConfig{
    std::numeric_limits<std::uint16_t>::max(),               // buffer_capacity
    (std::numeric_limits<std::uint16_t>::max() / 2) - 1,     // window_size
    (std::numeric_limits<std::uint16_t>::max() / 2) - 1 - 1, // max_data_size
    10,                                                      // max_repeat_count
    std::chrono::milliseconds{600}, // wait_confirm_timeout
    {},                             // send_confirm_timeout
    std::chrono::milliseconds{400}, // send_repeat_timeout
};

ae::RcPtr<AetherApp> construct_aether_app() {
  return AetherApp::Construct(
      AetherAppContext{}.AdaptersFactory([](AetherAppContext const &context) {
        auto adapter_registry =
            context.domain().CreateObj<ae::AdapterRegistry>();
#if CLOUD_TEST_ESP_WIFI == 1
        adapter_registry->Add(context.domain().CreateObj<ae::WifiAdapter>(
            ae::GlobalId::kWiFiAdapter, context.aether(), context.poller(),
            context.dns_resolver(), std::string(kWifiSsid),
            std::string(kWifiPass)));
#endif
#if CLOUD_TEST_MODEM == 1
        adapter_registry->Add(context.domain().CreateObj<ae::ModemAdapter>(
            ae::GlobalId::kModemAdapter, context.aether(), modem_init));
#endif
        return adapter_registry;
      }));
}

} // namespace ae::cloud_test

void AetherCloudExample();

static ae::RcPtr<ae::AetherApp> aether_app{};

///
///\brief Test function.
///\param[in] void
///\return void
///
void AetherCloudExample(void) {
  /**
   * Construct a main aether application class.
   * It's include a Domain and Aether instances accessible by getter methods.
   * It has Update, WaitUntil, Exit, IsExit, ExitCode methods to integrate it in
   * your update loop.
   * Also it has action context protocol implementation \see Action.
   * To configure its creation \see AetherAppContext.
   */
  aether_app = ae::cloud_test::construct_aether_app();

  /**
   * Start clients selection or registration.
   * Clients might be loaded from data storage saved during previous run
   * or Registered if not found.
   */
  ae::Client::ptr client_a;
  ae::Client::ptr client_b;

  auto select_client_a = aether_app->aether()->SelectClient(
      ae::Uid::FromString("3ac93165-3d37-4970-87a6-fa4ee27744e4"), 0);
  select_client_a->StatusEvent().Subscribe(ae::ActionHandler{
      ae::OnResult{[&](auto const &action) { client_a = action.client(); }},
      ae::OnError{[&]() { aether_app->Exit(1); }}});

  auto select_client_b = aether_app->aether()->SelectClient(
      ae::Uid::FromString("3ac93165-3d37-4970-87a6-fa4ee27744e4"), 1);
  select_client_b->StatusEvent().Subscribe(ae::ActionHandler{
      ae::OnResult{[&](auto const &action) { client_b = action.client(); }},
      ae::OnError{[&]() { aether_app->Exit(1); }}});

  aether_app->WaitActions(select_client_a, select_client_b);

  // clients must be selected
  assert(client_a && client_b);

  // Make clients messages exchange.
  int received_count = 0;
  int confirmed_count = 0;

  /**
   * Make required preparation for receiving messages.
   * Subscribe to opening new stream event.
   * Subscribe to receiving message event.
   * Send confirmation to received message.
   */
  std::unique_ptr<ae::ByteIStream> receiver_stream;
  client_a->message_stream_manager().new_stream_event().Subscribe(
      [&](auto p2p_stream) {
        receiver_stream = ae::make_unique<ae::P2pSafeStream>(
            *aether_app, ae::cloud_test::kSafeStreamConfig,
            std::move(p2p_stream));

        receiver_stream->out_data_event().Subscribe([&](auto const &data) {
          auto str_msg = std::string(
              reinterpret_cast<const char *>(data.data()), data.size());
          AE_TELED_DEBUG("Received a message [{}]", str_msg);
          received_count++;
          auto confirm_msg = std::string{"confirmed "} + str_msg;
          auto response_action = receiver_stream->Write(
              {confirm_msg.data(), confirm_msg.data() + confirm_msg.size()});
          response_action->StatusEvent().Subscribe(ae::OnError{[&]() {
            AE_TELED_ERROR("Send response failed");
            aether_app->Exit(1);
          }});
        });
      });

  /**
   * Make required preparation to send messages.
   * Create a sender to receiver stream.
   * Subscribe to receiving message event for confirmations.
   */
  auto sender_stream = ae::make_unique<ae::P2pSafeStream>(
      *aether_app, ae::cloud_test::kSafeStreamConfig,
      ae::MakeRcPtr<ae::P2pStream>(*aether_app, client_b, client_a->uid()));

  sender_stream->out_data_event().Subscribe([&](auto const &data) {
    auto str_response =
        std::string(reinterpret_cast<const char *>(data.data()), data.size());
    AE_TELED_DEBUG("Received a response [{}], confirm_count {}", str_response,
                   confirmed_count);
    confirmed_count++;
  });

  AE_TELED_INFO("Send messages");
  auto messages = std::array<std::string, 9>{
      "Hello, it's me",
      "I was wondering if, after all these years, you'd like to meet",
      "To go over everything",
      "They say that time's supposed to heal ya",
      "But I ain't done much healin'",
      "Hello, can you hear me?",
      "I'm in California dreaming about who we used to be",
      "When we were younger and free",
      "I've forgotten how it felt before the world fell at our feet"};

  for (auto const &msg : messages) {
    auto send_action =
        sender_stream->Write(ae::DataBuffer{std::begin(msg), std::end(msg)});
    send_action->StatusEvent().Subscribe(ae::OnError{[&](auto const &) {
      AE_TELED_ERROR("Send message failed");
      aether_app->Exit(1);
    }});
  }
}

void setup() {
  Serial.begin(115200);

  Serial.printf("Arduino Stack was set to %d bytes",
                getArduinoLoopTaskStackSize());
  Serial.println();

  AetherCloudExample();
}

/**
 * Application loop.
 * All the asynchronous actions are updated on this loop.
 * WaitUntil either waits until the next selected time or some action
 * triggers new event.
 */
void loop() {
  // put your main code here, to run repeatedly:
  if (aether_app->IsExited()) {
    Serial.printf("Exit error code: %d", aether_app->ExitCode());
    Serial.println();
    while (1) {
    };
  }

  // Wait for next event or timeout
  auto current_time = ae::Now();
  auto next_time = aether_app->Update(current_time);
  aether_app->WaitUntil(std::min(
      next_time,
      current_time + std::chrono::seconds{ae::cloud_test::kWaitUntil}));
  Serial.printf("Arduino Stack was set to %d bytes",
                getArduinoLoopTaskStackSize());
  Serial.println();
}
