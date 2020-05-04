
# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "io",
    srcs = ["read_file_list.cpp","write_shape_to_file.cpp"],
    hdrs = ["read_file_list.h","write_shape_to_file.h"],
    deps = ["@qt//:qt_widgets","//zview/io/readers"],
    visibility = ["//zview:__subpackages__"],
)