TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += app daemon

app.file = app.pro
daemon.file = daemon/daemon.pro
