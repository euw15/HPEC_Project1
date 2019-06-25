DESCRIPTION = "TEC IMAGE" 
SUMMARY = "Custom Image called TEC IMAGE"

require ../../../meta-rpi/images/console-image.bb

IMAGE_INSTALL += "rgb2yuv"
IMAGE_INSTALL += "rgb2yuv-intrinsics"


export IMAGE_BASENAME = "tec-image"
