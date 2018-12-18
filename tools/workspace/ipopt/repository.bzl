# -*- mode: python -*-

load(
    "@drake//tools/workspace:pkg_config.bzl",
    "setup_pkg_config_repository",
)

def _impl(repository_ctx):
    result = setup_pkg_config_repository(repository_ctx, ["IPOPT_PATH"])
    if result.error != None:
        fail("Unable to complete pkg-config setup for " +
             "@{} repository: {}".format(
                 repository_ctx.name,
                 result.error,
             ))

ipopt_repository = repository_rule(
    attrs = {
        # TODO(jamiesnape): Pass down licenses to setup_pkg_config_repository.
        # "licenses": attr.string_list(
        #     default = [
        #         "reciprocal",  # CPL-1.0
        #         "unencumbered",  # Public-Domain
        #     ]
        # ),
        "modname": attr.string(default = "ipopt"),
        "pkg_config_paths": attr.string_list(
            default = ["/usr/local/opt/ipopt/lib/pkgconfig"],
        ),
    },
    environ = ["IPOPT_PATH"],
    local = True,
    implementation = _impl,
)
