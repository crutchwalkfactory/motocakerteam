#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugins.h"
#include "SPU_register.h"

/////////////////////////////////////////////////////////

//#define PSE_LT_SPU                  4
//#define PSE_SPU_ERR_SUCCESS         0
//#define PSE_SPU_ERR                 -60
//#define PSE_SPU_ERR_NOTCONFIGURED   PSE_SPU_ERR -1
//#define PSE_SPU_ERR_INIT            PSE_SPU_ERR -2

/////////////////////////////////////////////////////////
// main emu calls:
// 0. Get type/name/version
// 1. Init
// 2. SetCallbacks
// 3. SetConfigFile
// 4. Open
// 5. Dma/register/xa calls...
// 6. Close
// 7. Shutdown
/////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

unsigned short  regArea[10000];                        // psx buffer
unsigned short  spuMem[256*1024];
unsigned char * spuMemC;
unsigned char * pSpuIrq=0;

unsigned short spuCtrl, spuStat, spuIrq=0;             // some vars to store psx reg infos
unsigned long  spuAddr=0xffffffff;                     // address into spu mem
char *         pConfigFile=0;

////////////////////////////////////////////////////////////////////////
void (*irqCallback)(void) = 0;                // func of main emu, called on spu irq
void (*cddavCallback)(unsigned short,unsigned short) = 0;

int bSPUIsOpen = 0;

long SPU_open(void)
{
	if(bSPUIsOpen) return 0;
	bSPUIsOpen = 1;
	//if(pConfigFile) ReadConfigFile(pConfigFile);
	return PSE_SPU_ERR_SUCCESS;   
}

long SPU_init(void)
{
	spuMemC=(unsigned char *)spuMem;                      // just small setup
	return 0;
}

long SPU_shutdown(void)
{
	return 0;
}

long SPU_close(void)
{
	if(!bSPUIsOpen) return 0;
	bSPUIsOpen=0;
	return 0;
}

void SPU_playSample(unsigned char)
{
//	printf("SPU_playSample\n");
}

void SPU_startChannels1(unsigned short)
{
//	printf("SPU_startChannels1\n");
}

void SPU_startChannels2(unsigned short)
{
//	printf("SPU_startChannels2\n");
}

void SPU_stopChannels1(unsigned short)
{
//	printf("SPU_stopChannels1\n");
}

void SPU_stopChannels2(unsigned short)
{
//	printf("SPU_stopChannels2\n");
}

void SPU_putOne(unsigned long val,unsigned short data)
{
//	printf("SPU_putOne\n");
	if(spuAddr!=0xffffffff) { 
		SPU_writeDMA(data); 
		return; 
	} 
	if(val>=512*1024) val=512*1024-1; 
	spuMem[val>>1] = data;
}

unsigned short SPU_getOne(unsigned long val)
{
//	printf("SPU_getOne\n");
	if(spuAddr!=0xffffffff) 
		return SPU_readDMA(); 
	if(val>=512*1024) 
		val=512*1024-1; 
	return spuMem[val>>1]; 
}

void SPU_setAddr(unsigned char, unsigned short)
{
//	printf("SPU_setAddr\n");
}

void SPU_setPitch(unsigned char, unsigned short)
{
//	printf("SPU_setPitch\n");
}

void SPU_setVolumeL(unsigned char, short )
{
//	printf("SPU_setVolumeL\n");
}

void SPU_setVolumeR(unsigned char, short )
{
//	printf("SPU_setVolumeR\n");
}

////////////////////////////////////////////////////////////////////////
// CODE AREA
////////////////////////////////////////////////////////////////////////
//psemu pro 2 functions from now..
void SPU_writeRegister(unsigned long reg, unsigned short val)
{
//	printf("SPU_writeRegister\n");
	
	unsigned long r=reg&0xfff;
	regArea[(r-0xc00)>>1] = val;

	if(r>=0x0c00 && r<0x0d80){
		//int ch=(r>>4)-0xc0;
		switch(r&0x0f) {
		//------------------------------------------------// l volume
		case 0:                    
			//SetVolumeL(ch,val);
			return;
		//------------------------------------------------// r volume
		case 2:                                           
			//SetVolumeR(ch,val);
			return;
		//------------------------------------------------// pitch
		case 4:                                           
			//SetPitch(ch,val);
			return;
		//------------------------------------------------// start
		case 6:
			//s_chan[ch].pStart=spuMemC+((unsigned long) val<<3);
			return;
		//------------------------------------------------// adsr level 
		case 8:
			return;
		//------------------------------------------------// adsr rate 
		case 10:
			return;
		//------------------------------------------------// adsr volume
		case 12:
			return;
		//------------------------------------------------// loop adr
		case 14:                                          
			return;
		//------------------------------------------------//
		}
		return;
	}

	switch(r) {
	//-------------------------------------------------//
	case H_SPUaddr:
		spuAddr = (unsigned long) val<<3;
		return;
	//-------------------------------------------------//
	case H_SPUdata:
		spuMem[spuAddr>>1] = val;
		spuAddr+=2;
		if(spuAddr>0x7ffff) spuAddr=0;
		return;
	//-------------------------------------------------//
	case H_SPUctrl:
		spuCtrl=val;
		return;
	//-------------------------------------------------//
	case H_SPUstat:
		spuStat=val & 0xf800;
		return;
	//-------------------------------------------------//
	case H_SPUirqAddr:
		spuIrq = val;
		pSpuIrq=spuMemC+((unsigned long) val<<3);
		return;
	//-------------------------------------------------//
	case H_SPUon1:
		//SoundOn(0,16,val);
		return;
	//-------------------------------------------------//
	case H_SPUon2:
		//SoundOn(16,24,val);
		return;
	//-------------------------------------------------//
	case H_SPUoff1:
		//SoundOff(0,16,val);
		return;
	//-------------------------------------------------//
	case H_SPUoff2:
		//SoundOff(16,24,val);
		return;
	//-------------------------------------------------//
	case H_CDLeft:
		if(cddavCallback) cddavCallback(0,val);
		return;
	case H_CDRight:
		if(cddavCallback) cddavCallback(1,val);
		return;
	//-------------------------------------------------//
	case H_FMod1:
		//FModOn(0,16,val);
		return;
	//-------------------------------------------------//
	case H_FMod2:
		//FModOn(16,24,val);
		return;
	//-------------------------------------------------//
	case H_Noise1:
		//NoiseOn(0,16,val);
		return;
	//-------------------------------------------------//
	case H_Noise2:
		//NoiseOn(16,24,val);
		return;
	//-------------------------------------------------//
	case H_RVBon1:
		//ReverbOn(0,16,val);
		return;
	//-------------------------------------------------//
	case H_RVBon2:
		//ReverbOn(16,24,val);
		return;
	//-------------------------------------------------//
	case H_Reverb:
		return;
	}
}

