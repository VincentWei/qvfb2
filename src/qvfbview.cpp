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

#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#include <unistd.h>
#include <fcntl.h>

#include <qapplication.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qpainter.h>

#include "qanimationwriter.h"
#include "qvfbview.h"
#include "qvfbhdr.h"

#define QTE_PIPE "QtEmbedded-%1"

typedef int BOOL;
#define SCANCODE_USER                   (NR_KEYS + 1)

#define SCANCODE_ESCAPE                 1

#define SCANCODE_1                      2
#define SCANCODE_2                      3
#define SCANCODE_3                      4
#define SCANCODE_4                      5
#define SCANCODE_5                      6
#define SCANCODE_6                      7
#define SCANCODE_7                      8
#define SCANCODE_8                      9
#define SCANCODE_9                      10
#define SCANCODE_0                      11

#define SCANCODE_MINUS                  12
#define SCANCODE_EQUAL                  13

#define SCANCODE_BACKSPACE              14
#define SCANCODE_TAB                    15

#define SCANCODE_Q                      16
#define SCANCODE_W                      17
#define SCANCODE_E                      18
#define SCANCODE_R                      19
#define SCANCODE_T                      20
#define SCANCODE_Y                      21
#define SCANCODE_U                      22
#define SCANCODE_I                      23
#define SCANCODE_O                      24
#define SCANCODE_P                      25
#define SCANCODE_BRACKET_LEFT           26
#define SCANCODE_BRACKET_RIGHT          27

#define SCANCODE_ENTER                  28

#define SCANCODE_LEFTCONTROL            29

#define SCANCODE_A                      30
#define SCANCODE_S                      31
#define SCANCODE_D                      32
#define SCANCODE_F                      33
#define SCANCODE_G                      34
#define SCANCODE_H                      35
#define SCANCODE_J                      36
#define SCANCODE_K                      37
#define SCANCODE_L                      38
#define SCANCODE_SEMICOLON              39
#define SCANCODE_APOSTROPHE             40
#define SCANCODE_GRAVE                  41

#define SCANCODE_LEFTSHIFT              42
#define SCANCODE_BACKSLASH              43

#define SCANCODE_Z                      44
#define SCANCODE_X                      45
#define SCANCODE_C                      46
#define SCANCODE_V                      47
#define SCANCODE_B                      48
#define SCANCODE_N                      49
#define SCANCODE_M                      50
#define SCANCODE_COMMA                  51
#define SCANCODE_PERIOD                 52
#define SCANCODE_SLASH                  53

#define SCANCODE_RIGHTSHIFT             54
#define SCANCODE_KEYPADMULTIPLY         55

#define SCANCODE_LEFTALT                56
#define SCANCODE_SPACE                  57
#define SCANCODE_CAPSLOCK               58

#define SCANCODE_F1                     59
#define SCANCODE_F2                     60
#define SCANCODE_F3                     61
#define SCANCODE_F4                     62
#define SCANCODE_F5                     63
#define SCANCODE_F6                     64
#define SCANCODE_F7                     65
#define SCANCODE_F8                     66
#define SCANCODE_F9                     67
#define SCANCODE_F10                    68

#define SCANCODE_NUMLOCK                69
#define SCANCODE_SCROLLLOCK             70

#define SCANCODE_KEYPAD7                71
#define SCANCODE_CURSORUPLEFT           71
#define SCANCODE_KEYPAD8                72
#define SCANCODE_CURSORUP               72
#define SCANCODE_KEYPAD9                73
#define SCANCODE_CURSORUPRIGHT          73
#define SCANCODE_KEYPADMINUS            74
#define SCANCODE_KEYPAD4                75
#define SCANCODE_CURSORLEFT             75
#define SCANCODE_KEYPAD5                76
#define SCANCODE_KEYPAD6                77
#define SCANCODE_CURSORRIGHT            77
#define SCANCODE_KEYPADPLUS             78
#define SCANCODE_KEYPAD1                79
#define SCANCODE_CURSORDOWNLEFT         79
#define SCANCODE_KEYPAD2                80
#define SCANCODE_CURSORDOWN             80
#define SCANCODE_KEYPAD3                81
#define SCANCODE_CURSORDOWNRIGHT        81
#define SCANCODE_KEYPAD0                82
#define SCANCODE_KEYPADPERIOD           83

#define SCANCODE_LESS                   86

#define SCANCODE_F11                    87
#define SCANCODE_F12                    88

#define SCANCODE_KEYPADENTER            96
#define SCANCODE_RIGHTCONTROL           97
#define SCANCODE_CONTROL                97
#define SCANCODE_KEYPADDIVIDE           98
#define SCANCODE_PRINTSCREEN            99
#define SCANCODE_RIGHTALT               100
#define SCANCODE_BREAK                  101    /* Beware: is 119     */
#define SCANCODE_BREAK_ALTERNATIVE      119    /* on some keyboards! */

#define SCANCODE_HOME                   102
#define SCANCODE_CURSORBLOCKUP          103    /* Cursor key block */
#define SCANCODE_PAGEUP                 104
#define SCANCODE_CURSORBLOCKLEFT        105    /* Cursor key block */
#define SCANCODE_CURSORBLOCKRIGHT       106    /* Cursor key block */
#define SCANCODE_END                    107
#define SCANCODE_CURSORBLOCKDOWN        108    /* Cursor key block */
#define SCANCODE_PAGEDOWN               109
#define SCANCODE_INSERT                 110
#define SCANCODE_REMOVE                 111

