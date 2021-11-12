#include "config.h"

#include "envoy/buffer/buffer.h"
#include "envoy/extensions/filters/http/wasm/v3/wasm.pb.h"
#include "envoy/extensions/filters/http/wasm/v3/wasm.pb.validate.h"
#include "envoy/http/header_map.h"
#include "envoy/network/connection.h"
#include "envoy/registry/registry.h"

// #include "source/common/chromium_url/envoy_shim.h"
#include "source/common/common/assert.h"
#include "source/common/common/empty_string.h"
#include "source/common/common/logger.h"
#include "source/common/common/logger_impl.h"
#include "source/extensions/filters/network/dubbo_proxy/app_exception.h"
#include "source/extensions/filters/network/dubbo_proxy/filters/factory_base.h"
#include "source/extensions/filters/network/dubbo_proxy/filters/filter_config.h"
#include "source/extensions/filters/network/dubbo_proxy/message_impl.h"

#include "wasm_filter.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace DubboProxy {
namespace DubboFilters {
namespace Wasm {

class TestFilter : public DubboFilters::CodecFilter, Logger::Loggable<Logger::Id::dubbo> {
public:
  TestFilter(Http::StreamFilterSharedPtr f) : f_(f){};
  void onDestroy() override{};
  void setDecoderFilterCallbacks(DubboFilters::DecoderFilterCallbacks& callbacks) override {
    callbacks_ = &callbacks;
  };

  FilterStatus onMessageDecoded(MessageMetadataSharedPtr p, ContextSharedPtr ctx) override {
    const auto invocation =
        dynamic_cast<const DubboProxy::RpcInvocationImpl*>(&p->invocationInfo());

    auto h = Http::RequestHeaderMapImpl::create();
    h->setPath(p->invocationInfo().serviceName());
    h->setMethod(p->invocationInfo().methodName());
    invocation->attachment().headers().iterate(
        [&h](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
          h->setByKey(e.key().getStringView(), e.value().getStringView());
          return Http::HeaderMap::Iterate::Continue;
        });

    auto s = f_->decodeHeaders(*h, false);
    auto call = callbacks_;
    h->iterate([&call](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
      ENVOY_CONN_LOG(info, "dubbo wasm header after: {}: {}", *call->connection(),
                     e.key().getStringView(), e.value().getStringView());
      return Http::HeaderMap::Iterate::Continue;
    });
    if (s != Http::FilterHeadersStatus::Continue) {
      return FilterStatus::StopIteration;
    }
    f_->decodeData(ctx->originMessage(), false);
    ENVOY_CONN_LOG(info, "dubbo decode message after, {}", *callbacks_->connection(),
                   ctx->originMessage().toString());
    return FilterStatus::Continue;
  };

  // DubboFilter::EncoderFilter
  void setEncoderFilterCallbacks(DubboFilters::EncoderFilterCallbacks&) override{

  };
  FilterStatus onMessageEncoded(MessageMetadataSharedPtr, ContextSharedPtr ctx) override {
    // const auto invocation =
    //     dynamic_cast<const DubboProxy::RpcInvocationImpl*>(&p->invocationInfo());

    // auto h = Http::ResponseHeaderMapImpl::create();
    // invocation->attachment().headers().iterate(
    //     [&h](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
    //       Http::LowerCaseString hh{e.key().getStringView()};
    //       h->appendCopy(hh, e.value().getStringView());
    //       // h->setByKey(e.key().getStringView(), e.value().getStringView());
    //       return Http::HeaderMap::Iterate::Continue;
    //     });
    // f_->encodeHeaders(*h, false);
    // auto call = callbacks_;
    // h->iterate([&call](const Http::HeaderEntry& e) -> Http::HeaderMap::Iterate {
    //   ENVOY_CONN_LOG(info, "dubbo response wasm header after: {}: {}", *call->connection(),
    //                  e.key().getStringView(), e.value().getStringView());
    //   return Http::HeaderMap::Iterate::Continue;
    // });
    f_->encodeData(ctx->originMessage(), false);
    ENVOY_CONN_LOG(info, "dubbo encode message after, {}", *callbacks_->connection(),
                   ctx->originMessage().toString());
    return FilterStatus::Continue;
  };

private:
  DubboFilters::DecoderFilterCallbacks* callbacks_{};
  Http::StreamFilterSharedPtr f_;
};

DubboFilters::FilterFactoryCb WasmFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::wasm::v3::Wasm& proto_config, const std::string&,
    Server::Configuration::FactoryContext& context) {

  context.api().customStatNamespaces().registerStatNamespace(
      Extensions::Common::Wasm::CustomStatNamespace);
  // ENVOY_LOG(info, "get x {}", x);
  auto filter_config = std::make_shared<FilterConfig>(proto_config, context);
  return [filter_config](DubboFilters::FilterChainFactoryCallbacks& callbacks) -> void {
    auto filter = filter_config->createFilter();
    if (!filter) { // Fail open
      return;
    }
    auto f = std::make_shared<TestFilter>(filter);
    callbacks.addFilter(f);
  };
}

/**
 * Static registration for the echo2 filter. @see RegisterFactory.
 */
// static Registry::RegisterFactory<Echo2ConfigFactory, NamedDubboFilterConfigFactory> registered_;
REGISTER_FACTORY(WasmFilterConfig, DubboFilters::NamedDubboFilterConfigFactory);

} // namespace Wasm
} // namespace DubboFilters
} // namespace DubboProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
