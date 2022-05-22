#!/bin/bash
## build-from-host.sh
## @author gdm85
##
## Use './run-from-container.sh (18|20|21|22)' which will perform the build and then you will have a running container
## with Lotus 1-2-3 within a few seconds.
## Extra command-line arguments after the base image version (18|20|21|22) will be directly passed on to Lotus 1-2-3.
##
## This script has 2 behaviours depending on which symlink is used to invoke it:
## * build-from-host.sh: build Lotus 1-2-3 with Docker or Podman using the specified Ubuntu image
## * run-from-container.sh: same as above but then use a container to run Lotus 1-2-3
##
#

set -e

## find & enter script directory
D=$(dirname "$(readlink -f "$0")")
S=$(basename "$0")

DOCKER=docker
DOCKER_RUN=""
IMAGE="lotus123-builder"
if type podman 2>/dev/null >/dev/null; then
	DOCKER=podman
	DOCKER_RUN="--userns=keep-id"
	IMAGE="localhost/$IMAGE"
fi

if [ $# -ne 1 ]; then
	echo "Usage: ${S} (18|20|21|22)" 1>&2
	echo "Please specify which Ubuntu version to use for the build" 1>&2
	exit 1
fi

cd "$D"

TAG="$1"
shift
DEPS="lib32ncurses-dev"
if [ "$TAG" = 18 ]; then
	UBUNTU_VERSION="18.04"
	DEPS="lib32ncurses5-dev"
elif [ "$TAG" = 20 ]; then
	UBUNTU_VERSION="20.04"
elif [ "$TAG" = 21 ]; then
	UBUNTU_VERSION="21.10"
elif [ "$TAG" = 22 ]; then
	UBUNTU_VERSION="22.04"
else
	echo "ERROR: expected 20, 21 or 22 as Ubuntu version argument" 1>&2
	exit 2
fi
IMAGE="$IMAGE:$TAG"

## needed for runs from container
cp ../l123set.cf .

$DOCKER build -t $IMAGE --build-arg UID=$(id -u) --build-arg GID=$(id -g) \
	--build-arg UBUNTU_VERSION="$UBUNTU_VERSION" --build-arg DEPS="$DEPS" .
rm l123set.cf
cd ..

CONTAINER_RUN="$DOCKER run -ti --rm --tmpfs=/tmp $DOCKER_RUN -v "$PWD":/home/lotus/source -w /home/lotus/source $IMAGE"

$CONTAINER_RUN build-from-container.sh

if [ "$S" = "run-from-container.sh" ]; then
	exec $CONTAINER_RUN ./123 "$@"
fi
