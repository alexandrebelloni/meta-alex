SECTION = "console/network"
DESCRIPTION = "Open2300 is a package of software tools that reads (and writes) data from a Lacrosse WS2300/WS2305/WS2310/WS2315 Weather Station."
LICENSE = "GPL"
PR = "r1"

LIC_FILES_CHKSUM = "file://COPYING;md5=8ca43cbc842c2336e835926c2166c28b"

SRCREV = "12"

SRC_URI = "svn://www.lavrsen.dk/svn/open2300/;module=trunk;protocol=http \
	   file://date_format.patch"

S = "${WORKDIR}/trunk"

OPEN2300_BINS = "open2300 dump2300 log2300 fetch2300 wu2300 cw2300 history2300 \
               histlog2300 bin2300 xml2300 light2300 interval2300 minmax2300"

do_compile () {
	make CC="${CC}" LD="${LD}" ${OPEN2300_BINS}
}

do_install () {
	for prog in ${OPEN2300_BINS}; do
		install -D -m 0755 ${S}/$prog ${D}${bindir}/$prog ;
	done
}

FILES_${PN} = "${bindir}/*"

