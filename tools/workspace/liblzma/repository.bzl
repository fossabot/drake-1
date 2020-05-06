# -*- mode: python -*-

load(
    "@drake//tools/workspace:pkg_config.bzl",
    "pkg_config_repository",
)

def liblzma_repository(
        name,
        licenses = ["unencumbered"],  # Public-Domain
        modname = "liblzma",
        atleast_version = "5.2.2",
        pkg_config_paths = ["/usr/local/opt/xz/lib/pkgconfig"],
        **kwargs):
    pkg_config_repository(
        name = name,
        licenses = licenses,
        modname = modname,
        atleast_version = atleast_version,
        pkg_config_paths = pkg_config_paths,
        **kwargs
    )
