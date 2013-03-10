SECTION = "console/network"
DEPENDS = "openssl virtual/db"
DESCRIPTION = "Generic client/server library for SASL authentication."
LICENSE = "BSD"
PR = "r10"

LIC_FILES_CHKSUM = "file://COPYING;md5=3f55e0974e3d6db00ca6f57f2d206396"

SRC_URI = "ftp://ftp.andrew.cmu.edu/pub/cyrus-mail/cyrus-sasl-${PV}.tar.gz \
	   file://0027_db5_support.diff \
	   "

inherit autotools

acpaths = "-I ${S}/cmulocal -I ${S}/config -I ."
CFLAGS_append = " -I${S}/include -I${S}/saslauthd/include"
TARGET_LDFLAGS_append_thumb = " -lpthread"
EXTRA_OECONF = "--enable-shared --enable-static --with-dblib=berkeley \
	        --with-bdb-libdir=${STAGING_LIBDIR} \
	        --with-bdb-incdir=${STAGING_INCDIR} \
		--without-pam \
		--without-opie --without-des"

FILES_${PN} += "${prefix}/lib/sasl2/*.so*"
FILES_${PN}-dev += "${libdir}/sasl2/*.la ${libdir}/sasl2/*.a"

inherit useradd
USERADD_PACKAGES = "${PN}"
USERADD_PARAM_${PN} = "--system --home-dir /var/spool/mail -g mail \
                       --no-create-home --shell /bin/false cyrus;"
GROUPADD_PARAM_${PN} = "-r mail"

do_configure_prepend () {
	rm -f acinclude.m4 config/libtool.m4
}

do_compile_prepend () {
	cd include
	${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS} makemd5.c -o makemd5
	touch makemd5.o makemd5.lo makemd5
	cd ..
}

do_install () {
	oe_libinstall -so -C lib libsasl2 ${D}${libdir}
	install -d ${STAGING_LIBDIR}/sasl2
	install -d ${STAGING_INCDIR}/sasl
	install -m 0644 ${S}/include/hmac-md5.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/md5.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/md5global.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/sasl.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/saslplug.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/saslutil.h ${STAGING_INCDIR}/sasl/
	install -m 0644 ${S}/include/prop.h ${STAGING_INCDIR}/sasl/
}

do_install_append_class-target () {
#	chgrp mail ${D}/etc/sasldb2
}

pkg_postinst_${PN} () {
	#!/bin/sh -e
	if [ x"$D" = "x" ]; then
       echo "cyrus" | saslpasswd2 -p -c cyrus
       chgrp mail /etc/sasldb2
	else
		exit 1
	fi
}


SRC_URI[md5sum] = "341cffe829a4d71f2a6503d669d5a946"
SRC_URI[sha256sum] = "418c16e6240a4f9b637cbe3d62937b9675627bad27c622191d47de8686fe24fe"
