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

#ifndef AETHER_CLIENT_CONNECTIONS_ISERVER_CONNECTION_FACTORY_H_
#define AETHER_CLIENT_CONNECTIONS_ISERVER_CONNECTION_FACTORY_H_

#include "aether/server.h"
#include "aether/channel.h"
#include "aether/ptr/rc_ptr.h"
#include "aether/client_connections/client_server_connection.h"

namespace ae {
class IServerConnectionFactory {
 public:
  virtual ~IServerConnectionFactory() = default;

  virtual RcPtr<ClientServerConnection> CreateConnection(
      Server::ptr const& server, Channel::ptr const& channel) = 0;
};
}  // namespace ae

#endif  // AETHER_CLIENT_CONNECTIONS_ISERVER_CONNECTION_FACTORY_H_
