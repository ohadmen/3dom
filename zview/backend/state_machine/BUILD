# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
cc_library(
    name = "state_machine",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    visibility = ["//libcurie:__subpackages__"],
    deps = ["@qt//:qt_gui","//libcurie/common","//libcurie/drawables:drawables_buffer","//libcurie/drawables:drawable_basic_shapes"]
)