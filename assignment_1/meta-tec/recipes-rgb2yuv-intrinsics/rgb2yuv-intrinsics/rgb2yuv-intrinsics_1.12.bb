DESCRIPTION = "RGB2YUV INTRINSICS Aplication"
SECTION = "rgb2yuv_intrinsics"
DEPENDS = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"


FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI = "file://rgb2yuv-intrinsics_${PV}.tar.xz"


S = "${WORKDIR}/"

do_install_append(){
	install -d ${D}/rgb2yuv-intrinsics-doc/
	cp ${WORKDIR}/image.bgr /${D}/rgb2yuv-intrinsics-doc/
	cp ${WORKDIR}/output.yuv /${D}/rgb2yuv-intrinsics-doc/
	cp ${WORKDIR}/intrinsics_results.pdf /${D}/rgb2yuv-intrinsics-doc/
}

FILES_${PN} += "/rgb2yuv-intrinsics-doc"


inherit autotools


PARALLEL_MAKE = ""
