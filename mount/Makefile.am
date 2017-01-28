AM_CPPFLAGS = \
	$(MATE_APPLETS4_CFLAGS) \
	$(MATEDESKTOP_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	-I$(srcdir) \
	-DMATELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(DISABLE_DEPRECATED_CFLAGS)

APPLET_LOCATION = $(libexecdir)/mount

mount_LDADD = $(MATE_APPLETS4_LIBS) $(MATEDESKTOP_LIBS) $(LIBNOTIFY_LIBS)
libexec_PROGRAMS = mount
mount_applet_SOURCES = mount.c
mount_applet_LDADD = $(MATE_APPLETS4_LIBS) $(MATEDESKTOP_LIBS) $(LIBNOTIFY_LIBS)
mount_applet_CFLAGS = $(WARN_CFLAGS)

appletsdir       = $(datadir)/mate-panel/applets
applets_in_files = org.mate.applets.mountApplet.mate-panel-applet.in
applets_DATA     = $(applets_in_files:.mate-panel-applet.in=.mate-panel-applet)

$(applets_in_files): $(applets_in_files).in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
		$< > $@
%.mate-panel-applet: %.mate-panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

servicedir       = $(datadir)/dbus-1/services
service_in_files = org.mate.panel.applet.mountAppletFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.mate.panel.applet.mountAppletFactory.service: $(service_in_files)
	$(AM_V_GEN)sed \
		-e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
		$< > $@

EXTRA_DIST = \
	$(applets_in_files).in \
	$(service_in_files)

CLEANFILES = $(applets_DATA) $(applets_DATA).in $(service_DATA)
