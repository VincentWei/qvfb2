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

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "src/config.h"
#include "qvfb.h"
#include "qvfbview.h"
#include "qvfbratedlg.h"
#include "qvfbconfig.h"
#include "skin.h"


static const char * logo[] = {
"16 16 198 2",
"  	c None",
". 	c #F8FAFF",
"+ 	c #F9FAFF",
"@ 	c #F8F9FF",
"# 	c #F5F7FF",
"$ 	c #F3F6FF",
"% 	c #F2F5FF",
"& 	c #F1F4FF",
"* 	c #E9EDFF",
"= 	c #F7F9FF",
"- 	c #676B8B",
"; 	c #343A65",
"> 	c #EDF1FF",
", 	c #E7EBFF",
"' 	c #E2E8FF",
") 	c #E0E6FF",
"! 	c #9094A4",
"~ 	c #555A7E",
"{ 	c #000631",
"] 	c #D5D8E5",
"^ 	c #5C6083",
"/ 	c #000638",
"( 	c #EBF0FF",
"_ 	c #E1E8FF",
": 	c #DBE3FF",
"< 	c #D8E0FF",
"[ 	c #D7E0FF",
"} 	c #9EA688",
"| 	c #363C66",
"1 	c #000427",
"2 	c #2B315E",
"3 	c #A2D875",
"4 	c #69C040",
"5 	c #44B121",
"6 	c #32A911",
"7 	c #4CB313",
"8 	c #D6E720",
"9 	c #D6E810",
"0 	c #F3F5FF",
"a 	c #00073C",
"b 	c #00052E",
"c 	c #F0F3FF",
"d 	c #F0F4FF",
"e 	c #000423",
"f 	c #CCE997",
"g 	c #A8DB78",
"h 	c #73C54A",
"i 	c #52B72C",
"j 	c #D4E36F",
"k 	c #D9EA1F",
"l 	c #D9E91D",
"m 	c #D6E61F",
"n 	c #000637",
"o 	c #000636",
"p 	c #EEF2FF",
"q 	c #191F50",
"r 	c #EFF2FF",
"s 	c #00052A",
"t 	c #CFEB9C",
"u 	c #AFDD7F",
"v 	c #7FC955",
"w 	c #DEEB55",
"x 	c #DDEC2F",
"y 	c #DDEC30",
"z 	c #CDC992",
"A 	c #B24B4C",
"B 	c #EFF3FF",
"C 	c #00073D",
"D 	c #444971",
"E 	c #ECF0FF",
"F 	c #0F1648",
"G 	c #D3EDA1",
"H 	c #BEE386",
"I 	c #E9F269",
"J 	c #E4F04E",
"K 	c #DDE560",
"L 	c #C56D68",
"M 	c #C34F41",
"N 	c #BF453A",
"O 	c #E9EEFF",
"P 	c #D0EB9D",
"Q 	c #E5F4A8",
"R 	c #F1F78E",
"S 	c #ECF475",
"T 	c #D7B99F",
"U 	c #D0755E",
"V 	c #CF735B",
"W 	c #CC6C57",
"X 	c #BE7A74",
"Y 	c #DFE7FF",
"Z 	c #E3EAFF",
"` 	c #E5ECFF",
" .	c #00031A",
"..	c #F7FBAB",
"+.	c #EDF1AC",
"@.	c #DFAC95",
"#.	c #DC9B7A",
"$.	c #DC9877",
"%.	c #D6ADA4",
"&.	c #C4D3FF",
"*.	c #A1ACC2",
"=.	c #C2D1FF",
"-.	c #CEDAFF",
";.	c #D9E2FF",
">.	c #DAE4FF",
",.	c #CDEA99",
"'.	c #D1EC9E",
").	c #F2F8A7",
"!.	c #F6FAA6",
"~.	c #E7D9BB",
"{.	c #E6B795",
"].	c #E7B791",
"^.	c #E3C2AC",
"/.	c #D4DFFF",
"(.	c #D2DDFF",
"_.	c #CAD8FF",
":.	c #B0BACA",
"<.	c #9CB6FF",
"[.	c #97B2FF",
"}.	c #A3BBFF",
"|.	c #93D168",
"1.	c #9CD56E",
"2.	c #E5F278",
"3.	c #EDF282",
"4.	c #DEAE96",
"5.	c #E4B08F",
"6.	c #E3C2A9",
"7.	c #D8E1FB",
"8.	c #070705",
"9.	c #000000",
"0.	c #D3DFFF",
"a.	c #BDC7D0",
"b.	c #7B9CFF",
"c.	c #668DFF",
"d.	c #6F94FF",
"e.	c #61BD39",
"f.	c #DAEC47",
"g.	c #DBD15F",
"h.	c #D27E6A",
"i.	c #D99277",
"j.	c #CBC2D6",
"k.	c #D1DDFF",
"l.	c #0C0C09",
"m.	c #080808",
"n.	c #CFDBFF",
"o.	c #0A0A0C",
"p.	c #C7CFD5",
"q.	c #6088FF",
"r.	c #3E70FF",
"s.	c #3BAD14",
"t.	c #CEE621",
"u.	c #BE8F4A",
"v.	c #C24E42",
"w.	c #BE7170",
"x.	c #9BB4FF",
"y.	c #B7C9FF",
"z.	c #CCDAFF",
"A.	c #CCD9FF",
"B.	c #010101",
"C.	c #050506",
"D.	c #CBD9FF",
"E.	c #4C7BFF",
"F.	c #2DA603",
"G.	c #C6E10B",
"H.	c #A74B25",
"I.	c #B42B2A",
"J.	c #816AB1",
"K.	c #7196FF",
"L.	c #8DABFF",
"M.	c #AEC3FF",
"N.	c #C8D6FF",
"O.	c #1A1C21",
"P.	c #C4CDD5",
"Q.	c #4C7BED",
"R.	c #BFD606",
"S.	c #A2200F",
"T.	c #9D2139",
"U.	c #3D6AF7",
"V.	c #4D7BFF",
"W.	c #668EFF",
"X.	c #82A3FF",
"Y.	c #C2D2FF",
"Z.	c #101115",
"`.	c #C4D4FF",
" +	c #191B21",
".+	c #C0CAD5",
"++	c #ABBA04",
"@+	c #A0120A",
"#+	c #531A45",
"$+	c #183174",
"%+	c #273F7B",
"&+	c #3A4F83",
"*+	c #53658E",
"=+	c #6F7F9A",
"-+	c #909CA8",
";+	c #ADB6B5",
">+	c #C0C7BD",
",+	c #AEB6B3",
". + + . @ . + + @ # $ % & & & * ",
"= - = = = = = ; # > , ' ) ) ) ! ",
"= ~ { # # ] ^ / # ( _ : < < [ } ",
"# | 1 % % % 2 1 $ 3 4 5 6 7 8 9 ",
"0 a b c c d 2 e f g h i j k l m ",
"& n > o p q r s t u v w x y z A ",
"B C ( D E F > C G H I J K L M N ",
"> C O 2 { b P C Q R S T U V W X ",
"Y { Z `  .t G { ..+.@.#.$.%.&.*.",
"=.-.;.>.,.'.).!.~.{.].^./.(._.:.",
"<.[.}.|.1.2.3.4.5.6.7.8.9.9.0.a.",
"b.c.d.e.f.g.h.i.j.k.l.m.n.o.9.p.",
"q.r.s.t.u.v.w.x.y.z.A.B.C.D.D.p.",
"E.F.G.H.I.J.K.L.M.N.N.N.O.B.B.P.",
"Q.R.S.T.U.V.W.X.}.Y.9.Z.`. +9..+",
"++@+#+$+%+&+*+=+-+;+>+9.9.9.>+,+"
};

