// Copyright (C) 2003  Jeremy Stanley
// This is free software licensed under the GNU GPL; see COPYING for details

#include "slideshow.h"

#include <iostream>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>

#include "SDLFont.h"
#include "fontdata.h"
#include "SDL_image.h"
#include "file_list.h"

static volatile bool waiting_for_keypress = false;

Uint32 timer_callback(Uint32 interval, void *param)
{
	// avoid backlog by not firing the next-slide event while busy
	if ( waiting_for_keypress )
	{
		SDL_Event e;
		e.type = SDL_USEREVENT;
		SDL_PushEvent( &e );
	}
	return interval;
}

slideshow::user_command slideshow::wait_for_command()
{
	SDL_Event e;

	while(1)
	{
		waiting_for_keypress = true;
	
		int res = SDL_WaitEvent(&e);
		
		waiting_for_keypress = false;

		if ( !res )
			return c_quit;

		switch(e.type)
		{
		case SDL_QUIT:
			return c_quit;

		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return c_quit;

			case SDLK_t: 			
				if (m_timer_id == NULL)
					start_timer();
				else
					stop_timer();
				break;

			case SDLK_PAGEUP:
			case SDLK_LEFT:
				return c_prev_slide;

			case SDLK_PAGEDOWN:
			case SDLK_RIGHT:
			case SDLK_RETURN:
			case SDLK_SPACE:
				return c_next_slide;

			case SDLK_HOME:
				return c_first_slide;

			case SDLK_END:
				return c_last_slide;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			if ( e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_WHEELDOWN )
				return c_next_slide;
			else if ( e.button.button == SDL_BUTTON_RIGHT || e.button.button == SDL_BUTTON_WHEELUP )
				return c_prev_slide;
			break;

		case SDL_USEREVENT:
			return c_timer_advance;

		case SDL_VIDEORESIZE:
			m_options->width = e.resize.w;
			m_options->height = e.resize.h;
			if ( !init_graphics() )
				return c_quit;
			m_image_cache->flush();
			m_prev_image_index = -1;
			return c_redraw;
		}
	}

	// shouldn't get here
	assert( false );
	return c_quit;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

slideshow::slideshow(options *opt, file_list *fl) :
	m_options(opt),
	m_file_list(fl),
	m_font(NULL),
	m_timer_id(NULL),
	m_sdl(NULL),
	m_image_index(0),
	m_prev_image_index(0)
{
	srand( (unsigned)time(NULL) );

	m_cache_callback = new image_cache_callback(*this);
	m_image_cache    = new image_cache(m_cache_callback, 5);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);
	init_true_color_format();
}

slideshow::~slideshow()
{
	stop_timer();
	SDL_Quit();

	delete m_image_cache;
	delete m_cache_callback;
}

void slideshow::start_timer()
{
	if ( (m_timer_id == NULL) && (m_options->advance_time > 0.0) )
	{
		m_timer_id = SDL_AddTimer( (Uint32)(m_options->advance_time * 1000.0), timer_callback, NULL );
	}
}

void slideshow::stop_timer()
{
	if ( m_timer_id != NULL )
	{
		SDL_RemoveTimer( m_timer_id );
		m_timer_id = NULL;
	}
}