#define SCANCODE_PAUSE                  119

#define SCANCODE_POWER                  120
#define SCANCODE_SLEEP                  121
#define SCANCODE_WAKEUP                 122

#define SCANCODE_LEFTWIN                125
#define SCANCODE_RIGHTWIN               126
#define SCANCODE_MENU                   127

#define SCANCODE_LEFTBUTTON             0x1000
#define SCANCODE_RIGHTBUTTON            0x2000
#define SCANCODE_MIDDLBUTTON            0x4000


int my_connect(int sockfd, struct sockaddr *address, int len)
{
	return connect(sockfd, address, len);
}

static unsigned char keycode_scancode [256];

static int isAlpha(unsigned char keycode)
{
	if (keycode > 0x41 && keycode < 0x5a || keycode == 0x20 || keycode == 0x21)
		return 1;
	else
		return 0;
}

static void init_code_map (void)
{
    keycode_scancode [0x00] = SCANCODE_ESCAPE;

    keycode_scancode [0x29] = SCANCODE_0;
    keycode_scancode [0x21] = SCANCODE_1;
    keycode_scancode [0x40] = SCANCODE_2;
    keycode_scancode [0x23] = SCANCODE_3;
    keycode_scancode [0x24] = SCANCODE_4;
    keycode_scancode [0x25] = SCANCODE_5;
    keycode_scancode [0x5E] = SCANCODE_6;
    keycode_scancode [0x26] = SCANCODE_7;
    keycode_scancode [0x2A] = SCANCODE_8;
    keycode_scancode [0x28] = SCANCODE_9;

    keycode_scancode [0x30] = SCANCODE_0;
    keycode_scancode [0x31] = SCANCODE_1;
    keycode_scancode [0x32] = SCANCODE_2;
    keycode_scancode [0x33] = SCANCODE_3;
    keycode_scancode [0x34] = SCANCODE_4;
    keycode_scancode [0x35] = SCANCODE_5;
    keycode_scancode [0x36] = SCANCODE_6;
    keycode_scancode [0x37] = SCANCODE_7;
    keycode_scancode [0x38] = SCANCODE_8;
    keycode_scancode [0x39] = SCANCODE_9;

//    keycode_scancode [0x30] = SCANCODE_F1;
//    keycode_scancode [0x31] = SCANCODE_F2;
//    keycode_scancode [0x32] = SCANCODE_F3;
//    keycode_scancode [0x33] = SCANCODE_F4;
//    keycode_scancode [0x34] = SCANCODE_F5;
//    keycode_scancode [0x35] = SCANCODE_F6;
//    keycode_scancode [0x36] = SCANCODE_F7;
//    keycode_scancode [0x37] = SCANCODE_F8;
//    keycode_scancode [0x38] = SCANCODE_F9;
//    keycode_scancode [0x39] = SCANCODE_F10;

    keycode_scancode [0x2D] = SCANCODE_MINUS;
    keycode_scancode [0x5F] = SCANCODE_MINUS;
    keycode_scancode [0x3D] = SCANCODE_EQUAL;
    keycode_scancode [0x2B] = SCANCODE_EQUAL;
    keycode_scancode [0x03] = SCANCODE_BACKSPACE;
    keycode_scancode [0x01] = SCANCODE_TAB;
    keycode_scancode [0x51] = SCANCODE_Q;
    keycode_scancode [0x57] = SCANCODE_W;
    keycode_scancode [0x45] = SCANCODE_E;
    keycode_scancode [0x52] = SCANCODE_R;
    keycode_scancode [0x54] = SCANCODE_T;
    keycode_scancode [0x59] = SCANCODE_Y;
    keycode_scancode [0x55] = SCANCODE_U;
    keycode_scancode [0x49] = SCANCODE_I;
    keycode_scancode [0x4F] = SCANCODE_O;
    keycode_scancode [0x50] = SCANCODE_P;
    keycode_scancode [0x5B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x7B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x5D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x7D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x04] = SCANCODE_ENTER;
    keycode_scancode [0x41] = SCANCODE_A;
    keycode_scancode [0x53] = SCANCODE_S;
    keycode_scancode [0x44] = SCANCODE_D;
    keycode_scancode [0x46] = SCANCODE_F;
    keycode_scancode [0x47] = SCANCODE_G;
    keycode_scancode [0x48] = SCANCODE_H;
    keycode_scancode [0x4A] = SCANCODE_J;
    keycode_scancode [0x4B] = SCANCODE_K;
    keycode_scancode [0x4C] = SCANCODE_L;

    keycode_scancode [0x3A] = SCANCODE_SEMICOLON;
    keycode_scancode [0x3B] = SCANCODE_SEMICOLON;
//    keycode_scancode [0x3B] = SCANCODE_F12;


    keycode_scancode [0x27] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x22] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x60] = SCANCODE_GRAVE;
    keycode_scancode [0x7E] = SCANCODE_GRAVE;

    keycode_scancode [0x20] = SCANCODE_SPACE;
//    keycode_scancode [0x20] = SCANCODE_LEFTSHIFT;

    keycode_scancode [0x5C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x7C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x5A] = SCANCODE_Z;
    keycode_scancode [0x58] = SCANCODE_X;
    keycode_scancode [0x43] = SCANCODE_C;
    keycode_scancode [0x56] = SCANCODE_V;
    keycode_scancode [0x42] = SCANCODE_B;
    keycode_scancode [0x4E] = SCANCODE_N;
    keycode_scancode [0x4D] = SCANCODE_M;
    keycode_scancode [0x2C] = SCANCODE_COMMA;
    keycode_scancode [0x3C] = SCANCODE_COMMA;
    keycode_scancode [0x2E] = SCANCODE_PERIOD;
    keycode_scancode [0x3E] = SCANCODE_PERIOD;
    keycode_scancode [0x2F] = SCANCODE_SLASH;
    keycode_scancode [0x3F] = SCANCODE_SLASH;
