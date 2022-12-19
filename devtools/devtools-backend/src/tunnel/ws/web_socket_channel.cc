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

#include "tunnel/ws/web_socket_channel.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

#include "footstone/logging.h"
#include "footstone/string_utils.h"
#include "tunnel/ws/web_socket_no_tls_client.h"
#include "tunnel/ws/web_socket_tls_client.h"

constexpr char kWssKey[] = "wss";

namespace hippy::devtools {
WebSocketChannel::WebSocketChannel(const std::string& ws_uri) {
  ws_uri_ = ws_uri;
  auto split_result = footstone::StringUtils::SplitString(ws_uri, ":");
  if (split_result.size() > 1 && split_result[0] == kWssKey) {
    need_tls_ = true;
  }
  if (need_tls_) {
    ws_client_ = std::make_shared<WebSocketTlsClient>();
  } else {
    ws_client_ = std::make_shared<WebSocketNoTlsClient>();
  }
}

void WebSocketChannel::Connect(ReceiveDataHandler handler) {
  if (ws_uri_.empty()) {
    FOOTSTONE_DLOG(ERROR) << kDevToolsTag << "websocket uri is empty, connect error";
    return;
  }
  data_handler_ = handler;
  ws_client_->SetNeedsHandler();
  SetNeedsHandlers();
  ws_client_->Connect(ws_uri_);
}

void WebSocketChannel::Send(const std::string& rsp_data) {
  if (ws_client_->GetStatus() != WebSocketStatus::kConnecting) {
    unset_messages_.emplace_back(rsp_data);
    return;
  }
  ws_client_->Send(rsp_data);
}

void WebSocketChannel::Close(int32_t code, const std::string& reason) {
  if (ws_client_->GetStatus() != WebSocketStatus::kConnecting) {
    FOOTSTONE_DLOG(ERROR) << kDevToolsTag << "send message error, handler is null";
    return;
  }
  ws_client_->Close(code, reason);
}

void WebSocketChannel::SetNeedsHandlers() {
  if (!ws_client_) {
    return;
  }
  ws_client_->SetConnectOpenCallback([WEAK_THIS]() {
    DEFINE_AND_CHECK_SELF(WebSocketChannel)
    self->HandleConnectOpen();
  });
  ws_client_->SetConnectFailCallback([WEAK_THIS](int32_t fail_code, const std::string& fail_reason) {
    DEFINE_AND_CHECK_SELF(WebSocketChannel)
    self->HandleConnectFail();
  });
  ws_client_->SetReceiveMessageCallback([WEAK_THIS](const std::string& message) {
    DEFINE_AND_CHECK_SELF(WebSocketChannel)
    self->HandleReceiveMessage(message);
  });
  ws_client_->SetCloseCallback([WEAK_THIS](int32_t close_code, const std::string& close_reason) {
    DEFINE_AND_CHECK_SELF(WebSocketChannel)
    self->HandleClose();
  });
}

void WebSocketChannel::HandleConnectFail() {
  data_handler_ = nullptr;
  unset_messages_.clear();
}

void WebSocketChannel::HandleConnectOpen() {
  if (ws_client_->GetStatus() != WebSocketStatus::kConnecting || unset_messages_.empty()) {
    return;
  }
  for (auto& message : unset_messages_) {
    ws_client_->Send(message);
  }
  unset_messages_.clear();
}

void WebSocketChannel::HandleReceiveMessage(const std::string& message) {
  if (data_handler_) {
    data_handler_(message, hippy::devtools::kTaskFlag);
  }
}

void WebSocketChannel::HandleClose() {
  data_handler_ = nullptr;
  unset_messages_.clear();
}
}  // namespace hippy::devtools
