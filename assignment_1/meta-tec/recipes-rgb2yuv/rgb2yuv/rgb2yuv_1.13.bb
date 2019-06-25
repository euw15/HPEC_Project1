DESCRIPTION = "RGB2YUV Aplication"
SECTION = "rgb2yuv"
DEPENDS = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"


FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"


SRC_URI = "file://rgb2yuv_${PV}.tar.xz"

S = "${WORKDIR}/"

do_install_append(){
	install -d ${D}/rgb2yuv-doc/
	cp ${WORKDIR}/image.bgr /${D}/rgb2yuv-doc/
	cp ${WORKDIR}/output.yuv /${D}/rgb2yuv-doc/
	cp ${WORKDIR}/c_results.pdf /${D}/rgb2yuv-doc/
}

FILES_${PN} += "/rgb2yuv-doc"




inherit autotools


PARALLEL_MAKE = ""
