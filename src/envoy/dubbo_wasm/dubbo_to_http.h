
#include "source/common/common/assert.h"
#include "source/common/common/empty_string.h"
#include "source/common/common/logger.h"
#include "source/common/common/logger_impl.h"
#include "source/extensions/filters/network/dubbo_proxy/app_exception.h"
// #include "source/extensions/filters/network/dubbo_proxy/message_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace DubboProxy {
namespace DubboFilters {
namespace Wasm {

class Dubbo2HttpFilter : public DubboFilters::CodecFilter, Logger::Loggable<Logger::Id::dubbo> {
public:
  Dubbo2HttpFilter(Http::StreamFilterSharedPtr f) : f_(f){};
  void onDestroy() override{};
  void setDecoderFilterCallbacks(DubboFilters::DecoderFilterCallbacks& callbacks) override {
    callbacks_ = &callbacks;
  };

  FilterStatus onMessageDecoded(MessageMetadataSharedPtr, ContextSharedPtr) override;

  // DubboFilter::EncoderFilter
  void setEncoderFilterCallbacks(DubboFilters::EncoderFilterCallbacks& callbacks) override {
    encode_callbacks_ = &callbacks;
  };
  FilterStatus onMessageEncoded(MessageMetadataSharedPtr, ContextSharedPtr ) override;

private:
  DubboFilters::DecoderFilterCallbacks* callbacks_{};
  DubboFilters::EncoderFilterCallbacks* encode_callbacks_{};
  Http::StreamFilterSharedPtr f_;
};
} // namespace Wasm
} // namespace DubboFilters
} // namespace DubboProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy