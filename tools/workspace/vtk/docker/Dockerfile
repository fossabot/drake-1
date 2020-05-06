# This Dockerfile and the accompanying shell scripts are used by the project
# maintainers to create the precompiled vtk binaries that are downloaded during
# the build. They are neither called during the build nor expected to be called
# by most developers or users of the project.

ARG CODENAME
FROM ubuntu:${CODENAME}
COPY dockerfile_install_ispc /
RUN ./dockerfile_install_ispc \
  && rm -f dockerfile_install_ispc
COPY dockerfile_build_embree_binaries /
RUN ./dockerfile_build_embree_binaries \
  && rm -f dockerfile_build_embree_binaries
COPY dockerfile_build_ospray_binaries /
COPY ospray_*.patch /
RUN ./dockerfile_build_ospray_binaries \
  && rm -f dockerfile_build_ospray_binaries /usr/local/bin/ispc
COPY dockerfile_build_vtk_binaries /
COPY vtk_*.patch /
RUN ./dockerfile_build_vtk_binaries \
  && rm -f dockerfile_build_vtk_binaries
COPY dockerfile_create_archive /
RUN ./dockerfile_create_archive \
  && rm -f dockerfile_create_archive
