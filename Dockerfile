FROM rootproject/root:6.34.00-ubuntu24.04

LABEL name="hdtv"

# TODO: When updating to a ubuntu26.04 based image, the ENV
#       can be removed by giving pipx the `--global` option.
#       https://pipx.pypa.io/stable/installation/
ENV PIPX_HOME=/opt/pipx \
    PIPX_BIN_DIR=/usr/local/bin

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends pipx && \
    rm -rf /var/lib/apt/lists/*

COPY . /hdtv-src

RUN pipx install /hdtv-src && \
    hdtv --rebuild-sys && \
    rm -rf /hdtv-src

WORKDIR /work
CMD hdtv
