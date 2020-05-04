load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

new_local_repository(
    name = "qt",
    path = "/usr/include/x86_64-linux-gnu/qt5/",
    build_file = "third_party/qt.BUILD"
)

git_repository(
    name = "tinyply",
    remote = "https://github.com/ohadmen/tinyply.git",
    commit = "84d9ef245f7baae84767cc841e7ba653e10b09d1",
    shallow_since = "1573773597 +0100"
)