bool slideshow::run()
{
	if ( !init_graphics() )
	{
		return false;
	}

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Have the first image ready in the cache before starting the timer
	m_image_cache->lookup(0);
	
	m_image_index = 0;
	m_prev_image_index = -1;

	start_timer();

	while(1)
	{
		if ( m_prev_image_index != m_image_index )
		{
			show_image();
			m_prev_image_index = m_image_index;
		}

		// After the image is shown and before we wait for a key, 
		// get the prev and next images ready.  We most likely only
		// need to load one of these, as the other will already be
		// in the cache.
		if ( m_image_index > 0 )
			m_image_cache->lookup( m_image_index - 1 );
		if ( m_image_index < m_file_list->count() - 1 )
			m_image_cache->lookup( m_image_index + 1 );

		if ( m_options->show_ready_indicator )
		{
			SDL_Rect rc;
			rc.x = m_sdl->w - 5;
			rc.y = 0;
			rc.w = 5;
			rc.h = 5;
			SDL_FillRect(m_sdl, &rc, SDL_MapRGB(m_sdl->format, 0, 255, 0));
			SDL_UpdateRects(m_sdl, 1, &rc);
		}

		user_command cmd = wait_for_command();

		switch(cmd)
		{
		case c_quit:
			stop_timer();
			return true;
		case c_first_slide:
			stop_timer();
			m_image_index = 0;
			break;
		case c_last_slide:
			stop_timer();
			m_image_index = m_file_list->count() - 1;
			break;
		case c_prev_slide:
			stop_timer();
			if ( m_image_index > 0 )
				m_image_index--;
			break;
		case c_next_slide:
			stop_timer();
			// fall through
		case c_timer_advance:
			if ( m_image_index < m_file_list->count() - 1 )
				m_image_index++;
			break;
		case c_redraw:
			break;
		}

		// stop the auto-advance when the last picture is about to be shown
		if ( m_image_index == m_file_list->count() - 1 )
			stop_timer();
	}

	return true;
}

SDL_Surface *slideshow::prep_image(int index)
{
	std::string filename = m_file_list->get(index);
	SDL_Surface *image = IMG_Load(filename.c_str());

	if ( !image )
	{
		return create_placeholder_image( filename, IMG_GetError() );
	}

	// convert the image to an acceptable pixel format
	// -----------------------------------------------------------------------------

	if ( !is_true_color(image) )
	{
		SDL_Surface *tc_image = convert_to_true_color(image);
		SDL_FreeSurface(image); 

		if ( !tc_image )
			return NULL;

		image = tc_image;
	}
	
	// scale the image if necessary
	// -----------------------------------------------------------------------------

	int w = image->w;
	int h = image->h;

	if (w != m_options->width)
	{
		h = (h * m_options->width) / w;
		w = m_options->width;
	}

	if (h > m_options->height)
	{
		w = (w * m_options->height) / h;
		h = m_options->height;
	}

	if ( w != image->w || h != image->h)
	{
		// scale the image
		SDL_Surface *scaled_image = scale_image(image, w, h);
		SDL_FreeSurface(image);

		if ( !scaled_image )
			return NULL;

		image = scaled_image;
	}

	// frame the scaled image, if necessary
	// -----------------------------------------------------------------------------

	if ( m_options->width != image->w || m_options->height != image->h)
	{
		SDL_Surface *framed_image = frame_image(image, m_options->width, m_options->height);
		SDL_FreeSurface(image); 

		if ( !framed_image )
			return NULL;

		image = framed_image;
	}

	// return the processed image
	// -----------------------------------------------------------------------------
	
	return image;
}

SDL_Surface *slideshow::create_placeholder_image( const std::string &filename, const char *error_str )
{
	static const Uint32 surface_create_mask 
		= SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA;

	Uint32 flags = m_sdl->flags & surface_create_mask;
	
	SDL_Surface *error_img = SDL_CreateRGBSurface(flags, m_sdl->w, m_sdl->h, m_sdl->format->BitsPerPixel,
		m_sdl->format->Rmask, m_sdl->format->Gmask, m_sdl->format->Bmask, m_sdl->format->Amask);
	if ( !error_img )
		return NULL;
	
	SDLFont font(error_img);
	font.InitFont(fontdata, font_width, font_height);

	int xc = error_img->w / 2;
	int y  = error_img->h / 2 - 16;

	std::string message =  std::string("Cannot display image ") + filename + ":";
	center_shadow_text(font, message.c_str(), xc, y, 255, 127, 127);
	y += 16;

	center_shadow_text(font, error_str, xc, y, 255, 127, 127);

	return error_img;
}

