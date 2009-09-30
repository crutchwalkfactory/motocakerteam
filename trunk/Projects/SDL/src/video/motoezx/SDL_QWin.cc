/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include <dirent.h>

#include "SDL_config.h"

#include "SDL_QWin.h"
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <zapplication.h>
#include <qcopchannel_qws.h>
#include <stdlib.h>

#include <ZConfig.h>


extern "C" int UTIL_GetSliderStatus(void);

screenRotationT screenRotation = SDL_QT_NO_ROTATION;

static QCString SDL_MotoChannel;

#ifdef __cplusplus
extern "C" {
#endif
void SDL_ChannelExists(const char *channelName);
void SDL_ShowSplash();
void SDL_HideSplash();

extern int UTIL_GetIncomingCallStatus();
extern int UTIL_GetFlipStatus();
extern int UTIL_GetSideKeyLock();
#ifdef __cplusplus
}
#endif

void CargarTeclas();
int myRED, myCENTER, myUP, myDOWN, myLEFT, myRIGHT, mySIDE, myMUSIC, myC, myLSOFT, myRSOFT, myCALL, myCAMERA, mySLIDER, myVOLUP, myVOLDOWN, my0, my1, my2, my3, my4, my5, my6, my7, my8, my9, myASTERISK, myNUMERAL;
int SmyRED, SmyCENTER, SmyUP, SmyDOWN, SmyLEFT, SmyRIGHT, SmySIDE, SmyMUSIC, SmyC, SmyLSOFT, SmyRSOFT, SmyCALL, SmyCAMERA, SmySLIDER, SmyVOLUP, SmyVOLDOWN, Smy0, Smy1, Smy2, Smy3, Smy4, Smy5, Smy6, Smy7, Smy8, Smy9, SmyASTERISK, SmyNUMERAL;


void CargarTeclas()
{
	ZConfig mySDL("/mmc/mmca1/.system/mySDL.cfg",false);
	ZConfig keyCFG("/mmc/mmca1/.system/lib/keyconfig.cfg",false);
	QString val;
	val = mySDL.readEntry("SDL", "Red", "ESCAPE");
  	myRED = keyCFG.readNumEntry("KEYCODES", QString(val), 27);
	val = mySDL.readEntry("SDL", "Center", "RETURN");
	myCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), 13);
	val = mySDL.readEntry("SDL", "Up", "UP");
	myUP = keyCFG.readNumEntry("KEYCODES", QString(val), 273);
	val = mySDL.readEntry("SDL", "Down", "DOWN");
	myDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), 274);
	val = mySDL.readEntry("SDL", "Left", "LEFT");
	myLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), 276);
	val = mySDL.readEntry("SDL", "Right", "RIGHT");
	myRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), 275);
	val = mySDL.readEntry("SDL", "SideKey", "SPACE");
	mySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), 32);
	val = mySDL.readEntry("SDL", "Music", "F10");
	myMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), 291);
	val = mySDL.readEntry("SDL", "C", "BACKSPACE");
	myC = keyCFG.readNumEntry("KEYCODES", QString(val), 8);
	val = mySDL.readEntry("SDL", "LeftSoftkey", "F9");
	myLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), 290);
	val = mySDL.readEntry("SDL", "RightSoftkey", "F11");
	myRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), 292);
	val = mySDL.readEntry("SDL", "Call", "TAB");
	myCALL = keyCFG.readNumEntry("KEYCODES", QString(val), 9);
	val = mySDL.readEntry("SDL", "Camera", "PAUSE");
	myCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), 19);
	val = mySDL.readEntry("SDL", "Slider", "F8");
	mySLIDER = keyCFG.readNumEntry("KEYCODES", QString(val), 289);
	val = mySDL.readEntry("SDL", "VolumeUp", "PLUS");
	myVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), 43);
	val = mySDL.readEntry("SDL", "VolumeDown", "MINUS");
	myVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), 45);
	val = mySDL.readEntry("SDL", "0", "0");
	my0 = keyCFG.readNumEntry("KEYCODES", QString(val), 48);
	val = mySDL.readEntry("SDL", "1", "1");
	my1 = keyCFG.readNumEntry("KEYCODES", QString(val), 49);
	val = mySDL.readEntry("SDL", "2", "2");
	my2 = keyCFG.readNumEntry("KEYCODES", QString(val), 50);
	val = mySDL.readEntry("SDL", "3", "3");
	my3 = keyCFG.readNumEntry("KEYCODES", QString(val), 51);
	val = mySDL.readEntry("SDL", "4", "4");
	my4 = keyCFG.readNumEntry("KEYCODES", QString(val), 52);
	val = mySDL.readEntry("SDL", "5", "5");
	my5 = keyCFG.readNumEntry("KEYCODES", QString(val), 53);
	val = mySDL.readEntry("SDL", "6", "6");
	my6 = keyCFG.readNumEntry("KEYCODES", QString(val), 54);
	val = mySDL.readEntry("SDL", "7", "7");
	my7 = keyCFG.readNumEntry("KEYCODES", QString(val), 55);
	val = mySDL.readEntry("SDL", "8", "8");
	my8 = keyCFG.readNumEntry("KEYCODES", QString(val), 56);
	val = mySDL.readEntry("SDL", "9", "9");
	my9 = keyCFG.readNumEntry("KEYCODES", QString(val), 57);
	val = mySDL.readEntry("SDL", "Asterisk", "ASTERISK");
	myASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), 42);
	val = mySDL.readEntry("SDL", "Numeral", "HASH");
	myNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), 35);
	val = mySDL.readEntry("SDLextra", "Red", "ESCAPE");
  	SmyRED = keyCFG.readNumEntry("KEYCODES", QString(val), 27);
	val = mySDL.readEntry("SDLextra", "Center", "RETURN");
	SmyCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), 13);
	val = mySDL.readEntry("SDLextra", "Up", "UP");
	SmyUP = keyCFG.readNumEntry("KEYCODES", QString(val), 273);
	val = mySDL.readEntry("SDLextra", "Down", "DOWN");
	SmyDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), 274);
	val = mySDL.readEntry("SDLextra", "Left", "LEFT");
	SmyLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), 276);
	val = mySDL.readEntry("SDLextra", "Right", "RIGHT");
	SmyRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), 275);
	val = mySDL.readEntry("SDLextra", "SideKey", "SPACE");
	SmySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), 32);
	val = mySDL.readEntry("SDLextra", "Music", "F10");
	SmyMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), 291);
	val = mySDL.readEntry("SDLextra", "C", "BACKSPACE");
	SmyC = keyCFG.readNumEntry("KEYCODES", QString(val), 8);
	val = mySDL.readEntry("SDLextra", "LeftSoftkey", "F9");
	SmyLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), 290);
	val = mySDL.readEntry("SDLextra", "RightSoftkey", "F11");
	SmyRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), 292);
	val = mySDL.readEntry("SDLextra", "Call", "TAB");
	SmyCALL = keyCFG.readNumEntry("KEYCODES", QString(val), 9);
	val = mySDL.readEntry("SDLextra", "Camera", "PAUSE");
	SmyCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), 19);
	val = mySDL.readEntry("SDLextra", "Slider", "F8");
	SmySLIDER = keyCFG.readNumEntry("KEYCODES", QString(val), 289);
	val = mySDL.readEntry("SDLextra", "VolumeUp", "PLUS");
	SmyVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), 43);
	val = mySDL.readEntry("SDLextra", "VolumeDown", "MINUS");
	SmyVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), 45);
	val = mySDL.readEntry("SDLextra", "0", "0");
	Smy0 = keyCFG.readNumEntry("KEYCODES", QString(val), 48);
	val = mySDL.readEntry("SDLextra", "1", "1");
	Smy1 = keyCFG.readNumEntry("KEYCODES", QString(val), 49);
	val = mySDL.readEntry("SDLextra", "2", "2");
	Smy2 = keyCFG.readNumEntry("KEYCODES", QString(val), 50);
	val = mySDL.readEntry("SDLextra", "3", "3");
	Smy3 = keyCFG.readNumEntry("KEYCODES", QString(val), 51);
	val = mySDL.readEntry("SDLextra", "4", "4");
	Smy4 = keyCFG.readNumEntry("KEYCODES", QString(val), 52);
	val = mySDL.readEntry("SDLextra", "5", "5");
	Smy5 = keyCFG.readNumEntry("KEYCODES", QString(val), 53);
	val = mySDL.readEntry("SDLextra", "6", "6");
	Smy6 = keyCFG.readNumEntry("KEYCODES", QString(val), 54);
	val = mySDL.readEntry("SDLextra", "7", "7");
	Smy7 = keyCFG.readNumEntry("KEYCODES", QString(val), 55);
	val = mySDL.readEntry("SDLextra", "8", "8");
	Smy8 = keyCFG.readNumEntry("KEYCODES", QString(val), 56);
	val = mySDL.readEntry("SDLextra", "9", "9");
	Smy9 = keyCFG.readNumEntry("KEYCODES", QString(val), 57);
	val = mySDL.readEntry("SDLextra", "Asterisk", "ASTERISK");
	SmyASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), 42);
	val = mySDL.readEntry("SDLextra", "Numeral", "HASH");
	SmyNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), 35);
}


