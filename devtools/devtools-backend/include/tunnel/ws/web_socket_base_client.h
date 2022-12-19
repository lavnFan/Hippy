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

#include <functional>
#include "footstone/logging.h"
#include "api/devtools_define.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Werror"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wnull-pointer-subtraction"
#pragma clang diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#pragma clang diagnostic ignored "-Wunused-variable"
#define ASIO_STANDALONE
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/common/connection_hdl.hpp"
#pragma clang diagnostic pop

enum class WebSocketStatus { kUnknown, kConnecting, kClose };

namespace hippy::devtools {
using WSMessagePtr = websocketpp::config::asio_client::message_type::ptr;
using WSThread = websocketpp::lib::shared_ptr<websocketpp::lib::thread>;
using WSInitCallback = std::function<void()>;
using WSConnectOpenCallback = std::function<void()>;
using WSConnectFailCallback = std::function<void(int32_t fail_code, const std::string& fail_reason)>;
using WSReceiveMessageCallback = std::function<void(const std::string& message)>;
using WSCloseCallback = std::function<void(int32_t close_code, const std::string& close_reason)>;
/**
 * @brief base websocket client
 */
class WebSocketBaseClient {
 public:
  virtual void SetNeedsHandler() = 0;
  virtual void Connect(const std::string& ws_uri) = 0;
  virtual void Send(const std::string& rsp_data) = 0;
  virtual void Close(int32_t code, const std::string& reason) = 0;
  virtual ~WebSocketBaseClient() = default;

  inline void SetInitCallback(WSInitCallback call_back) { init_call_back_ = call_back; }
  inline WSInitCallback GetInitCallback() { return init_call_back_; }

  inline void SetConnectOpenCallback(WSConnectOpenCallback call_back) { open_call_back_ = call_back; }
  inline WSConnectOpenCallback GetConnectOpenCallback() { return open_call_back_; }

  inline void SetConnectFailCallback(WSConnectFailCallback call_back) { fail_call_back_ = call_back; }
  inline WSConnectFailCallback GetConnectFailCallback() { return fail_call_back_; }

  inline void SetReceiveMessageCallback(WSReceiveMessageCallback call_back) { message_call_back_ = call_back; }
  inline WSReceiveMessageCallback GetReceiveMessageCallback() { return message_call_back_; }

  inline void SetCloseCallback(WSCloseCallback call_back) { close_call_back_ = call_back; }
  inline WSCloseCallback GetCloseCallback() { return close_call_back_; }

  inline WebSocketStatus GetStatus() { return status_; }
  inline void SetStatus(WebSocketStatus status) { status_ = status; }

 private:
  WebSocketStatus status_;
  WSInitCallback init_call_back_;
  WSConnectOpenCallback open_call_back_;
  WSConnectFailCallback fail_call_back_;
  WSReceiveMessageCallback message_call_back_;
  WSCloseCallback close_call_back_;
};
}  // namespace hippy::devtools

namespace asio::detail {
/**
 * implement asio no-exception function
 */
template <typename Exception>
inline void throw_exception(const Exception& e) {
  FOOTSTONE_DLOG(ERROR) << hippy::devtools::kDevToolsTag << " asio exception:" << e.what();
}
}  // namespace asio::detail