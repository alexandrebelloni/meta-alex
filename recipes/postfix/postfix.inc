SECTION = "console/network"
DEPENDS = "virtual/db libpcre postfix-native"
DEPENDS_virtclass-native = "virtual/db-native libpcre-native"
LICENSE = "IPL"

LIC_FILES_CHKSUM = "file://LICENSE;md5=64375f37431336ea1b1b3005fe3fa354"

INC_PR = "r1"

SRC_URI = "ftp://ftp.porcupine.org/mirrors/postfix-release/official/postfix-${PV}.tar.gz;name=postfix-${PV} \
file://makedefs.patch \
file://main.cf \
file://volatiles \
file://postfix \
file://internal_recipient \
file://postfix_install.patch \
"

S = "${WORKDIR}/postfix-${PV}"

BBCLASSEXTEND = "native"

inherit update-rc.d

INITSCRIPT_NAME = "postfix"
INITSCRIPT_PARAMS = "start 58 3 4 5 . stop 13 0 1 6 ."

inherit useradd
USERADD_PACKAGES = "${PN}"
USERADD_PARAM_${PN} = "--system --home-dir /var/spool/postfix -g postfix \
                       --no-create-home --shell /bin/false postfix;"
USERADD_PARAM_${PN} += "--system --home-dir /var/spool/postfix -g vmail \
                       --no-create-home --shell /bin/false vmail"
GROUPADD_PARAM_${PN} = "-r postfix; -r vmail; -r postdrop"

#FIXME: this is broken because the native build won't work on systems where
# native bdb does not require libpthread.  ARM doesn't require libpthread
# because it uses an assembler mutex implementation.
LIBBDB_EXTRA = "-lpthread"
LIBBDB_EXTRA_arm = ""
LIBBDB_EXTRA_armeb = ""

export SYSLIBS = "-lpcre -ldb ${LIBBDB_EXTRA} -lnsl -lresolv ${LDFLAGS}"
export EXPORT = "AUXLIBS='-lpcre' CCARGS='-DHAS_PCRE ${CFLAGS}' OPT='' DEBUG='-g'"
export CC_append = " -DHAS_PCRE ${CFLAGS}"
export EXTRA_OEMAKE = "-e"
export POSTCONF = "${STAGING_DIR_NATIVE}/usr/sbin/postconf"

do_compile () {
	unset CFLAGS CPPFLAGS CXXFLAGS
	oe_runmake makefiles
	oe_runmake
}

do_install_prepend_virtclass-native() {
	export POSTCONF="bin/postconf"
}

do_install () {
	sh ./postfix-install 'install_root=${D}' \
	'config_directory=${sysconfdir}/postfix' \
	'daemon_directory=${libexecdir}/postfix' \
	'command_directory=${sbindir}' \
	'queue_directory=${localstatedir}/spool/postfix' \
	'sendmail_path=${sbindir}/sendmail.postfix' \
	'newaliases_path=${bindir}/newaliases' \
	'mailq_path=${bindir}/mailq' \
	'manpage_directory=${mandir}' \
	'readme_directory=${datadir}/doc/postfix' \
	'data_directory=${localstatedir}/lib/postfix' \
	-non-interactive
	rm -rf ${D}${localstatedir}/spool/postfix
	mv ${D}${sysconfdir}/postfix/main.cf ${D}${sysconfdir}/postfix/sample-main.cf
	install -d ${D}${localstatedir}/tmp
	install -d ${D}${sysconfdir}/default/volatiles
	install -d ${D}${sysconfdir}/init.d
	install -m 644 ${WORKDIR}/main.cf ${D}${sysconfdir}/
	install -m 644 ${WORKDIR}/volatiles ${D}${sysconfdir}/default/volatiles/01_postfix
	install -m 755 ${WORKDIR}/postfix ${D}${sysconfdir}/init.d/postfix
	install -m 644 ${WORKDIR}/internal_recipient ${D}${sysconfdir}/postfix/internal_recipient
}

do_install_append_class-target () {
	chgrp postdrop ${D}${sbindir}/postqueue
	chgrp postdrop ${D}${sbindir}/postdrop
	chgrp postdrop ${D}${sbindir}/postqueue
	chmod g+s ${D}${sbindir}/postqueue
	chmod g+s ${D}${sbindir}/postdrop
}

NATIVE_INSTALL_WORKS = "1"

# TODO
pkg_postinst_${PN} () {
	#!/bin/sh -e

	if [ "x$D" != "x" ] ; then
		exit 1
	fi

	touch ${D}/etc/aliases
	echo POUET
	update-alternatives --install ${sbindir}/sendmail sendmail sendmail.${PN} 40
	[ -d /var/spool/postfix ] && rmdir /var/spool/postfix
	/etc/init.d/populate-volatile.sh update
	newaliases
}

pkg_postrm_${PN} () {
	update-alternatives --remove sendmail sendmail.${PN}
}

# Exclude .debug directories from the main package
FILES_${PN} = "${sysconfdir} ${localstatedir} ${bindir}/* ${sbindir}/* \
	${libexecdir}/postfix/*"
FILES_${PN}-dbg += "${libexecdir}/postfix/.debug"