static pid_t pid = -1;
void SDL_ShowSplash()
{
  char buf[16];
  sprintf(buf, "%d", getpid());
  char *const argv[] = { "/mmc/mmca1/games/bin/splash.sh", buf, 0 };
  if( (pid = vfork()) == 0){
    execve("/mmc/mmca1/games/bin/splash.sh", argv, 0);
    printf("execve splash: %s\n", strerror(errno));
    exit(-1);
  }
  else if(pid < 0) printf("vfork splash: %s\n", strerror(errno));
  else printf("show splash: %d\n", pid);
}

void SDL_HideSplash()
{
  printf("hide splash: %d\n", pid);
  if(pid > 0){
    int r = kill(pid, SIGINT);
    if(r < 0) printf("kill splash: %s\n", strerror(errno));
    sleep(1);
    r = waitpid(pid, 0, WNOHANG);
    if(r < 0) printf("wait splash: %s\n", strerror(errno));
  }
  CargarTeclas();

}

static inline bool needSuspend()
{
  if( UTIL_GetIncomingCallStatus() || UTIL_GetFlipStatus() || UTIL_GetSideKeyLock() )
    return true;
  return false;
}

void SDL_ChannelExists(const char *channelName)
{
  SDL_MotoChannel = channelName;
  QString cmd = "/mmc/mmca1/games/bin/qcop " + SDL_MotoChannel + " up";
  int r = system(cmd.latin1());
  if(WIFEXITED(r)){
    if(WEXITSTATUS(r) == 0) exit(0);
  }
  printf("'%s' - no such channel, run application\n", channelName);
}

