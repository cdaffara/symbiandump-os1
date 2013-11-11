// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __TLLD_H_
#define __TLLD_H_

#include <gdi.h>
#include <bitdraw.h>
#include "TGraphicsHarness.h"

#define KRgbFadedBlack		TRgb(0x808080)
const TInt KNumDispModes = 13;
const TInt KNumDrawModes = 6;
const TInt KNumTestRects = 10;
const TInt KNumTestColors = 12;
const TInt KNumTestBackgrounds = 6;
const TInt KNumShadowModes = 4;
const TInt KCheckBinaryPixelBufferSize = 64;
const TInt KNumBlendingColors = 3;
const TInt KNumColorsToBlend = 4;

//this is to allow EColor16MAP to be tested with extra colours
const TUint32 KMaxNon16Colours = 9;

//this is to allow EColor16MAP to be tested with extra background colours
const TUint32 KMaxNon16BackColours = 3;

const TDisplayMode TestDisplayMode[KNumDispModes] =
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor4K,
	EColor64K,
	EColor16M,
	ERgb,
	EColor16MU,
	EColor16MAP,
	EColor16MA,
	};

const TPtrC DisplayModeNames[KNumDispModes] =
	{
	_L("EGray2"),
	_L("EGray4"),
	_L("EGray16"),
	_L("EGray256"),
	_L("EColor16"),
	_L("EColor256"),
	_L("EColor4K"),
	_L("EColor64K"),
	_L("EColor16M"),
	_L("ERgb"),
	_L("EColor16MU"),
	_L("EColor16MAP"),
	_L("EColor16MA"),
	};

const TDisplayMode TestDisplayMode1[] =
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor4K,
	EColor64K,
	EColor16M,
	EColor16MU,
	EColor16MAP,
	EColor16MA,
	};

const TPtrC DisplayModeNames1[] =
	{
	_L("EGray2"),
	_L("EGray4"),
	_L("EGray16"),
	_L("EGray256"),
	_L("EColor16"),
	_L("EColor256"),
	_L("EColor4K"),
	_L("EColor64K"),
	_L("EColor16M"),
	_L("EColor16MU"),
	_L("EColor16MAP"),
	_L("EColor16MA"),
	};

const CGraphicsContext::TDrawMode TestDrawMode[KNumDrawModes] =
	{
	CGraphicsContext::EDrawModePEN,
	CGraphicsContext::EDrawModeAND,
	CGraphicsContext::EDrawModeXOR,
	CGraphicsContext::EDrawModeOR,
	CGraphicsContext::EDrawModeNOTSCREEN,
	CGraphicsContext::EDrawModeNOTPEN
	};

const TRect TestRect[KNumTestRects] =
	{
	TRect(0,0,31,31),
	TRect(1,1,2,2),
	TRect(1,1,2,31),
	TRect(2,1,31,2),
	TRect(1,1,2,31),
	TRect(1,1,30,31),
	TRect(1,1,33,33),
	TRect(31,0,65,1),
	TRect(0,0,30,34), //needs a height >32
	TRect(1,1,7,7) // for DEF123772
	};

const TRgb TestColor[KNumTestColors] =
	{
	KRgbBlack,
	KRgbDarkGray,
	KRgbGray,
	KRgbWhite,
	KRgbRed,
	KRgbCyan,
	TRgb(230,180,70),
	TRgb(150,240,30),
	TRgb(20,250,160),
	TRgb((TUint32)0xff2665A3),
	TRgb((TUint32)0x002766A4),
	TRgb((TUint32)0x57339A4E),
	};

const TRgb TestBackground[KNumTestBackgrounds] =
	{
	KRgbWhite,
	KRgbBlack,
	TRgb(20,250,160),
	TRgb(0xff27D99f),
	TRgb(0x00E73121),
	TRgb(0x771FE3B7),
	};

const TUint8 ditherlutab[16][4]=
	{{	0x0,	0x0,	0x0,	0x0,	},
	{	0x5,	0x0,	0x0,	0x0,	},
	{	0x5,	0x0,	0x0,	0x5,	},
	{	0x5,	0x0,	0x0,	0x5,	},
	{	0x5,	0x0,	0x5,	0x5,	},
	{	0x5,	0x5,	0x5,	0x5,	},
	{	0xa,	0x5,	0x5,	0x5,	},
	{	0xa,	0x5,	0x5,	0xa,	},
	{	0xa,	0x5,	0x5,	0xa,	},
	{	0xa,	0x5,	0xa,	0xa,	},
	{	0xa,	0xa,	0xa,	0xa,	},
	{	0xf,	0xa,	0xa,	0xa,	},
	{	0xf,	0xa,	0xa,	0xf,	},
	{	0xf,	0xa,	0xa,	0xf,	},
	{	0xf,	0xa,	0xf,	0xf,	},
	{	0xf,	0xf,	0xf,	0xf		}};

