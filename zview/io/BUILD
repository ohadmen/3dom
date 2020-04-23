
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "io",
    srcs = ["read_file_list.cpp"],
    hdrs = ["read_file_list.h"],
    deps = ["@qt//:qt_widgets","//zview/io/file_types"],
    visibility = ["//zview:__subpackages__"],
)