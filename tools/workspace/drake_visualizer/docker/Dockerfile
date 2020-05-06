# This Dockerfile and the accompanying shell scripts are used by the project
# maintainers to create the precompiled drake-visualizer binaries that are
# downloaded during the build. They are neither called during the build nor
# expected to be called by most developers or users of the project.

ARG CODENAME
FROM vtk-${CODENAME}
COPY dockerfile_build_lcm_binaries /
RUN ./dockerfile_build_lcm_binaries \
  && rm -f dockerfile_build_lcm_binaries
COPY dockerfile_build_drake_visualizer_binaries /
RUN ./dockerfile_build_drake_visualizer_binaries \
  && rm -f dockerfile_build_drake_visualizer_binaries
COPY dockerfile_create_archive /
RUN ./dockerfile_create_archive \
  && rm -f dockerfile_create_archive
