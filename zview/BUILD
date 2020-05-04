
cc_binary(
    name = "zview",
    srcs = ["zview.cpp"],
#     copts = [
#     '-fpic',
# ],
    copts = [
    '-g',
],

    deps = [
        "@qt//:qt_widgets",
        "//zview/gui:main_window",
        ],
    visibility = ["//visibility:public"],
)


