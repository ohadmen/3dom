load("//third_party:qt.bzl", "qt_cc_library")

qt_cc_library(
    name = "tree_model",
    src = "tree_model.cpp",
    hdr = "tree_model.h",
    srcs = ['tree_item.cpp'],
    hdrs = ['tree_item.h'],
    visibility = ["//zview:__subpackages__"],
    deps = ["@qt//:qt_gui","//zview/common","//zview/backend:vp_mat","//zview/gui/drawables:drawables_buffer"]
)

