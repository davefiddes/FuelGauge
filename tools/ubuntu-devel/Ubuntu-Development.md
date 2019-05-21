# Ubuntu Xenial Development Environment

This directory contains a Dockerfile which emulates the development environment used by Travis-CI. It should make it easier to debug issues locally on non-Ubuntu Xenial machines.

## Building

    docker build --tag=fuelgauge tools/ubuntu-devel/

## Running

    docker run  -it -v "$(pwd):/src" --entrypoint bash fuelgauge

## Developing

    cd /src
    mkdir ubuntu-build
    cd ubuntu-build
    cmake ..
    make all test
