#pragma once

#include "envoy/extensions/filters/http/wasm/v3/wasm.pb.h"
#include "envoy/extensions/filters/http/wasm/v3/wasm.pb.validate.h"

#include "source/extensions/filters/network/dubbo_proxy/filters/factory_base.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace DubboProxy {
namespace DubboFilters {
namespace Wasm {

/**
 * Config registration for the Wasm filter. @see NamedHttpFilterConfigFactory.
 */
class WasmFilterConfig
    : public DubboFilters::FactoryBase<envoy::extensions::filters::http::wasm::v3::Wasm> {
public:
  WasmFilterConfig() : FactoryBase("envoy.filters.dubbo.wasm") {}

private:
  DubboFilters::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::http::wasm::v3::Wasm& proto_config, const std::string&,
      Server::Configuration::FactoryContext& context) override;
};

} // namespace Wasm
} // namespace DubboFilters
} // namespace DubboProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy