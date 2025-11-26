##################################
########## Build Stage ###########
##################################
FROM debian:trixie as build
RUN apt update -y && \
    apt install -y \
        build-essential \
        autoconf \
        automake \
        libtool \
        pkg-config \
        cmake \
        curl \
        git \
        cpio \
        libpcre2-dev \
        zlib1g-dev \
        nlohmann-json3-dev \
        libcli11-dev \
        libsqlitecpp-dev \
        libyaml-cpp-dev \
        libopenblas-dev \
        rapidjson-dev \
        libicu-dev

# Build dependencies
WORKDIR /deps
RUN mkdir installdir

# Build Intgemm
RUN curl -LO https://github.com/kroketio/intgemm/archive/refs/tags/0.0.3.tar.gz
RUN tar -xzf 0.0.3.tar.gz && \
    rm 0.0.3.tar.gz
RUN cd intgemm-0.0.3 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf intgemm-0.0.3

# Build pathie-cpp
RUN curl -LO https://github.com/kroketio/pathie-cpp/archive/refs/tags/0.1.3.tar.gz
RUN tar -xzf 0.1.3.tar.gz && \
    rm 0.1.3.tar.gz
RUN cd pathie-cpp-0.1.3 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf pathie-cpp-0.1.3

# Build Protobuf
RUN curl -LO https://github.com/protocolbuffers/protobuf/archive/refs/tags/v21.12.tar.gz
RUN tar -xvf v21.12.tar.gz && \
    rm v21.12.tar.gz
RUN cd protobuf-21.12 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -Dprotobuf_ABSL_PROVIDER=package -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf protobuf-21.12

# Build sentencepiece
RUN curl -LO https://github.com/kroketio/sentencepiece-browsermt/archive/refs/tags/0.2.tar.gz
RUN tar -xvf 0.2.tar.gz && \
    rm 0.2.tar.gz
RUN cd sentencepiece-browsermt-0.2 && \
    mkdir build && \
    cd build && \
    cmake -DSPM_BUILD_LIBRARY_ONLY=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf sentencepiece-browsermt-0.2

# Build marian-lite
RUN curl -LO https://github.com/terslang/marian-lite/archive/refs/tags/v0.2.9-1.tar.gz
RUN tar -xvf v0.2.9-1.tar.gz && \
    rm v0.2.9-1.tar.gz
RUN cd marian-lite-0.2.9-1 && \
    mkdir build && \
    cd build && \
    cmake -DSTATIC=OFF -DSHARED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf marian-lite-0.2.9-1

# Build Kotki
RUN curl -LO https://github.com/terslang/kotki/archive/refs/tags/v0.6.0-1.tar.gz
RUN tar -xvf v0.6.0-1.tar.gz && \
    rm v0.6.0-1.tar.gz
RUN cd kotki-0.6.0-1 && \
    mkdir build && \
    cd build && \
    cmake -DSTATIC=OFF -DSHARED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/deps/installdir .. && \
    make && \
    make install
RUN rm -rf kotki-0.6.0-1

# Build mecab
RUN curl -LO https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mecab/mecab-0.996.tar.gz
RUN tar -xzf mecab-0.996.tar.gz && \
    rm mecab-0.996.tar.gz
RUN cd mecab-0.996 && \
    # Update config scripts for modern architecture support (from Flatpak manifest)
    cp -p /usr/share/automake-*/config.sub . && \
    cp -p /usr/share/automake-*/config.guess . && \
    ./configure --prefix=/deps/installdir --with-charset=utf-8 --enable-utf8-only && \
    make && \
    make install
RUN rm -rf mecab-0.996

# Build mecab-ipadic
# Note: Needs mecab-config in path or passed explicitly
RUN curl -LO https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mecab/mecab-ipadic-2.7.0-20070801.tar.gz
RUN tar -xzf mecab-ipadic-2.7.0-20070801.tar.gz && \
    rm mecab-ipadic-2.7.0-20070801.tar.gz
RUN cd mecab-ipadic-2.7.0-20070801 && \
    # Update config scripts for modern architecture support (from Flatpak manifest)
    cp -p /usr/share/automake-*/config.sub . && \
    cp -p /usr/share/automake-*/config.guess . && \
    # We must point to the mecab-config we just installed in /deps/installdir
    ./configure --prefix=/deps/installdir --with-charset=utf-8 --with-mecab-config=/deps/installdir/bin/mecab-config && \
    make && \
    make install
RUN rm -rf mecab-ipadic-2.7.0-20070801


# Set ENV variables for compiled deps
ENV PATH="/deps/installdir/bin:$PATH"
ENV CPLUS_INCLUDE_PATH="/deps/installdir/include:$CPLUS_INCLUDE_PATH"
ENV LIBRARY_PATH="/deps/installdir/lib:$LIBRARY_PATH"
ENV LD_LIBRARY_PATH="/deps/installdir/lib:$LD_LIBRARY_PATH"
ENV PKG_CONFIG_PATH="/deps/installdir/lib/pkgconfig:$PKG_CONFIG_PATH"

# Build Glotter
WORKDIR /app
COPY . .
WORKDIR /app/build
RUN cmake -DMODELS_REGISTRY_FILE_PATH=/usr/share/glotter/models/firefox/registry.json -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/deps/installdir .. && \
    make

##################################
########## Final Stage ###########
##################################
FROM debian:trixie-slim as final
RUN apt update -y && \
    apt install -y \
        curl \
        xz-utils \
        nlohmann-json3-dev \
        libyaml-cpp-dev  \
        libicu76 \
        libopenblas0

RUN mkdir -p /deps/installdir

COPY --from=build /deps/installdir /deps/installdir

ENV PATH="/deps/installdir/bin:$PATH"
ENV LD_LIBRARY_PATH="/deps/installdir/lib:$LD_LIBRARY_PATH"
    
WORKDIR /app
COPY --from=build /app/build/glotter .

# Install firefox models
RUN curl -LO https://github.com/terslang/LocalTranslate/releases/download/v0.5.0/firefox-models.tar.xz
RUN mkdir -p /usr/share/glotter/models
RUN tar -Jxf firefox-models.tar.xz -C /usr/share/glotter/models
RUN rm firefox-models.tar.xz

EXPOSE 9107
ENTRYPOINT ["./glotter"]
