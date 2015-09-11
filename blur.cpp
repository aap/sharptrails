#include "sharptrails.h"
#include "d3d8.h"
#include "d3d8types.h"

HMODULE dllModule;
int gtaversion = -1;

static uint32_t DefinedState_A = AddressByVersion<uint32_t>(0x526330, 0x526570, 0x57F9C0);
WRAPPER void DefinedState(void) { VARJMP(DefinedState_A); }

void *overridePS = NULL;
void *blurps = NULL;

class CMBlur {
public:
	static bool &ms_bJustInitialised;
	static bool &BlurOn;
	static float &Drunkness;
	static RwRaster *&pFrontBuffer;
	static void MotionBlurRender(RwCamera *cam, RwUInt8 red, RwUInt8 green, RwUInt8 blue, RwUInt8 alpha, int type);
	static void OverlayRenderVC_noblur(RwCamera *cam, RwRaster *raster, RwRGBA color, int type);
	static void OverlayRenderIII_noblur(RwCamera *cam, RwRaster *raster, RwRGBA color, int type, int alpha);
	static void OverlayRenderVC(RwCamera *cam, RwRaster *raster, RwRGBA color, int type);
	static void OverlayRenderIII(RwCamera *cam, RwRaster *raster, RwRGBA color, int type, int alpha);
	static void OverlayRenderFx(RwCamera *cam, RwRaster *raster);
};

//bool &CMBlur::ms_bJustInitialised = *(bool*)0xA10B71;
bool &CMBlur::BlurOn = *AddressByVersion<bool*>(0x95CDAD, 0x5FDB84, 0x697D54);
//float &CMBlur::Drunkness = *(float*)0x983B38;
RwRaster *&CMBlur::pFrontBuffer = *AddressByVersion<RwRaster**>(0x8E2C48, 0x8E2CFC, 0x9753A4);

WRAPPER void CMBlur::OverlayRenderFx(RwCamera*, RwRaster*) { EAXJMP(0x55D870); }
WRAPPER void CMBlur::OverlayRenderVC(RwCamera*, RwRaster*, RwRGBA, int) { EAXJMP(0x55E750); }
static uint32_t OverlayRenderIII_A = AddressByVersion<uint32_t>(0x50A9C0, 0x50AAA0, 0);
WRAPPER void CMBlur::OverlayRenderIII(RwCamera*, RwRaster*, RwRGBA, int, int) { VARJMP(OverlayRenderIII_A); }


/*
void
CMBlur::MotionBlurRender(RwCamera *cam, RwUInt8 red, RwUInt8 green, RwUInt8 blue, RwUInt8 alpha, int type)
{
	RwRGBA color = { red, green, blue, alpha };
	if(CMBlur::ms_bJustInitialised)
		CMBlur::ms_bJustInitialised = 0;
	else
		CMBlur::OverlayRender_noblur(cam, CMBlur::pFrontBuffer, color, type);
	if(CMBlur::BlurOn){
		RwRasterPushContext(CMBlur::pFrontBuffer);
		RwRasterRenderFast(cam->frameBuffer, 0, 0);
		RwRasterPopContext();
	}
}
*/

RwD3D8Vertex *blurVertices = AddressByVersion<RwD3D8Vertex*>(0x62F780, 0x62F780, 0x7097A8);
//RwD3D8Vertex *blurVertices2 = (RwD3D8Vertex*)0x709818;
RwImVertexIndex *blurIndices = AddressByVersion<RwImVertexIndex*>(0x5FDD90, 0x5FDB78, 0x697D48);

void
setps(void)
{
	if(blurps == NULL){
		HRSRC resource = FindResource(dllModule, MAKEINTRESOURCE(isVC() ? IDR_VCBLURPS : IDR_IIIBLURPS), RT_RCDATA);
		RwUInt32 *shader = (RwUInt32*)LoadResource(dllModule, resource);
		RwD3D9CreatePixelShader(shader, &blurps);
		assert(blurps);
		FreeResource(shader);
	}
	overridePS = blurps;
}

int dontblur = 1;

void
CMBlur::OverlayRenderVC_noblur(RwCamera *cam, RwRaster *raster, RwRGBA color, int type)
{
	if(type != 2 || !CMBlur::BlurOn || !dontblur || !RwD3D9Supported()){
		CMBlur::OverlayRenderVC(cam, raster, color, type);
		return;
	}

	RwRasterPushContext(CMBlur::pFrontBuffer);
	RwRasterRenderFast(cam->frameBuffer, 0, 0);
	RwRasterPopContext();

	DefinedState();
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)1);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 0);
	RwUInt32 emissiveColor = D3DCOLOR_ARGB(color.alpha, color.red, color.green, color.blue);
	for(int i = 0; i < 4; i++)
		blurVertices[i].emissiveColor = emissiveColor;
	setps();
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blurVertices, 4, blurIndices, 6);
	overridePS = NULL;
}

