# -*- mode: dockerfile -*-
# vi: set ft=dockerfile :

FROM ubuntu:bionic
EXPOSE 8888/tcp
COPY jupyter_notebook_config.py /root/.jupyter/jupyter_notebook_config.py
ADD drake-latest-bionic.tar.gz /opt
RUN export DEBIAN_FRONTEND=noninteractive \
  && apt-get update -qq \
  && apt-get install --no-install-recommends -o Dpkg::Use-Pty=0 -qy \
    $(cat /opt/drake/share/drake/setup/packages-bionic.txt | tr "\n" " ") \
  && rm -rf /var/lib/apt/lists/*
ENV LD_LIBRARY_PATH="/opt/drake/lib:$LD_LIBRARY_PATH" \
  PATH="/opt/drake/bin:$PATH" \
  PYTHONPATH="/opt/drake/lib/python3.6/site-packages:$PYTHONPATH" \
  SHELL=/bin/bash \
  USER=root
