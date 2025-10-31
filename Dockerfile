# Use the same Ubuntu image GitHub Actions uses
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install all project dependencies (build tools, test tools, docs)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gcc \
    clang-format \
    cppcheck \
    valgrind \
    doxygen \
    graphviz \
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-fonts-recommended \
    texlive-latex-extra \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# (We will copy files and run builds using commands,
#  not by default in the image)