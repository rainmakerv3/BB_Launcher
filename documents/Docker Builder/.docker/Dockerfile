FROM archlinux:latest

RUN pacman-key --init && \
    pacman-key --populate archlinux && \
    pacman -Syu --noconfirm

RUN pacman -S --noconfirm \
    base-devel \
    clang \
    cmake \
	ninja \
    git \
    wget \
    ca-certificates \
    python \
    python-pip \
    alsa-lib \
    libpulse \
    openal \
    openssl \
    zlib \
    libedit \
    systemd-libs \
    libevdev \
    sdl2 \
    jack \
    sndio \
    libxtst \
    vulkan-headers \
    vulkan-validation-layers \
    libpng \
    clang-tools-extra \
    xcb-util-cursor \
    xcb-util-image \
    xcb-util-keysyms \
    xcb-util-renderutil \
    xcb-util-wm \
    fontconfig \
    dbus \
    qt6-base \
    qt6-tools \
    qt6-declarative \
    qt6-webengine \
    qt6-webview \
    qt6-multimedia \
    qt6-websockets \
    qt6-webchannel \
    && pacman -Scc --noconfirm

RUN pacman -S --noconfirm \
    fmt \
    pugixml \
    crypto++ \
    && pacman -Scc --noconfirm

WORKDIR /workspaces/cppsrc