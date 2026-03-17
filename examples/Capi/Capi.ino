/*
 * Copyright 2026 Aethernet Inc.
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

void PrintUid(CUid const *uid) {
  int i = 0;
  for (; i < 4; i++) {
    Serial.printf("%02x", uid->value[i]);
  }
  Serial.printf("-");
  for (; i < 6; i++) {
    Serial.printf("%02x", uid->value[i]);
  }
  Serial.printf("-");
  for (; i < 8; i++) {
    Serial.printf("%02x", uid->value[i]);
  }
  Serial.printf("-");
  for (; i < 10; i++) {
    Serial.printf("%02x", uid->value[i]);
  }
  Serial.printf("-");
  for (; i < 16; i++) {
    Serial.printf("%02x", uid->value[i]);
  }
}

#define WIFI_SSID "test"
#define WIFI_PASSWORD "test"

#define TEST_UID "3ac93165-3d37-4970-87a6-fa4ee27744e4"

void ClientSelected(AetherClient *client, void *user_data) {
  CUid uid;
  GetClientUid(client, &uid);
  Serial.printf(">>> Loaded clieint with uid:");
  PrintUid(&uid);
  Serial.printf("<<<\n");
}

void MessageSentCb(ActionStatus status, void *user_data) {
  // close app
  AetherExit(0);
}

void MessageReceived(AetherClient *client, CUid sender, void const *data,
                     size_t size, void *user_data) {
  Serial.printf(">>> Received message size: %zu text: %s\n", size,
                (char const *)data);
  // send message and wait till it sent
  char const *message = "Hello!";
  SendStr(sender, message, MessageSentCb, NULL);
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Arduino Stack was set to %d bytes",
                getArduinoLoopTaskStackSize());
  Serial.println();

  // This init aether library
  // Send a message and wait for response
  AeWifiAdapterConf wifi_adapter_conf = {
      .type = AeWifiAdapter,
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD,
  };
  AdapterBase *adapter_confs = (AdapterBase *)&wifi_adapter_conf;
  Adapters adapters = {
      .count = 1,
      .adapters = &adapter_confs,
  };

  ClientConfig client_config = {
      .id = "ground control",
      .parent_uid = CUidFromString(TEST_UID),
      .client_selected_cb = ClientSelected,
      .message_received_cb = MessageReceived,
  };

  AetherConfig aether_config = {
      .adapters = &adapters,
      .default_client = &client_config,
  };

  AetherInit(&aether_config);
}

void loop() {
  if (AetherExcited() != AE_NOK) {
    return;
  }
  uint64_t time = AetherUpdate();
  AetherWait(time);
}
