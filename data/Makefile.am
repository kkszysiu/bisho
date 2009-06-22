desktopdir = $(datadir)/applications
desktop_in_files = bisho.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

icondir = $(datadir)/icons/hicolor/48x48/apps
dist_icon_DATA = bisho.png

CLEANFILES = $(desktop_DATA)
EXTRA_DIST = $(desktop_in_files)
