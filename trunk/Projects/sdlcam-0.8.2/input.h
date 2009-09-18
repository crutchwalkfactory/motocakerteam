#ifndef INPUT_H
#define INPUT_H
#include "SDL_v4l_image.h"

class cInput {
	public:
virtual const char 	*Name		(void				) = 0;
virtual bool		 isOK		(void				) = 0;
virtual	int 		 Resolution	(int  w, int  h			) = 0;
virtual	int 		 Resolution	(int *w, int *h			) = 0;
virtual	SDL_v4l_image	*Fetch		(void				) = 0;
virtual	int	 	 Parameter  	(const char *name, int val	) = 0;	// Set an free parameter
virtual int	 	 Parameter  	(const char *name		) = 0;	// Get an free parameter
	};

#endif