QVFb::QVFb( int display_id, int ppid, int w, int h, int d, const QString &skin, QWidget *parent,
        const char *name, uint flags )
: QMainWindow( parent, name, flags )
{
    imagesave = new QFileDialog(".", "*.png", this, 0, TRUE);
    imagesave->setSelection("snapshot.png");
    setIcon(QPixmap(logo));
    rateDlg = 0;
	this->ppid = ppid;
    view = 0;
    init( display_id, w, h, d, skin );
    createMenu( menuBar() );

}

QVFb::~QVFb()
{
}

void QVFb::popupMenu()
{
    QPopupMenu *pm = new QPopupMenu( this );
    createMenu( pm );
    pm->exec(QCursor::pos());
}

void QVFb::init( int display_id, int w, int h, int d, const QString &skin_name )
{
  //  setCaption( QString("Virtual framebuffer %1x%2 %3bpp Display :%4")
    //        .arg(w).arg(h).arg(d).arg(display_id) );
    delete view;

    menuBar()->hide();

    if ( !skin_name.isEmpty() && QFile::exists(skin_name) ) {
        bool vis = isVisible();
        if ( vis ) hide();
        Skin *skin = new Skin( this, skin_name, w, h );
        view = new QVFbView( display_id, ppid, w, h, d, skin );
        skin->setView( view );
        view->setMargin( 0 );
        view->setFrameStyle( QFrame::NoFrame );
        view->setFixedSize( w, h );
        setCentralWidget( skin );
		//clearWFlags(Qt::WStyle_DialogBorder|Qt::WStyle_NormalBorder|Qt::WStyle_Title|Qt::WStyle_SysMenu|Qt::WStyle_MinMax);
		setWFlags(Qt::WStyle_NoBorder|Qt::WStyle_Customize);
        adjustSize();
        view->show(); 
        if ( vis ) show();
    } else {
        if ( !currentSkin.isEmpty() ) {
            clearMask();
            reparent( 0, 0, pos(), TRUE );
        }

#if _ENABLE_MENUBAR
        menuBar()->show();
#endif
        view = new QVFbView( display_id, ppid, w, h, d, this );
        view->setMargin( 0 );
        view->setFrameStyle( QFrame::NoFrame );
        setCentralWidget( view );
        resize(w,menuBar()->height()+h);
        view->show(); 
    }

    currentSkin = skin_name;

	//connect views
	connect(view, SIGNAL(captionChanged(QString)), this, SLOT(onCaptionChanged(QString)));

	connect(view, SIGNAL(showHide(bool)), this, SLOT(showHide(bool)));

}