//    keycode_scancode [0x2A] = SCANCODE_KEYPADMULTIPLY;

    keycode_scancode [0x09] = SCANCODE_PRINTSCREEN;
    keycode_scancode [0x08] = SCANCODE_BREAK;
    keycode_scancode [0x06] = SCANCODE_INSERT;
    keycode_scancode [0x07] = SCANCODE_REMOVE;
    keycode_scancode [0x10] = SCANCODE_HOME;
    keycode_scancode [0x11] = SCANCODE_END;
    keycode_scancode [0x16] = SCANCODE_PAGEUP;
    keycode_scancode [0x17] = SCANCODE_PAGEDOWN;
    keycode_scancode [0x13] = SCANCODE_CURSORBLOCKUP;
    keycode_scancode [0x12] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode [0x14] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode [0x15] = SCANCODE_CURSORBLOCKDOWN;

//    keycode_scancode [0x23] = SCANCODE_RIGHTALT;
//    keycode_scancode [0x2F] = SCANCODE_KEYPADDIVIDE;
//    keycode_scancode [0x20] = SCANCODE_RIGHTSHIFT;
//    keycode_scancode [0x21] = SCANCODE_RIGHTCONTROL;
//    keycode_scancode [0x37] = SCANCODE_KEYPAD7;
//    keycode_scancode [0x38] = SCANCODE_KEYPAD8;
//    keycode_scancode [0x39] = SCANCODE_KEYPAD9;
//    keycode_scancode [0x2D] = SCANCODE_KEYPADMINUS;
//    keycode_scancode [0x34] = SCANCODE_KEYPAD4;
//    keycode_scancode [0x35] = SCANCODE_KEYPAD5;
//    keycode_scancode [0x36] = SCANCODE_KEYPAD6;
//    keycode_scancode [0x2B] = SCANCODE_KEYPADPLUS;
//    keycode_scancode [0x31] = SCANCODE_KEYPAD1;
//    keycode_scancode [0x32] = SCANCODE_KEYPAD2;
//    keycode_scancode [0x33] = SCANCODE_KEYPAD3;
//    keycode_scancode [0x30] = SCANCODE_KEYPAD0;
//    keycode_scancode [0x2E] = SCANCODE_KEYPADPERIOD;
//    keycode_scancode [] = SCANCODE_LESS;
//    keycode_scancode [] = SCANCODE_KEYPADENTER;
//    keycode_scancode [] = SCANCODE_PAUSE;
//    keycode_scancode [] = SCANCODE_LEFTWIN;
//    keycode_scancode [] = SCANCODE_RIGHTWIN;
//    keycode_scancode [] = SCANCODE_MENU;
}

static unsigned char keycode_to_scancode (unsigned char keycode, BOOL asscii)
{
    if(!asscii)
    {
        switch (keycode) {
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
                return SCANCODE_F1 + keycode - 0x30;
            case 0x3B:
                return SCANCODE_F12;
            case 0x20:
                return SCANCODE_LEFTSHIFT;
            case 0x21:
                return SCANCODE_LEFTCONTROL;
            case 0x23:
                return SCANCODE_LEFTALT;
            case 0x24:
                return SCANCODE_CAPSLOCK;
            case 0x25:
                return SCANCODE_NUMLOCK;
            case 0x26:
                return SCANCODE_SCROLLLOCK;
            case 0x3A:
                return SCANCODE_F11;
        }
    }

    return keycode_scancode [keycode];
}

QVFbView::QVFbView( int display_id, int ppid, int w, int h, int d, QWidget *parent,
        const char *name, uint flags )
