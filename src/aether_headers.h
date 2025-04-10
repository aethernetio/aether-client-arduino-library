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

  #ifndef AETHER_HEADERS_H_
  #define AETHER_HEADERS_H_

  #include "aether/async_for_loop.h"
  #include "aether/server.h"
  #include "aether/crypto.h"
  #include "aether/memory.h"
  #include "aether/cloud.h"
  #include "aether/state_machine.h"
  #include "aether/registration_cloud.h"
  #include "aether/server_keys.h"
  #include "aether/socket_initializer.h"
  #include "aether/common.h"
  #include "aether/mstream_buffers.h"
  #include "aether/ring_buffer.h"
  #include "aether/warning_disable.h"
  #include "aether/env.h"
  #include "aether/address.h"
  #include "aether/fixed_point.h"
  #include "aether/proof_of_work.h"
  #include "aether/config.h"
  #include "aether/literal_array.h"
  #include "aether/aether_tele.h"
  #include "aether/global_ids.h"
  #include "aether/client.h"
  #include "aether/crc.h"
  #include "aether/work_cloud.h"
  #include "aether/aether.h"
  #include "aether/variant_type.h"
  #include "aether/address_parser.h"
  #include "aether/packed_int.h"
  #include "aether/uid.h"
  #include "aether/mstream.h"
  #include "aether/memory_buffer.h"
  #include "aether/config_consts.h"
  #include "aether/aether_app.h"
  #include "aether/channel.h"
  #include "aether/type_traits.h"

  #include "aether/events/delegate.h"
  #include "aether/events/event_handler.h"
  #include "aether/events/events_mt.h"
  #include "aether/events/multi_subscription.h"
  #include "aether/events/event_subscription.h"
  #include "aether/events/events.h"
  #include "aether/events/barrier_event.h"

  #include "aether/statistics/statistic_counter.h"
  #include "aether/statistics/channel_statistics.h"

  #include "aether/adapters/esp32_wifi.h"
  #include "aether/adapters/adapter_tele.h"
  #include "aether/adapters/ethernet.h"
  #include "aether/adapters/adapter_factory.h"
  #include "aether/adapters/proxy.h"
  #include "aether/adapters/adapter.h"
  #include "aether/adapters/parent_wifi.h"

  #include "aether/client_connections/iserver_connection_factory.h"
  #include "aether/client_connections/client_connection.h"
  #include "aether/client_connections/client_to_server_stream.h"
  #include "aether/client_connections/client_connections_tele.h"
  #include "aether/client_connections/cloud_cache.h"
  #include "aether/client_connections/client_server_connection_pool.h"
  #include "aether/client_connections/client_cloud_connection.h"
  #include "aether/client_connections/client_server_connection.h"
  #include "aether/client_connections/client_connection_manager.h"
  #include "aether/client_connections/server_connection_selector.h"

  #include "aether/transport/data_buffer.h"
  #include "aether/transport/transport_tele.h"
  #include "aether/transport/itransport.h"

  #include "aether/transport/low_level/tcp/lwip_tcp.h"
  #include "aether/transport/low_level/tcp/socket_packet_send_action.h"
  #include "aether/transport/low_level/tcp/data_packet_collector.h"
  #include "aether/transport/low_level/tcp/unix_tcp.h"
  #include "aether/transport/low_level/tcp/win_tcp.h"
  #include "aether/transport/low_level/tcp/socket_packet_queue_manager.h"

  #include "aether/transport/actions/channel_connection_action.h"
  #include "aether/transport/actions/packet_send_action.h"
  #include "aether/transport/actions/ip_channel_connection.h"
  #include "aether/transport/actions/name_address_channel_connection.h"

  #include "aether/transport/server/server_channel_stream.h"

  #include "aether/server_list/list_policy.h"
  #include "aether/server_list/server_list.h"
  #include "aether/server_list/no_filter_server_list_policy.h"

  #include "aether/obj/registrar.h"
  #include "aether/obj/obj_id.h"
  #include "aether/obj/registry.h"
  #include "aether/obj/domain.h"
  #include "aether/obj/version_iterator.h"
  #include "aether/obj/obj_ptr.h"
  #include "aether/obj/obj.h"
  #include "aether/obj/dummy_obj.h"
  #include "aether/obj/obj_tele.h"

  #include "aether/api_protocol/api_protocol.h"
  #include "aether/api_protocol/packet_builder.h"
  #include "aether/api_protocol/api_message.h"
  #include "aether/api_protocol/send_result.h"
  #include "aether/api_protocol/request_id.h"
  #include "aether/api_protocol/protocol_context.h"
  #include "aether/api_protocol/child_data.h"

  #include "aether/port/tele_init.h"

  #include "aether/port/file_systems/facility_factory.h"
  #include "aether/port/file_systems/file_system_std.h"
  #include "aether/port/file_systems/file_system_header.h"
  #include "aether/port/file_systems/file_systems_tele.h"
  #include "aether/port/file_systems/file_system_spifs_v2.h"
  #include "aether/port/file_systems/file_system_ram.h"
  #include "aether/port/file_systems/file_system_spifs_v1.h"

  #include "aether/port/file_systems/drivers/driver_header.h"
  #include "aether/port/file_systems/drivers/driver_spifs.h"

  #include "aether/reflect/domain_visitor_impl.h"
  #include "aether/reflect/node_visitor.h"
  #include "aether/reflect/cycle_detector.h"
  #include "aether/reflect/type_index.h"
  #include "aether/reflect/container_node_visitor.h"
  #include "aether/reflect/domain_visitor.h"
  #include "aether/reflect/reflect.h"
  #include "aether/reflect/reflect_node_visitor.h"
  #include "aether/reflect/ptr_like_node_visitor.h"
  #include "aether/reflect/override_func.h"

  #include "aether/ptr/ptr.h"
  #include "aether/ptr/rc_ptr.h"
  #include "aether/ptr/ptr_management.h"
  #include "aether/ptr/storage.h"
  #include "aether/ptr/ptr_view.h"

  #include "aether/client_messages/message_stream.h"
  #include "aether/client_messages/message_stream_dispatcher.h"
  #include "aether/client_messages/p2p_message_stream.h"
  #include "aether/client_messages/client_messages_tele.h"
  #include "aether/client_messages/p2p_safe_message_stream.h"

  #include "aether/format/default_formatters.h"
  #include "aether/format/format.h"
  #include "aether/format/format_time.h"
  #include "aether/format/formatter.h"
  #include "aether/format/format_impl.h"

  #include "aether/tele/tele.h"
  #include "aether/tele/levels.h"
  #include "aether/tele/defines.h"
  #include "aether/tele/tags.h"
  #include "aether/tele/modules.h"
  #include "aether/tele/declaration.h"
  #include "aether/tele/sink.h"
  #include "aether/tele/collectors.h"
  #include "aether/tele/env_collectors.h"

  #include "aether/tele/traps/null_traps.h"
  #include "aether/tele/traps/proxy_trap.h"
  #include "aether/tele/traps/tele_statistics.h"
  #include "aether/tele/traps/io_stream_traps.h"
  #include "aether/tele/traps/statistics_trap.h"

  #include "aether/tele/configs/config_provider.h"
  #include "aether/tele/configs/config_selector.h"
  #include "aether/tele/configs/sink_to_stream.h"
  #include "aether/tele/configs/sink_to_statistics_trap.h"
  #include "aether/tele/configs/sink_null_trap.h"

  #include "aether/tele/env/compilation_options.h"
  #include "aether/tele/env/compiler.h"
  #include "aether/tele/env/platform_type.h"
  #include "aether/tele/env/library_version.h"
  #include "aether/tele/env/cpu_architecture.h"

  #include "aether/actions/timer_action.h"
  #include "aether/actions/action.h"
  #include "aether/actions/notify_action.h"
  #include "aether/actions/action_list.h"
  #include "aether/actions/action_view.h"
  #include "aether/actions/action_registry.h"
  #include "aether/actions/action_trigger.h"
  #include "aether/actions/action_processor.h"
  #include "aether/actions/action_context.h"

  #include "aether/poller/poller.h"
  #include "aether/poller/poller_tele.h"
  #include "aether/poller/win_poller.h"
  #include "aether/poller/epoll_poller.h"
  #include "aether/poller/kqueue_poller.h"
  #include "aether/poller/poller_types.h"
  #include "aether/poller/freertos_poller.h"

  #include "aether/methods/uid_and_cloud.h"
  #include "aether/methods/server_descriptor.h"

  #include "aether/methods/work_server_api/authorized_api.h"
  #include "aether/methods/work_server_api/login_api.h"

  #include "aether/methods/client_api/client_safe_api.h"

  #include "aether/methods/server_reg_api/root_api.h"
  #include "aether/methods/server_reg_api/server_registration_api.h"
  #include "aether/methods/server_reg_api/global_reg_server_api.h"

  #include "aether/methods/client_reg_api/client_global_reg_api.h"
  #include "aether/methods/client_reg_api/client_reg_api.h"

  #include "aether/ae_actions/get_client_cloud_connection.h"
  #include "aether/ae_actions/ping.h"
  #include "aether/ae_actions/get_client_cloud.h"
  #include "aether/ae_actions/ae_actions_tele.h"

  #include "aether/ae_actions/registration/registration_key_provider.h"
  #include "aether/ae_actions/registration/registration.h"

  #include "aether/dns/dns_resolve.h"
  #include "aether/dns/dns_tele.h"
  #include "aether/dns/esp32_dns_resolve.h"
  #include "aether/dns/dns_c_ares.h"

  #include "aether/stream_api/serialize_stream.h"
  #include "aether/stream_api/transport_write_gate.h"
  #include "aether/stream_api/buffer_gate.h"
  #include "aether/stream_api/debug_gate.h"
  #include "aether/stream_api/protocol_stream.h"
  #include "aether/stream_api/istream.h"
  #include "aether/stream_api/tied_stream.h"
  #include "aether/stream_api/one_gate_stream.h"
  #include "aether/stream_api/header_gate.h"
  #include "aether/stream_api/stream_api.h"
  #include "aether/stream_api/crypto_stream.h"
  #include "aether/stream_api/unidirectional_gate.h"
  #include "aether/stream_api/stream_write_action.h"
  #include "aether/stream_api/splitter_gate.h"
  #include "aether/stream_api/safe_stream.h"
  #include "aether/stream_api/sized_packet_stream.h"

  #include "aether/stream_api/safe_stream/sending_chunk_list.h"
  #include "aether/stream_api/safe_stream/sending_data_action.h"
  #include "aether/stream_api/safe_stream/safe_stream_config.h"
  #include "aether/stream_api/safe_stream/safe_stream_sending.h"
  #include "aether/stream_api/safe_stream/safe_stream_receiving.h"
  #include "aether/stream_api/safe_stream/send_data_buffer.h"
  #include "aether/stream_api/safe_stream/safe_stream_api.h"
  #include "aether/stream_api/safe_stream/safe_stream_types.h"

  #include "aether/crypto/key_gen.h"
  #include "aether/crypto/ikey_provider.h"
  #include "aether/crypto/key.h"
  #include "aether/crypto/signed_key.h"
  #include "aether/crypto/crypto_nonce.h"
  #include "aether/crypto/async_crypto_provider.h"
  #include "aether/crypto/crypto_definitions.h"
  #include "aether/crypto/icrypto_provider.h"
  #include "aether/crypto/sync_crypto_provider.h"
  #include "aether/crypto/sign.h"

  #include "aether/crypto/hydrogen/hydro_async_crypto_provider.h"
  #include "aether/crypto/hydrogen/hydro_sync_crypto_provider.h"

  #include "aether/crypto/sodium/sodium_async_crypto_provider.h"
  #include "aether/crypto/sodium/sodium_sync_crypto_provider.h"


  #endif // AETHER_HEADERS_H_