void QVFb::enableCursor( bool e )
{
    view->viewport()->setCursor( e ? ArrowCursor : BlankCursor );
    viewMenu->setItemChecked( cursorId, e );
}

void QVFb::createMenu(QMenuData *menu)
{
    QPopupMenu *file = new QPopupMenu( this );
#if 0
    file->insertItem( "&Configure...", this, SLOT(configure()), ALT+CTRL+Key_C );
    file->insertSeparator();
#endif
    file->insertItem( "&Save image...", this, SLOT(saveImage()), ALT+CTRL+Key_S );
    file->insertItem( "&Animation...", this, SLOT(toggleAnimation()), ALT+CTRL+Key_A );
    file->insertSeparator();
    file->insertItem( "&Quit", qApp, SLOT(quit()) );

    menu->insertItem( "&File", file );

    viewMenu = new QPopupMenu( this );
    viewMenu->setCheckable( true );
    cursorId = viewMenu->insertItem( "Show &Cursor", this, SLOT(toggleCursor()) );
    enableCursor(TRUE);
    viewMenu->insertItem( "&Refresh Rate...", this, SLOT(changeRate()) );
    viewMenu->insertSeparator();
    viewMenu->insertItem( "Zoom scale &1", this, SLOT(setZoom1()) );
    viewMenu->insertItem( "Zoom scale &2", this, SLOT(setZoom2()) );
    viewMenu->insertItem( "Zoom scale &3", this, SLOT(setZoom3()) );
    viewMenu->insertItem( "Zoom scale &4", this, SLOT(setZoom4()) );
    viewMenu->insertItem( "Zoom scale &0.5", this, SLOT(setZoomHalf()) );

    menu->insertItem( "&View", viewMenu );

    QPopupMenu *help = new QPopupMenu( this );
    help->insertItem("About...", this, SLOT(about()));
    menu->insertSeparator();
    menu->insertItem( "&Help", help );
}

void QVFb::setZoom(double z)
{
    view->setZoom(z);
}

void QVFb::setZoomHalf()
{
    setZoom(0.5);
}

void QVFb::setZoom1()
{
    setZoom(1);
}

void QVFb::setZoom2()
{
    setZoom(2);
}

void QVFb::setZoom3()
{
    setZoom(3);
}

void QVFb::setZoom4()
{
    setZoom(4);
}

void QVFb::saveImage()
{
    QImage img = view->image();
    if ( imagesave->exec() ) {
        QString filename = imagesave->selectedFile();
        if ( !!filename )
            img.save(filename,"PNG");
    }
}

void QVFb::toggleAnimation()
{
    if ( view->animating() ) {
        view->stopAnimation();
    } else {
        QString filename = imagesave->getSaveFileName("animation.mng", "*.mng", this, "", "Save animation...");
        if ( !filename ) {
            view->stopAnimation();
        } else {
            view->startAnimation(filename);
        }
    }
}

void QVFb::toggleCursor()
{
    enableCursor( !viewMenu->isItemChecked( cursorId ) );
}

void QVFb::changeRate()
{
    if ( !rateDlg ) {
        rateDlg = new QVFbRateDialog( view->rate(), this );
        connect( rateDlg, SIGNAL(updateRate(int)), view, SLOT(setRate(int)) );
    }

    rateDlg->show();
}

