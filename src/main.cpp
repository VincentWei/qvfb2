/****************************************************************************
**
** Qt/Embedded virtual framebuffer
**
** Created : 20000605
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about the Professional Edition licensing.
**
*****************************************************************************/

#include <qapplication.h>
#include <stdlib.h>
#include <stdio.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qfiledialog.h>

#include "qvfb.h"

void usage( const char *app )
{
    printf( "Usage: %s [-width width] [-height height] [-depth depth] "
			"[-skin skin-name]\n"
            "[-nocursor] [-qwsdisplay :id]\n"
            "Supported depths: 1, 4, 8, 32\n", app );
}

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv );

    int width = 240;
    int height = 320;
    int depth = 32;
    bool cursor = TRUE;
	int ppid = 0;
    int displayId = 0;
    QString displaySpec( ":0" );
    QString skin ;
    QString caption;
    
    if(argc >= 2)
    {
        ppid = atoi (argv[1]);
        displayId = atoi(argv[1]);
    }

    if(argc >= 3)
        caption = QString("%1").arg(argv[2]);
    else
        caption = QString("%1").arg("QVFB ");
    
    if(argc >= 4)
    {
        width = atoi (argv[3]);
        height = atoi (strchr (argv[3], 'x') + 1);
        depth = atoi (strrchr (argv[3], '-') + 1);
    }
	if(argc >= 5) //skin
	{
		skin=argv[4];
	}

	if(skin.isEmpty() || !QFile::exists(skin))
	{
		//get env
		skin = getenv("QVFB2_SKIN");
	}

    QVFb mw( displayId, ppid, width, height, depth, skin);
    app.setMainWidget( &mw );
    mw.enableCursor(cursor);
    mw.setCaption(caption);
    mw.show();

    return app.exec();
}


