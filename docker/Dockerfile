
ARG UBUNTU_VERSION
FROM ubuntu:$UBUNTU_VERSION

MAINTAINER gdm85@users.noreply.github.com

RUN export DEBIAN_FRONTEND=noninteractive && export TERM=xterm && apt-get update && \
        apt-get install -y build-essential gcc-multilib lib32ncurses-dev cpio file \
        wget texinfo && \
	rm -rf /var/lib/apt/lists/*

ARG GID=1000
ARG UID=1000

RUN groupadd --gid=$GID lotus && useradd --gid=$GID --uid=$GID --create-home --shell /bin/bash lotus

COPY l123set.cf /home/lotus/.l123set

RUN chown lotus.lotus /home/lotus/.l123set

COPY build-from-container.sh /usr/local/bin/

USER lotus