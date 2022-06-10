#include "config.h"
#include "dubbo_to_http.h"

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
#include "source/extensions/filters/http/wasm/wasm_filter.h"
#include "source/extensions/filters/network/dubbo_proxy/app_exception.h"
#include "source/extensions/filters/network/dubbo_proxy/filters/factory_base.h"
#include "source/extensions/filters/network/dubbo_proxy/filters/filter_config.h"
#include "source/extensions/filters/network/dubbo_proxy/message_impl.h"


namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace DubboProxy {
namespace DubboFilters {
namespace Wasm {

DubboFilters::FilterFactoryCb WasmFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::wasm::v3::Wasm& proto_config, const std::string&,
    Server::Configuration::FactoryContext& context) {

  // context.api().customStatNamespaces().registerStatNamespace(
  //     Extensions::Common::Wasm::CustomStatNamespace);
  // ENVOY_LOG(info, "get x {}", x);
  auto filter_config =
      std::make_shared<Extensions::HttpFilters::Wasm::FilterConfig>(proto_config, context);
  return [filter_config](DubboFilters::FilterChainFactoryCallbacks& callbacks) -> void {
    auto filter = filter_config->createFilter();
    if (!filter) { // Fail open
      return;
    }
    auto f = std::make_shared<Dubbo2HttpFilter>(filter);
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