SDL_QWin::SDL_QWin(const QSize& size)
  : QWidget(0, "SDL_main", WType_TopLevel | WStyle_Customize | WStyle_NoBorder),
		my_image(0),
    my_inhibit_resize(false), my_mouse_pos(-1, -1), my_flags(0),
    my_locked(0), my_special(false),
    cur_mouse_button(EZX_LEFT_BUTTON),
    my_suspended(false), last_mod(false)
{
  last.scancode = 0;
  setBackgroundMode(NoBackground);
  qcop = new QCopChannel(SDL_MotoChannel, 0, 0);
  connect(qcop, SIGNAL(received(const QCString &, const QByteArray &)), this, SLOT(channel(const QCString &, const QByteArray &)));
  connect(qApp, SIGNAL(signalRaise(void)), this, SLOT(signalRaise()));
	//connect(qApp, SIGNAL(clickAppIcon(void)), this, SLOT(clickAppIcon()));
	setFocusPolicy(QWidget::StrongFocus);
	vmem = (char *)-1;
  fbdev = open("/dev/fb0", O_RDWR);
  if(fbdev < 0){
    printf("open(/dev/fb0): %s\n", strerror(errno));
    return;
  }

  struct fb_fix_screeninfo fsi;
  if(ioctl(fbdev, FBIOGET_FSCREENINFO, &fsi) < 0){
    printf("ioctl(FBIOGET_FSCREENINFO): %s\n", strerror(errno));
    return;
  }
  vmem_length = fsi.smem_len;

  vmem = (char *)mmap(0, fsi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev, 0);
  if(vmem == (char *)-1){
    printf("mmap: %s\n", strerror(errno));
  }
  else printf("map framebuffer at %p (size %d)\n", vmem, fsi.smem_len);

  setFixedSize(qApp->desktop()->size());
  init();
}

SDL_QWin::~SDL_QWin() {
  if(my_image) {
    delete my_image;
  }

  if(vmem != (char *)-1){
    munmap(vmem, vmem_length);
    vmem = (char *)-1;
  }

  if(fbdev != -1){
    close(fbdev);
    fbdev = -1;
  }

  delete qcop;
  printf("~SDL_QWin(): chau\n");
}

void SDL_QWin::channel(const QCString &, const QByteArray &)
{
  printf("got signal from channel\n");
  resume();
}

static bool suspended = false;
void SDL_QWin::signalRaise()
{
	printf("signal raise\n");
  resume();
}

/*void SDL_QWin::clickAppIcon()
{
	printf("click app icon\n");
}*/

void SDL_QWin::setImage(QImage *image) {
  if ( my_image ) {
    delete my_image;
  }
  my_image = image;
}

void SDL_QWin::resizeEvent(QResizeEvent *e) {
}

#ifdef MOTOEZX_TEST
void SDL_QWin::focusInEvent(QFocusEvent *) {
  printf("focus in\n");
}

void SDL_QWin::focusOutEvent(QFocusEvent *) {
  printf("focus out\n");
	suspend();
}
#endif

void SDL_QWin::init()
{
  grabKeyboard();
  grabMouse();
  my_suspended = false;
  my_timer = startTimer(1000);
}

void SDL_QWin::suspend()
{
  printf("suspend\n");
  killTimer(my_timer);
  releaseKeyboard();
  releaseMouse();
  SDL_PrivateAppActive(false, SDL_APPINPUTFOCUS);
  my_suspended = true;
  hide();
}

void SDL_QWin::resume()
{
  if(!my_suspended || needSuspend()) return;
  setFocus();	
  printf("resume\n");
  init();
  show();
  SDL_PrivateAppActive(true, SDL_APPINPUTFOCUS);
}

void SDL_QWin::timerEvent(QTimerEvent *)
{
  if(needSuspend() && !my_suspended) suspend();
}

void SDL_QWin::closeEvent(QCloseEvent *e) {
  SDL_PrivateQuit();
  e->ignore();
}

void SDL_QWin::setMousePos(const QPoint &pos) {
  if(my_image->width() == height()) {
    if (screenRotation == SDL_QT_ROTATION_90)
      my_mouse_pos = QPoint(height()-pos.y(), pos.x());
    else if (screenRotation == SDL_QT_ROTATION_270)
      my_mouse_pos = QPoint(pos.y(), width()-pos.x());
  } else {
    my_mouse_pos = pos;
  }
}

void SDL_QWin::mouseMoveEvent(QMouseEvent *e) {
  int sdlstate = 0;
  if(cur_mouse_button == EZX_LEFT_BUTTON) {
    sdlstate |= SDL_BUTTON_LMASK;
  }
  else {
    sdlstate |= SDL_BUTTON_RMASK;
  }
  setMousePos(e->pos());
  SDL_PrivateMouseMotion(sdlstate, 0, my_mouse_pos.x(), my_mouse_pos.y());
}