: QScrollView( parent, name, flags ), lockId(-1)
{
    int actualdepth=d;
    int nr_entry = 0;
    int bpl;
	int len;
	struct sockaddr_un address;
	int result;
	char socket_file[50];

	imeOpened = true;
	//imeStared = false;
	//last_key_data.event_type = -1;
	//last_key_data.data.key.key_state = 0;
	memset(unpressedKey, 0, sizeof(unpressedKey));

    zm = 1;
    animation = 0;
    viewdepth = d;
    displayid = display_id;
    palette = new QRgb[256];
    viewport()->setMouseTracking( TRUE );
    viewport()->setFocusPolicy( StrongFocus );
	viewport()->setInputMethodEnabled(true);


    if(d <= 8) 
        nr_entry = 1 << d;
    
    init_code_map ();

    switch ( d ) {
        case 12:
            actualdepth=16;
            break;
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
            break;
        default:
            qFatal( "Unsupported bit depth %d\n", d );
    }

    if ( d == 1 )
        bpl = (w*d+7)/8;
    else
        bpl = ((w*actualdepth+31)/32)*4;

    key_t key = ppid;
    
    int dataSize = bpl * h + sizeof(QVFBHeader) + nr_entry * sizeof(QVFBPalEntry);
    shmId = shmget( key, dataSize, IPC_CREAT|0666);
    if ( shmId != -1 )
        data = (unsigned char *)shmat( shmId, 0, 0 );
    else {
        struct shmid_ds shm;
        shmctl( shmId, IPC_RMID, &shm );
        shmId = shmget( key, dataSize, IPC_CREAT|0666);
        data = (unsigned char *)shmat( shmId, 0, 0 );
    }

    if ( (int)data == -1 )
        qFatal( "Cannot attach to shared memory" );

    withalpha = false;
    if (actualdepth == 16 || actualdepth == 32) {
        char* env_value = getenv ("MG_XVFB_WITH_ALPHA");
        if (env_value && strcmp (env_value, "0")) {
            withalpha = true;
        }
    }

    hdr = (QVFBHeader *)data;
    hdr->info_size       = 0;
    hdr->width           = w;
    hdr->height          = h;
    hdr->depth           = actualdepth;
    hdr->pitch           = bpl;
    hdr->palette_changed = 0;
    hdr->palette_offset  = sizeof(QVFBHeader);
    hdr->fb_offset       = sizeof(QVFBHeader) + nr_entry * sizeof(QVFBPalEntry);
    hdr->dirty           = 0;
    hdr->dirty_rc_l      = 0;
    hdr->dirty_rc_t      = 0;
    hdr->dirty_rc_r      = 0;
    hdr->dirty_rc_b      = 0;
    switch(d)
    {
        case 2:
            hdr->MSBLeft = 1;
            break;
        case 1:
        case 4:
        case 8:
            hdr->MSBLeft = 0;
            break;
        case 16:
            if (withalpha) {
                hdr->Amask = 0x8000;
                hdr->Rmask = 0x7C00;
                hdr->Gmask = 0x03E0;
                hdr->Bmask = 0x001F;
            }
            else {
                hdr->Amask = 0x0000;
                hdr->Rmask = 0xF800;
                hdr->Gmask = 0x07E0;
                hdr->Bmask = 0x001F;
            }
            break;
        case 32:
            if (withalpha) {
                hdr->Amask = 0xFF000000;
                hdr->Rmask = 0x00FF0000;
                hdr->Gmask = 0x0000FF00;
                hdr->Bmask = 0x000000FF;
            }
            else {
                hdr->Amask = 0x00000000;
                hdr->Rmask = 0x00FF0000;
                hdr->Gmask = 0x0000FF00;
                hdr->Bmask = 0x000000FF;
            }
            break;
        default:
            break;
    }

	sprintf(socket_file, "/tmp/pcxvfb_socket%d", ppid);
	socket_file[49] = '\0';
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socket_file);
	len = sizeof(address);

	result = my_connect(sockfd, (struct sockaddr *)&address, len);
	if (result == -1) {
		perror("oops:client1 ");
		exit(1);
	}

	write(sockfd, &shmId, sizeof(int));

    resizeContents( w, h );

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timeout()) );

    setPalette();

    gammatable=0;
    setGamma(1.0,1.0,1.0);
    setRate( 30 );


}

QVFbView::~QVFbView()
{
    stopAnimation();
    //sendKeyboardData( 0, 0, 0, TRUE, FALSE ); // magic die key
	sendQVFBCloseEvent();			

    struct shmid_ds shm;
    shmdt( (char*)data );
    shmctl( shmId, IPC_RMID, &shm );

    ::close(sockfd);
}

void QVFbView::setGamma(double gr, double gg, double gb)
{
    if ( viewdepth < 12 )
        return; // not implemented

    gred=gr; ggreen=gg; gblue=gb;

    switch ( viewdepth ) {
        case 12:
            rsh = 12;
            gsh = 7;
            bsh = 1;
            rmax = 15;
            gmax = 15;
            bmax = 15;
            break;
        case 16:
            if (withalpha) {
                rsh = 10;
                gsh = 5;
                bsh = 0;
                rmax = 31;
                gmax = 31;
                bmax = 31;
            }
            else {
                rsh = 11;
                gsh = 5;
                bsh = 0;
                rmax = 31;
                gmax = 63;
                bmax = 31;
            }
            break;
        case 32:
            rsh = 16;
            gsh = 8;
            bsh = 0;
            rmax = 255;
            gmax = 255;
            bmax = 255;
    }
    int mm = QMAX(rmax,QMAX(gmax,bmax))+1;

    if ( gammatable )
        delete [] gammatable;
    gammatable = new QRgb[mm];
    for (int i=0; i<mm; i++) {
        int r = int(pow(i,gr)*255/rmax);
        int g = int(pow(i,gg)*255/gmax);
        int b = int(pow(i,gb)*255/bmax);
        if ( r > 255 ) r = 255;
        if ( g > 255 ) g = 255;
        if ( b > 255 ) b = 255;
        gammatable[i] = qRgb(r,g,b);
        //qDebug("%d: %d,%d,%d",i,r,g,b);
    }

    setDirty(rect());
}

void QVFbView::getGamma(int i, QRgb& rgb)
{
    if ( i > 255 ) i = 255;
    if ( i < 0 ) i = 0;
    rgb = qRgb(qRed(gammatable[i*rmax/255]),
            qGreen(gammatable[i*rmax/255]),
            qBlue(gammatable[i*rmax/255]));
}

int QVFbView::displayId() const
{
    return displayid;
}

int QVFbView::displayWidth() const
{
    return hdr->width;
}

int QVFbView::displayHeight() const
{
    return hdr->height;
}

int QVFbView::displayDepth() const
{
    return viewdepth;
}



void QVFbView::setZoom( double z )
{
    if ( zm != z ) {
        zm = z;
        setDirty(QRect(0,0,hdr->width,hdr->height));
        resizeContents( int(hdr->width*z), int(hdr->height*z) );
        updateGeometry();
        qApp->sendPostedEvents();
        topLevelWidget()->adjustSize();
        drawScreen();
    }
}