void slideshow::show_image()
{
	SDL_Surface *image = m_image_cache->lookup(m_image_index);

	if ( image )
	{
		if ( (m_options->transition != tran_none) && (m_prev_image_index >= 0) && (m_image_index != m_prev_image_index) )
		{
			SDL_Surface *prev_image = m_image_cache->lookup(m_prev_image_index);
			do_transition(prev_image, image);
		}

		SDL_BlitSurface(image, NULL, m_sdl, NULL);
	}
	else
	{
		// For mundane errors such as failures to load pictures, we'll store
		// a placeholder in the image cache which will contain a somewhat
		// more useful error message.  We'll get this if some surface convert/
		// scaling/framing operation failed, for whatever reason.
		SDL_FillRect(m_sdl, NULL, SDL_MapRGB(m_sdl->format, 0, 0, 0));
		center_shadow_text(*m_font, "[unspeakable error]", m_options->width / 2, m_options->height / 2 - 7, 255, 127, 127);
	}

	if ( m_options->display_name )
	{
		draw_picture_name();
		draw_picture_number();
	}

	SDL_Flip(m_sdl);
}

void slideshow::draw_picture_name()
{
	std::string name = m_file_list->get(m_image_index);
#ifdef WIN32
	// show backslashes to the luser on Winblows
	std::replace(name.begin(), name.end(), '/', '\\');
#endif
	draw_shadow_text(*m_font, name, 1, m_options->height - 16, 224, 224, 224); 
}

void slideshow::draw_picture_number()
{
	char pcount_buf[64];
	sprintf(pcount_buf, "%d/%d", m_image_index + 1, m_file_list->count());
	draw_shadow_text(*m_font, pcount_buf, m_options->width - strlen(pcount_buf) * m_font->getcw() - 1, m_options->height - 16, 224, 224, 224);
}

void slideshow::draw_shadow_text(SDLFont &font, const std::string &text, int x, int y, int r, int g, int b)
{
	font.SetColor(r/2, g/2, b/2);
	font.WriteText(x + 1, y + 1, text.c_str());
	font.SetColor(r, g, b);
	font.WriteText(x, y, text.c_str());
}

void slideshow::center_shadow_text(SDLFont &font, const std::string &text, int x, int y, int r, int g, int b)
{
	font.SetColor(r/2, g/2, b/2);
	font.CenterText(x + 1, y + 1, text.c_str());
	font.SetColor(r, g, b);
	font.CenterText(x, y, text.c_str());
}

bool slideshow::init_graphics()
{
    Uint32 flags = SDL_SWSURFACE;
	if ( m_options->windowed )
		flags |= SDL_RESIZABLE;
	else
		flags |= SDL_FULLSCREEN;

	m_sdl = SDL_SetVideoMode(m_options->width, m_options->height, 32, flags);

	if ( !m_sdl )
	{
		std::cerr << SDL_GetError() << std::endl;
		return false;
	}

	delete m_font;
	m_font = new SDLFont(m_sdl);
	if ( !m_font->InitFont(fontdata, font_width, font_height) )
	{
		std::cerr << "Failed to initialize font." << std::endl;
		return false;
	}

	SDL_WM_SetCaption("Perigee Slideshow", "Slideshow");

	return true;
}

void slideshow::init_true_color_format()
{
	memset(&m_true_color_format, 0, sizeof(SDL_PixelFormat));

	m_true_color_format.palette       = NULL;
	m_true_color_format.BitsPerPixel  = 32;
	m_true_color_format.BytesPerPixel = 4;
	m_true_color_format.Rmask         = 0x00FF0000UL;
	m_true_color_format.Gmask         = 0x0000FF00UL;
	m_true_color_format.Bmask         = 0x000000FFUL;
	m_true_color_format.Amask         = 0x00000000UL;
	m_true_color_format.Rshift        = 16;
	m_true_color_format.Gshift        = 8;
	m_true_color_format.Bshift        = 0;
	m_true_color_format.Ashift        = 0;
	m_true_color_format.Rloss         = 0;
	m_true_color_format.Gloss         = 0;
	m_true_color_format.Bloss         = 0;
	m_true_color_format.Aloss         = 8;
	m_true_color_format.colorkey      = 0;
	m_true_color_format.alpha         = SDL_ALPHA_OPAQUE;
}

