cc_library(
    name = "Tokens",
    srcs = [
        "Tokens.cpp",
    ],
    hdrs = [
        "Tokens.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "JackDeclarations",
    srcs = [
        "JackDeclarations.cpp",
    ],
    hdrs = [
        "JackDeclarations.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "JackTokenizer",
    srcs = [
        "JackTokenizer.cpp",
    ],
    hdrs = [
        "JackTokenizer.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [":Tokens"],
)

cc_library(
    name = "CompilationEngine",
    srcs = ["CompilationEngine.cpp"],
    hdrs = ["CompilationEngine.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":JackDeclarations",
        ":Tokens",
    ],
)

cc_library(
    name = "JackAnalyzer",
    srcs = ["JackAnalyzer.cpp"],
    hdrs = ["JackAnalyzer.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":CompilationEngine",
        ":JackTokenizer",
    ],
)