void QVFbView::setRate( int r )
{
    refreshRate = r;
    timer->start( 1000/r );
}

void QVFbView::initLock()
{
    /*
    QString username = "unknown";
    const char *logname = getenv("LOGNAME");
    if ( logname )
        username = logname;
    QString dataDir = "/tmp/qtembedded-" + username;

    QString pipe = dataDir + "/" + QString( QTE_PIPE ).arg( displayid );
    int semkey = ftok( pipe.latin1(), 'd' );
    lockId = semget( semkey, 0, 0 );
    */
    lockId = semget( displayid, 0, 0 );
}

void QVFbView::lock()
{
    if ( lockId == -1 )
        initLock();

    sembuf sops;

    sops.sem_num = 0;
    sops.sem_op  = -1;
    sops.sem_flg = SEM_UNDO;

    semop(lockId,&sops,1);
    /*
    int rv;
    do {
        rv = semop(lockId,&sops,1);
    } while ( rv == -1 && errno == EINTR );

    if ( rv == -1 )
        lockId = -1;
    */
}

void QVFbView::unlock()
{
    if ( lockId >= 0 ) {
        sembuf sops;

        sops.sem_num = 0;
        sops.sem_op  = +1;
        sops.sem_flg = SEM_UNDO;

        semop(lockId,&sops,1);
        /*
        int rv;
      //  do {
            rv = semop(lockId,&sops,1);
       // } while ( rv == -1 && errno == EINTR );
       */
    }
}

void QVFbView::sendMouseData( const QPoint &pos, int buttons )
{
	QVFBEventData event;

	event.event_type            = MOUSE_TYPE;
	event.data.mouse.x 		    = pos.x();
	event.data.mouse.y 		    = pos.y();
	event.data.mouse.button     = buttons;
    
    write( sockfd, &event, sizeof(QVFBEventData) );
}

void QVFbView::sendKeyboardData( int unicode, int keycode, int modifiers,
        bool press, bool repeat )
{
	QVFBEventData event;
    unsigned char scancode;

	//if(imeStared)
	//	return ;
	
	scancode = keycode_to_scancode (keycode, unicode);
	event.event_type         = KB_TYPE;
	event.data.key.key_code  = scancode;
	event.data.key.key_state = press | repeat << 8;

	//printf("sancode:%d, unicode=%d\n",scancode, unicode);

	/*if(press)
	{
		last_key_data.event_type = KB_TYPE;
		last_key_data.data.key.key_code = scancode;
	}
	else
	{
		last_key_data.event_type = -1;
	}*/

    write( sockfd, &event, sizeof(QVFBEventData) );
}

void QVFbView::timeout()
{
   // lock();

	checkEvents();

    if ( animation ) {

        QRect r( hdr->dirty_rc_l, hdr->dirty_rc_t, 
                hdr->dirty_rc_r, hdr->dirty_rc_b );
        r = r.intersect( QRect(0, 0, hdr->width, hdr->height ) );

        if ( r.isEmpty() ) {
            animation->appendBlankFrame();
        } else {
            int l;
            QImage img = getBuffer( r, l );
            animation->appendFrame(img,QPoint(r.x(),r.y()));
        }
    }

    if ( hdr->dirty ) {
        drawScreen();
    }

   // unlock();
}

void QVFbView::setPalette(void) const
{
    int i;
    QVFBPalEntry *q_pal =(QVFBPalEntry *)((BYTE *)hdr + hdr->palette_offset);
    
    if(viewdepth <= 8) { 
        for(i = 0; i < (1 << viewdepth); i++ ) {
            palette[i] = q_pal->a << 24 
				         | q_pal->r << 16 
						 | q_pal->g << 8 
						 | q_pal->b;
            q_pal++;
        }
    }

}

