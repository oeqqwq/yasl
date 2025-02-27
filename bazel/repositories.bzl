load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def yasl_deps():
    _rule_python()
    _rules_foreign_cc()
    _com_github_madler_zlib()
    _com_google_protobuf()
    _com_github_gflags_gflags()
    _com_google_googletest()
    _com_google_absl()
    _com_github_google_leveldb()
    _com_github_brpc_brpc()
    _com_github_fmtlib_fmt()
    _com_github_gabime_spdlog()
    _com_github_google_benchmark()
    _com_github_gperftools_gperftools()
    _com_github_google_cpu_features()
    _com_github_dltcollab_sse2neon()

    # crypto related
    _com_github_openssl_openssl()
    _com_github_floodyberry_curve25519_donna()
    _com_github_blake3team_blake3()
    _com_github_intel_ipp()
    _com_github_libsodium()
    _com_github_emptoolkit_emp_tool()

    maybe(
        git_repository,
        name = "simplest_ot",
        commit = "f40a33a37e3ff8cd81655c35237c177e358dc5b1",
        recursive_init_submodules = True,
        remote = "https://github.com/secretflow/simplest-ot.git",
    )

def _com_github_brpc_brpc():
    maybe(
        http_archive,
        name = "com_github_brpc_brpc",
        sha256 = "f803406a81a82a7d2fef6e0003100682bd98d7b991f16f2e1875dfefc322c90b",
        strip_prefix = "incubator-brpc-1.3.0",
        type = "tar.gz",
        patch_args = ["-p1"],
        patches = [
            "@yasl//bazel:patches/brpc.patch",
        ],
        urls = [
            "https://github.com/apache/incubator-brpc/archive/refs/tags/1.3.0.tar.gz",
        ],
    )

def _com_github_gflags_gflags():
    maybe(
        http_archive,
        name = "com_github_gflags_gflags",
        strip_prefix = "gflags-2.2.2",
        sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
        type = "tar.gz",
        urls = [
            "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
        ],
    )

def _com_github_google_leveldb():
    maybe(
        http_archive,
        name = "com_github_google_leveldb",
        strip_prefix = "leveldb-1.23",
        sha256 = "9a37f8a6174f09bd622bc723b55881dc541cd50747cbd08831c2a82d620f6d76",
        type = "tar.gz",
        build_file = "@yasl//bazel:leveldb.BUILD",
        patch_args = ["-p1"],
        patches = ["@yasl//bazel:patches/leveldb.patch"],
        urls = [
            "https://github.com/google/leveldb/archive/refs/tags/1.23.tar.gz",
        ],
    )

def _com_github_madler_zlib():
    maybe(
        http_archive,
        name = "zlib",
        build_file = "@yasl//bazel:zlib.BUILD",
        strip_prefix = "zlib-1.2.12",
        sha256 = "d8688496ea40fb61787500e863cc63c9afcbc524468cedeb478068924eb54932",
        type = ".tar.gz",
        urls = [
            "https://github.com/madler/zlib/archive/refs/tags/v1.2.12.tar.gz",
        ],
    )

def _com_google_protobuf():
    maybe(
        http_archive,
        name = "com_google_protobuf",
        sha256 = "ba0650be1b169d24908eeddbe6107f011d8df0da5b1a5a4449a913b10e578faf",
        strip_prefix = "protobuf-3.19.4",
        type = "tar.gz",
        urls = [
            "https://github.com/protocolbuffers/protobuf/releases/download/v3.19.4/protobuf-all-3.19.4.tar.gz",
        ],
    )

def _com_google_absl():
    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "4208129b49006089ba1d6710845a45e31c59b0ab6bff9e5788a87f55c5abd602",
        type = "tar.gz",
        strip_prefix = "abseil-cpp-20220623.0",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/refs/tags/20220623.0.tar.gz",
        ],
    )

def _com_github_openssl_openssl():
    maybe(
        http_archive,
        name = "com_github_openssl_openssl",
        sha256 = "0686897afd3a08223760db73d8034550401b53ffc545798d7ca476564f80315e",
        type = "tar.gz",
        strip_prefix = "openssl-OpenSSL_1_1_1q",
        urls = [
            "https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_1_1_1q.tar.gz",
        ],
        build_file = "@yasl//bazel:openssl.BUILD",
    )

def _com_github_fmtlib_fmt():
    maybe(
        http_archive,
        name = "com_github_fmtlib_fmt",
        strip_prefix = "fmt-8.1.1",
        sha256 = "3d794d3cf67633b34b2771eb9f073bde87e846e0d395d254df7b211ef1ec7346",
        build_file = "@yasl//bazel:fmtlib.BUILD",
        urls = [
            "https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz",
        ],
    )

def _com_github_gabime_spdlog():
    maybe(
        http_archive,
        name = "com_github_gabime_spdlog",
        strip_prefix = "spdlog-1.10.0",
        type = "tar.gz",
        sha256 = "697f91700237dbae2326b90469be32b876b2b44888302afbc7aceb68bcfe8224",
        build_file = "@yasl//bazel:spdlog.BUILD",
        urls = [
            "https://github.com/gabime/spdlog/archive/refs/tags/v1.10.0.tar.gz",
        ],
    )

def _com_google_googletest():
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2",
        type = "tar.gz",
        strip_prefix = "googletest-release-1.12.1",
        urls = [
            "https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz",
        ],
    )

