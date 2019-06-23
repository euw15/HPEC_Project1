DESCRIPTION = "RGB2YUV Aplication"
SECTION = "rgb2yuv"
DEPENDS = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

# This tells bitbake where to find the files we're providing on the local filesystem
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

# Use local tarball
SRC_URI = "file://rgb2yuv_1.2.tar.xz"

# Make sure our source directory (for the build) matches the directory structure in the tarball
S = "${WORKDIR}/rgb2yuv"

inherit autotools

# The autotools configuration I am basing this on seems to have a problem with a race condition when parallel make is enabled
PARALLEL_MAKE = ""