QImage QVFbView::getBuffer( const QRect &r, int &leading ) const
{
    if(hdr->palette_changed)
    {
        setPalette();
        hdr->palette_changed = 0;
    }

    switch ( viewdepth ) 
    {
        case 12:
        case 16: 
            {
                static unsigned char *imgData = 0;

                if ( !imgData ) {
                    int bpl = ((hdr->width*32+31)/32)*4;

                    imgData = new unsigned char [ bpl * hdr->height ];
                }

                QImage img( imgData, r.width(), r.height(),
					     	32, 0, 0, QImage::IgnoreEndian );

                int rsh, gsh, bsh, rmax, gmax, bmax;
                if (withalpha) {
                    /* viewdepth is 16 */
                    rsh = 10;
                    gsh = 5;
                    bsh = 0;
                    rmax = 31;
                    gmax = 31;
                    bmax = 31;
                }
                else {
                    rsh = viewdepth == 12 ? 12 : 11;
                    gsh = viewdepth == 12 ? 7 : 5;
                    bsh = viewdepth == 12 ? 1 : 0;
                    rmax = viewdepth == 12 ? 15 : 31;
                    gmax = viewdepth == 12 ? 15 : 63;
                    bmax = viewdepth == 12 ? 15 : 31;
                }

                for ( int row = 0; row < r.height(); row++ ) {
                    QRgb *dptr = (QRgb*)img.scanLine( row );
                    ushort *sptr = (ushort*)(data + hdr->fb_offset 
							                      + (r.y()+row)*hdr->pitch);
                    sptr += r.x();
                    for ( int col=0; col < r.width(); col++ ) {
                        ushort s = *sptr++;
                        if (withalpha) {
                            *dptr++ = qRgba(qRed(gammatable[(s>>rsh)&rmax]),
								       qGreen(gammatable[(s>>gsh)&gmax]),
									   qBlue(gammatable[(s>>bsh)&bmax]),
                                       (s & 0x8000)?255:0);
                        }
                        else {
                            *dptr++ = qRgb(qRed(gammatable[(s>>rsh)&rmax]),
								       qGreen(gammatable[(s>>gsh)&gmax]),
									   qBlue(gammatable[(s>>bsh)&bmax]));
                        }
                    }
                }
                leading = 0;
                return img;
            }
        case 2: 
            {
                static unsigned char *imgData = 0;

                if ( !imgData ) {
                    int bpl = ((hdr->width*8+31)/32)*4;

                    imgData = new unsigned char [ bpl * hdr->height ];
                }
                QImage img( imgData, r.width(), r.height(), 8, palette, 4,
                        QImage::IgnoreEndian );
                for ( int row = 0; row < r.height(); row++ ) {
                    unsigned char *dptr = img.scanLine( row );
                    unsigned char *sptr = data + hdr->fb_offset
						                       + (r.y()+row)*hdr->pitch;
                    int col = 0;
                    unsigned char s;

                    sptr += r.x()/4;
                    if(r.x()%4 == 3)
                    {
                        s = *sptr++;
                        
                        *dptr++ = s & 0x03;
                        
                        col+= 1;
                    }
                    if(r.x()%4 == 2)
                    {
                        s = *sptr++;
                        
                        *dptr++ = (s >> 2) & 0x03;
                        *dptr++ = s & 0x03;
                        
                        col += 2;
                    }
                    if(r.x()%4 == 1)
                    {
                        s = *sptr++;
                        
                        *dptr++ = (s >> 4) & 0x03;
                        *dptr++ = (s >> 2) & 0x03;
                        *dptr++ = s & 0x03;
                        
                        col += 3;
                    }
                    for ( ; col < r.width(); col+=4 ) {
                        s = *sptr++;

                        *dptr++ = (s >> 6) & 0x03;
                        *dptr++ = (s >> 4) & 0x03;
                        *dptr++ = (s >> 2) & 0x03;
                        *dptr++ = s & 0x03;
                    }
                    /*
                    if(r.right()%4 == 1)
                    {
                        s = *sptr;
                        
                        *dptr = s & 0x03;
                    }
                    if(r.right()%4 == 2)
                    {
                        s = *sptr;
                        
                        *dptr++ = (s >> 2) & 0x03;
                        *dptr = s & 0x03;
                    }
                    if(r.right()%4 == 3)
                    {
                        s = *sptr;
                        
                        *dptr++ = (s >> 4) & 0x03;
                        *dptr++ = (s >> 2) & 0x03;
                        *dptr = s & 0x03;
                    }
                    */
                }
                
                leading = 0;
                return img;
            }

        case 4: 
            {
                static unsigned char *imgData = 0;

                if ( !imgData ) {
                    int bpl = ((hdr->width*8+31)/32)*4;

                    imgData = new unsigned char [ bpl * hdr->height ];
                }
                QImage img( imgData, r.width(), r.height(), 8, palette, 16,
                        QImage::IgnoreEndian );
                for ( int row = 0; row < r.height(); row++ ) {
                    unsigned char *dptr = img.scanLine( row );
                    unsigned char *sptr = data + hdr->fb_offset
						                       + (r.y()+row)*hdr->pitch;
                    int col = 0;

                    sptr += r.x()/2;
                    if ( r.x() & 1 ) {
                        *dptr++ = *sptr++ >> 4;
                        col++;
                    }
                    for ( ; col < r.width()-1; col+=2 ) {
                        unsigned char s = *sptr++;

                        *dptr++ = s & 0x0f;
                        *dptr++ = s >> 4;
                    }
                    if ( !(r.right() & 1) )
                        *dptr = *sptr & 0x0f;
                }
                leading = 0;
                return img;
            }
        case 32: 
            {
                leading = r.x();
                return QImage( data + hdr->fb_offset + r.y() * hdr->pitch,
                        hdr->width, r.height(), hdr->depth, 0,
                        0, QImage::LittleEndian );
            }
        case 8: 
            {
                leading = r.x();
                return QImage( data + hdr->fb_offset + r.y() * hdr->pitch,
                        hdr->width, r.height(), hdr->depth, palette,
                        256, QImage::LittleEndian );
            }
        case 1: 
            {
                leading = r.x();
                return QImage( data + hdr->fb_offset + r.y() * hdr->pitch,
                        hdr->width, r.height(), hdr->depth, palette,
                        0, QImage::LittleEndian );
            }
    }
    return QImage();
}

