cc_test(
    name = "TokensTest",
    srcs = [
        "Tokens.test.cpp",
    ],
    deps = [
        "//lib:Tokens",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "JackTokenizerTest",
    srcs = [
        "JackTokenizer.test.cpp",
    ],
    data = [":testdata"],
    deps = [
        "//lib:JackTokenizer",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "CompilationEngineTest",
    srcs = [
        "CompilationEngine.test.cpp",
    ],
    data = [":testdata"],
    deps = [
        "//lib:CompilationEngine",
        "@gtest//:gtest_main",
    ],
)

filegroup(
    name = "testdata",
    srcs = [
        "data/test.jack",
    ],
)