unsigned short SPU_readRegister(unsigned long reg)
{
//	printf("SPU_readRegister\n");
	
	unsigned long r=reg&0xfff;

	if(r>=0x0c00 && r<0x0d80) {
		switch(r&0x0f) {
		case 12:                                          // adsr vol
			{
				//int ch=(r>>4)-0xc0;
				static unsigned short adsr_dummy_vol=0;
				adsr_dummy_vol=!adsr_dummy_vol;
				return adsr_dummy_vol;
			}
		case 14:                                          // return curr loop adr
			//int ch=(r>>4)-0xc0;        
			return 0;
		}
	}

	switch(r){ 
	case H_SPUctrl:
		return spuCtrl;

	case H_SPUstat:
		return spuStat;
        
	case H_SPUaddr:
		return (unsigned short)(spuAddr>>3);

	case H_SPUdata:
		{
			unsigned short s=spuMem[spuAddr>>1];
			spuAddr+=2;
			if(spuAddr>0x7ffff) spuAddr=0;
			return s;
		}
	case H_SPUirqAddr:
		return spuIrq;
	}
	return regArea[(r-0xc00)>>1];
}

void SPU_writeDMA(unsigned short val)
{
//	printf("SPU_writeDMA\n");
	spuMem[spuAddr>>1] = val;                             // spu addr got by writeregister
	spuAddr+=2;                                           // inc spu addr
	if(spuAddr>0x7ffff) spuAddr=0;                        // wrap
}

unsigned short SPU_readDMA(void)
{
//	printf("SPU_readDMA\n");
	unsigned short s=spuMem[spuAddr>>1];
	spuAddr+=2;
	if(spuAddr>0x7ffff) spuAddr=0;
	return s;
}

void SPU_writeDMAMem(unsigned short * pusPSXMem, int iSize)
{
	int i;
	for(i=0;i<iSize;i++) {
		spuMem[spuAddr>>1] = *pusPSXMem++;                  // spu addr got by writeregister
		spuAddr+=2;                                         // inc spu addr
		if(spuAddr>0x7ffff) spuAddr=0;                      // wrap
	}
}

void SPU_readDMAMem(unsigned short * pusPSXMem, int iSize)
{
	int i;
	for(i=0;i<iSize;i++) {
		*pusPSXMem++=spuMem[spuAddr>>1];                    // spu addr got by writeregister
		spuAddr+=2;                                         // inc spu addr
		if(spuAddr>0x7ffff) spuAddr=0;                      // wrap
	}
}

void SPU_playADPCMchannel(xa_decode_t *)
{
//	printf("SPU_playADPCMchannel\n");
}

void SPU_registerCallback(void (*callback)(void))
{
//	printf("SPU_registerCallback\n");
	irqCallback = callback;
}

void SPU_registerCDDAVolume(void (*CDDAVcallback)(unsigned short,unsigned short))
{
	cddavCallback = CDDAVcallback;
}

long SPU_configure(void)
{
	return 0;
}

long SPU_test(void)
{
	return 0;
}

void SPU_about(void)
{
}

////////////////////////////////////////////////////////////////////////

typedef struct
{
	unsigned long Future[256];

} SPUNULLFreeze_t;

long SPU_freeze(unsigned long ulFreezeMode, SPUFreeze_t *pF)
{
	int i;

	if(!pF) return 0;

	if(ulFreezeMode) {
		if(ulFreezeMode==1)
			memset(pF,0,sizeof(SPUFreeze_t)+sizeof(SPUNULLFreeze_t));

		strcpy((char *)pF->PluginName,"PBNUL");
		pF->PluginVersion = 1;
		pF->Size = sizeof(SPUFreeze_t) + sizeof(SPUNULLFreeze_t);

		if(ulFreezeMode==2) return 1;

		memcpy(pF->SPURam,spuMem,0x80000);
		memcpy(pF->SPUPorts,regArea,0x200);
		// dummy:
		memset(&pF->xa, 0, sizeof(xa_decode_t));
		return 1;
	}

	if(ulFreezeMode!=0) return 0;

	memcpy(spuMem, pF->SPURam, 0x80000);
	memcpy(regArea, pF->SPUPorts, 0x200);

	for(i=0;i<0x100;i++){
		if(i!=H_SPUon1-0xc00 && i!=H_SPUon2-0xc00)
			SPU_writeRegister(0x1f801c00+i*2,regArea[i]);
	}
	SPU_writeRegister(H_SPUon1,regArea[(H_SPUon1-0xc00)/2]);
	SPU_writeRegister(H_SPUon2,regArea[(H_SPUon2-0xc00)/2]);

	return 1;
}

void SPU_async(unsigned long)
{
}


