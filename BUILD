cc_library(
    name = "rgbe",
    hdrs = ["rgbe.h"],
    srcs = ["rgbe.c"],
)

cc_binary(
    name = "rgbe_to_pfm",
    srcs = ["rgbe_to_pfm.c"],
    deps = [
        ":rgbe",
    ],
)