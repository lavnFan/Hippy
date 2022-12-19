/*
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2017-2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "tunnel/ws/web_socket_base_client.h"

namespace hippy::devtools {
using WSClient = websocketpp::client<websocketpp::config::asio_client>;
/**
 * @brief websocket client without tls
 */
class WebSocketNoTlsClient : public WebSocketBaseClient, public std::enable_shared_from_this<WebSocketNoTlsClient> {
 public:
  WebSocketNoTlsClient();
  void SetNeedsHandler() override;
  void Connect(const std::string& ws_uri) override;
  void Send(const std::string& rsp_data) override;
  void Close(int32_t code, const std::string& reason) override;

 private:
  void HandleSocketInit(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectFail(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectOpen(const websocketpp::connection_hdl& handle);
  void HandleSocketConnectMessage(const websocketpp::connection_hdl& handle, const WSMessagePtr& message_ptr);
  void HandleSocketConnectClose(const websocketpp::connection_hdl& handle);

  WSClient ws_client_;
  WSThread ws_thread_;
  websocketpp::connection_hdl connection_hdl_;
};
}  // namespace hippy::devtools