void QVFbView::drawScreen()
{
    lock();

    QRect r( hdr->dirty_rc_l, hdr->dirty_rc_t, 
            hdr->dirty_rc_r-hdr->dirty_rc_l, hdr->dirty_rc_b-hdr->dirty_rc_t );

    hdr->dirty      = 0;
    hdr->dirty_rc_l = 0;
    hdr->dirty_rc_t = 0;
    hdr->dirty_rc_r = 0;
    hdr->dirty_rc_b = 0;

    r = r.intersect( QRect(0, 0, hdr->width, hdr->height ) );
    if ( !r.isEmpty() )  {
        if ( int(zm) != zm ) {
            r.rLeft()   = int(int(r.left()*zm)/zm);
            r.rTop()    = int(int(r.top()*zm)/zm);
            r.rRight()  = int(int(r.right()*zm+zm+0.0000001)/zm+1.9999);
            r.rBottom() = int(int(r.bottom()*zm+zm+0.0000001)/zm+1.9999);
            r.rRight()  = QMIN(r.right(),hdr->width-1);
            r.rBottom() = QMIN(r.bottom(),hdr->height-1);
        }

        int leading;

        QImage img = getBuffer (r, leading);
#if 1
        QPixmap pm (r.width(), r.height());
        QPainter _p;

        _p.begin (&pm);
        _p.setPen (blue);
        _p.setBrush (green);
        _p.fillRect (0, 0, r.width(), r.height(), Qt::SolidPattern);
        if (withalpha) {
            img.setAlphaBuffer (TRUE);
        }
        else {
            img.setAlphaBuffer (FALSE);
        }

        _p.drawImage (0, 0, img, leading, 0, r.width(), r.height(), Qt::OrderedAlphaDither);
        _p.end ();

        unlock();

        if (zm != 1 && int(zm) == zm) {
            QWMatrix m;
            m.scale(zm,zm);
            pm = pm.xForm(m);
        }

        QPainter p (viewport());
        p.translate (-contentsX(), -contentsY());
        p.drawPixmap (int(r.x()*zm), int(r.y()*zm), pm, 0, 0, pm.width(), pm.height());
#else
        if (withalpha) {
            img.setAlphaBuffer (TRUE);
        }
        else {
            img.setAlphaBuffer (FALSE);
        }

        QPainter p (viewport());
        p.translate (-contentsX(), -contentsY());
        p.drawImage (r.x(), r.y(), img, leading, 0, -1, -1, OrderedAlphaDither);
        unlock();
#endif
    } else {
        unlock();
    }
}

bool QVFbView::eventFilter( QObject *obj, QEvent *e )
{
	//printf("e->type:%d\n",e->type());
	switch(e->type())
	{
	//case QEvent::Accel:
	//case QEvent::AccelOverride:
	case QEvent::Hide:
	case QEvent::FocusOut:
//	case QEvent::KeyPress:
		//printf("key event=%d\n", e->type());
		sendUnPressedKeys();
		break;
	case QEvent::IMStart:
	{
		//imeStared = true;
		//if(last_key_data.event_type == KB_TYPE)
		//{
		//	write(sockfd, &last_key_data, sizeof(QVFBEventData));
		//	last_key_data.event_type = -1;
		//}
		break;
	}
	case QEvent::IMEnd:
		{
			//imeStared = false;
			if(imeOpened)
			{
				QIMEvent *qime = (QIMEvent*)e;
				//send 
				sendIMEText(qime->text().utf8());
				//return QScrollView::eventFilter(obj, e);
			}
		}
		break;
	}

    if ( obj == viewport() &&
            (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut) )
	{	
        return TRUE;
	}
    return QScrollView::eventFilter( obj, e );
}

void QVFbView::viewportPaintEvent( QPaintEvent *pe )
{
    QRect r( pe->rect() );

    r.moveBy( contentsX(), contentsY() );
    r = QRect(int(r.x()/zm),int(r.y()/zm),
            int(r.width()/zm)+1,int(r.height()/zm)+1);

    setDirty(r);

    drawScreen();
}

void QVFbView::setDirty( const QRect& r )
{
    lock();

    QRect r_old( hdr->dirty_rc_l,  
			     hdr->dirty_rc_t, 
				 hdr->dirty_rc_r, 
				 hdr->dirty_rc_b );
    r_old |= r;
    
    hdr->dirty_rc_l = r_old.left();
    hdr->dirty_rc_t = r_old.top();
    hdr->dirty_rc_r = r_old.right();
    hdr->dirty_rc_b = r_old.bottom();
    
    hdr->dirty = TRUE;
	
    unlock();
}

void QVFbView::contentsMousePressEvent( QMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void QVFbView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void QVFbView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->stateAfter() );
}

void QVFbView::contentsMouseMoveEvent( QMouseEvent *e )
{
    sendMouseData( e->pos()/zm, e->state() );
}

void QVFbView::keyPressEvent( QKeyEvent *e )
{
	//printf("key state=%x, key=%x, unicode=%d\n", e->state(), e->key(), e->text()[0].unicode());
	setPressedKey(e->key(), e->text()[0].unicode());

/*	if(e->state() & Qt::ControlButton)
	{
		if(setPressedKey(Qt::Key_Control, 0)==1)
			sendKeyboardData(0, Qt::Key_Control, 0, TRUE, 1); 
	}
	else{
		if(clearPressedKey(Qt::Key_Control))
			sendKeyboardData(0, Qt::Key_Control, 0, FALSE, 0); 
	}

	if(e->state() & Qt::ShiftButton)
	{
		if(setPressedKey(Qt::Key_Shift, 0)==1)
			sendKeyboardData(0, Qt::Key_Shift, 0, TRUE, 1); 
	}
	else{
		if(clearPressedKey(Qt::Key_Shift))
			sendKeyboardData(0, Qt::Key_Shift, 0, FALSE, 0); 
	}

	if(e->state() & Qt::AltButton)
	{
		if(setPressedKey(Qt::Key_Alt, 0) == 1)
			sendKeyboardData(0, Qt::Key_Alt, 0, TRUE, 1); 
	}
	else
	{
		if(clearPressedKey(Qt::Key_Alt))
			sendKeyboardData(0, Qt::Key_Alt, 0, FALSE, 0); 
	}
*/	

	if (isAlpha(e->key()))
	{
		//fprintf(stderr, "%x is alpha\n", e->key());
		sendKeyboardData(e->text()[0].unicode(), e->key(), 
				0,//e->state()&(ShiftButton|ControlButton|AltButton),
				TRUE, e->isAutoRepeat());
	}
	else
	{
		//fprintf(stderr, "%x is __NOT__ alpha\n", e->key());
		sendKeyboardData(e->text()[0].unicode(), e->key(), 
				e->state()&(ShiftButton|ControlButton|AltButton),
				TRUE, e->isAutoRepeat());
		sendKeyboardData(e->ascii(), e->key(), 
				e->state()&(ShiftButton|ControlButton|AltButton),
				FALSE, 0);
	}
}