bool slideshow::is_true_color(SDL_Surface *in)
{
	return (in->format->palette == NULL) && 
		(in->format->Rloss == in->format->Gloss == in->format->Bloss == 0);
}

SDL_Surface *slideshow::convert_to_true_color(SDL_Surface *in)
{
	static const Uint32 surface_convert_mask 
		= SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA;

	Uint32 flags = in->flags & surface_convert_mask;

	return SDL_ConvertSurface(in, &m_true_color_format, surface_convert_mask);
}

// Returns a copy of in, centered inside a w x h frame.
SDL_Surface *slideshow::frame_image(SDL_Surface *in, int w, int h)
{
	static const Uint32 surface_create_mask 
		= SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA;

	Uint32 flags = in->flags & surface_create_mask;

	SDL_Surface *framed = SDL_CreateRGBSurface(flags, w, h, in->format->BitsPerPixel,
		in->format->Rmask, in->format->Gmask, in->format->Bmask, in->format->Amask);
	if ( !framed )
		return NULL;

	SDL_FillRect(framed, NULL, SDL_MapRGB(framed->format, 0, 0, 0));

	SDL_Rect srcrect;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = in->w;
	srcrect.h = in->h;

	SDL_Rect dstrect;
	dstrect.x = (w - in->w) / 2;
	dstrect.y = (h - in->h) / 2;
	dstrect.w = in->w;
	dstrect.h = in->h;

	SDL_BlitSurface(in, &srcrect, framed, &dstrect);

	return framed;
}

// Returns a copy of in, scaled to w x h using bilinear interpolation.
// in must be true color (i.e., have 8 bits per color component)
SDL_Surface *slideshow::scale_image(SDL_Surface *in, int w, int h)
{
	static const Uint32 surface_create_mask 
		= SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA;

	Uint32 flags = in->flags & surface_create_mask;
	
	SDL_Surface *scaled = SDL_CreateRGBSurface(flags, w, h, in->format->BitsPerPixel,
		in->format->Rmask, in->format->Gmask, in->format->Bmask, in->format->Amask);
	if ( !scaled )
		return NULL;

	int Bpp = in->format->BytesPerPixel;
	assert( Bpp == scaled->format->BytesPerPixel );
	assert( Bpp == 3 || Bpp == 4 );

	int x, y, i;
	Uint8 c[4];
	double u = 0.0, v = 0.0;
	double du = (double)in->w / w;
	double dv = (double)in->h / h;
	
	int dr = scaled->pitch - (w * Bpp);

	unsigned char *p = (unsigned char *)scaled->pixels;

	for(y = 0; y < h; y++)
	{
		u = 0.0;

		for(x = 0; x < w; x++)
		{
			bilinearpix(in, u, v, c);

			for(i = 0; i < Bpp; i++)
			{
				p[i] = c[i];
			}
			p += Bpp;

			u += du;
		}

		p += dr;
		v += dv;
	}

	return scaled;
}

// Returns the value of the "pixel" at noninteger coordinates (u, v) using
// bilinear interpolation.  in must be true color.
void slideshow::bilinearpix(SDL_Surface *in, double u, double v, Uint8 *col)
{
    int Bpp = in->format->BytesPerPixel;
	
	unsigned int x = (unsigned int)u, y = (unsigned int)v;
    int i;
    Uint8 c00[4], c01[4], c10[4], c11[4];
    double dx = u - x;
    double dy = v - y;
    double fx0, fx1;

    // error:  return a red pixel, for no good reason
    if( x >= (unsigned int)in->w || y >= (unsigned int)in->h )
    {
        col[0] = 255;
        col[1] = 0;
        col[2] = 0;
        return;
    }

    // ensure we have pixels both down and to the right to work with
    if(x == (unsigned int)in->w - 1) { x--; dx = 1.0; }
    if(y == (unsigned int)in->h - 1) { y--; dy = 1.0; }

    // Now... we have the pixels at (x, y) .. (x+1, y+1) to interpolate about
    // dx = weight toward (x+1); dy = weight toward (y+1)

    // Find pointer to upper-left pixel
    Uint8 *p = (Uint8 *)in->pixels + in->pitch * y + Bpp * x;

    // Get colors of 4 neighbors
    memcpy(c00, p,                   Bpp);
    memcpy(c10, p + Bpp,             Bpp);
    memcpy(c01, p       + in->pitch, Bpp);
    memcpy(c11, p + Bpp + in->pitch, Bpp);

    // Perform bilinear interpolation
    for(i = 0; i < Bpp; i++)
    {
        fx0 = c00[i] + dx * (c10[i] - c00[i]);
        fx1 = c01[i] + dx * (c11[i] - c01[i]);
        col[i] = (Uint8)(fx0 + dy * (fx1 - fx0));
    }
}

