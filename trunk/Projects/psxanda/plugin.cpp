#include <stdio.h>
#include <stdlib.h>

#include "plugins.h"
#include "System.h"
#include "Spu.h"

unsigned long gpuDisp;

int LoadPlugins() 
{
	int ret;

	ret = CDR_init();
	if (ret < 0) { SysMessage ( "CDRinit error : %d", ret); return -1; }
	ret = GPU_init();
	if (ret < 0) { SysMessage ( "GPUinit error: %d", ret); return -1; }
	ret = SPU_init();
	if (ret < 0) { SysMessage ( "SPUinit error: %d", ret); return -1; }
	ret = PAD1_init(1);
	if (ret < 0) { SysMessage ( "PAD1init error: %d", ret); return -1; }
	ret = PAD2_init(2);
	if (ret < 0) { SysMessage ( "PAD2init error: %d", ret); return -1; }

	return 0;
}

void ReleasePlugins() 
{
	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();
}

void clearDynarec(void) 
{
	printf("psxCpu->Reset();\n");
//	psxCpu->Reset();
}

int OpenPlugins() 
{
	int ret;
	
	GPU_clearDynarec(clearDynarec);

	ret = CDR_open();
	if (ret < 0) { SysMessage( "Error Opening CDR Plugin" ); return -1; }
	ret = SPU_open();
	if (ret < 0) { SysMessage( "Error Opening SPU Plugin" ); return -1; }
	SPU_registerCallback(SPUirq);
	ret = GPU_open(&gpuDisp, "PCSX", 0);
	if (ret < 0) { SysMessage( "Error Opening GPU Plugin" ); return -1; }
	ret = PAD1_open();
	if (ret < 0) { SysMessage( "Error Opening PAD1 Plugin" ); return -1; }
	ret = PAD2_open();
	if (ret < 0) { SysMessage( "Error Opening PAD2 Plugin" ); return -1; }

	return 0;
}

void ClosePlugins() 
{
	int ret;

//	signal(SIGINT, SIG_DFL);
//	signal(SIGPIPE, SIG_DFL);

	ret = CDR_close();
	if (ret < 0) { SysMessage( "Error Closing CDR Plugin" ); return; }
	ret = SPU_close();
	if (ret < 0) { SysMessage( "Error Closing SPU Plugin" ); return; }
	ret = PAD1_close();
	if (ret < 0) { SysMessage( "Error Closing PAD1 Plugin" ); return; }
	ret = PAD2_close();
	if (ret < 0) { SysMessage( "Error Closing PAD2 Plugin" ); return; }
	ret = GPU_close();
	if (ret < 0) { SysMessage( "Error Closing GPU Plugin" ); return; }
	
}

