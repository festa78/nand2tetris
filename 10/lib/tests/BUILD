cc_test(
    name = "JackTokenizerTest",
    srcs = ["JackTokenizer.test.cpp"],
    data = [":testdata"],
    deps = [
        "//lib:JackTokenizer",
        "@gtest//:gtest_main",
    ],
)

filegroup(
    name = "testdata",
    srcs = [
        "data/test.jack",
    ],
)