/**
This array has three colours, It would be used to blend the four colours using WriteRgbOutlineAndShadow ().
Four colours which needs blending are
O - Outline colour, first entry of this table
S - Shadow colour, second entry in this table
F - Foreground colour i.e. third entry and,
B - Background colur, It will be set inside the test function.
*/
const TRgb TestOSFBColorsForBlending[KNumBlendingColors][KNumColorsToBlend] =
	{
	{ KRgbBlack, TRgb(220,150,164,138), KRgbMagenta, KRgbWhite},
	{ KRgbDarkMagenta, KRgbBlue, KRgbDarkYellow, TRgb(20,250,160, 128)},
	{ TRgb(230,180,70,10), KRgbGreen, TRgb(20,250,160,168), KRgbBlack}
	};

/**
This lookup table is used for outline and shadow fonts. Four entries of this table represents
% of Outline, Shadow, Fill and Background color to be used to get the final color to be
displayed on screen.

While doing the blending of four colors it would use the following formula,

blendedXXXColor = (XXXOutlinePenColor * FourColorBlendLookup[index][KOutlineColorIndex] +
			   		XXXShadowColor * FourColorBlendLookup[index][KShadowColorIndex] +
			  		XXXFillColor * FourColorBlendLookup[index][KFillColorIndex] +
			  		backgroundColor.XXX() * FourColorBlendLookup[index][KBackgroundColorIndex]) >> 8;
where XXX can be Red, Green or Blue
and index is the 8-bit value which will be provided by the rasterizer to decide which
entry of lookup table should be used. Right shift by eight will divide the calculated value in % with
256 to get the actual colour.

This table is provided by Monotype and MUST NOT be changed.
*/
const TInt FourColorBlendLookup[256][4] = {
	{ 0, 0, 0, 255 },
	{ 0, 0, 17, 238 },
	{ 0, 0, 34, 221 },
	{ 0, 0, 51, 204 },
	{ 0, 0, 68, 187 },
	{ 0, 0, 85, 170 },
	{ 0, 0, 102, 153 },
	{ 0, 0, 119, 136 },
	{ 0, 0, 136, 119 },
	{ 0, 0, 153, 102 },
	{ 0, 0, 170, 85 },
	{ 0, 0, 187, 68 },
	{ 0, 0, 204, 51 },
	{ 0, 0, 221, 34 },
	{ 0, 0, 238, 17 },
	{ 0, 0, 255, 0 },
	{ 0, 17, 0, 238 },
	{ 0, 34, 0, 221 },
	{ 0, 51, 0, 204 },
	{ 0, 68, 0, 187 },
	{ 0, 85, 0, 170 },
	{ 0, 102, 0, 153 },
	{ 0, 119, 0, 136 },
	{ 0, 136, 0, 119 },
	{ 0, 153, 0, 102 },
	{ 0, 170, 0, 85 },
	{ 0, 187, 0, 68 },
	{ 0, 204, 0, 51 },
	{ 0, 221, 0, 34 },
	{ 0, 238, 0, 17 },
	{ 0, 255, 0, 0 },
	{ 17, 0, 0, 238 },
	{ 34, 0, 0, 221 },
	{ 51, 0, 0, 204 },
	{ 68, 0, 0, 187 },
	{ 85, 0, 0, 170 },
	{ 102, 0, 0, 153 },
	{ 119, 0, 0, 136 },
	{ 136, 0, 0, 119 },
	{ 153, 0, 0, 102 },
	{ 170, 0, 0, 85 },
	{ 187, 0, 0, 68 },
	{ 204, 0, 0, 51 },
	{ 221, 0, 0, 34 },
	{ 238, 0, 0, 17 },
	{ 255, 0, 0, 0 },
	{ 0, 17, 238, 0 },
	{ 0, 34, 221, 0 },
	{ 0, 51, 204, 0 },
	{ 0, 68, 187, 0 },
	{ 0, 85, 170, 0 },
	{ 0, 102, 153, 0 },
	{ 0, 119, 136, 0 },
	{ 0, 136, 119, 0 },
	{ 0, 153, 102, 0 },
	{ 0, 170, 85 ,0 },
	{ 0, 187, 68 ,0 },
	{ 0, 204, 51, 0 },
	{ 0, 221, 34, 0 },
	{ 0, 238, 17, 0 },
	{ 238, 17, 0, 0 },
	{ 221, 34, 0, 0 },
	{ 204, 51, 0, 0 },
	{ 187, 68, 0, 0 },
	{ 170, 85, 0, 0 },
	{ 153, 102, 0, 0 },
	{ 136, 119, 0, 0 },
	{ 119, 136, 0, 0 },
	{ 102, 153, 0, 0 },
	{ 85, 170, 0, 0 },
	{ 68, 187, 0, 0 },
	{ 51, 204, 0, 0 },
	{ 34, 221, 0, 0 },
	{ 17, 238, 0, 0 },
	{ 238, 0, 17, 0 },
	{ 221, 0, 34, 0 },
	{ 204, 0, 51, 0 },
	{ 187, 0, 68, 0 },
	{ 170, 0, 85, 0 },
	{ 153, 0, 102, 0 },
	{ 136, 0, 119, 0 },
	{ 119, 0, 136, 0 },
	{ 102, 0, 153, 0 },
	{ 85, 0, 170, 0 } ,
	{ 68, 0, 187, 0 },
	{ 51, 0, 204, 0 },
	{ 34, 0, 221, 0 },
	{ 17, 0, 238, 0 },
	{ 210, 23, 0, 22 },
	{ 179, 60, 0, 16 },
	{ 145, 94, 0, 16 },
	{ 119, 119, 0, 17 },
	{ 94, 145, 0, 16 },
	{ 60, 179, 0, 16 },
	{ 23, 210, 0, 22 },
	{ 176, 40, 0, 39 },
	{ 142, 74, 0, 39 },
	{ 113, 96, 0, 46 },
	{ 91, 125, 0, 39 },
	{ 62, 147, 0, 46 },
	{ 40, 176, 0, 39 },
	{ 179, 17, 0, 59 },
	{ 147, 45, 0, 63 },
	{ 17, 179, 0, 59 },
	{ 145, 17, 0, 93 },
	{ 125, 40, 0, 90 },
	{ 108, 74, 0, 73 },
	{ 74, 108, 0, 73 },
	{ 85, 85, 0, 85 },
	{ 40, 142, 0, 73 },
	{ 119, 17, 0, 119 },
	{ 96, 45, 0, 114 },
	{ 74, 74, 0, 107 },
	{ 45, 113, 0, 97 },
	{ 17, 145, 0, 93 },
	{ 94, 17, 0, 144 },
	{ 74, 40, 0, 141 },
	{ 45, 62, 0, 148 },
	{ 40, 91, 0, 124 },
	{ 17, 119, 0, 119 },
	{ 17, 94, 0, 144 },
	{ 60, 17, 0, 178 },
	{ 40, 40, 0, 175 },
	{ 17, 60, 0, 178 },
	{ 23, 23, 0, 209 },
	{ 0, 210, 23, 22 },
	{ 0, 179, 60, 16 },
	{ 0, 145, 94, 16 },
	{ 0, 119, 119, 17 },
	{ 0, 94 ,145, 16 },
	{ 0, 60 ,179, 16 },
	{ 0, 23, 210, 22 },
	{ 0, 176, 40, 39 },
	{ 0, 142, 74, 39 },
	{ 0, 113, 96, 46 },
	{ 0, 91, 125, 39 },
	{ 0, 62, 147, 46 },
	{ 0, 40, 176, 39 },
	{ 0, 179, 17, 59 },
	{ 0, 147, 45, 63 },
	{ 0, 17, 179, 59 },
	{ 0, 145, 17, 93 },
	{ 0, 125, 40, 90 },
	{ 0, 108, 74, 73 },
	{ 0, 74, 108, 73 },
	{ 0, 85, 85, 85 },
	{ 0, 40, 142, 73 },
	{ 0, 119, 17, 119 },
	{ 0, 96, 45, 114 },
	{ 0, 74, 74, 107 },
	{ 0, 45, 113, 97 },
	{ 0, 17, 145, 93 },
	{ 0, 94, 17, 144 },
	{ 0, 74, 40, 141 },
	{ 0, 45, 62, 148 },
	{ 0, 40, 91, 124 },
	{ 0, 17, 119, 119 },
	{ 0, 17, 94, 144 },
	{ 0, 60, 17, 178 },
	{ 0, 40, 40, 175 },
	{ 0, 17, 60, 178 },
	{ 0, 23, 23, 209 },
	{ 23, 0, 210, 22 },
	{ 60, 0, 179, 16 },
	{ 94, 0, 145, 16 },
	{ 119, 0, 119, 17 },
	{ 145, 0, 94, 16 },
	{ 179, 0, 60, 16 },
	{ 210, 0, 23, 22 },
	{ 40, 0, 176, 39 },
	{ 74, 0, 142, 39 },
	{ 96, 0, 113, 46 },
	{ 125, 0, 91, 39 },
	{ 147, 0, 62, 46 },
	{ 176, 0, 40, 39 },
	{ 17, 0, 179, 59 },
	{ 45, 0, 147, 63 },
	{ 179, 0, 17, 59 },
	{ 17, 0, 145, 93 },
	{ 40, 0, 125, 90 },
	{ 74, 0, 108, 73 },
	{ 108, 0, 74, 73 },
	{ 85, 0, 85, 85 },
	{ 142, 0, 40, 73 },
	{ 17, 0, 119, 119 },
	{ 45, 0, 96, 114 },
	{ 74, 0, 74, 107 },
	{ 113, 0, 45, 97 },
	{ 145, 0, 17, 93 },
	{ 17, 0, 94, 144 },
	{ 40, 0, 74, 141 },
	{ 62, 0, 45, 148 },
	{ 91, 0, 40, 124 },
	{ 119, 0, 17, 119 },
	{ 94, 0, 17, 144 },
	{ 17, 0, 60, 178 },
	{ 40, 0, 40, 175 },
	{ 60, 0, 17, 178 },
	{ 23, 0, 23, 209 },
	{ 209, 23, 23, 0 },
	{ 178, 17, 60, 0 },
	{ 144, 17, 94, 0 },
	{ 119, 17, 119, 0 },
	{ 93, 17, 145, 0 },
	{ 59, 17, 179, 0 },
	{ 22, 23, 210, 0 },
	{ 175, 40, 40, 0 },
	{ 141, 40, 74, 0 },
	{ 114, 45, 96, 0 },
	{ 90, 40, 125, 0 },
	{ 63, 45, 147, 0 },
	{ 39, 40, 176, 0 },
	{ 178, 60, 17, 0 },
	{ 148, 62, 45, 0 },
	{ 16, 60, 179, 0 },
	{ 144, 94, 17, 0 },
	{ 124, 91, 40, 0 },
	{ 107, 74, 74, 0 },
	{ 73, 74, 108, 0 },
	{ 85, 85, 85, 0 },
	{ 39, 74, 142, 0 },
	{ 119, 119, 17, 0 },
	{ 97, 113, 45, 0 },
	{ 73, 108, 74, 0 },
	{ 46, 96, 113, 0 },
	{ 16, 94, 145, 0 },
	{ 93, 145, 17, 0 },
	{ 73, 142, 40, 0 },
	{ 46, 147, 62, 0 },
	{ 39, 125, 91, 0 },
	{ 17, 119, 119, 0 },
	{ 16, 145, 94, 0 },
	{ 59, 179, 17, 0 },
	{ 39, 176, 40, 0 },
	{ 16, 179, 60, 0 },
	{ 22, 210, 23, 0 },
	{ 32, 32 ,32, 159 },
	{ 32, 32 ,74, 117 },
	{ 32, 32 ,116, 75 },
	{ 32, 32 ,158, 33 },
	{ 74, 32, 32, 117 },
	{ 74, 32, 74, 75 },
	{ 74, 32, 116, 33 },
	{ 116, 32, 32, 75 },
	{ 116, 32, 74, 33 },
	{ 158, 32, 32, 33 },
	{ 32, 74, 32, 117 },
	{ 32, 74, 74, 75 },
	{ 32, 74, 116, 33 },
	{ 74, 74, 32, 75 },
	{ 74, 74, 74, 33 },
	{ 116, 74, 32, 33 },
	{ 32, 116, 32, 75 },
	{ 32, 116, 74, 33 },
	{ 74, 116, 32, 33 },
	{ 32, 158, 32, 33 },
	};

class TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) = 0;
	virtual TRgb Color(TInt aIndex) = 0;
	};

class TNullColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb /*aColor*/) { return 0; }
	virtual TRgb Color(TInt /*aIndex*/) { return KRgbBlack; }
	};

class TGray2ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Gray2(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Gray2(aIndex); }
	};

class TGray4ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Gray4(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Gray4(aIndex); }
	};

class TGray16ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Gray16(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Gray16(aIndex); }
	};

class TGray256ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Gray256(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Gray256(aIndex); }
	};

class TColor16ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color16(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color16(aIndex); }
	};

class TColor256ColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color256(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color256(aIndex); }
	};

class TColor4KColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color4K(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color4K(aIndex); }
	};

class TColor64KColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color64K(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color64K(aIndex); }
	};

class TColor16MColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color16M(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color16M(aIndex); }
	};

class TColor16MUColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color16MU(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color16MU(aIndex); }
	};

class TColor16MAColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color16MA(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color16MA(aIndex); }
	};

class TColor16MAPColorConvertor : public TColorConvertor
	{
public:
	virtual TInt Index(TRgb aColor) { return aColor.Color16MAP(); }
	virtual TRgb Color(TInt aIndex) { return TRgb::Color16MAP(aIndex); }
	};

class CTLowLevel : public CTGraphicsBase
	{
enum TPostShadowMode
	{
	ENoPostShadow,
	EPostShadow,
	EPostFade,
	EPostShadowFade,
	};
public:
	CTLowLevel(CTestStep* aStep);
	~CTLowLevel();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void TestScreenDrawL(TDisplayMode aDisplayMode);
private:
	void Reset();
	void Report();
	void TestBitmapDraw(TDisplayMode aDisplayMode,const TSize& aSize);
	void Test();
	void TestParams();
	void TestReadLine();
	void TestWriteRgb();
	void TestWriteLine();
	void TestWriteBinary();
	void TestWriteRGBAlpha();
	void TestWriteAlphaLineEx();
	void TestWriteAlphaLineNoShadowEx();
	void TestWriteMaskLineNoShadowEx();
	void ClosePanicDialogs();
	void TestFastBlendBitmapMasked(const TInt aRetry = 0);
	void TestShadow();
	void TestWriteRgbOutlineAndShadow();
	inline TInt ByteSize();
	inline TBool Check(TBool aValue);
	void CheckBuffer(TUint8* aWriteBuffer,TDisplayMode aWriteDispMode,TUint8* aReadBuffer,TDisplayMode aReadDispMode,TInt aPixelLength);
	void CheckBinary(const TRect& aRect,TUint32* aBuffer,TRgb aForeColor,TRgb aBackColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode,TBool aWrapDataWords,TBool aUp);
	void CheckRgb(const TPoint& aPoint,TRgb aColor,TRgb aBackgroundColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode);
	void CheckRgb(const TRect& aRect,TRgb aColor,TRgb aBackgroundColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode);
	void CheckShadowRgb(const TRect& aRect,TRgb aColor,TInt aShadowMode);
	void CheckScanline(TInt aX,TInt aY,TInt aLength,TRgb aFore1,TRgb aFore2,TRgb aBack1,TRgb aBack2,CGraphicsContext::TDrawMode aDrawMode);
	void CheckLine(TUint8* aWriteBuffer,TUint8* aReadBuffer,TUint8* aBackBuffer,TInt aPixelLength,CGraphicsContext::TDrawMode aDrawMode,TDisplayMode aDispMode);
	void CheckPixel(TUint8* aWriteBuffer,TDisplayMode aWriteDispMode,TUint8* aReadBuffer,TDisplayMode aReadDispMode,TInt aPixelLength,TDisplayMode aCompareDispMode);
	void CheckBackground(const TRect& aRect,TRgb aBackgroundColor);
	TBool CheckBlendedOutlineAndShadow(TRgb aOutlinePenColor, TRgb aShadowColor, TRgb aFillColor, TRgb aBackgroundColor, TInt aLookupIndex, TInt aLength, TUint8* aReadBuffer);
	void Clear(TRgb aColor);
	void CheckMatch(TUint32 aFirst, TUint32 aSecond);
	TRgb ExtractRgbValue(TInt aX,TUint8* aBuffer,TDisplayMode aDispMode);
	TUint32 ExtractBinaryValue(TInt aX,TUint32* aBuffer,TDisplayMode aDispMode);
	void FillArray(TRgb aColor,TRgb aArray[4]);
	void FillBuffer(TUint8* aBuffer, TInt aByteSize, TDisplayMode aDispMode, TBool aNoAlpha16MU = EFalse);
	TInt LongWidth(TInt aWidth,TDisplayMode aDispMode);
	void Normalize(TRgb& aColor);
	void Normalize(TRgb& aColor, TDisplayMode aDispMode);
	void Normalize(TRgb& aColor,TRgb aDitherColors[4]);
	TRgb RgbValue(TRgb aFore,TRgb aBack,CGraphicsContext::TDrawMode aDrawMode);
	TUint32 BinaryValue(TRgb aFore,TRgb aBack,CGraphicsContext::TDrawMode aDrawMode);
	void Shadow(TRgb& aColor,TInt aShadowMode);
	void Shadow(TUint8* aBuffer,TInt aByteLength,TInt aShadowMode);
	TUint8 FadeGray(TInt aGray256);
	TRgb FadeRgb(TRgb aColor, TBool aFastFade = EFalse);
	TColorConvertor& ColorConvertor(TDisplayMode aDisplayMode);
	void Blend(TUint8* aBuffer,TUint8* aBufferDest,TDisplayMode aDispMode);
	void PostBlendShadow(TUint32 &aPmaColor);
	void TestUserDisplayModeMapping();
	void PrepareDestPixel(TDisplayMode aDevDisplayMode, TRgb& aRgb, TInt aDstAlpha);
	void CheckMappedRgb(TDisplayMode aDevDisplayMode, TDisplayMode aUserDisplayMode, TRgb aRgb);
	void CheckNormalizedRgb(TRgb aReadRgb, TRgb aCheckRgb, TDisplayMode aDevDisplayMode, TDisplayMode aUserDisplayMode, TBool aWriteRgbAlphaLine);
	void WriteBinaryValue(TUint32 aX, TAny* const aPtr, TDisplayMode aMode, TUint32 aValue) const;
	void MaskedBlendBuffer(TAny* aSrc, TDisplayMode aSrcMode, TAny* aDst,
			TDisplayMode aDstMode, TUint32* aMsk, TInt aWidth, TBool aInvert);
	TUint32 BytesForPixels(TUint32 aPixels, TDisplayMode aMode);
	TBool CompareBlendMaskResults(TAny* aActual, TAny* aExpected, 
			TUint32 aWidth, TDisplayMode aMode, TBool aBlended, TDisplayMode aSrcMode = ENone);
protected:
	CFbsDrawDevice* iDrawDevice;
	TUint8* iBits;
	TUint32* iBuf;
	TDisplayMode iDispMode;
	TDisplayMode iUserDispMode;
	TInt iOrientation;
	TSize iSize;
	TInt iLongWidth;
	TInt iTestNo;
	TInt iIteration;
	TInt iReportIteration;
	TInt iTotalReportIterations;
	TNullColorConvertor iNullConvertor;
	TGray2ColorConvertor iGray2Convertor;
	TGray4ColorConvertor iGray4Convertor;
	TGray16ColorConvertor iGray16Convertor;
	TGray256ColorConvertor iGray256Convertor;
	TColor16ColorConvertor iColor16Convertor;
	TColor256ColorConvertor iColor256Convertor;
	TColor4KColorConvertor iColor4KConvertor;
	TColor64KColorConvertor iColor64KConvertor;
	TColor16MColorConvertor iColor16MConvertor;
	TColor16MUColorConvertor iColor16MUConvertor;
	TColor16MAColorConvertor iColor16MAConvertor;
	TColor16MAPColorConvertor iColor16MAPConvertor;
	TColorConvertor* iColorConvertor[15];
	TInt iOrientationEnd;
	TInt iCurScreenDeviceModeIndex;
	TBool iFuzzyMatch;
	TBool iBlendTestColors;
	TPostShadowMode iPostBlendShadow;
	TBool i16MAUserDispMode;
	TBool iUseFastFade; //used to check whether fast fading method is required or not
						// while calling FadeRgb from CTLowLevel::Shadow(TRgb& aColor,TInt aShadowMode)
	TBool iLastFailTestNo;
	};//end of - class TestLowLevel

class CTLowLevel1 : public CTLowLevel
	{
public:
	CTLowLevel1(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	};

class CTLowLevelStep : public CTGraphicsStep
	{
public:
	CTLowLevelStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLowLevelStep,"TLowLevel");

class CTLowLevel1Step : public CTGraphicsStep
	{
public:
	CTLowLevel1Step();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLowLevel1Step,"TLowLevel1");

#endif//__TLLD_H_
