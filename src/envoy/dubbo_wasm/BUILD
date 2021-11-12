load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_cc_test",
)

# Public docs: docs/root/configuration/http_filters/wasm_filter.rst

envoy_cc_library(
    name = "wasm_filter_lib",
    srcs = ["wasm_filter.cc"],
    hdrs = ["wasm_filter.h"],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        "@envoy//envoy/http:codes_interface",
        "@envoy//envoy/server:filter_config_interface",
        "@envoy//envoy/upstream:cluster_manager_interface",
        "@envoy//source/extensions/common/wasm:wasm_lib",
        "@envoy_api//envoy/extensions/filters/http/wasm/v3:pkg_cc_proto",
    ],
)

envoy_cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        ":wasm_filter_lib",
        "@envoy//envoy/registry",
        "@envoy//envoy/http:header_map_interface",
        "@envoy//source/common/common:empty_string",
        "@envoy//source/common/common:logger_lib",
        # "@envoy//source/common/config:datasource_lib",
        # "@envoy//source/common/chromium_url",
        "@envoy//source/extensions/common/wasm:wasm_lib",
        "@envoy_api//envoy/extensions/filters/http/wasm/v3:pkg_cc_proto",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:factory_base_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy/filters:filter_config_interface",
        "@envoy//source/extensions/filters/network/dubbo_proxy:app_exception_lib",
        "@envoy//source/extensions/filters/network/dubbo_proxy:message_lib",
    ],
)