void SDL_QWin::mousePressEvent(QMouseEvent *e) {
  mouseMoveEvent(e);
  Qt::ButtonState button = e->button();
  cur_mouse_button = my_special ? EZX_RIGHT_BUTTON : EZX_LEFT_BUTTON;
  SDL_PrivateMouseButton(SDL_PRESSED, cur_mouse_button,
			 my_mouse_pos.x(), my_mouse_pos.y());
}

void SDL_QWin::mouseReleaseEvent(QMouseEvent *e) {
  setMousePos(e->pos());
  Qt::ButtonState button = e->button();
  SDL_PrivateMouseButton(SDL_RELEASED, cur_mouse_button,
			 my_mouse_pos.x(), my_mouse_pos.y());
  my_mouse_pos = QPoint(-1, -1);
}

void SDL_QWin::repaintRect(const QRect& rect) {
  if(!rect.width() || !rect.height() || !isVisible())
    return;
  
  /* next - special for 18bpp framebuffer */
  /* so any other - back off */
  if(QPixmap::defaultDepth() != 24) return;

#if 1
  // 18 bpp - really 3 bytes per pixel
  if(screenRotation == SDL_QT_ROTATION_90){
    QRect rs = my_image->rect();
    QRect rd;

    int id, jd;

    if(rect.y() + rect.height() > 240){
      rs.setRect(rect.y(), 240 - rect.width() - rect.x(), rect.height(), rect.width());
      rd = rect;
      jd = rect.y() + rect.height() - 1;
      id = rect.x();
    }
    else{
      rs = rect;
      rd.setRect(rect.y(), 320 - rect.width() - rect.x(), rect.height(), rect.width());
      jd = 319 - rect.x();
      id = rect.y();
    }

    //printf("rs: %d %d %d %d\n", rs.x(), rs.y(), rs.width(), rs.height());
    //printf("rd: %d %d %d %d\n", rd.x(), rd.y(), rd.width(), rd.height());
    //printf("id: %d, jd: %d\n", id, jd);

    int ii = id, jj;
    uchar *src0 = (uchar *)my_image->bits();
    uchar *dst0 = (uchar *)vmem;
    uchar *dst, *src;

    src += rs.y() * my_image->bytesPerLine() + rs.x() * 3;

    int is_lim = rs.y() + rs.height();
    int dst_offset = jd * 720 + id * 3;
    int src_offset = rs.y() * my_image->bytesPerLine() + rs.x() * 3;

    for(int ii = rs.y(); ii < is_lim;
        dst_offset += 3, src_offset += my_image->bytesPerLine(), ii++){
      dst = dst0 + dst_offset;
      src = src0 + src_offset;
      for(int j = 0; j < rs.width(); j++){
        //unsigned short tmp = ((unsigned short)(src[1] & 0xf8)) << 2;
        //dst[0] = src[0] << 1;
        //dst[1] = ((src[0] & 0x80) >> 7) | ((src[1] & 0x7) << 1) | (tmp & 0xff);
        //dst[2] = (tmp & 0x300) >> 8;
		dst[0] = (src[0] >> 2) | ((src[1] & 0x0c) << 4);
		dst[1] = (src[1] >> 4) | ((src[2] & 0x3c) << 2);
		dst[2] = src[2] >> 6;
        dst -= 720;
        src += 3;//2
      }
    }
    //printf("done\n");
  }
  else if(screenRotation == SDL_QT_ROTATION_270){
    QRect rs = my_image->rect();
    QRect rd;

    int id, jd;

    if(rect.y() + rect.height() > 240){
      rs.setRect(rect.y(), 240 - rect.width() - rect.x(), rect.height(), rect.width());
      rd = rect;
      jd = rect.y();
      id = rect.x() + rect.width() - 1;
    }
    else{
      rs = rect;
      rd.setRect(rect.y(), 320 - rect.width() - rect.x(), rect.height(), rect.width());
      jd = rect.x();
      id = 239 - rect.y();
    }

    int ii = id, jj;
    uchar *src0 = (uchar *)my_image->bits();
    uchar *dst0 = (uchar *)vmem;
    uchar *dst, *src;

    src += rs.y() * my_image->bytesPerLine() + rs.x() * 3;

    int is_lim = rs.y() + rs.height();
    int dst_offset = jd * 720 + id * 3;
    int src_offset = rs.y() * my_image->bytesPerLine() + rs.x() * 3;

    for(int ii = rs.y(); ii < is_lim;
        dst_offset -= 3, src_offset += my_image->bytesPerLine(), ii++){
      dst = dst0 + dst_offset;
      src = src0 + src_offset;
      for(int j = 0; j < rs.width(); j++){
        //unsigned short tmp = ((unsigned short)(src[1] & 0xf8)) << 2;
        //dst[0] = src[0] << 1;
        //dst[1] = ((src[0] & 0x80) >> 7) | ((src[1] & 0x7) << 1) | (tmp & 0xff);
        //dst[2] = (tmp & 0x300) >> 8;
		dst[0] = (src[0] >> 2) | ((src[1] & 0x0c) << 4);
		dst[1] = (src[1] >> 4) | ((src[2] & 0x3c) << 2);
		dst[2] = src[2] >> 6;
        dst += 720;
        src += 3;//2
      }
    }
  }
  else
#endif
  {
    uchar *src0 = (uchar *)my_image->bits();
    uchar *dst0 = (uchar *)vmem;
    uchar *dst, *src;
    
    int is_lim = rect.y() + rect.height();
    int s_offset = rect.y() * my_image->bytesPerLine() + rect.x() * 3;
    int offset = rect.y() * 720 + rect.x() * 3;

    for(int ii = rect.y(); ii < is_lim; ii++, offset += 720,
                s_offset += my_image->bytesPerLine()){
      dst = dst0 + offset;
      src = src0 + s_offset;
      for(int j = 0; j < rect.width(); j++){      
		dst[0] = (src[0] >> 2) | ((src[1] & 0x0c) << 4);
		dst[1] = (src[1] >> 4) | ((src[2] & 0x3c) << 2);
		dst[2] = src[2] >> 6;
		src += 3;
		dst += 3;
      }
    }
  }
}