// Perform some transition between the two images.
// Both are the same dimensions (padded with black to the screen size).
// Transitions should last about half a second.
void slideshow::do_transition(SDL_Surface *from, SDL_Surface *to)
{
	transition_t tran = m_options->transition;

	if ( tran == tran_random )
	{
		int range = (int)tran_END - (int)tran_BEGIN - 1;
		tran = (transition_t) (( rand() % range ) + tran_BEGIN + 1);
	}

	switch(tran)
	{
	case tran_fade:
		{
			// base alpha increment on pixel count, so that we get
			// roughly consistent fading speed in different resolutions
			int pixcount = m_options->width * m_options->height;
			int da = pixcount / 30000;
			if ( da < 2 ) da = 2;
			
			for(int i = 2; i < 0x100; i += da)
			{
				SDL_BlitSurface(from, NULL, m_sdl, NULL);
			
				SDL_SetAlpha(to, SDL_SRCALPHA, i);
				SDL_BlitSurface(to, NULL, m_sdl, NULL);

				SDL_Flip(m_sdl);
			}

			SDL_SetAlpha(to, 0, SDL_ALPHA_OPAQUE);
		}
		break;
	
	case tran_blinds:
		{
			SDL_BlitSurface(from, NULL, m_sdl, NULL);
			SDL_Flip(m_sdl);

			SDL_BlitSurface(to, NULL, m_sdl, NULL);

			int bh = m_options->height / 12;
			int bs = bh / 24;
			if ( bs < 1 ) bs = 1;

			for(int i = 1; i < bh; i += bs)
			{
				for(int y = 0; y < m_options->height; y += bh)
				{
					SDL_UpdateRect(m_sdl, 0, y, m_options->width, i);
				}
				SDL_Delay(10);
			}
		}
		break;

	case tran_checkers:
		{
			SDL_BlitSurface(from, NULL, m_sdl, NULL);
			SDL_Flip(m_sdl);

			SDL_BlitSurface(to, NULL, m_sdl, NULL);

			double cw = (double)m_options->width / 8;
			double ch = (double)m_options->height / 6;

			std::vector<SDL_Rect> rects;
			for(int w = 0; w < 8; w++)
			{
				for(int h = 0; h < 6; h++)
				{
					SDL_Rect rc;
					rc.x = (int)(cw * w);
					rc.y = (int)(ch * h);
					rc.w = (int)(cw * (w + 1)) - rc.x;
					rc.h = (int)(ch * (h + 1)) - rc.y;
					rects.push_back(rc);
				}
			}
			std::random_shuffle(rects.begin(), rects.end());

			for(int i = 0; i < rects.size(); i++)
			{
				SDL_UpdateRects(m_sdl, 1, &rects[i]);
				SDL_Delay(10);
			}
		}
		break;

	case tran_uncover:
		{
			SDL_Rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.w = m_options->width;
			rect.h = m_options->height;
			
			int inc = (m_options->width * m_options->height) / 25000;
			if ( inc < 1 ) inc = 1;
			
			while(rect.x < m_options->width && rect.y < m_options->height)
			{
				SDL_BlitSurface(to, NULL, m_sdl, NULL);
				SDL_BlitSurface(from, NULL, m_sdl, &rect);	// I'd slide up, but if rect.y < 0, this will set it == 0
				
				rect.x += inc;
				rect.y += inc;

				SDL_Flip(m_sdl);
			}
		}
		break;	
	}
}
