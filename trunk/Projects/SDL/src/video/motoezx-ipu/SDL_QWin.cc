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
#include <stdlib.h>

screenRotationT screenRotation = SDL_QT_NO_ROTATION;

#ifdef __cplusplus
extern "C" {
#endif

extern int UTIL_GetIncomingCallStatus();
extern int UTIL_GetFlipStatus();
extern int UTIL_GetSideKeyLock();
#ifdef __cplusplus
}
#endif

static inline bool needSuspend()
{
  if(UTIL_GetIncomingCallStatus() || UTIL_GetFlipStatus() ||
      UTIL_GetSideKeyLock())
    return true;
  return false;
}

#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_BGRA6666  ipu_fourcc('B','G','R','6')
#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3') 
#define IPU_PIX_FMT_BGRA6666  ipu_fourcc('B','G','R','6')
#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P')
#define IPU_PIX_FMT_BGR565  ipu_fourcc('B','G','R','P')

#define IPU_PIX_FMT_YUV420P ipu_fourcc('I','4','2','0')
#define IPU_PIX_FMT_YUV420P2 ipu_fourcc('Y','U','1','2')
#define IPU_PIX_FMT_YUYV    ipu_fourcc('Y','U','Y','V')
#define IPU_PIX_FMT_YVU422P ipu_fourcc('Y','V','1','6')

int SDL_QWin::ipu_init()
{
	pp_init_params pp_init;
	pp_reqbufs_params pp_reqbufs;
 
	if ((fd_pp = open("/dev/alt_mxc_ipu_pp", O_RDWR, 0)) < 0) {
		perror("open(/dev/alt_mxc_ipu_pp)\n");
		fprintf(stderr, "Z6: if video stalls sometimes, load module alt_mxc_pp.ko\n");
 
 		if ((fd_pp = open("/dev/mxc_ipu_pp", O_RDWR, 0)) < 0) {
 			perror("open(/dev/mxc_ipu_pp)\n");
 			goto err_1;
 		}
	}
 
	memset(&pp_init, 0, sizeof(pp_init));
	pp_init.mode = PP_PP;
	pp_init.in_width = 240;
	pp_init.in_height = 320;
	pp_init.in_stride = 240;
	pp_init.in_pixel_fmt = IPU_PIX_FMT_RGB24;
	pp_init.out_width = 240;
	pp_init.out_height = 320;
	pp_init.out_stride = 240;
	pp_init.mid_stride = 240;
	pp_init.rot = 0;
	pp_init.out_pixel_fmt = IPU_PIX_FMT_BGRA6666;
 
	if (ioctl(fd_pp, PP_IOCTL_INIT, &pp_init) < 0) {
		perror("PP_IOCTL_INIT");
		goto err_1;
	}
	
	pp_reqbufs.count = 1;
	pp_reqbufs.req_size = ((((240*320*3)) + 63) & ~63) ;

	if (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs) < 0) {
		perror("PP_IOCTL_REQBUFS");
		goto err_1;
	}

	pp_desc.index = 0;
	if (ioctl(fd_pp, PP_IOCTL_QUERYBUF, &pp_desc) < 0) {
		perror("PP_IOCTL_QUERYBUF failed");
		goto err_1;
	}
	
	pp_dma_buffer = (char*)mmap (NULL, pp_desc.size, 
	                      PROT_READ | PROT_WRITE, MAP_SHARED, 
	                      fd_pp, pp_desc.addr);
	if (pp_dma_buffer == MAP_FAILED) {
		pp_dma_buffer = NULL;
		perror("mmap IPU");
		goto err_1;
	}

	memset(pp_dma_buffer, 0x10, 240*320);
	memset(pp_dma_buffer+320*240, 0x80, 240*320/2);
 
	return 0;
	
