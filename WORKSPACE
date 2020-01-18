load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# new_local_repository(
#     name = "opencv",
#     path = "/usr/local/",
#     build_file = "third_party/opencv.BUILD",
# )
new_local_repository(
    name = "qt",
    path = "/opt/Qt/5.14.0/gcc_64/include",
    build_file = "third_party/qt.BUILD"
)

git_repository(
    name = "tinyply",
    remote = "https://github.com/ohadmen/tinyply.git",
    branch = "master",
    visibility = ["//libcurie:__subpackages__"],
)