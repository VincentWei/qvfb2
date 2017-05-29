####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qvfb2

MOC=moc
UIC=uic

## INCLUDES were found outside kdevelop specific part

qvfb2_SOURCES = skin.cpp qvfbview.cpp qvfbratedlg.cpp qvfb.cpp qanimationwriter.cpp main.cpp \
	moc_gammaview.cpp  moc_qvfb.cpp  moc_qvfbratedlg.cpp  moc_qvfbview.cpp \
	config.cpp moc_config.cpp \
	config.h  gammaview.h  qanimationwriter.h  qvfb.h  qvfbhdr.h  qvfbratedlg.h  qvfbview.h  skin.h  

qvfb.h:config.h

qvfb.cpp:qvfb.h

config.h:config.ui
	$(UIC) $< -o $@

config.cpp:config.ui
	$(UIC) -impl config.h $< -o $@

moc_%.cpp:%.h
	$(MOC) $< -o $@



SOURCES = skin.cpp qvfbview.cpp qvfbratedlg.cpp qvfb.cpp qanimationwriter.cpp main.cpp \
	moc_gammaview.cpp  moc_qvfb.cpp  moc_qvfbratedlg.cpp  moc_qvfbview.cpp \
	config.h  gammaview.h  qanimationwriter.h  qvfb.h  qvfbhdr.h  qvfbratedlg.h  qvfbview.h  skin.h

qvfb2_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)

EXTRA_DIST = config.ui gammaview.h main.cpp pda_down.png pda.skin pda_up.png qanimationwriter.cpp qanimationwriter.h qvfb.cpp qvfb.h qvfbratedlg.cpp qvfbratedlg.h qvfbview.cpp qvfbview.h README skin.cpp skin.h 

####### kdevelop will overwrite this part!!! (end)############

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qvfb2_METASOURCES = AUTO

# the library search path. 
qvfb2_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qvfb2_METASOURCES)