/*
void
CMBlur::OverlayRender(RwCamera *cam, RwRaster *raster, RwRGBA color, int type)
{
	RwUInt8 red, green, blue, alpha;
	red = color.red;
	green = color.green;
	blue = color.blue;
	alpha = color.alpha;
	DefinedState();
	switch(type){
	case 3:
		red = 0;
		green = 0xFF;
		blue = 0;
		alpha = 0x80;
		break;
	case 5:
		red = 0x64;
		green = 0xDC;
		blue = 0xE6;
		alpha = 0x9E;
		break;
	case 6:
		red = 0x50;
		green = 0xFF;
		blue = 0xE6;
		alpha = 0x8A;
		break;
	case 8:
		red = 0xFF;
		green = 0x3C;
		blue = 0x3C;
		alpha = 0xC8;
		break;
	case 9:
		red = 0xFF;
		green = 0xB4;
		blue = 0xB4;
		alpha = 0x80;
		break;
	}
	printf("blurtype: %d\n", type);
	if(!CMBlur::BlurOn){
		assert(0 && "no blur");
	}
	RwUInt32 emissiveColor = D3DCOLOR_ARGB(alpha, red, green, blue);
	for(int i = 0; i < 4; i++)
		blurVertices[i].emissiveColor = blurVertices2[i].emissiveColor = emissiveColor;
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)1);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	if(CMBlur::BlurOn){
		if(type == 1){
			assert(0 && "type 1");
			emissiveColor = D3DCOLOR_ARGB(0x50, red, green, blue);
			for(int i = 0; i < 4; i++)
				blurVertices2[i].emissiveColor = emissiveColor;
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
			*(int*)0x97F888 = 0;	// what's that?
		}else{
			emissiveColor = D3DCOLOR_ARGB(0x1E, red*2, green*2, blue*2);
			for(int i = 0; i < 4; i++)
				blurVertices[i].emissiveColor = blurVertices2[i].emissiveColor = emissiveColor;
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blurVertices, 4, blurIndices, 6);
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
			emissiveColor = D3DCOLOR_ARGB(alpha, red, green, blue);
			for(int i = 0; i < 4; i++)
				blurVertices[i].emissiveColor = blurVertices2[i].emissiveColor = emissiveColor;
			setps();
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blurVertices, 4, blurIndices, 6);	//
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blurVertices, 4, blurIndices, 6);
			overridePS = NULL;
		}
	}
	if(CMBlur::Drunkness * 175.0f){
		assert(0 && "drunk");
	}
	if((GetAsyncKeyState(VK_F4) & 0x8000) == 0 && type != 1)
		CMBlur::OverlayRenderFx(cam, raster);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 0);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
}
*/

RwBool
RwD3D8SetPixelShader_hook(RwUInt32 handle)
{
	if(overridePS){
		RwD3D9SetPixelShader(overridePS);
		return 1;
	}
	return RwD3D8SetPixelShader(handle);
}

void
CMBlur::OverlayRenderIII_noblur(RwCamera *cam, RwRaster *raster, RwRGBA color, int type, int alpha)
{
	if(!CMBlur::BlurOn || !dontblur || !RwD3D9Supported()){
		CMBlur::OverlayRenderIII(cam, raster, color, type, alpha);
		return;
	}

	RwRasterPushContext(CMBlur::pFrontBuffer);
	RwRasterRenderFast(cam->frameBuffer, 0, 0);
	RwRasterPopContext();

	DefinedState();
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)1);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 0);
	RwUInt32 emissiveColor = D3DCOLOR_ARGB(color.alpha, color.red, color.green, color.blue);
	for(int i = 0; i < 4; i++)
		blurVertices[i].emissiveColor = emissiveColor;
	setps();
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blurVertices, 4, blurIndices, 6);
	overridePS = NULL;
}

void
patch10(void)
{
	if(isVC()){
		MemoryVP::InjectHook(0x55D838, CMBlur::OverlayRenderVC_noblur);
		MemoryVP::InjectHook(0x6666B8, RwD3D8SetPixelShader_hook);
		MemoryVP::InjectHook(0x666928, RwD3D8SetPixelShader_hook);
	}else{
		MemoryVP::InjectHook(AddressByVersion<uint32_t>(0x50ADDD, 0x50AEBD, 0), CMBlur::OverlayRenderIII_noblur);
		MemoryVP::InjectHook(AddressByVersion<uint32_t>(0x5BF9D6, 0x5BFC96, 0), RwD3D8SetPixelShader_hook);
		MemoryVP::InjectHook(AddressByVersion<uint32_t>(0x5BFBD3, 0x5BFE93, 0), RwD3D8SetPixelShader_hook);
		MemoryVP::Nop(AddressByVersion<uint32_t>(0x50AE2F, 0x50AF0F, 0), 5);
	}
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH){
		dllModule = hInst;

/*		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);*/

		AddressByVersion<uint32_t>(0, 0, 0);
		if(gtaversion == III_10 || gtaversion == III_11 || gtaversion == VC_10)
			patch10();
	}

	return TRUE;
}
