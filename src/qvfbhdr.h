/*
 * qvfbhdr.h
 * for minigui pc_xvfb 
 * wangjian
 */

#ifndef __QVFBHDR_H
#define __QVFBHDR_H

#define QT_VFB_MOUSE_PIPE	"/tmp/.qtvfb_mouse-%1"
#define QT_VFB_KEYBOARD_PIPE	"/tmp/.qtvfb_keyboard-%1"

#define MOUSE_TYPE   0
#define KB_TYPE      1
#define CAPTION_TYPE 2
#define IME_TYPE     3
#define IME_MESSAGE_TYPE 4
#define SHOW_HIDE_TYPE   5
#define QVFB_CLOSE_TYPE  6

typedef unsigned char   BYTE;

typedef struct _QVFBHeader {
    unsigned int info_size;
    int width;
    int height;
    int depth;
    int pitch;
    int dirty;            /* true for dirty, and should reset to false after refreshing the dirty area */
    int dirty_rc_l, dirty_rc_t, dirty_rc_r, dirty_rc_b;
    int palette_changed;  /* true for changed, and should reset to false after reflecting the change */
    int palette_offset;
    int fb_offset;
    int MSBLeft;
    int Rmask;
    int Gmask;
    int Bmask;
    int Amask;
} QVFBHeader;

typedef struct _QVFBPalEntry {
    unsigned char r, g, b, a;
} QVFBPalEntry;
/*
typedef struct _QVFBPalette {
        QVFBPalEntry [nr_entries];
} QVFBPalette;
*/
/* for mouse & key input event */
typedef struct _QVFBKeyData
{
    unsigned short key_code;
    unsigned short key_state;
} QVFBKeyData;

typedef struct _QVFBMouseData
{
    unsigned short x;
    unsigned short y;
    unsigned int   button;
} QVFBMouseData;

typedef struct _QVFBEventData
{
    int event_type;
    union 
    {
        QVFBKeyData key;
        QVFBMouseData mouse;
    } data;
} QVFBEventData;

typedef struct _QVFBCaptionEventData{
	int event_type;
	int size;
	char buff[1];
} QVFBCaptionEventData;

typedef struct _QVFBIMEEventData {
	int event_type;
	int ime_open;
}QVFBIMEEventData;

typedef struct _QVFBIMETextData{
	int event_type;
	int size;
	char buff[1];
}QVFBIMETextData;

#endif
