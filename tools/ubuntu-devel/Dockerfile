FROM        ubuntu:xenial
LABEL description="Basic Ubuntu Xenial development environment with GCC 8"

# update and install dependencies
RUN         apt-get update \
                && apt-get install -y \
                    software-properties-common \
                    wget \
                && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
                && apt-get update \
                && apt-get install -y \
                    make \
                    git \
                    curl \
                && apt-get install -y cmake=3.5.1-1ubuntu3 \
                && apt-get install -y \
                    gcc-8 g++-8 gcc-8-base \
                && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 \
                && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 100