def _com_github_google_benchmark():
    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        type = "tar.gz",
        strip_prefix = "benchmark-1.7.0",
        sha256 = "3aff99169fa8bdee356eaa1f691e835a6e57b1efeadb8a0f9f228531158246ac",
        urls = [
            "https://github.com/google/benchmark/archive/refs/tags/v1.7.0.tar.gz",
        ],
    )

def _com_github_gperftools_gperftools():
    maybe(
        http_archive,
        name = "com_github_gperftools_gperftools",
        type = "tar.gz",
        strip_prefix = "gperftools-2.10",
        sha256 = "83e3bfdd28b8bcf53222c3798d4d395d52dadbbae59e8730c4a6d31a9c3732d8",
        urls = [
            "https://github.com/gperftools/gperftools/releases/download/gperftools-2.10/gperftools-2.10.tar.gz",
        ],
        build_file = "@yasl//bazel:gperftools.BUILD",
    )

def _com_github_blake3team_blake3():
    maybe(
        http_archive,
        name = "com_github_blake3team_blake3",
        strip_prefix = "BLAKE3-1.3.0",
        sha256 = "a559309c2dad8cc8314ea779664ec5093c79de2e9be14edbf76ae2ce380222c0",
        build_file = "@yasl//bazel:blake3.BUILD",
        urls = [
            "https://github.com/BLAKE3-team/BLAKE3/archive/refs/tags/1.3.0.tar.gz",
        ],
    )

def _rule_python():
    maybe(
        http_archive,
        name = "rules_python",
        sha256 = "c03246c11efd49266e8e41e12931090b613e12a59e6f55ba2efd29a7cb8b4258",
        strip_prefix = "rules_python-0.11.0",
        urls = [
            "https://github.com/bazelbuild/rules_python/archive/refs/tags/0.11.0.tar.gz",
        ],
    )

def _rules_foreign_cc():
    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
        strip_prefix = "rules_foreign_cc-0.9.0",
        urls = [
            "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/0.9.0.tar.gz",
        ],
    )

def _com_github_floodyberry_curve25519_donna():
    maybe(
        http_archive,
        name = "com_github_floodyberry_curve25519_donna",
        strip_prefix = "curve25519-donna-2fe66b65ea1acb788024f40a3373b8b3e6f4bbb2",
        sha256 = "ba57d538c241ad30ff85f49102ab2c8dd996148456ed238a8c319f263b7b149a",
        type = "tar.gz",
        build_file = "@yasl//bazel:curve25519-donna.BUILD",
        urls = [
            "https://github.com/floodyberry/curve25519-donna/archive/2fe66b65ea1acb788024f40a3373b8b3e6f4bbb2.tar.gz",
        ],
    )

def _com_github_intel_ipp():
    maybe(
        http_archive,
        name = "com_github_intel_ipp",
        sha256 = "23e250dcf281aa00d186be8dc4e34fa8fc5c95a0895694cd00b33f18af5d60c7",
        strip_prefix = "ipp-crypto-ippcp_2021.4",
        build_file = "@yasl//bazel:ipp.BUILD",
        patch_args = ["-p1"],
        patches = ["@yasl//bazel:patches/ippcp.patch"],
        urls = [
            "https://github.com/intel/ipp-crypto/archive/refs/tags/ippcp_2021.4.tar.gz",
        ],
    )

def _com_github_libsodium():
    maybe(
        http_archive,
        name = "com_github_libsodium",
        type = "tar.gz",
        strip_prefix = "libsodium-1.0.18",
        sha256 = "6f504490b342a4f8a4c4a02fc9b866cbef8622d5df4e5452b46be121e46636c1",
        build_file = "@yasl//bazel:libsodium.BUILD",
        urls = [
            "https://download.libsodium.org/libsodium/releases/libsodium-1.0.18.tar.gz",
        ],
    )

def _com_github_google_cpu_features():
    maybe(
        http_archive,
        name = "com_github_google_cpu_features",
        strip_prefix = "cpu_features-0.7.0",
        type = "tar.gz",
        sha256 = "df80d9439abf741c7d2fdcdfd2d26528b136e6c52976be8bd0cd5e45a27262c0",
        build_file = "@yasl//bazel:cpu_features.BUILD",
        patch_args = ["-p1"],
        patches = ["@yasl//bazel:patches/cpu_features.patch"],
        urls = [
            "https://github.com/google/cpu_features/archive/refs/tags/v0.7.0.tar.gz",
        ],
    )

def _com_github_emptoolkit_emp_tool():
    maybe(
        http_archive,
        name = "com_github_emptoolkit_emp_tool",
        sha256 = "217a2cc46f1839efe0f23f6e615fd032094fb53695925be4ca18ae6c7c3e643c",
        strip_prefix = "emp-tool-0.2.3",
        type = "tar.gz",
        patch_args = ["-p1"],
        patches = ["@yasl//bazel:patches/emp-tool.patch"],
        urls = [
            "https://github.com/emp-toolkit/emp-tool/archive/refs/tags/0.2.3.tar.gz",
        ],
        build_file = "@yasl//bazel:emp-tool.BUILD",
    )

def _com_github_dltcollab_sse2neon():
    maybe(
        http_archive,
        name = "com_github_dltcollab_sse2neon",
        sha256 = "4001e2dfb14fcf3831211581ed83bcc83cf6a3a69f638dcbaa899044a351bb2a",
        strip_prefix = "sse2neon-1.5.1",
        type = "tar.gz",
        urls = [
            "https://github.com/DLTcollab/sse2neon/archive/refs/tags/v1.5.1.tar.gz",
        ],
        build_file = "@yasl//bazel:sse2neon.BUILD",
    )