void QVFbView::keyReleaseEvent( QKeyEvent *e )
{
	//printf("key state, release=%x\n", e->state());
	if(!clearPressedKey(e->key()))
		return ;
   if (isAlpha(e->key()))
	{
		//fprintf(stderr, "%x is alpha\n", e->key());
		sendKeyboardData(e->ascii(), e->key(), 
			    e->state()&(ShiftButton|ControlButton|AltButton),
				FALSE, e->isAutoRepeat());
	}
}

QImage QVFbView::image() const
{
    ((QVFbView*)this)->lock();
    int l;
    QImage r = getBuffer( QRect(0, 0, hdr->width, hdr->height), l ).copy();
    ((QVFbView*)this)->unlock();
    return r;
}

void QVFbView::startAnimation( const QString& filename )
{
    delete animation;
    animation = new QAnimationWriter(filename,"MNG");
    animation->setFrameRate(refreshRate);
    animation->appendFrame(QImage(data + hdr->fb_offset,
                hdr->width, hdr->height, hdr->depth, palette,
                256, QImage::LittleEndian));
}

void QVFbView::stopAnimation()
{
    delete animation;
    animation = 0;
}

void QVFbView::checkEvents()
{
    fd_set fds;
    struct timeval tv;
    int ret;

    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        memset(&tv, 0, sizeof(tv));
        if((ret = select(sockfd+1, &fds, NULL, NULL, &tv)) == 0)
        {
            break;
        }else if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }else{
                perror("select()");
                exit(1);
            }
            return;
        }

        int type;
        if ((ret = read(sockfd, &type, sizeof(type))) == 0) {
            exit(1);
            return;
        }else if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }else{
                perror("read(socket)");
                exit(1);
            }
            return;
        }

        switch(type)
        {
            case CAPTION_TYPE:
                {
                    char szBuff[1024];
                    QString caption = "";
                    int size;
                    if(read(sockfd, &size, sizeof(size)) != sizeof(size))
                        return;
                    if(size <= 0)
                        return;

                    while(size > 0)
                    {
                        int len = read(sockfd, szBuff, size > (sizeof(szBuff))-1?(sizeof(szBuff)-1):size);
                        szBuff[len] = '\0';
                        caption += szBuff;
                        size -= len;
                    }
                    emit captionChanged(caption);
                }
                break;
            case IME_TYPE:
                {
                    if(read(sockfd, &imeOpened, sizeof(imeOpened)) != sizeof(imeOpened))
                    {
                        return;
                    }
                }
                break;
            case SHOW_HIDE_TYPE:
                {
                    int show_hide = 0;
                    if(read(sockfd, &show_hide, sizeof(int)) != sizeof(int))
                        return;
                    //printf("--- show_hide=%d\n",show_hide);
                    emit showHide(show_hide?true:false);		
                }
                break;
        }
    }

}

void QVFbView::sendIMEText(const char* szText)
{
	int len;
	char szbuff[1024];
	QVFBIMETextData *imt = (QVFBIMETextData*)szbuff;
	if(szText == NULL || (len = strlen(szText)) <= 0)
		return;

	imt->event_type = IME_MESSAGE_TYPE;
	imt->size = len;
	strcpy(imt->buff, szText);
	write(sockfd, imt, sizeof(QVFBIMETextData)+len -4);
}

void QVFbView::sendQVFBCloseEvent()
{
	unsigned int type = QVFB_CLOSE_TYPE;
	write(sockfd, &type, sizeof(type));
}

int QVFbView::setPressedKey(int keycode, int unicode)
{
	int i = 0;
	//printf("-- set keycode=%d\n",keycode);
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		if(unpressedKey[i][0] == keycode)
			return 0;
	}

	unpressedKey[i][0] = keycode;
	unpressedKey[i][1] = unicode;
	unpressedKey[++i][0] = 0;
	return 1;
}

bool QVFbView::clearPressedKey(int keycode)
{
	int i = 0;
	int find_idx = -1;
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		if(keycode == unpressedKey[i][0])
			find_idx = i;
	}
	//printf("--clean key code=%d, find:%d\n",keycode, find_idx);

	if(find_idx == -1)
		return false;
	if(--i >= 0)
	{
		if(find_idx != i)
		{
			unpressedKey[find_idx][0] = unpressedKey[i][0];
			unpressedKey[find_idx][1] = unpressedKey[i][1];
		}
		unpressedKey[i][0] = 0;
	}

	return true;

}

void QVFbView::sendUnPressedKeys()
{
	int i = 0;
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		//printf("send key : %d\n", unpressedKey[0]);
		sendKeyboardData(unpressedKey[i][1], unpressedKey[i][0], 0, FALSE, 0);
		unpressedKey[i][0] = 0;
	}
}
