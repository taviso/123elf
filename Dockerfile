FROM debian:bookworm-slim as stage

RUN apt-get update 

RUN apt-get install -y \
    wget \
    build-essential \
    gcc-multilib \
    lib32ncurses-dev \
    cpio \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY . .

RUN wget https://archive.org/download/123-unix/123UNIX1.IMG \
	&& wget https://archive.org/download/123-unix/123UNIX2.IMG \
	&& wget https://archive.org/download/123-unix/123UNIX3.IMG \
	&& wget https://archive.org/download/123-unix/123UNIX4.IMG \
	&& wget https://archive.org/download/123-unix/123UNIX5.IMG

RUN ./binutils.sh \
	&& ./extract.sh

RUN make install \
	&& cd / \
	&& rm -rf /build

WORKDIR /work

