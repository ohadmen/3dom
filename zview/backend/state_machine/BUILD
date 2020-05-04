load("//third_party:qt.bzl", "qt_cc_library")

qt_cc_library(
    name = "state_machine",
    src = "state_machine.cpp",
    hdr = "state_machine.h",
    srcs = glob(["track*.cpp"]),
    hdrs = glob(["track*.h"]),
    visibility = ["//zview:__subpackages__"],
    deps = ["@qt//:qt_gui","//zview/common","//zview/backend:vp_mat","//zview/gui/drawables:drawables_buffer"]
)

