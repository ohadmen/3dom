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
    deps = qtdeps+["drawable_base"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
 
)

cc_library(
    name = "drawables_buffer",
    srcs = ["drawables_buffer.cpp"],
    hdrs = ["drawables_buffer.h"],
    deps = qtdeps+["drawable_mesh","drawable_edges","drawable_pcl"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
 
)


cc_library(
    name = "drawable_base",
    srcs = ["drawable_base.cpp"],
    hdrs = ["drawable_base.h"],
    deps = qtdeps+["//zview/common","//zview/shaders"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
    

)

qt_cc_library(
    name = "drawable_mesh",
    src = "drawable_mesh.cpp",
    hdr = "drawable_mesh.h",
    deps = qtdeps+["drawable_base"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
 
)

qt_cc_library(
    name = "drawable_edges",
    src = "drawable_edges.cpp",
    hdr = "drawable_edges.h",
    deps = qtdeps+["drawable_base"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
 
)



qt_cc_library(
    name = "drawable_pcl",
    src = "drawable_pcl.cpp",
    hdr = "drawable_pcl.h",
    deps = qtdeps+["drawable_base","//zview/common"],
    visibility = ["//zview:__subpackages__"],
        copts = [
        '-fpic',
    ],
 
)

