bin_PROGRAMS = dockmonitortool
dockmonitortool_SOURCES = dockmonitortool.c
AM_CPPFLAGS = ${X11_CFLAGS} ${XI_CFLAGS} ${GTK_CFLAGS} -Wall
LDADD = ${GTK_LIBS} ${X11_LIBS} ${XI_LIBS}
dist-hook:
	emacs README.org --batch -f org-ascii-export-to-ascii --kill
	mv README.txt $(distdir)/README
	echo -e "\n\n\nDistribution date: `date`" >> $(distdir)/README
