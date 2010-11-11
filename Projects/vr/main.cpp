#include <qcopchannel_qws.h>
#include <ZApplication.h>
#include "vr.h"

BotonVR *VR;

int main ( int argc, char **argv )
{
	VR = new BotonVR ( argc, argv );
	int ret = VR->exec();
	return ret;
}
