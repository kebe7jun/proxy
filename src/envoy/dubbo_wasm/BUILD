load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_extension_package",
)

envoy_cc_library(
    name = "dubbo_to_http",
    srcs = ["dubbo_to_http.cc"],
    hdrs = ["dubbo_to_http.h"],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    # external_deps = [
    #     "hessian2_codec_codec_impl",
    #     "hessian2_codec_object_codec_lib",
    # ],
    deps = [
        "@envoy//envoy/registry",
        "@envoy//envoy/http:header_map_interface",
        "@envoy//source/common/common:empty_string",
        "@envoy//source/common/common:logger_lib",
        # "//source/common/config:datasource_lib",
        # "//source/common/chromium_url",
        "@envoy_api//envoy/extensions/filters/http/wasm/v3:pkg_cc_proto",
        "@envoy//source/extensions/filters/network/dubbo_proxy:hessian_utils_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_config_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy:app_exception_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy:message_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy:dubbo_protocol_impl_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy:dubbo_hessian2_serializer_impl_lib",
    ],
)

envoy_cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        ":dubbo_to_http",
        "@envoy//envoy/registry",
        "@envoy//envoy/http:header_map_interface",
        "@envoy//source/common/common:empty_string",
        "@envoy//source/common/common:logger_lib",
        # "//source/common/config:datasource_lib",
        # "//source/common/chromium_url",
        "@envoy//source/extensions/common/wasm:wasm_lib",
        "@envoy_api//envoy/extensions/filters/http/wasm/v3:pkg_cc_proto",
        "@envoy//source/extensions/filters/http/wasm:wasm_filter_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:factory_base_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_config_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy:app_exception_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy:message_lib",
    ],
)