err_1:
	//ioctl(fbdev, FBIOPUT_VSCREENINFO, &fb_orig_vinfo);
	return -1;
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
	connect(qApp, SIGNAL(signalRaise(void)), this, SLOT(signalRaise()));
	//connect(qApp, SIGNAL(clickAppIcon(void)), this, SLOT(clickAppIcon()));
	setFocusPolicy(QWidget::StrongFocus);
	vmem = (char *)-1;
	fbdev = open("/dev/fb0", O_RDWR);
	if(fbdev < 0){
		fprintf(stderr, "open(/dev/fb0): %s\n", strerror(errno));
		return;
	}

	if(ioctl(fbdev, FBIOGET_FSCREENINFO, &fsi) < 0){
		fprintf(stderr, "ioctl(FBIOGET_FSCREENINFO): %s\n", strerror(errno));
		return;
	}
	vmem_length = fsi.smem_len;

	vmem = (char *)mmap(0, fsi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev, 0);
	if(vmem == (char *)-1){
		fprintf(stderr, "mmap: %s\n", strerror(errno));
	} else
		fprintf(stderr, "map framebuffer at %p (size %d)\n", vmem, fsi.smem_len);

	setFixedSize(qApp->desktop()->size());

	if (ipu_init() != 0)
		fprintf(stderr, "WARNING: IPU initialization failed\n");
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
	if (fd_pp != -1) {
		close(fd_pp);
		fd_pp = -1;
	}
//	printf("~SDL_QWin(): done\n");
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
	fprintf(stderr, "focus in\n");
}