// This paints the current buffer to the screen, when desired. 
void SDL_QWin::paintEvent(QPaintEvent *ev) {  
  if(my_image) {
    repaintRect(ev->rect());
  }
}  

/* Function to translate a keyboard transition and queue the key event
 * This should probably be a table although this method isn't exactly
 * slow.
 */
void SDL_QWin::QueueKey(QKeyEvent *e, int pressed)
{  
  SDL_keysym keysym;
  int scancode = e->key();

  //if(pressed){
    if(last.scancode){
      // we press/release mod-key without releasing another key
      if(last_mod != my_special){
        SDL_PrivateKeyboard(SDL_RELEASED, &last);
      }
    }
  //}

  /* Set the keysym information */

    switch(scancode) {
      case 0x1031: //Cancel
		scancode = my_special ? SmyRED : myRED;
      break;
      case 0x1004: //Joystick center
		scancode = my_special ? SmyCENTER : myCENTER;
      break;
      case 0x1012: // Qt::Key_Left
        if (screenRotation == SDL_QT_ROTATION_90) scancode = my_special ? SmyUP : myUP;
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = my_special ? SmyDOWN : myDOWN;
        else scancode = my_special ? SmyLEFT : myLEFT; 
      break;
      case 0x1013: // Qt::Key_Up
        if (screenRotation == SDL_QT_ROTATION_90) scancode = my_special ? SmyRIGHT : myRIGHT;
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = my_special ? SmyLEFT : myLEFT;
        else scancode = my_special ? SmyUP : myUP;
      break;
      case 0x1014: // Qt::Key_Right
        if (screenRotation == SDL_QT_ROTATION_90) scancode = my_special ? SmyDOWN : myDOWN;
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = my_special ? SmyUP : myUP;
        else scancode = my_special ? SmyRIGHT : myRIGHT;
      break;
      case 0x1015: // Qt::Key_Down
        if (screenRotation == SDL_QT_ROTATION_90) scancode = my_special ? SmyLEFT : myLEFT;
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = my_special ? SmyRIGHT : myRIGHT;
        else scancode = my_special ? SmyDOWN : myDOWN;
      break;
      case 0x1005: //special key
        scancode = my_special ? SmySIDE : mySIDE;
      break;
      case 0x1033: //Music key
        scancode = my_special ? SmyMUSIC : myMUSIC;
      break; 
	  case 0x1032: // C key
        scancode = my_special ? SmyC : myC;
      break;
	  case 0x1038: // Left softkey
        scancode = my_special ? SmyLSOFT : myLSOFT;
      break;
	  case 0x103a: // Right softkey
        scancode = my_special ? SmyRSOFT : myRSOFT;
      break;
	  case 0x1030: // Call
        scancode = my_special ? SmyCALL : myCALL;
      break;
	  case 0x1034: // Photo
        scancode = my_special ? SmyCAMERA : myCAMERA;
      break;
	  case 0x1046: // Slide toggle
        scancode = my_special ? SmySLIDER : mySLIDER;
      break;
	  case 0x1016: //VolUp
        scancode = my_special ? SmyVOLUP : myVOLUP;
      break;
      case 0x1017: //VolDown
		scancode = my_special ? SmyVOLDOWN : myVOLDOWN;
      break;
      case 0x0030: // 0
		scancode = my_special ? Smy0 : my0;
      break;
      case 0x0031: // 1
		scancode = my_special ? Smy1 : my1;
      break;
      case 0x0032: // 2
		scancode = my_special ? Smy2 : my2;
      break;
      case 0x0033: // 3
		scancode = my_special ? Smy3 : my3;
      break;
      case 0x0034: // 4
		scancode = my_special ? Smy4 : my4;
      break;
      case 0x0035: // 5
		scancode = my_special ? Smy5 : my5;
      break;
      case 0x0036: // 6
		scancode = my_special ? Smy6 : my6;
      break;
      case 0x0037: // 7
		scancode = my_special ? Smy7 : my7;
      break;
      case 0x0038: // 8
		scancode = my_special ? Smy8 : my8;
      break;
      case 0x0039: // 9
		scancode = my_special ? Smy9 : my9;
      break;
      case 0x002a: // Asterisk
		scancode = my_special ? SmyASTERISK : myASTERISK;
      break;
      case 0x0023: // Numeral
		scancode = my_special ? SmyNUMERAL : myNUMERAL;
      break;
      default:
		//printf("Unknown key %x\n", scancode);
        scancode = SDLK_0;
      break;
	}

	if ( scancode == 0 ) suspend();

    if ( scancode == 9999 ) { 
		if (my_special == false) { 
			if(pressed) my_special = true; else my_special = false;
		} else {
			if(pressed) my_special = false; else my_special = true;
		}	
		return;
	} 
  
	keysym.sym = static_cast<SDLKey>(scancode);    
  
	keysym.scancode = scancode;
	keysym.mod = KMOD_NONE;
	if ( SDL_TranslateUNICODE ) {
	QChar qchar = e->text()[0];
	keysym.unicode = qchar.unicode();
	} else {
	keysym.unicode = 0;
	}
	
	last = keysym;
	last_mod = my_special;

	/* Queue the key event */
	if ( pressed ) {
		SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
	} else {
		last.scancode = 0;
		SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
	}
}