void QVFb::about()
{
    QMessageBox::about(this, "About QVFB2",
            "<h2>The Virtual X11 Framebuffer for MiniGUI V3.0</h2>"
            "<p>This application runs under Qt/X11, emulating a framebuffer, "
            "which MiniGUI V3.0 can attach to just as if "
            "it was a hardware Linux framebuffer. "
            "<p>With the aid of this development tool, you can develop MiniGUI V3.0 "
            "applications under X11 without having to switch to a virtual console. "
            "This means you can comfortably use your other development tools such "
            "as GUI profilers and debuggers."
            );
}

#if 0
void QVFb::configure()
{
    config = new Config(this,0,TRUE);

    int w = view->displayWidth();
    int h = view->displayHeight();
    QString skin;
    config->size_width->setValue(w);
    config->size_height->setValue(h);
    config->size_custom->setChecked(TRUE); // unless changed by settings below
    config->size_240_320->setChecked(w==240&&h==320);
    config->size_320_240->setChecked(w==320&&h==240);
    config->size_640_480->setChecked(w==640&&h==480);
    config->skin->setEditable(TRUE);
    if (!currentSkin.isNull()) {
        config->size_skin->setChecked(TRUE);
        config->skin->setEditText(currentSkin);
    }
    config->depth_1->setChecked(view->displayDepth()==1);
    config->depth_4gray->setChecked(view->displayDepth()==4);
    config->depth_8->setChecked(view->displayDepth()==8);
    config->depth_12->setChecked(view->displayDepth()==12);
    config->depth_16->setChecked(view->displayDepth()==16);
    config->depth_32->setChecked(view->displayDepth()==32);
    if ( view->gammaRed() == view->gammaGreen() && view->gammaGreen() == view->gammaBlue() ) {
        config->gammaslider->setValue(int(view->gammaRed()*400));
        config->rslider->setValue(100);
        config->gslider->setValue(100);
        config->bslider->setValue(100);
    } else {
        config->gammaslider->setValue(100);
        config->rslider->setValue(int(view->gammaRed()*400));
        config->gslider->setValue(int(view->gammaGreen()*400));
        config->bslider->setValue(int(view->gammaBlue()*400));
    }
    connect(config->gammaslider, SIGNAL(valueChanged(int)), this, SLOT(setGamma400(int)));
    connect(config->rslider, SIGNAL(valueChanged(int)), this, SLOT(setR400(int)));
    connect(config->gslider, SIGNAL(valueChanged(int)), this, SLOT(setG400(int)));
    connect(config->bslider, SIGNAL(valueChanged(int)), this, SLOT(setB400(int)));
    updateGammaLabels();

    double ogr=view->gammaRed(), ogg=view->gammaGreen(), ogb=view->gammaBlue();

    if ( config->exec() ) {
        int id = view->displayId(); // not settable yet
        if ( config->size_240_320->isChecked() ) {
            w=240; h=320;
        } else if ( config->size_320_240->isChecked() ) {
            w=320; h=240;
        } else if ( config->size_640_480->isChecked() ) {
            w=640; h=480;
        } else if ( config->size_skin->isChecked() ) {
            skin = config->skin->currentText();
        } else {
            w=config->size_width->value();
            h=config->size_height->value();
        }
        int d;
        if ( config->depth_1->isChecked() )
            d=1;
        else if ( config->depth_4gray->isChecked() )
            d=4;
        else if ( config->depth_8->isChecked() )
            d=8;
        else if ( config->depth_12->isChecked() )
            d=12;
        else if ( config->depth_16->isChecked() )
            d=16;
        else
            d=32;

        if ( w != view->displayWidth() || h != view->displayHeight() 
                || d != view->displayDepth() || skin != currentSkin )
            init( id, w, h, d, skin );
    } else {
        view->setGamma(ogr, ogg, ogb);
    }

    delete config;
    config=0;
}
#endif

void QVFb::setGamma400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
            config->gslider->value()/100.0*g,
            config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::setR400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
            view->gammaGreen(),
            view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setG400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
            config->gslider->value()/100.0*g,
            view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setB400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
            view->gammaGreen(),
            config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::updateGammaLabels()
{
    config->rlabel->setText(QString::number(view->gammaRed(),'g',2));
    config->glabel->setText(QString::number(view->gammaGreen(),'g',2));
    config->blabel->setText(QString::number(view->gammaBlue(),'g',2));
}

QSize QVFb::sizeHint() const
{
    return QSize(view->displayWidth()*view->zoom(),
            menuBar()->height()+view->displayHeight()*view->zoom());
}

/////////////////////////////
void QVFb::onCaptionChanged(QString strCaption)
{
	setCaption(strCaption);
}

void QVFb::showHide(bool bshow)
{
	//printf("showHide:%d\n",bshow);
	if(bshow){
		showMaximized();
		setActiveWindow();
		raise();
		showNormal();
	}
	else
		showMinimized();
}