void SDL_QWin::focusOutEvent(QFocusEvent *) {
	fprintf(stderr, "focus out\n");
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
  if(/*my_image->width()*/240 == height()) {
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
  if(!rect.width() || !rect.height() /*|| !isVisible()*/) {
    return;
  }
  
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

    src += rs.y() * my_image->bytesPerLine() + rs.x() * 2;

    int is_lim = rs.y() + rs.height();
    int dst_offset = jd * 720 + id * 3;
    int src_offset = rs.y() * my_image->bytesPerLine() + rs.x() * 2;

    for(int ii = rs.y(); ii < is_lim;
        dst_offset += 3, src_offset += my_image->bytesPerLine(), ii++){
      dst = dst0 + dst_offset;
      src = src0 + src_offset;
      for(int j = 0; j < rs.width(); j++){
        unsigned short tmp = ((unsigned short)(src[1] & 0xf8)) << 2;
        dst[0] = src[0] << 1;
        dst[1] = ((src[0] & 0x80) >> 7) | ((src[1] & 0x7) << 1) | (tmp & 0xff);
        dst[2] = (tmp & 0x300) >> 8;
        dst -= 720;
        src += 2;
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

    src += rs.y() * my_image->bytesPerLine() + rs.x() * 2;

    int is_lim = rs.y() + rs.height();
    int dst_offset = jd * 720 + id * 3;
    int src_offset = rs.y() * my_image->bytesPerLine() + rs.x() * 2;

    for(int ii = rs.y(); ii < is_lim;
        dst_offset -= 3, src_offset += my_image->bytesPerLine(), ii++){
      dst = dst0 + dst_offset;
      src = src0 + src_offset;
      for(int j = 0; j < rs.width(); j++){
        unsigned short tmp = ((unsigned short)(src[1] & 0xf8)) << 2;
        dst[0] = src[0] << 1;
        dst[1] = ((src[0] & 0x80) >> 7) | ((src[1] & 0x7) << 1) | (tmp & 0xff);
        dst[2] = (tmp & 0x300) >> 8;
        dst += 720;
        src += 2;
      }
    }
  }
  else
#endif
  {
//	printf("Repaint\n");
	pp_start_params pp_st;
	memset(&pp_st, 0, sizeof(pp_st));
	pp_st.wait = 0;

	pp_st.in.index = -1;
	pp_st.in.size = ((((240*320*3)) + 63) & ~63);
	pp_st.in.addr = pp_desc.addr;

	pp_st.out.index = -1;
	pp_st.out.size = 240*320*3;
	pp_st.out.addr = fsi.smem_start;
//	pp_st.out.addr += 3*(240*320);

	if (ioctl(fd_pp, PP_IOCTL_START, &pp_st) == 0) {
		return;
	}
	if (ioctl(fd_pp, PP_IOCTL_WAIT, 0)) {
		perror("PP_IOCTL_WAIT");
		return;
	}
//	printf("Done\n");
/*    uchar *src0 = (uchar *)my_image->bits();
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
    }*/
  }
}

// This paints the current buffer to the screen, when desired. 
void SDL_QWin::paintEvent(QPaintEvent *ev) {  
//	if(my_image) {
		repaintRect(ev->rect());
//	}
}  

inline int SDL_QWin::keyUp()
{
  return my_special ? SDLK_g : SDLK_UP;
}

inline int SDL_QWin::keyDown()
{
  return my_special ? SDLK_h : SDLK_DOWN;
}

inline int SDL_QWin::keyLeft()
{
  return my_special ? SDLK_i : SDLK_LEFT;
}

inline int SDL_QWin::keyRight()
{
  return my_special ? SDLK_j : SDLK_RIGHT;
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
  if(scancode >= 'A' && scancode <= 'Z') {
    // Qt sends uppercase, SDL wants lowercase
    keysym.sym = static_cast<SDLKey>(scancode + 32);
  } else if(scancode  >= 0x1000) {
    // Special keys
    switch(scancode) {
      case 0x1031: //Cancel
        scancode = my_special ? SDLK_a : SDLK_ESCAPE;
      break;
      case 0x1004: //Joystick center
        scancode = my_special ? SDLK_b : SDLK_RETURN;
      break;
      case 0x1012: // Qt::Key_Left
        if (screenRotation == SDL_QT_ROTATION_90) scancode = keyUp();
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = keyDown();
        else scancode = keyLeft();
      break;
      case 0x1013: // Qt::Key_Up
        if (screenRotation == SDL_QT_ROTATION_90) scancode = keyRight();
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = keyLeft();
        else scancode = keyUp();
      break;
      case 0x1014: // Qt::Key_Right
        if (screenRotation == SDL_QT_ROTATION_90) scancode = keyDown();
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = keyUp();
        else scancode = keyRight();
      break;
      case 0x1015: // Qt::Key_Down
        if (screenRotation == SDL_QT_ROTATION_90) scancode = keyLeft();
        else if (screenRotation == SDL_QT_ROTATION_270) scancode = keyRight();
        else scancode = keyDown();
      break;
      case 0x1005: //special key
      case 0x104d: //special key
        if(pressed) my_special = true;
        else my_special = false;
      return;
	  // functional keys
	  case 0x1032:
        scancode = SDLK_BACKSPACE;
      break;
	  case 0x1035:
        scancode = SDLK_F6;
      break;
	  case 0x1036:
        scancode = SDLK_F7;
      break;
	  case 0x1037:
        scancode = SDLK_F8;
      break;
	  case 0x1038:
        scancode = SDLK_F9;
      break;
	  case 0x1039:
        scancode = SDLK_F10;
      break;
	  case 0x103a:
        scancode = SDLK_F11;
      break;
	  case 0x103b:
        scancode = SDLK_F12;
      break;
	  // end functional keys
	  case 0x1016: //VolUp
        scancode = my_special ? SDLK_c : SDLK_PLUS;
      break;
      case 0x1017: //VolDown
        scancode = my_special ? SDLK_d : SDLK_MINUS;
      break;
      case 0x1034: //Photo
        scancode = my_special ? SDLK_e : SDLK_PAUSE;
      break;
      case 0x1030: //Call
        scancode = my_special ? SDLK_f : SDLK_SPACE;
      break;
			case 0x104b: //Prev
				scancode = my_special ? SDLK_k : SDLK_o;
			break;
			case 0x1049: //Play
				scancode = my_special ? SDLK_l : SDLK_p;
			break;
			case 0x104c: //Next
				scancode = my_special ? SDLK_m : SDLK_q;
			break;
			case 0x1033: //Browser
				scancode = my_special ? SDLK_n : SDLK_r;
			break;
    
      default:
        scancode = SDLK_UNKNOWN;
      break;
    }
    keysym.sym = static_cast<SDLKey>(scancode);    
  } else {
    keysym.sym = static_cast<SDLKey>(scancode);    
  }
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
