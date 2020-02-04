load("//third_party:qt.bzl", "qt_cc_library")

qtdeps = [
        "@qt//:qt_core",
        "@qt//:qt_widgets",
        "@qt//:qt_gui",
    ]

qt_cc_library(
    name = "main_window",
    src = "main_window.cpp",
    hdr = "main_window.h",
    copts = [
        '-fpic',
    ],
    deps = qtdeps+[":canvas"],
    visibility = ["//zview:__subpackages__"],
)
qt_cc_library(
    name = "canvas",
    src = "canvas.cpp",
    hdr = "canvas.h",
    deps = qtdeps+["//zview/gui/drawables:backdrop","//zview/shaders:TARGETNAME"] ,
    visibility = ["//zview:__subpackages__"],
)
