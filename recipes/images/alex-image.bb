DESCRIPTION = "Image Alex"

IMAGE_INSTALL = "packagegroup-core-boot ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"


IMAGE_INSTALL += "python-core postfix"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image

# remove not needed ipkg informations
ROOTFS_POSTPROCESS_COMMAND += "remove_packaging_data_files ; "
