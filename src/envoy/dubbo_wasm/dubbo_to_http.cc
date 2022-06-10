#include "dubbo_to_http.h"
#include "envoy/buffer/buffer.h"
#include "envoy/http/header_map.h"
#include "envoy/network/connection.h"
#include "envoy/registry/registry.h"
#include "source/common/common/assert.h"
#include "source/common/common/empty_string.h"
#include "source/common/common/logger.h"
#include "source/common/common/logger_impl.h"
// #include "source/extensions/filters/network/dubbo_proxy/hessian_utils.h"
#include "source/extensions/filters/network/dubbo_proxy/app_exception.h"
#include "source/extensions/filters/network/dubbo_proxy/message_impl.h"
#include "source/extensions/filters/network/dubbo_proxy/dubbo_protocol_impl.h"
#include "source/extensions/filters/network/dubbo_proxy/dubbo_hessian2_serializer_impl.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <type_traits>

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace DubboProxy {
namespace DubboFilters {
namespace Wasm {

std::string str2hex(const std::string& input) {
  static const char hex_digits[] = "0123456789ABCDEF";

  std::string output;
  output.reserve(input.length() * 2);
  for (unsigned char c : input) {
    output.push_back(hex_digits[c >> 4]);
    output.push_back(hex_digits[c & 15]);
  }
  return output;
}

FilterStatus Dubbo2HttpFilter::onMessageDecoded(MessageMetadataSharedPtr p, ContextSharedPtr ctx) {
  const auto invocation = dynamic_cast<const DubboProxy::RpcInvocationImpl*>(&p->invocationInfo());

  auto h = Http::RequestHeaderMapImpl::create();
  h->setPath(p->invocationInfo().serviceName());
  h->setMethod(p->invocationInfo().methodName());
  auto c = callbacks_;
  invocation->attachment().headers().iterate(
      [&h, c](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
        ENVOY_CONN_LOG(error, "dubbo header before: {}={}", *c->connection(),
                       e.key().getStringView(), e.value().getStringView());
        // h->setByKey(e.key().getStringView(), e.value().getStringView());
        const Http::LowerCaseString k(e.key().getStringView());
        // const Http::LowerCaseString v(e.value().getStringView());
        h->setCopy(k, e.value().getStringView());
        return Http::HeaderMap::Iterate::Continue;
      });

  ENVOY_CONN_LOG(error, "dubbo header size: {}, body size {}", *callbacks_->connection(),
                 ctx->headerSize(), ctx->bodySize());
  if (callbacks_->connection()->state() != Envoy::Network::Connection::State::Open) {
    ENVOY_CONN_LOG(error, "connection not open", *callbacks_->connection());
  }
  auto s = f_->decodeHeaders(*h, callbacks_->connection()->state() != Envoy::Network::Connection::State::Open);

  if (s != Http::FilterHeadersStatus::Continue) {
    return FilterStatus::StopIteration;
  }

  h->iterate([c, invocation](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
    ENVOY_CONN_LOG(error, "dubbo header after: {}={}", *c->connection(), e.key().getStringView(),
                   e.value().getStringView());
    const std::string& key = std::string(e.key().getStringView());
    const std::string& value = std::string(e.value().getStringView());
    invocation->mutableAttachment()->insert(key, value);
    return Http::HeaderMap::Iterate::Continue;
  });
  auto data_status = f_->decodeData(ctx->originMessage(), callbacks_->connection()->state() != Envoy::Network::Connection::State::Open);
  if (data_status != Http::FilterDataStatus::Continue) {
    return FilterStatus::StopIteration;
  }

  return FilterStatus::Continue;
};

FilterStatus Dubbo2HttpFilter::onMessageEncoded(MessageMetadataSharedPtr, ContextSharedPtr ctx) {
  // const auto invocation =
  //     dynamic_cast<const DubboProxy::RpcInvocationImpl*>(&p->invocationInfo());

  auto h = Http::ResponseHeaderMapImpl::create();
  Http::LowerCaseString pt{":protocol"};
  h->appendCopy(pt, "dubbo");
  // invocation->attachment().headers().iterate(
  //     [&h](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
  //       Http::LowerCaseString hh{e.key().getStringView()};
  //       h->appendCopy(hh, e.value().getStringView());
  //       // h->setByKey(e.key().getStringView(), e.value().getStringView());
  //       return Http::HeaderMap::Iterate::Continue;
  //     });
  // f_->encodeHeaders(*h, false);
  auto s = f_->encodeHeaders(*h, false);
  if (s != Http::FilterHeadersStatus::Continue) {
    return FilterStatus::StopIteration;
  }
  f_->encodeData(ctx->originMessage(), false);
  ENVOY_CONN_LOG(info, "dubbo encode message after, {}", *callbacks_->connection(),
                 ctx->originMessage().toString());
  ENVOY_CONN_LOG(info, "dubbo response header size: {}, body size {}", *callbacks_->connection(),
                 ctx->headerSize(), ctx->bodySize());
  return FilterStatus::Continue;
};

} // namespace Wasm
} // namespace DubboFilters
} // namespace DubboProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy