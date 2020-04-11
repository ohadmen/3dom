load("//third_party:qt.bzl", "qt_cc_library")
qtdeps = [
        "@qt//:qt_core",
        "@qt//:qt_widgets",
        "@qt//:qt_gui",
    ]
qt_cc_library(
    name = "backdrop",
    src = "backdrop.cpp",
    hdr = "backdrop.h",
    deps = qtdeps+["//zview/common","//zview/shaders"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
    

)
