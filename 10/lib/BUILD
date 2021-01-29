cc_library(
    name = "JackTokenizer",
    srcs = ["JackTokenizer.cpp"],
    hdrs = ["JackTokenizer.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "JackAnalyzer",
    srcs = ["JackAnalyzer.cpp"],
    hdrs = ["JackAnalyzer.hpp"],
    visibility = ["//visibility:public"],
    deps = [":JackTokenizer"],
)
