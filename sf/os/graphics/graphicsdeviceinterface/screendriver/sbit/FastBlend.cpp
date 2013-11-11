// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitstd.h>
#include <graphics/lookuptable.h>
#include <bitdrawinterfaceid.h>
#include "../inc/BMDRAW.H"
#include <graphics/gdi/gdiinline.inl>

// Current plan is to depreciate 16MA targets so we have removed acceleration of these.
// To re-enable put the __SUPPORT_16MA_TARGET__ back in
//#define __SUPPORT_16MA_TARGET__

// Enabling __CHECK_ALPHA01__ causes checks for alpha values of 0 and 1 to be made to attempt to
// accelerate handling of these cases. This may not always work as branching the execution path
// can potentially lose more time than is gained.

#define __CHECK_ALPHA01__

#define WRITE_RUN \
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 7:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 6:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 5:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 4:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 3:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 2:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 1:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN2ROT \
	FORCEINLINE static void write2rot(const TUint8* aSrc, TUint8* aDst, TInt aLen, TInt aSrcStep, TInt aDstStep) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 7:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 6:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 5:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 4:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 3:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 2:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
			case 1:              write(aSrc, aDst);aSrc+=aSrcStep;aDst+=aDstStep; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN2H \
	static void write2(const TUint8* aSrc, TUint8* aDst, TInt aLen);

#define WRITE_RUN2C(class3232) \
	void class3232::write2(const TUint8* aSrc, TUint8* aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 7:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 6:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 5:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 4:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 3:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 2:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
			case 1:              write(aSrc, aDst);aSrc+=4;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN2416 \
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 7:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 6:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 5:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 4:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 3:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 2:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 1:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN3216 \
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 7:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 6:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 5:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 4:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 3:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 2:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 1:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN3216C2(class3216) \
	void class3216::write2(const TUint8* aSrc, TUint8* aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 7:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 6:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 5:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 4:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 3:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 2:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
			case 1:              write(aSrc, aDst);aSrc+=4;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN2416C2(class2416) \
	void class2416::write2(const TUint8* aSrc, TUint8* aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 7:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 6:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 5:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 4:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 3:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 2:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
			case 1:              write(aSrc, aDst);aSrc+=3;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_RUN1632 \
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen) \
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 7:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 6:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 5:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 4:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 3:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 2:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
			case 1:              write(aSrc, aDst);aSrc+=2;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

// Using non-inline versions can make better usage of registers, need to experiment to find fastest mix

#define WRITE_RUN1632C(class1632) \
	void class1632::write2(const TUint8* aSrc, TUint8* aDst, TInt aLen)\
		{ \
		const TUint16* lowAdd = Convert16to32bppLow();\
		const TUint32* highAdd = Convert16to32bppHigh();\
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 7:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 6:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 5:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 4:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 3:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 2:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
			case 1:              Write64KTo16MLookup(aSrc, aDst, lowAdd, highAdd);aSrc+=2;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN \
	FORCEINLINE static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 7:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 6:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 5:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 4:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 3:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 2:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
			case 1:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN3216 \
	FORCEINLINE static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 7:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 6:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 5:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 4:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 3:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 2:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
			case 1:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=4;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN2416 \
	FORCEINLINE static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 7:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 6:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 5:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 4:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 3:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 2:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
			case 1:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=3;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN1632 \
	FORCEINLINE static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 7:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 6:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 5:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 4:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 3:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 2:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
			case 1:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN1632H \
	static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen);

#define WRITE_ALPHA_MASK_RUN1632C(class1632) \
	void class1632::writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		const TUint8* srcPtr=aSrc;\
		TUint8* dstPtr=aDst;\
		const TUint8* alphaPtr=aAlpha;\
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 7:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 6:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 5:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 4:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 3:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 2:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
			case 1:              writeMask(srcPtr, dstPtr, *alphaPtr++);srcPtr+=2;dstPtr+=4; \
				} while ((aLen -= 8) > 0); \
			} \
		aSrc=srcPtr;\
		aDst=dstPtr;\
		aAlpha=alphaPtr;\
		}

#define WRITE_ALPHA_MASK_RUN1616 \
	FORCEINLINE static void writeAlphaMask(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 7:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 6:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 5:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 4:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 3:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 2:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
			case 1:              writeMask(aSrc, aDst, *aAlpha++);aSrc+=2;aDst+=2; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN_ROT32 \
	FORCEINLINE static void writeAlphaMaskRot(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen, TInt aSrcStride, TInt aMaskStride)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 7:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 6:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 5:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 4:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 3:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 2:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
			case 1:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=4;aAlpha+=aMaskStride; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define WRITE_ALPHA_MASK_RUN_ROT16 \
	FORCEINLINE static void writeAlphaMaskRot(const TUint8* &aSrc, TUint8* &aDst, const TUint8* aAlpha, TInt aLen, TInt aSrcStride, TInt aMaskStride)\
		{ \
		switch (aLen % 8)  /* aLen > 0 assumed */ \
			{ \
			case 0:        do {  writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 7:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 6:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 5:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 4:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 3:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 2:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
			case 1:              writeMask(aSrc, aDst, *aAlpha);aSrc+=aSrcStride;aDst+=2;aAlpha+=aMaskStride; \
				} while ((aLen -= 8) > 0); \
			} \
		}

#define MultAlphaWithSrcAlpha(aAlpha, aSrc)\
	{\
	const TUint32 src=*(TUint32*)aSrc;\
	TUint32 srcAlpha=src>>24;\
	aAlpha=(aAlpha*srcAlpha);\
	aAlpha+=srcAlpha;\
	aAlpha>>=8;\
	}

// Takes a pre-multipled alpha source and additionally multiplies it by the alpha
// value so the source is effectively now pre-multiplied by both it's own alpha
// and the specified alpha.
// No aAlpha==0xFF or ==0 checks as should never come here in those situations
#define MultMapSrcByAlpha(aAlpha, aSrc)\
	{\
	TUint32 d1 = (aSrc>>8)&0x00FF00FF;\
	d1=d1*aAlpha+d1;\
	TUint32 d2=aSrc&0x00FF00FF;\
	d2=d2*aAlpha+d2;\
	aSrc=(d1&0xFF00FF00)|((d2&0xFF00FF00)>>8);\
	}

#define Write16MTo64K(aSrc, aDst)\
	{\
	TInt color64K=(aSrc & 0x0000f8) >> 3;\
	color64K|=(aSrc & 0x00fc00) >> 5;\
	color64K|=(aSrc & 0xf80000) >> 8;\
	*(TUint16*)aDst = color64K;\
	}

// Calc new alpha as src+(1-src)*dst;
#define CalcDestMultAlpha(aDestMultAlpha, aDA, aSrcAlpha, aDstAlpha)\
	{\
	const TUint32 srcAlpha=aSrcAlpha;\
	const TUint32 dstAlpha=aDstAlpha;\
	aDA=dstAlpha<<16;\
	aDA=aDA*(0x100-srcAlpha);\
	aDA+=srcAlpha<<24;\
	aDestMultAlpha=(((0x100-srcAlpha)*dstAlpha)>>8)+1;\
	}

// Note: This function assumes incoming rgb's are shifted up by an extra 8 bits as that's the
// most efficient way of processing the preceding functions with this final write to 64K handling
// the extra shift down.
#define WriteRedBlueAndGreenTo64K(aRedBlue, aGreen, aDst)\
	{\
	TInt targ64K=(aRedBlue&0x00f800) >> 11;\
	targ64K|=(aRedBlue&0xf8000000) >> 16;\
	targ64K|=(aGreen&0xfc0000) >> 13;\
	*(TUint16*)aDst = targ64K;\
	}


// Used for calculating blending from a MAP source to any of 16M dest formats.
#define CalcMapToMxRGBA(aSrcPixel, aDst, aDestMult, aDestAG, aDestRB)\
	{\
	const TUint32 dstPixel=*(TUint32*)aDst;\
	aDestAG=(dstPixel&0xFF00FF00)>>8;\
	aDestAG=aDestAG*aDestMult;\
	aDestAG+=aSrcPixel&0xFF00FF00;\
	aDestRB=dstPixel&0x00FF00FF;\
	aDestRB=(aDestRB*aDestMult)>>8;\
	aDestRB+=aSrcPixel&0x00FF00FF;\
	}

// Used For non MAP source blending using dest=src*alpha+dest*destAlpha
// aDestMultAlpha is typically (1-aAlpha) or (srcAlpha+(1-srcAlpha)*destAlpha)
#define CalcMxToMxRGBA2A(aMxMxSrcPixel, aMxMxDestPixel, aMxMxAlpha, aMxMxDestMultAlpha, aMxMxDestAG, aMxMxDestRB)\
	{\
	aMxMxDestAG=(aMxMxDestPixel & 0xFF00FF00)>>8;\
	aMxMxDestAG=aMxMxDestAG*aMxMxDestMultAlpha;\
	TUint32 srcAG=(aMxMxSrcPixel&0xFF00FF00)>>8;\
	aMxMxDestAG&=0xFF00FF00;\
	TUint32 alphaPlus1=aMxMxAlpha+1;\
	aMxMxDestAG+=srcAG*alphaPlus1;\
	aMxMxDestRB=aMxMxDestPixel&0x00FF00FF;\
	aMxMxDestRB=aMxMxDestRB*aMxMxDestMultAlpha;\
	aMxMxDestRB&=0xFF00FF00;\
	TUint32 srcRB=(aMxMxSrcPixel&0x00FF00FF);\
	aMxMxDestRB+=srcRB*alphaPlus1;\
	aMxMxDestRB>>=8;\
	}

// Used For non MAP source blending using dest=src*alpha+dest*(1-alpha)
#define CalcMxToMxRGBA(aSrc, aDest, aAlpha, aDestAG, aDestRB)\
	{\
	const TUint32 srcPixel=*(TUint32*)aSrc;\
	const TUint32 dstPixel=*(TUint32*)aDst;\
	const TUint32 oneMinusAlpha = 0x100 - aAlpha;\
	CalcMxToMxRGBA2A(srcPixel, dstPixel, aAlpha, oneMinusAlpha , aDestAG, aDestRB);\
	}

#define WriteMu(aDestAG, aDestRB, aDst)\
	*(TUint32*)aDst=(aDestAG&0xFF00FF00)|(aDestRB&0x00FF00FF)|0xFF000000

#define WriteMxA(aDestG, aDestRB, aDestA, aDst)\
	*(TUint32*)aDst=(aDestG&0x0000FF00)|(aDestRB&0x00FF00FF)|(aDestA&0xFF000000)

#define WriteMx(aDestAG, aDestRB, aDst)\
	*(TUint32*)aDst=(aDestAG&0xFF00FF00)|(aDestRB&0x00FF00FF)

struct TMapToMu
	{
	#define writeMapMu(aSrc, aDst)\
		{\
		const TUint32 oneMinusAlpha = 0x100-(aSrc>>24);\
		TUint32 d1;\
		TUint32 d2;\
		CalcMapToMxRGBA(aSrc,aDst,oneMinusAlpha,d1,d2);\
		WriteMu(d1,d2,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			*(TUint32*)aDst = src;
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		writeMapMu(src, aDst);
		}
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src>0x00FFFFFF)
#endif
			{
			MultMapSrcByAlpha(aAlpha,src);
			// No aAlpha==0xFF check as should never come here in that situation
			writeMapMu(src, aDst);
			}
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMapToMa
	{
	#define writeMapMa(aSrc, aDst)\
		{\
		const TUint32 d=*(TUint32*)aDst;\
		TUint32 da;\
		TUint32 destMultAlpha;\
		CalcDestMultAlpha(destMultAlpha,da,aSrc>>24,d>>24);\
		TUint32 d1;\
		TUint32 d2;\
		CalcMapToMxRGBA(aSrc,aDst,destMultAlpha,d1,d2);\
		WriteMxA(d1,d2,da,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src>0x00FFFFFF)
			{
			if (src >= 0xFF000000)
				*(TUint32*)aDst=src;
			else
				writeMapMa(src,aDst);
			}
#else
		writeMapMa(src,aDst);
#endif
		}
	WRITE_RUN
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src>0x00FFFFFF)
#endif
			{
			MultMapSrcByAlpha(aAlpha,src);
			// No aAlpha==0xFF check as should never come here in that situation
			writeMapMa(src,aDst);
			}
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMapToMap
	{
	#define writeMapMap(aSrc, aDst)\
		{\
		const TUint32 oneMinusAlpha = 0x100-(aSrc>>24);\
		TUint32 d1;\
		TUint32 d2;\
		CalcMapToMxRGBA(aSrc,aDst,oneMinusAlpha,d1,d2);\
		WriteMx(d1,d2,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			*(TUint32*)aDst = src;
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		writeMapMap(src,aDst);
		}
	WRITE_RUN
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src<=0x00FFFFFF)
			return;
#endif
		MultMapSrcByAlpha(aAlpha,src);
		// No aAlpha==0xFF check as should never come here in that situation
		writeMapMap(src,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMaToMu
	{
	#define writeMaMu(aSrc, aAlpha, aDst)\
		{\
		TUint32 d1;\
		TUint32 d2;\
		CalcMxToMxRGBA(aSrc, aDst, aAlpha, d1, d2);\
		WriteMu(d1,d2,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			*(TUint32*)aDst = src;
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		const TUint32 alpha=src>>24;
		writeMaMu(aSrc,alpha,aDst);
		}
	WRITE_RUN
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		MultAlphaWithSrcAlpha(aAlpha,aSrc);
		writeMaMu(aSrc,aAlpha,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMuToMu
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		*(TUint32*)aDst = *(TUint32*)aSrc;
		}
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		aSrc+=aLen*4;
		aDst+=aLen*4;
		}
	FORCEINLINE static void write2(const TUint8* aSrc, TUint8* aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		}
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 d1;
		TUint32 d2;
		CalcMxToMxRGBA(aSrc, aDst, aAlpha, d1, d2);
		WriteMu(d1,d2,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMuToMa
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		*(TUint32*)aDst = (*(TUint32*)aSrc);
		}
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		aSrc+=aLen*4;
		aDst+=aLen*4;
		}
	FORCEINLINE static void write2(const TUint8* aSrc, TUint8* aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		}
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 da;
		TUint32 d1;
		TUint32 d2;
		TUint32 destMultAlpha;
		const TUint32 d = *(TUint32*)aDst;
		CalcDestMultAlpha(destMultAlpha,da,aAlpha,d>>24);
		const TUint32 srcPixel=*(TUint32*)aSrc;
		const TUint32 dstPixel=*(TUint32*)aDst;
		CalcMxToMxRGBA2A(srcPixel, dstPixel, aAlpha, destMultAlpha, d1, d2);
		WriteMxA(d1,d2,da,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMaToMa
	{
	#define writeMaMa(aSrc, aAlpha, aDst)\
		{\
		TUint32 da;\
		TUint32 d1;\
		TUint32 d2;\
		TUint32 destMultAlpha;\
		const TUint32 d = *(TUint32*)aDst;\
		CalcDestMultAlpha(destMultAlpha,da,aAlpha,d>>24);\
		const TUint32 srcPixel=*(TUint32*)aSrc;\
		const TUint32 dstPixel=*(TUint32*)aDst;\
		CalcMxToMxRGBA2A(srcPixel, dstPixel, aAlpha, destMultAlpha, d1, d2);\
		WriteMxA(d1,d2,da,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			*(TUint32*)aDst = src;
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		const TUint32 alpha=src>>24;
		writeMaMa(aSrc,alpha,aDst);
		}
	WRITE_RUN
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		MultAlphaWithSrcAlpha(aAlpha,aSrc);
		writeMaMa(aSrc,aAlpha,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMaToMap
	{
	#define writeMaToMap(aSrc, aAlpha, aDst)\
		{\
		const TUint32 d = *(TUint32*)aDst;\
		const TUint32 oneMinusAlpha = 0x100-aAlpha;\
		TUint32 da=(d&0xFF000000)>>8;\
		da=da*oneMinusAlpha;\
		da+=aAlpha<<24;\
		TUint32 d1;\
		TUint32 d2;\
		CalcMxToMxRGBA(aSrc, aDst, aAlpha, d1, d2);\
		WriteMxA(d1,d2,da,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			*(TUint32*)aDst = src;
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		const TUint32 alpha=src>>24;
		writeMaToMap(aSrc,alpha,aDst);
		}
	WRITE_RUN
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		MultAlphaWithSrcAlpha(aAlpha,aSrc);
		writeMaToMap(aSrc,aAlpha,aDst);
		}
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct TMuToMap
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		*(TUint32*)aDst = (*(TUint32*)aSrc);
		}
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		aSrc+=aLen*4;
		aDst+=aLen*4;
		}
	FORCEINLINE static void write2(const TUint8* aSrc, TUint8* aDst, TInt aLen)
		{
		Mem::Move(aDst,aSrc,aLen*sizeof(TUint32));
		}
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 d1;
		TUint32 d2;
		CalcMxToMxRGBA(aSrc, aDst, aAlpha, d1, d2);
		WriteMx(d1,d2,aDst);
		}
	WRITE_ALPHA_MASK_RUN_ROT32
	WRITE_RUN2ROT
	WRITE_ALPHA_MASK_RUN
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

WRITE_RUN2C(TMapToMap)
WRITE_RUN2C(TMapToMa)
WRITE_RUN2C(TMapToMu)
WRITE_RUN2C(TMaToMap)
WRITE_RUN2C(TMaToMa)
WRITE_RUN2C(TMaToMu)

// reads green value from 64K source, or's it with 0xFF alpha channel, and leaves
// it shifted down by 8 ready for multiplying by alpha value.
#define ReadGreen64K(aRG64KGreen, aRG64KSrc)\
	aRG64KGreen=(aRG64KSrc&0x07E0)>>3;\
	aRG64KGreen=((aRG64KGreen+(aRG64KGreen>>6))&0x000000FF)|0x00FF0000;

// Reads the red and blue values from a 64K source into their RGBA values.
#define ReadRedBlue64K(aRedBlue,aColor64K)\
	{\
	aRedBlue=(aColor64K&0xF800)<<8;\
	aRedBlue|=(aColor64K&0x001F)<<3;\
	aRedBlue+=aRedBlue>>5;\
	aRedBlue&=0x00FF00FF;\
	}

// reads green value from 64K source into aGreen and red and blue into aRedBlue
// All left in correct place for 16M operations
#define Read64KColors(aGreen,aRedBlue,aPtr)\
	{\
	const TUint32 r64Kcolor64K=*(TUint16*)aPtr;\
	aGreen=(r64Kcolor64K&0x07E0)<<5;\
	aGreen+=aGreen>>6;\
	aGreen&=0x0000FF00;\
	ReadRedBlue64K(aRedBlue,r64Kcolor64K);\
	}

struct TMapTo64K
	{
	#define writeMap64K(aSrc, aDst)\
		{\
		TUint32 green;\
		TUint32 redBlue;\
		Read64KColors(green,redBlue,aDst);\
		const TUint32 oneMinusAlpha = 0x100-(aSrc>>24);\
		green=green*oneMinusAlpha;\
		green+=(aSrc<<8)&0x00FF00FF;\
		redBlue=redBlue*oneMinusAlpha;\
		redBlue+=(aSrc&0x00FF00FF)<<8;\
		WriteRedBlueAndGreenTo64K(redBlue,green,aDst);\
		}
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			Write16MTo64K(src,aDst);
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		writeMap64K(src,aDst);
		}
	WRITE_RUN2H
	WRITE_RUN3216
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src<=0x00FFFFFF)
			return;
#endif
		MultMapSrcByAlpha(aAlpha,src);
		// No aAlpha==0xFF check as should never come here in that situation
		writeMap64K(src,aDst);
		}
	WRITE_ALPHA_MASK_RUN3216
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint16));};
	};

// Calculates green and redBlue shifted up by 8, that will be sorted in WriteRedBlueAndGreenTo64K
// this works out the most efficient way of getting the end result
//
// Note +++
// Having the extra +greenSrc and +redBlueSrc (as in lines shown below) gives a better result, but
// is inconsistent with old code and causes test code failures, so it has been removed.
//	greenSrc=greenSrc*aAlpha+greenSrc;
//	redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
#define WriteMaOrMuTo64K(aSrc, aDst, aAlpha)\
	{\
	const TUint32 src=*(TUint32*)aSrc;\
	TUint32 green;\
	TUint32 redBlue;\
	Read64KColors(green,redBlue,aDst);\
	const TUint32 oneMinusAlpha = 0x100-aAlpha;\
	green=green*oneMinusAlpha;\
	TUint32 greenSrc=src&0x0000FF00;\
	greenSrc=greenSrc*aAlpha;\
	green+=greenSrc;\
	redBlue=redBlue*oneMinusAlpha;\
	TUint32 redBlueSrc=src&0x00FF00FF;\
	redBlueSrc=redBlueSrc*aAlpha;\
	redBlue+=redBlueSrc;\
	WriteRedBlueAndGreenTo64K(redBlue,green,aDst);\
	}

struct TMTo64K
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=aSrc[0]+(aSrc[1]<<8)+(aSrc[2]<<16);
		Write16MTo64K(src,aDst);
		}
	WRITE_RUN2416
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		const TUint32 src=aSrc[0]+(aSrc[1]<<8)+(aSrc[2]<<16);
		TUint32 green;
		TUint32 redBlue;
		Read64KColors(green,redBlue,aDst);
		const TUint32 oneMinusAlpha = 0x100-aAlpha;
		green=green*oneMinusAlpha;
		TUint32 greenSrc=src&0x0000FF00;
		greenSrc=greenSrc*aAlpha+greenSrc;
		green+=greenSrc;
		redBlue=redBlue*oneMinusAlpha;
		TUint32 redBlueSrc=src&0x00FF00FF;
		redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
		redBlue+=redBlueSrc;
		WriteRedBlueAndGreenTo64K(redBlue,green,aDst);
		}
	WRITE_ALPHA_MASK_RUN2416
	FORCEINLINE static TInt SrcPixelBytes() {return(3);};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint16));};
	};

struct TMuTo64K
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
		Write16MTo64K(src,aDst);
		}
	WRITE_RUN3216
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		WriteMaOrMuTo64K(aSrc,aDst,aAlpha);
		}
	WRITE_ALPHA_MASK_RUN3216
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint16));};
	};

struct TMaTo64K
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		const TUint32 src=*(TUint32*)aSrc;
#if defined(__CHECK_ALPHA01__)
		if (src >= 0xFF000000)
			{
			Write16MTo64K(src,aDst);
			return;
			}
		if (src <= 0x00FFFFFF)
			return;
#endif
		const TUint32 alpha=src>>24;
		WriteMaOrMuTo64K(aSrc,aDst,alpha);
		}
	WRITE_RUN3216
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		MultAlphaWithSrcAlpha(aAlpha,aSrc);
		WriteMaOrMuTo64K(aSrc,aDst,aAlpha);
		}
	WRITE_ALPHA_MASK_RUN3216
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint32));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint16));};
	};

struct T64KTo64K
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		*(TUint16*)aDst=*(const TUint16 *)aSrc;
		}
	FORCEINLINE static void write(const TUint8* &aSrc, TUint8* &aDst, TInt aLen)
		{
		Mem::Copy(aDst,aSrc,aLen*sizeof(TUint16));
		aSrc+=aLen*2;
		aDst+=aLen*2;
		}
	FORCEINLINE static void write2(const TUint8* aSrc, TUint8* aDst, TInt aLen)
		{
		Mem::Copy(aDst,aSrc,aLen*sizeof(TUint16));
		}
	WRITE_ALPHA_MASK_RUN_ROT16
	WRITE_RUN2ROT
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		const TUint32 oneMinusAlpha = 0x100-aAlpha;
//
		TUint32 green;
		TUint32 redBlue;
		Read64KColors(green,redBlue,aDst);
		green=green*oneMinusAlpha;
		TUint32 greenSrc;
		TUint32 redBlueSrc;
		Read64KColors(greenSrc,redBlueSrc,aSrc);
// See note +++
//		greenSrc=greenSrc*aAlpha+greenSrc;
		greenSrc=greenSrc*aAlpha;
		green+=greenSrc;	// needs shift down by 8, but do that when going to 64K
//
		redBlue=redBlue*oneMinusAlpha;
// See note +++
//		redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
		redBlueSrc=redBlueSrc*aAlpha;
		redBlue+=redBlueSrc;	// needs shift down by 8, but do that when going to 64K
		WriteRedBlueAndGreenTo64K(redBlue,green,aDst);
		}
	WRITE_ALPHA_MASK_RUN1616
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint16));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint16));};
	};

#define Write64KTo16M(aSrc, aDst)\
	TUint32 w6216Mgreen;\
	TUint32 w6216MredBlue;\
	Read64KColors(w6216Mgreen,w6216MredBlue, aSrc);\
	WriteMu(w6216Mgreen,w6216MredBlue,aDst);

#define Write64KTo16MLookup(aSrc, aDst, aLowAdd, aHighAdd)\
	{\
	const TUint32 srcData=*(TUint16*)aSrc;\
	*(TUint32*)aDst = aHighAdd[srcData>>8] | aLowAdd[srcData&0xff];\
	}

struct T64KToMu
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		Write64KTo16M(aSrc,aDst);
		}
	WRITE_RUN1632
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		const TUint32 oneMinusAlpha = 0x100-aAlpha;
		const TUint32 d = *(TUint32*)aDst;
		TUint32 src=*(TUint16*)aSrc;
//
		TUint32 green = (d&0x0000FF00)>>8;
		green=green*oneMinusAlpha;
		TUint32 greenSrc;
		ReadGreen64K(greenSrc,src);
// See note +++
//		greenSrc=greenSrc*aAlpha+greenSrc;
		greenSrc=greenSrc*aAlpha;
		green+=greenSrc;
//
		TUint32 redBlue = d&0x00FF00FF;
		redBlue=redBlue*oneMinusAlpha;
		TUint32 redBlueSrc;
		ReadRedBlue64K(redBlueSrc,src);
// See note +++
//		redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
		redBlueSrc=redBlueSrc*aAlpha;
		redBlue+=redBlueSrc;
//
		redBlue>>=8;
		WriteMu(green,redBlue,aDst);
		}
	WRITE_ALPHA_MASK_RUN1632
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint16));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

struct T64KToMa
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		Write64KTo16M(aSrc,aDst);
		}
	WRITE_RUN1632
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		const TUint32 destSrc = *(TUint32*)aDst;
		TUint32 destAlpha=destSrc>>24;
// Calc new alpha as src+(1-src)*dst;
		destAlpha=(0x100-aAlpha)*destAlpha;
		TUint32 targDestPixel=(destAlpha>>8)<<24;
		targDestPixel+=aAlpha<<24;
//
		TUint32 greenAlpha=(destSrc>>8)&0x000000FF;
		const TUint32 destMultAlpha = (destAlpha>>8)+1;
		greenAlpha=greenAlpha*destMultAlpha;
		TUint32 redBlue = destSrc&0x00FF00FF;
		redBlue=redBlue*destMultAlpha;
//
		TUint32 src=*(TUint16*)aSrc;
		TUint32 greenSrc;
		ReadGreen64K(greenSrc,src);
		greenSrc=greenSrc*aAlpha+greenSrc;
		greenAlpha+=greenSrc;
		targDestPixel|=greenAlpha&0x0000FF00;
//
		TUint32 redBlueSrc;
		ReadRedBlue64K(redBlueSrc,src);
		redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
		redBlue+=redBlueSrc;
//
		targDestPixel|=(redBlue>>8)&0x00FF00FF;
		*(TUint32*)aDst=targDestPixel;
		}
	WRITE_ALPHA_MASK_RUN1632H
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint16));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

WRITE_ALPHA_MASK_RUN1632C(T64KToMa)

struct T64KToMap
	{
	FORCEINLINE static void write(const TUint8 *aSrc, TUint8* aDst)
		{
		Write64KTo16M(aSrc,aDst);
		}
	WRITE_RUN1632
	WRITE_RUN2H
	FORCEINLINE static void writeMask(const TUint8 *aSrc, TUint8* aDst, TUint aAlpha)
		{
		const TUint32 oneMinusAlpha = 0x100-aAlpha;
		const TUint32 d = *(TUint32*)aDst;
		TUint32 src=*(TUint16*)aSrc;
//
		TUint32 greenAlpha = (d&0xFF00FF00)>>8;
		greenAlpha=greenAlpha*oneMinusAlpha;
		TUint32 greenSrc;
		ReadGreen64K(greenSrc,src);
		greenSrc=greenSrc*aAlpha+greenSrc;
		greenAlpha&=0xFF00FF00;	// Needed to stop adding rounding errors together in next step
		greenAlpha+=greenSrc;
//
		TUint32 redBlue = d&0x00FF00FF;
		redBlue=redBlue*oneMinusAlpha;
		TUint32 redBlueSrc;
		ReadRedBlue64K(redBlueSrc,src);
		redBlueSrc=redBlueSrc*aAlpha+redBlueSrc;
		redBlue&=0xFF00FF00; // Needed to stop adding rounding errors together in next step
		redBlue+=redBlueSrc;	// needs shift down by 8, but do that when writing to dest
//
		redBlue>>=8;
		WriteMx(greenAlpha,redBlue,aDst);
		}
	WRITE_ALPHA_MASK_RUN1632
	FORCEINLINE static TInt SrcPixelBytes() {return(sizeof(TUint16));};
	FORCEINLINE static TInt DestPixelBytes() {return(sizeof(TUint32));};
	};

WRITE_RUN1632C(T64KToMap)
WRITE_RUN1632C(T64KToMa)
WRITE_RUN1632C(T64KToMu)
WRITE_RUN3216C2(TMapTo64K)
WRITE_RUN3216C2(TMaTo64K)
WRITE_RUN3216C2(TMuTo64K)
WRITE_RUN2416C2(TMTo64K)

inline TInt InitDda(TInt &aDdaCount, TBool &aStretching, TInt aSrcValue, TInt aDstValue, TInt aSkipSteps)
	{
	aDdaCount=0;
	aStretching=aDstValue>aSrcValue;
	TInt skip=0;
	if (aStretching)
		{
		aDdaCount=aDstValue-1;
		while(aSkipSteps--)
			{
			aDdaCount-=aSrcValue;
			if (aDdaCount<0)
				{
				skip++;
				aDdaCount+=aDstValue;
				}
			}
		}
	else
		{
		aDdaCount=aSrcValue-1;
		while(aSkipSteps)
			{
			aDdaCount-=aDstValue;
			if (aDdaCount<0)
				{
				aSkipSteps--;
				aDdaCount+=aSrcValue;
				}
			skip++;
			}
		}
	return(skip);
	}

inline void DdaStep(TInt &aDdaCount, TBool aStretching, TInt aSrcValue, TInt aDstValue, TInt &aSkipCount)
	{
	if (aStretching)
		{
		aDdaCount-=aSrcValue;
		if (aDdaCount<0)
			{
			aSkipCount++;
			aDdaCount+=aDstValue;
			}
		}
	else
		{
		do
			{
			aSkipCount++;
			aDdaCount-=aDstValue;
			} while(aDdaCount>=0);
		aDdaCount+=aSrcValue;
		}
	}

template <class op>
static void ScaledFastBlit(const TUint8* aSrcBase, TInt aSrcStride, TRect& aSrcRect, TUint8 *aDataAddress, TUint32 aDstStride, TRect& aDstRect, const TRect &aClipRect)
	{
	TInt srcWidth = aSrcRect.Width();
	TInt srcHeight = aSrcRect.Height();
	TInt dstWidth = aDstRect.Width();
	TInt dstHeight = aDstRect.Height();
//
	TInt yDdaCount;
	TBool yStretching;
	TInt ySrcOffset=aSrcRect.iTl.iY+InitDda(yDdaCount, yStretching, srcHeight, dstHeight, aClipRect.iTl.iY-aDstRect.iTl.iY);
//
	TInt xDdaCountBase;
	TBool xStretching;
	TInt sxOffset=aSrcRect.iTl.iX+InitDda(xDdaCountBase, xStretching, srcWidth, dstWidth, aClipRect.iTl.iX-aDstRect.iTl.iX);
	sxOffset*=op::SrcPixelBytes();
//
	const TInt yEnd=aClipRect.iBr.iY-aDstRect.iTl.iY;
	const TInt xCount=aClipRect.Width();
	TUint8* dstPixelBase = aDataAddress + aDstStride*aClipRect.iTl.iY + aClipRect.iTl.iX*op::DestPixelBytes();
	for (TInt y = aClipRect.iTl.iY-aDstRect.iTl.iY; y < yEnd; ++y)
		{
		const TUint8* srcPixel = aSrcBase + aSrcStride*ySrcOffset + sxOffset;
		TUint8* dstPixel = dstPixelBase;
		TInt xCountDown=xCount;
		TInt ddaCount = xDdaCountBase;
		if (xStretching)
			{
			do
				{
				op::write(srcPixel, dstPixel);
				dstPixel+=op::DestPixelBytes();
				ddaCount-=srcWidth;
				if (ddaCount<0)
					{
					srcPixel+=op::SrcPixelBytes();
					ddaCount+=dstWidth;
					}
				} while(--xCountDown);
			}
		else
			{
			do
				{
				op::write(srcPixel, dstPixel);
				dstPixel+=op::DestPixelBytes();
				do
					{
					srcPixel+=op::SrcPixelBytes();
					ddaCount-=dstWidth;
					} while(ddaCount>=0);
				ddaCount+=srcWidth;
				} while(--xCountDown);
			}
		dstPixelBase+=aDstStride;
		DdaStep(yDdaCount, yStretching, srcHeight, dstHeight, ySrcOffset);
		}
	}

template <class op>
static void UnscaledFastBlit(const TUint8* aSrcBase, TInt aSrcStride, TRect& aSrcRect, TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos)
	{
	const TInt blitWidth = aSrcRect.Width();
	const TUint8* srcPixel = aSrcBase + aSrcStride*aSrcRect.iTl.iY + aSrcRect.iTl.iX*op::SrcPixelBytes();
	TUint8* dstPixel = aDataAddress + aDstStride*aDstPos.iY + aDstPos.iX*op::DestPixelBytes();
	const TUint8* dstPixelEnd = dstPixel+aSrcRect.Height()*aDstStride;
	do
		{
		op::write2(srcPixel,dstPixel,blitWidth);
		srcPixel+=aSrcStride;
		dstPixel+=aDstStride;
		} while(dstPixel<dstPixelEnd);
	}

static void ReplaceBlit(const TUint8* aSrcBase, TInt aSrcStride, TRect& aSrcRect, TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos)
	{
	TInt srcStride=aSrcStride/4;
	TInt dstStride=aDstStride/4;
	TInt dx = aDstPos.iX;
	TInt dstWidth = aSrcRect.Width();
	const TUint32* srcPixel = reinterpret_cast<const TUint32*>(aSrcBase) + srcStride*aSrcRect.iTl.iY + aSrcRect.iTl.iX;
	TUint32* dstPixel = ((TUint32*)aDataAddress) + dstStride*aDstPos.iY + dx;
	const TInt copyLen=dstWidth*4;
	for (TInt linesToGo=aSrcRect.Height();linesToGo;--linesToGo)
		{
		// Andy - not convinced that this function is any good atall
		// try stdlib memcpy instead.
		Mem::Move(dstPixel, srcPixel, copyLen);
		srcPixel+=srcStride;
		dstPixel+=dstStride;
		}
	}

template <class op>
static void UnscaledFastBlitRot(const TUint8* aSrcBase, CFbsDrawDevice::TOrientation aOrientation, TInt aSrcStride, TRect& aSrcRect, TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos, const TSize &aDestSize, TUint32* aScanLineBuffer)
	{
	TInt srcStep=op::SrcPixelBytes();
	TInt dstStep=op::DestPixelBytes();
	TInt dstStride=aDstStride;
	TInt blitWidth = aSrcRect.Width(); 
	TInt blitHeight = aSrcRect.Height(); 
	const TUint8* srcPixel = aSrcBase + aSrcRect.iTl.iY*aSrcStride + aSrcRect.iTl.iX*srcStep; 
	const TUint8* srcPixelEnd = srcPixel + blitHeight*aSrcStride;
	TUint8* dstPixel = aDataAddress; 
	
	
	switch(aOrientation)
		{
		case CFbsDrawDevice::EOrientationRotated90:
			dstPixel += (aDstPos.iX*aDstStride + (aDestSize.iWidth-aDstPos.iY-1)*op::DestPixelBytes());
			dstStep = aDstStride;
			dstStride = - op::DestPixelBytes();
			break;
		case CFbsDrawDevice::EOrientationRotated180: 
			dstPixel += ( (aDestSize.iHeight - aDstPos.iY -1 )*aDstStride +(aDestSize.iWidth - aDstPos.iX -1)*op::DestPixelBytes() ) ;
			dstStep = -dstStep;
			dstStride = -aDstStride;
			break;
		case CFbsDrawDevice::EOrientationRotated270:
			dstPixel += ( (aDestSize.iHeight- aDstPos.iX - 1 )*aDstStride + aDstPos.iY*op::DestPixelBytes() ) ;
			dstStep = -aDstStride;
			dstStride = op::DestPixelBytes();
			break;
		}
	do
		{
		Mem::Copy(aScanLineBuffer, srcPixel, blitWidth*srcStep);
		op::write2rot((TUint8*)aScanLineBuffer, dstPixel, blitWidth, srcStep, dstStep);
		srcPixel+=aSrcStride;
		dstPixel+=dstStride;
		} 
		while(srcPixel<srcPixelEnd);
	}

template <class op>
static void ScaledFastBlitMaskedG2(const TUint8* aSrcBase, TInt aSrcStride, const TRect& aSrcRect, const TUint8* aMaskBase, TInt aMaskStride, TBool aInvertMask, TUint8 *aDataAddress, TInt aDstStride, const TRect& aDstRect, const TRect &aClipRect)
	{
	TInt sx = aSrcRect.iTl.iX;
	TInt sxOffset=sx*op::SrcPixelBytes();
	
	TInt srcWidth = aSrcRect.Width();
	TInt srcHeight = aSrcRect.Height();
	TInt dstWidth = aDstRect.Width();
	TInt dstHeight = aDstRect.Height();
//
	TInt yDdaCount;
	TBool yStretching;
	TInt ySrcOffset=aSrcRect.iTl.iY+InitDda(yDdaCount, yStretching, srcHeight, dstHeight, aClipRect.iTl.iY-aDstRect.iTl.iY);
//
	TInt xDdaCountBase;
	TBool xStretching;
	TInt xOffsetBase=InitDda(xDdaCountBase, xStretching, srcWidth, dstWidth, aClipRect.iTl.iX-aDstRect.iTl.iX);
//
	const TInt yEnd=aClipRect.iBr.iY-aDstRect.iTl.iY;
	const TInt xCount=aClipRect.Width();
	TUint8* dstPixelBase = aDataAddress+aDstStride*aClipRect.iTl.iY+aClipRect.iTl.iX*op::DestPixelBytes();
	for (TInt y = aClipRect.iTl.iY-aDstRect.iTl.iY; y < yEnd; ++y)
		{
		const TUint8* srcPixel = aSrcBase+aSrcStride*ySrcOffset+sxOffset;
		const TUint32* mskRowBase = (const TUint32*)(aMaskBase+aMaskStride*ySrcOffset);
		TUint8* dstPixel = dstPixelBase;
		TInt xCountDown=xCount;
		TInt curMaskOffset=-1;
		TUint32 maskbits=0;
		TInt xOffset=xOffsetBase;
		TInt ddaCount = xDdaCountBase;
		if (xStretching)
			{
			do
				{
				const TInt maskOffset=xOffset+sx;
				TInt maskBitOffset=maskOffset>>5;
				if (curMaskOffset==maskBitOffset)
					{
blitIt1:			const TInt mask=1<<(maskOffset%32);
					if (maskbits&mask)
						op::write(srcPixel, dstPixel);
					dstPixel+=op::DestPixelBytes();
					ddaCount-=srcWidth;
					if (ddaCount<0)
						{
						xOffset++;
						srcPixel+=op::SrcPixelBytes();
						ddaCount+=dstWidth;
						}
					continue;
					}
				maskbits=*(mskRowBase+maskBitOffset);
				if (aInvertMask)
					maskbits=~maskbits;
				curMaskOffset=maskBitOffset;
				goto blitIt1;
				} while(--xCountDown);
			}
		else
			{
			do
				{
				const TInt maskOffset=xOffset+sx;
				TInt maskBitOffset=maskOffset>>5;
				if (curMaskOffset==maskBitOffset)
					{
blitIt2:			const TInt mask=1<<(maskOffset%32);
					if (maskbits&mask)
						op::write(srcPixel, dstPixel);
					dstPixel+=op::DestPixelBytes();
					do
						{
						xOffset++;
						srcPixel+=op::SrcPixelBytes();
						ddaCount-=dstWidth;
						} while(ddaCount>=0);
					ddaCount+=srcWidth;
					continue;
					}
				maskbits=*(mskRowBase+maskBitOffset);
				if (aInvertMask)
					maskbits=~maskbits;
				curMaskOffset=maskBitOffset;
				goto blitIt2;
				} while(--xCountDown);
			}
		dstPixelBase+=aDstStride;
		DdaStep(yDdaCount, yStretching, srcHeight, dstHeight, ySrcOffset);
		}
	}

template <class op>
static void UnscaledFastBlitMaskedG2(const TUint8* aSrcBase, TInt aSrcStride, TRect& aSrcRect, const TUint8* aMaskBase, TInt aMaskStride, TBool aInvertMask, TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos, const TPoint& aMaskSrcPos, const TSize &aMaskSize)
	{
	TInt sx = aSrcRect.iTl.iX;
	TInt sy = aSrcRect.iTl.iY;
	TInt dx = aDstPos.iX;
	TInt dy = aDstPos.iY;
	
	TInt dstWidth = aSrcRect.Width();
	TInt dstHeight = aSrcRect.Height();
	TInt maskWidth=aMaskSize.iWidth;
	TInt maskHeight=aMaskSize.iHeight;
	TInt maskXStart=aMaskSrcPos.iX%maskWidth;
	TInt maskStartShift=maskXStart&0x1F;
	TInt xMaskLoopCount=1+(dstWidth-1)/maskWidth;
	const TUint8* mskBasePtr=aMaskBase + (maskXStart/32)*4;
	for(TInt xMaskLoop=0;xMaskLoop<xMaskLoopCount;xMaskLoop++)
	  {
	  TInt xOffset=xMaskLoop*maskWidth;
	  TInt blitWidth=Min(maskWidth-maskStartShift,dstWidth-xOffset);
	  const TUint8* srcPixelStart = aSrcBase + aSrcStride*sy + (sx+xOffset)*op::SrcPixelBytes();
	  TUint8* dstPixelStart = aDataAddress + aDstStride*dy + (dx+xOffset)*op::DestPixelBytes();
	  for(TInt yPos=0;yPos<dstHeight;yPos++)
		{
		const TUint8* srcPixel=srcPixelStart;
		const TUint32* mskPixel=(const TUint32*)(mskBasePtr + aMaskStride*((aMaskSrcPos.iY+yPos)%maskHeight));
		TUint8* dstPixel=dstPixelStart;
		TUint mask=1<<maskStartShift;
		TUint32 maskPixels=*mskPixel;
		if (aInvertMask)
			maskPixels=~maskPixels;
		TInt runCount=0;
		TInt toGo=blitWidth;
		TUint32 endMask=0;
		const TInt tgMinusRun0=toGo+maskStartShift;
		if (tgMinusRun0<32)
			{
			endMask=1<<tgMinusRun0;
			maskPixels|=endMask;	// ensure the end of the scanline will fail set run where we will check for the end
			}
		// Into skip loop first, assume start of scanline more likely to be masked out than set
		FOREVER
			{
			if (!(mask&maskPixels))
				{
				runCount++;
rbm2startSkipRun:
				mask<<=1;
				if (mask!=0)
					continue;
				mask=1;
rbm2nextMaskSkip:
				const TInt tgMinusRun1=toGo-runCount;
				if (tgMinusRun1 == 0)
					{
					endMask = mask;
					maskPixels = endMask;
					}
				else
					{
					maskPixels=*++mskPixel;
					if (aInvertMask)
						{
						maskPixels=~maskPixels;
						}
					if (tgMinusRun1<32)
						{
						endMask=1<<tgMinusRun1;
						maskPixels|=endMask;	// ensure the end of the scanline will fail set run where we will check for the end
						}
					else if (maskPixels==0)
						{
						runCount+=32;
						goto rbm2nextMaskSkip;
						}
					}
				continue;
				}
			toGo-=runCount;
			if (toGo==0)
				goto rbm2nextY;
			dstPixel+=runCount*op::DestPixelBytes();
			srcPixel+=runCount*op::SrcPixelBytes();
			runCount=1;
			maskPixels^=endMask;	// toggle the end mask (if there is one)
			goto rbm2startSetRun;
			}
// Code within this section deals with runs of pixels to set
			{
rbm2startTopLoop:
			if (mask&maskPixels)
				{
				runCount++;
rbm2startSetRun:
				mask<<=1;
				if (mask!=0)
					goto rbm2startTopLoop;
				mask=1;
rbm2nextMaskSet:
				const TInt tgMinusRun2=toGo-runCount;
				if (tgMinusRun2 == 0)
					{
					endMask = mask;
					maskPixels = 0;
					}
				else
					{
					maskPixels=*++mskPixel;
					if (aInvertMask)
						{
						maskPixels=~maskPixels;
						}
					if (tgMinusRun2<32)
						{
						endMask=mask<<tgMinusRun2;
						maskPixels&=~endMask;	// ensure the end of the scanline will fail set run where we will check for the end
						}
					else if (maskPixels==0xFFFFFFFF)
						{
						runCount+=32;
						goto rbm2nextMaskSet;
						}
					}
				goto rbm2startTopLoop;
				}
			op::write(srcPixel, dstPixel, runCount);
			toGo-=runCount;
			if (toGo==0)
				goto rbm2nextY;
			maskPixels^=endMask;	// toggle the end mask (if there is one)
			runCount=1;
			goto rbm2startSkipRun;
			}
rbm2nextY:
		srcPixelStart+=aSrcStride;
		dstPixelStart+=aDstStride;
		}
	  maskStartShift=0;
	  }
	}

template <class op>
static void ScaledFastBlitMaskedG256(const TUint8* aSrcBase, TInt aSrcStride, const TRect& aSrcRect, const TUint8* aMaskBase, TInt aMaskStride, TUint8 *aDataAddress, TInt aDstStride, const TRect& aDstRect, const TRect &aClipRect)
	{
	TInt sx = aSrcRect.iTl.iX;
	TInt sxOffset=sx*op::SrcPixelBytes();
	
	TInt srcWidth = aSrcRect.Width();
	TInt srcHeight = aSrcRect.Height();
	TInt dstWidth = aDstRect.Width();
	TInt dstHeight = aDstRect.Height();
//
	TInt yDdaCount;
	TBool yStretching;
	TInt ySrcOffset=aSrcRect.iTl.iY+InitDda(yDdaCount, yStretching, srcHeight, dstHeight, aClipRect.iTl.iY-aDstRect.iTl.iY);
//
	TInt xDdaCountBase;
	TBool xStretching;
	TInt xOffsetBase=InitDda(xDdaCountBase, xStretching, srcWidth, dstWidth, aClipRect.iTl.iX-aDstRect.iTl.iX);
//
	const TInt yEnd=aClipRect.iBr.iY-aDstRect.iTl.iY;
	const TInt xCount=aClipRect.Width();
	TUint8* dstPixelBase = aDataAddress+aClipRect.iTl.iY*aDstStride+aClipRect.iTl.iX*op::DestPixelBytes();
	for (TInt y = aClipRect.iTl.iY-aDstRect.iTl.iY; y < yEnd; ++y)
		{
		const TUint8* srcRowBase = aSrcBase+aSrcStride*ySrcOffset+sxOffset;
		const TUint8* mskRowBase = aMaskBase+aMaskStride*ySrcOffset+sx;
		TUint8* dstPixel = dstPixelBase;
		TInt xCountDown=xCount;
		TInt xOffset=xOffsetBase;
//
		TInt ddaCount = xDdaCountBase;
		if (xStretching)
			{
			do
				{
				TUint mask=*(mskRowBase+xOffset);
				if (mask==0xFF)
					op::write(srcRowBase + xOffset*op::SrcPixelBytes(), dstPixel);
				else if (mask!=0)
					op::writeMask(srcRowBase + xOffset*op::SrcPixelBytes(), dstPixel, mask);
				dstPixel+=op::DestPixelBytes();
//
				ddaCount-=srcWidth;
				if (ddaCount<0)
					{
					xOffset++;
					ddaCount+=dstWidth;
					}
				} while(--xCountDown);
			}
		else
			{
			do
				{
				TUint mask=*(mskRowBase+xOffset);
				if (mask==0xFF)
					op::write(srcRowBase + xOffset*op::SrcPixelBytes(), dstPixel);
				else if (mask!=0)
					op::writeMask(srcRowBase + xOffset*op::SrcPixelBytes(), dstPixel, mask);
				dstPixel+=op::DestPixelBytes();
//
				do
					{
					xOffset++;
					ddaCount-=dstWidth;
					} while(ddaCount>=0);
				ddaCount+=srcWidth;
				} while(--xCountDown);
			}
		dstPixelBase+=aDstStride;
		DdaStep(yDdaCount, yStretching, srcHeight, dstHeight, ySrcOffset);
		}
	}

template <class op>
static void UnscaledFastBlitMaskedG256(const TUint8* aSrcBase, TInt aSrcStride, TRect& aSrcRect, const TUint8* aMaskBase, TInt aMaskStride, TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos, const TPoint& aMaskSrcPos, const TSize &aMaskSize)
	{
	TInt dstWidth = aSrcRect.Width();
	const TInt dstHeight = aSrcRect.Height();
	const TUint8* srcPixelStart = aSrcBase + aSrcStride*aSrcRect.iTl.iY + aSrcRect.iTl.iX*op::SrcPixelBytes();
	TUint8* dstPixelStart = aDataAddress + aDstStride*aDstPos.iY + aDstPos.iX*op::DestPixelBytes();
	TInt yPos=0;
	const TInt maskWidth=aMaskSize.iWidth;
	const TInt maskHeight=aMaskSize.iHeight;
	const TInt maskStartOffset=aMaskSrcPos.iX%maskWidth;
	FOREVER
		{
		const TUint8* srcPixel=srcPixelStart;
		const TUint8* maskBase=aMaskBase + (aMaskStride*((aMaskSrcPos.iY+yPos)%maskHeight));
		const TUint8* mskPixel=maskBase + maskStartOffset;
		const TUint8* mskEnd=maskBase + maskWidth;
		if (dstWidth<(mskEnd-mskPixel))
			mskEnd=mskPixel+dstWidth;
		TUint8* dstPixel=dstPixelStart;
		const TUint8* runStart=mskPixel;
		TInt toGo=dstWidth;
		FOREVER
			{
// Into skip loop first, assume start of scanline more likely to be masked out than set
			TInt runLen1=0;
skipPixels:
			while(mskPixel<mskEnd && *mskPixel==0)
				mskPixel++;
			const TInt runSubLen1=mskPixel-runStart;
			runLen1+=runSubLen1;
			toGo-=runSubLen1;
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo;
				goto skipPixels;
				}
			dstPixel+=runLen1*op::DestPixelBytes();
			srcPixel+=runLen1*op::SrcPixelBytes();
			if (toGo==0)
				break;
			runStart=mskPixel++;
			if (*runStart!=255)
				goto blendIt;
// Fall through to solid fill code
solidFill:
			{// bracketing to avoid gccxml compile errors
			TInt runLen2=0;
solidFill2:
			while(mskPixel<mskEnd && *mskPixel==0xFF)
				mskPixel++;
			{// bracketing to avoid gccxml compile errors
			const TInt runSubLen2=mskPixel-runStart;
			runLen2+=runSubLen2;
			toGo-=runSubLen2;
			}
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo;
				goto solidFill2;
				}
			if (runLen2)
				op::write(srcPixel, dstPixel, runLen2);
			}
			if (toGo==0)
				break;
			runStart=mskPixel++;
			if (*runStart==0)
				continue;
blendIt:
			while(mskPixel<mskEnd && *mskPixel!=0 && *mskPixel!=255)
				mskPixel++;
			const TInt runSubLen3=mskPixel-runStart;
			if (runSubLen3)
				{
				toGo-=runSubLen3;
				op::writeAlphaMask(srcPixel,dstPixel,runStart,runSubLen3);
				}
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo;
				goto blendIt;
				}
			if (toGo==0)
				break;
			runStart=mskPixel++;
			if (*runStart==255)
				goto solidFill;
			}
		if (++yPos==dstHeight)
			break;
		srcPixelStart+=aSrcStride;
		dstPixelStart+=aDstStride;
		}
	}

template <class op>
static void UnscaledFastBlitMaskedRotG256(const TUint8* aSrcBase, CFbsDrawDevice::TOrientation aOrientation, 
				TInt aSrcStride, TRect& aSrcRect, const TUint8* aMaskBase, TInt aMaskStride,
				TUint8 *aDataAddress, TUint32 aDstStride, const TPoint& aDstPos, const TSize &aDestSize,
				const TPoint& aMaskSrcPos, const TSize &aMaskSize)
	{
	TPoint dstPos(aDstPos);
	TInt srcStep=op::SrcPixelBytes();
	TInt maskStep=1;
	TRect srcRect(aSrcRect);
	TSize maskSize(aMaskSize);
	TPoint maskSrcPos(aMaskSrcPos);
	switch(aOrientation)
		{
	case CFbsDrawDevice::EOrientationRotated90:
		srcStep=-aSrcStride;
		aSrcStride=op::SrcPixelBytes();
		maskStep=-aMaskStride;
		aMaskStride=1;
		dstPos.iX=aDestSize.iWidth-aDstPos.iY-aSrcRect.Height();
		dstPos.iY=aDstPos.iX;
		srcRect.SetRect(aSrcRect.iTl.iY,aSrcRect.iTl.iX,aSrcRect.iBr.iY,aSrcRect.iBr.iX);
		maskSize.SetSize(aMaskSize.iHeight,aMaskSize.iWidth);
		maskSrcPos.iX=aMaskSrcPos.iY;
		maskSrcPos.iY=aMaskSrcPos.iX;
		break;
	case CFbsDrawDevice::EOrientationRotated180:
		srcStep=-op::SrcPixelBytes();
		maskStep=-1;
		aSrcStride=-aSrcStride;
		aMaskStride=-aMaskStride;
		dstPos.iX=aDestSize.iWidth-aDstPos.iX-aSrcRect.Width();
		dstPos.iY=aDestSize.iHeight-aDstPos.iY-aSrcRect.Height();
		break;
	case CFbsDrawDevice::EOrientationRotated270:
		srcStep=aSrcStride;
		aSrcStride=-op::SrcPixelBytes();
		maskStep=aMaskStride;
		aMaskStride=-1;
		dstPos.iX=aDstPos.iY;
		dstPos.iY=aDestSize.iHeight-aDstPos.iX-aSrcRect.Width();
		srcRect.SetRect(aSrcRect.iTl.iY,aSrcRect.iTl.iX,aSrcRect.iBr.iY,aSrcRect.iBr.iX);
		maskSrcPos.iX=aMaskSrcPos.iY;
		maskSrcPos.iY=aMaskSrcPos.iX;
		maskSize.SetSize(aMaskSize.iHeight,aMaskSize.iWidth);
		break;
		}
	const TUint8* srcPixelStart = aSrcBase;
	if (srcStep>0)
		srcPixelStart+=srcRect.iTl.iX*srcStep;
	else
		srcPixelStart-=(srcRect.iBr.iX-1)*srcStep;
	if (aSrcStride>0)
		srcPixelStart+=srcRect.iTl.iY*aSrcStride;
	else
		srcPixelStart-=(srcRect.iBr.iY-1)*aSrcStride;
//
	const TInt dstWidth = srcRect.Width();
	const TInt dstHeight = srcRect.Height();
	TUint8* dstPixelStart = aDataAddress + aDstStride*dstPos.iY + dstPos.iX*op::DestPixelBytes();
	TInt yPos=0;
	const TInt maskWidth=dstWidth<maskSize.iWidth?dstWidth:maskSize.iWidth;
	const TInt maskHeight=maskSize.iHeight;
	const TInt maskStartOffset=(maskStep>0?maskSrcPos.iX:srcRect.iBr.iX-1)%maskSize.iWidth;
	FOREVER
		{
		const TUint8* srcPixel=srcPixelStart;
		const TUint8* maskBase=aMaskBase;
		TInt maskYpos=(maskSrcPos.iY+yPos)%maskHeight;
		if (aMaskStride>0)
			maskBase+=maskYpos*aMaskStride;
		else
			{
			TInt maskEndPos=(maskSrcPos.iY+dstHeight-1-yPos)%maskHeight;
			maskBase-=maskEndPos*aMaskStride;
			}
		const TUint8* mskPixel=maskBase;
		const TUint8* mskEnd=maskBase;
		if (maskStep>0)
			{
			mskPixel+=maskStartOffset*maskStep;
			mskEnd+=maskSize.iWidth*maskStep;
			}
		else
			{
			maskBase-=(maskSize.iWidth-1)*maskStep;
			mskPixel-=maskStartOffset*maskStep;
			mskEnd+=maskStep;
			}
		const TInt maskToGo=(mskEnd-mskPixel)/maskStep;
		if (maskToGo>dstWidth)
			mskEnd=mskPixel+dstWidth*maskStep;
		TUint8* dstPixel=dstPixelStart;
		const TUint8* runStart=mskPixel;
		TInt toGo=dstWidth;
		FOREVER
			{
// Into skip loop first, assume start of scanline more likely to be masked out than set
			TInt runLen1=0;
skipPixels:
			while(mskPixel!=mskEnd && *mskPixel==0)
				mskPixel+=maskStep;
			const TInt runSubLen1=(mskPixel-runStart)/maskStep;
			runLen1+=runSubLen1;
			toGo-=runSubLen1;
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo*maskStep;
				goto skipPixels;
				}
			dstPixel+=runLen1*op::DestPixelBytes();
			srcPixel+=runLen1*srcStep;
			if (toGo==0)
				break;
			runStart=mskPixel;
			mskPixel+=maskStep;
			if (*runStart!=255)
				goto blendIt;
// Fall through to solid fill code
solidFill:
			{// bracketing to avoid gccxml compile errors
			TInt runLen2=0;
solidFill2:
			while(mskPixel!=mskEnd && *mskPixel==0xFF)
				mskPixel+=maskStep;
			{// bracketing to avoid gccxml compile errors
			const TInt runSubLen2=(mskPixel-runStart)/maskStep;
			runLen2+=runSubLen2;
			toGo-=runSubLen2;
			}
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo*maskStep;
				goto solidFill2;
				}
			if (runLen2)
				{
				op::write2rot(srcPixel,dstPixel,runLen2,srcStep, op::DestPixelBytes());
				srcPixel+=runLen2*srcStep;
				dstPixel+=runLen2*op::DestPixelBytes();
				}
			}
			if (toGo==0)
				break;
			runStart=mskPixel;
			mskPixel+=maskStep;
			if (*runStart==0)
				continue;
blendIt:
			while(mskPixel!=mskEnd && *mskPixel!=0 && *mskPixel!=255)
				mskPixel+=maskStep;
			const TInt runSubLen3=(mskPixel-runStart)/maskStep;
			if (runSubLen3)
				{
				toGo-=runSubLen3;
				op::writeAlphaMaskRot(srcPixel,dstPixel,runStart,runSubLen3,srcStep,maskStep);
				}
			if (mskPixel==mskEnd && toGo!=0)
				{
				mskPixel=maskBase;
				runStart=maskBase;
				if (toGo<maskWidth)
					mskEnd=mskPixel+toGo*maskStep;
				goto blendIt;
				}
			if (toGo==0)
				break;
			runStart=mskPixel;
			mskPixel+=maskStep;
			if (*runStart==255)
				goto solidFill;
			}
		if (++yPos==dstHeight)
			break;
		srcPixelStart+=aSrcStride;
		dstPixelStart+=aDstStride;
		}
	}

void CDrawBitmap::GetBlendPosAndRect(TRect &aSrcRect, const TRect &aSrcRectIn, const TSize &aSrcSize, const TPoint &aDestOffset)
	{
	aSrcRect.iTl=aSrcRectIn.iTl+aDestOffset;
	aSrcRect.iBr=aSrcRectIn.iBr;
// Wrap source top left to within source bitmap
	if (aSrcRect.iTl.iX<0)
		{
		TInt negOffset=1-(aSrcRect.iTl.iX+1)/aSrcSize.iWidth;
		aSrcRect.Move(negOffset*aSrcSize.iWidth,0);
		}
	else if (aSrcRect.iTl.iX>=aSrcSize.iWidth)
		aSrcRect.Move(-(aSrcRect.iTl.iX/aSrcSize.iWidth)*aSrcSize.iWidth,0);
	if (aSrcRect.iTl.iY<0)
		{
		TInt negOffset=1-(aSrcRect.iTl.iY+1)/aSrcSize.iHeight;
		aSrcRect.Move(0,negOffset*aSrcSize.iHeight);
		}
	else if (aSrcRect.iTl.iY>=aSrcSize.iHeight)
		aSrcRect.Move(0,-(aSrcRect.iTl.iY/aSrcSize.iHeight)*aSrcSize.iHeight);
	if (aSrcRect.iBr.iY>aSrcSize.iHeight)
		aSrcRect.iBr.iY=aSrcSize.iHeight;
	if (aSrcRect.iBr.iX>aSrcSize.iWidth)
		aSrcRect.iBr.iX=aSrcSize.iWidth;
	}

void CDrawBitmap::GetBlendPosAndRect(TRect &aDstRect, TRect &aSrcRect, const TRect &aDstRectIn, const TRect &aSrcRectIn, const TSize &aSrcSize)
	{
	aDstRect=aDstRectIn;
	aSrcRect=aSrcRectIn;
	if (aSrcRect.iTl.iX<0)
		aSrcRect.iTl.iX=0;
	if (aSrcRect.iTl.iY<0)
		aSrcRect.iTl.iY=0;
	TInt extraWidth=aSrcRect.iBr.iX-aSrcSize.iWidth;
	if (extraWidth>0)
		aSrcRect.iBr.iX-=extraWidth;
	TInt extraHeight=aSrcRect.iBr.iY-aSrcSize.iHeight;
	if (extraHeight>0)
		aSrcRect.iBr.iY-=extraHeight;
	}

TInt CDrawBitmap::FastBlendBitmapMaskedScaled(const TRect &aClipRect, const TRect& aDest,
							const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcLinePitch,
							TDisplayMode aSrcDisplayMode, const TSize &aSrcSize,
							const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize,TBool aInvertMask,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)
	{
	if (!FastBlendSupported(aSrcDisplayMode, aDrawMode, aShadowMode, aSrcLinePitch) ||
		!FastBlendMaskSupported(aMaskDisplayMode, aMaskStride) ||
    	iOrientation!=EOrientationNormal)
		return(KErrNotSupported);
	TRect srcRect;
	TRect dstRect;
	GetBlendPosAndRect(dstRect,srcRect,aDest,aSrcRect,aSrcSize);
	const TInt KDestModeShift=8;
	const TInt KMaskModeShift=16;
#define MASKED_MODE_SWITCH(src,dest,mask) case src|(dest<<KDestModeShift)|(mask<<KMaskModeShift)
#define MODE_SWITCH(src,dest) case src|(dest<<KDestModeShift)
	TInt switchValue=aSrcDisplayMode|(iDispMode<<KDestModeShift)|(aMaskDisplayMode<<KMaskModeShift);
	if (srcRect.iBr.iX>aMaskSize.iWidth || srcRect.iBr.iY>aMaskSize.iHeight)
		return(KErrNotSupported);
	TUint8 *dstBits=reinterpret_cast<TUint8*>(iBits);
	TInt dstStride=iScanLineWords*4;
	const TUint8* srcBase=reinterpret_cast<const TUint8*>(aSrcBase);
	const TUint8 *maskBits=reinterpret_cast<const TUint8*>(aMaskBase);
	if (!dstRect.IsEmpty() && !srcRect.IsEmpty())
		{
		switch (switchValue)
			{
// 16MA source
			MASKED_MODE_SWITCH(EColor16MA,EColor16MU,EGray2):
				ScaledFastBlitMaskedG2<TMaToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MAP,EGray2):
				ScaledFastBlitMaskedG2<TMaToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor64K,EGray2):
				ScaledFastBlitMaskedG2<TMaTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MU,EGray256):
				ScaledFastBlitMaskedG256<TMaToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MAP,EGray256):
				ScaledFastBlitMaskedG256<TMaToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor64K,EGray256):
				ScaledFastBlitMaskedG256<TMaTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
	// 16MU source
			MASKED_MODE_SWITCH(EColor16MU,EColor16MU,EGray2):
				ScaledFastBlitMaskedG2<TMuToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MAP,EGray2):
				ScaledFastBlitMaskedG2<TMuToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor64K,EGray2):
				ScaledFastBlitMaskedG2<TMuTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16M,EColor64K,EGray2):
				ScaledFastBlitMaskedG2<TMTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MU,EGray256):
				ScaledFastBlitMaskedG256<TMuToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MAP,EGray256):
				ScaledFastBlitMaskedG256<TMuToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor64K,EGray256):
				ScaledFastBlitMaskedG256<TMuTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16M,EColor64K,EGray256):
				ScaledFastBlitMaskedG256<TMTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
	// 16MAP source
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MU,EGray2):
				ScaledFastBlitMaskedG2<TMapToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MAP,EGray2):
				ScaledFastBlitMaskedG2<TMapToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor64K,EGray2):
				ScaledFastBlitMaskedG2<TMapTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MU,EGray256):
				ScaledFastBlitMaskedG256<TMapToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MAP,EGray256):
				ScaledFastBlitMaskedG256<TMapToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor64K,EGray256):
				ScaledFastBlitMaskedG256<TMapTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
	// 64K source
			MASKED_MODE_SWITCH(EColor64K,EColor16MU,EGray2):
				ScaledFastBlitMaskedG2<T64KToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MAP,EGray2):
				ScaledFastBlitMaskedG2<T64KToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor64K,EGray2):
				ScaledFastBlitMaskedG2<T64KTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MU,EGray256):
				ScaledFastBlitMaskedG256<T64KToMu>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MAP,EGray256):
				ScaledFastBlitMaskedG256<T64KToMap>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor64K,EGray256):
				ScaledFastBlitMaskedG256<T64KTo64K>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
#if defined(__SUPPORT_16MA_TARGET__)
			MASKED_MODE_SWITCH(EColor16MA,EColor16MA,EGray2):
				ScaledFastBlitMaskedG2<TMaToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MA,EGray256):
				ScaledFastBlitMaskedG256<TMaToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MA,EGray2):
				ScaledFastBlitMaskedG2<TMuToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MA,EGray256):
				ScaledFastBlitMaskedG256<TMuToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MA,EGray2):
				ScaledFastBlitMaskedG2<TMapToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MA,EGray256):
				ScaledFastBlitMaskedG256<TMapToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MA,EGray2):
				ScaledFastBlitMaskedG2<T64KToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, dstRect, aClipRect);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MA,EGray256):
				ScaledFastBlitMaskedG256<T64KToMa>(srcBase, aSrcLinePitch, srcRect, maskBits, aMaskStride, dstBits, dstStride, dstRect, aClipRect);
				break;
#endif
			default:
				return KErrNotSupported;
			}
		}
	return(KErrNone);
	}

TInt CDrawBitmap::FastBlendBitmapScaled(const TRect &aClipRect, const TRect& aDest, const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcLinePitch, TDisplayMode aSrcDisplayMode, const TSize &aSrcSize, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)
	{
	if (!FastBlendSupported(aSrcDisplayMode, aDrawMode, aShadowMode, aSrcLinePitch))
		return(KErrNotSupported);
	TRect srcRect;
	TRect dstRect;
	GetBlendPosAndRect(dstRect,srcRect,aDest,aSrcRect,aSrcSize);
	const TInt KDestModeShift=8;
#define MODE_SWITCH(src,dest) case src|(dest<<KDestModeShift)
	TInt switchValue=aSrcDisplayMode|(iDispMode<<KDestModeShift);
	TUint8 *dstBits=reinterpret_cast<TUint8*>(iBits);
	TInt dstStride=iScanLineWords*4;
	const TUint8* srcBase=reinterpret_cast<const TUint8*>(aSrcBase);
	if (!dstRect.IsEmpty() && !srcRect.IsEmpty())
		{
		switch (switchValue)
			{
// 16MA Source
			MODE_SWITCH(EColor16MA,EColor16MU):
				ScaledFastBlit<TMaToMu>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MA,EColor16MAP):
				ScaledFastBlit<TMaToMap>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MA,EColor64K):
				ScaledFastBlit<TMaTo64K>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
// 16MAP Source
			MODE_SWITCH(EColor16MAP,EColor16MU):
				ScaledFastBlit<TMapToMu>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MAP,EColor16MAP):
				ScaledFastBlit<TMapToMap>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MAP,EColor64K):
				ScaledFastBlit<TMapTo64K>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
// 16MU Source
			MODE_SWITCH(EColor16MU,EColor16MU):
				ScaledFastBlit<TMuToMu>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MU,EColor64K):
				ScaledFastBlit<TMuTo64K>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			// No EColor16MU to EColor16MAP fast blit performed because it
			// cannot be guaranteed that the alpha channel of the EColor16MU
			// source is 0xFF, which any fast blit would require. 
// 64K Source
			MODE_SWITCH(EColor64K,EColor16MU):
				ScaledFastBlit<T64KToMu>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor64K,EColor16MAP):
				ScaledFastBlit<T64KToMap>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor64K,EColor64K):
				ScaledFastBlit<T64KTo64K>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
// 16M source
			MODE_SWITCH(EColor16M,EColor64K):
				ScaledFastBlit<TMTo64K>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
#if defined(__SUPPORT_16MA_TARGET__)
			MODE_SWITCH(EColor16MA,EColor16MA):
				ScaledFastBlit<TMaToMa>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor16MAP,EColor16MA):
				ScaledFastBlit<TMapToMa>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			MODE_SWITCH(EColor64K,EColor16MA):
				ScaledFastBlit<T64KToMa>(srcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstRect, aClipRect);
				break;
			// No EColor16MU to EColor16MA fast blit performed because it
			// cannot be guaranteed that the alpha channel of the EColor16MU
			// source is 0xFF, which any fast blit would require. 
#endif
			default:
				return KErrNotSupported;
			}
		}
	return KErrNone;
	}

TInt CDrawBitmap::DoFastBlendBitmap(const TPoint &aDest, const TRect& aSrcRect, const TUint8 *aSrcBase, TInt aSrcLinePitch, TDisplayMode aSrcDisplayMode, const TSize &aSrcSize)
	{
	const TInt KDestModeShift=8;
	TInt dstStride=iScanLineWords*4;
	TUint8 *dstBits=(TUint8*)iBits;
#define MODE_SWITCH(src,dest) case src|(dest<<KDestModeShift)
	TInt switchValue=aSrcDisplayMode|(iDispMode<<KDestModeShift);
	TInt xEnd=aDest.iX+aSrcRect.Width();
	TInt yEnd=aDest.iY+aSrcRect.Height();
	TPoint dstPos;
	dstPos.iY=aDest.iY;
	while(dstPos.iY<yEnd)
		{
		TRect srcRect;
		dstPos.iX=aDest.iX;
		while(dstPos.iX<xEnd)
			{
// Clip source rect to within source bitmap size
			GetBlendPosAndRect(srcRect,aSrcRect,aSrcSize,dstPos-aDest);
			if (iOrientation==EOrientationNormal)
				{
				switch (switchValue)
					{
		// 16MA Source
				MODE_SWITCH(EColor16MA,EColor16MU):
					UnscaledFastBlit<TMaToMu>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MA,EColor16MAP):
					UnscaledFastBlit<TMaToMap>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MA,EColor64K):
					UnscaledFastBlit<TMaTo64K>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
		// 16MAP Source
				MODE_SWITCH(EColor16MAP,EColor16MU):
					UnscaledFastBlit<TMapToMu>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MAP,EColor16MAP):
					UnscaledFastBlit<TMapToMap>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MAP,EColor64K):
					UnscaledFastBlit<TMapTo64K>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
		// 16MU Source
				MODE_SWITCH(EColor16MU,EColor16MU):
					ReplaceBlit(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MU,EColor64K):
					UnscaledFastBlit<TMuTo64K>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				// No EColor16MU to EColor16MAP fast blit performed because it
				// cannot be guaranteed that the alpha channel of the EColor16MU
				// source is 0xFF, which any fast blit would require. 
		// 64K Source
				MODE_SWITCH(EColor64K,EColor16MU):
					UnscaledFastBlit<T64KToMu>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor64K,EColor16MAP):
					UnscaledFastBlit<T64KToMap>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor64K,EColor64K):
					UnscaledFastBlit<T64KTo64K>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
		// 16M Source
				MODE_SWITCH(EColor16M,EColor64K):
					UnscaledFastBlit<TMTo64K>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
#if defined(__SUPPORT_16MA_TARGET__)
				MODE_SWITCH(EColor16MA,EColor16MA):
					UnscaledFastBlit<TMaToMa>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor16MAP,EColor16MA):
					UnscaledFastBlit<TMapToMa>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				MODE_SWITCH(EColor64K,EColor16MA):
					UnscaledFastBlit<T64KToMa>(aSrcBase, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos);
					break;
				// No EColor16MU to EColor16MA fast blit performed because it
				// cannot be guaranteed that the alpha channel of the EColor16MU
				// source is 0xFF, which any fast blit would require. 
#endif
				default:
					return KErrNotSupported;
					}
				}
			else
				{
				switch (switchValue)
					{
		// 16MA Source
				MODE_SWITCH(EColor16MA,EColor16MU):
					UnscaledFastBlitRot<TMaToMu>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
				MODE_SWITCH(EColor16MA,EColor16MAP):
					UnscaledFastBlitRot<TMaToMap>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
		// 16MAP Source
				MODE_SWITCH(EColor16MAP,EColor16MU):
					UnscaledFastBlitRot<TMapToMu>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
				MODE_SWITCH(EColor16MAP,EColor16MAP):
					UnscaledFastBlitRot<TMapToMap>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
		// 16MU Source
				MODE_SWITCH(EColor16MU,EColor16MU):
					UnscaledFastBlitRot<TMuToMu>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
				MODE_SWITCH(EColor16MU,EColor16MAP):
					UnscaledFastBlitRot<TMuToMap>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
		// 64K Source
				MODE_SWITCH(EColor64K,EColor64K):
					UnscaledFastBlitRot<T64KTo64K>(aSrcBase, iOrientation, aSrcLinePitch, srcRect, dstBits, dstStride, dstPos, iSize, iScanLineBuffer);
					break;
				default:
					return KErrNotSupported;
					}
				}
			dstPos.iX+=srcRect.Width();
			}
		dstPos.iY+=srcRect.Height();
		}
	return KErrNone;
	}

TBool CDrawBitmap::FastBlendSupported(TDisplayMode aSrcDisplayMode, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode, TInt aSrcLinePitch)
    {
    // FastBlend supports all combinations of EColor16Mxx and EColor64K, and when User Display Mode is ENone.
    if ((iUserDispMode != EColor64K && iUserDispMode != EColor16M && 
    	 iUserDispMode != EColor16MU && iUserDispMode != EColor16MA && 
   	     iUserDispMode != EColor16MAP && iUserDispMode != ENone))
    	{
    	return (EFalse);
    	}
    if (!IsScalingOff() ||
        !iOriginIsZero)
        {
		return(EFalse);
        }
	return((aDrawMode==CGraphicsContext::EDrawModePEN || (aDrawMode==CGraphicsContext::EDrawModeWriteAlpha && !IsAlphaChannel(aSrcDisplayMode))) &&
		aShadowMode==CFbsDrawDevice::ENoShadow && aSrcLinePitch>0);
	}

TBool CDrawBitmap::FastBlendMaskSupported(TDisplayMode aMaskDisplayMode, TInt aMaskStride)
	{
	return((aMaskDisplayMode==EGray2 || aMaskDisplayMode==EGray256) && aMaskStride>0);
	}

/**
CDrawBitmap::BlendBitmap() implementation.
@internalTechnology
*/
TInt CDrawBitmap::FastBlendBitmap(const TPoint& aDest, CFbsDrawDevice* aSrcDrawDevice, const TRect& aSrcRect, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)
	{
	TInt srcPitch=aSrcDrawDevice->ScanLineBytes();
	if (!FastBlendSupported(aSrcDrawDevice->DisplayMode(), aDrawMode, aShadowMode, srcPitch))
		return(KErrNotSupported);
	MScalingSettings* scalingSettings=NULL;
	if (aSrcDrawDevice->GetInterface(KScalingSettingsInterfaceID, 
							reinterpret_cast<TAny*&>(scalingSettings))==KErrNone)
		{
		if (!scalingSettings->IsScalingOff())
			return(KErrNotSupported);
		}
	MDrawDeviceOrigin* originInterface = NULL;
	if(aSrcDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, 
							reinterpret_cast <TAny*&> (originInterface)) == KErrNone)
		{
		TPoint origin;
		originInterface->Get(origin);
		if (origin.iX!=0 || origin.iY!=0)
			return(KErrNotSupported);
		}
	MDrawDeviceOrientation* orientationInterface=NULL;
	if(aSrcDrawDevice->GetInterface(KOrientationInterfaceID,
							reinterpret_cast <TAny*&> (orientationInterface))!=KErrNone || (orientationInterface && orientationInterface->Orientation() != 0))
		{
		return KErrNotSupported;
		}
	TAny* interface=NULL;
	TInt ret=aSrcDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
	if (ret!=KErrNone)
		return(ret);
	MFastBlit2 *fb2=reinterpret_cast<MFastBlit2*>(interface);
	const TUint8* srcBase = reinterpret_cast<const TUint8*>(fb2->Bits());
	return(DoFastBlendBitmap(aDest, aSrcRect, srcBase, srcPitch, aSrcDrawDevice->DisplayMode(), aSrcDrawDevice->SizeInPixels()));
	}

TInt CDrawBitmap::FastBlendBitmap(const TPoint& aDest, const TUint32* aSrcBase, TInt aSrcStride, const TSize& aSrcSize, const TRect& aSrcRect, TDisplayMode aDisplayMode, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)
	{
	if (!FastBlendSupported(aDisplayMode, aDrawMode, aShadowMode, aSrcStride))
		return(KErrNotSupported);
	return(DoFastBlendBitmap(aDest, aSrcRect, reinterpret_cast<const TUint8*>(aSrcBase), aSrcStride, aDisplayMode, aSrcSize));
	}

TInt CDrawBitmap::FastBlendBitmapMasked(const TPoint& aDest, const TUint32* aSrcBase, TInt aSrcStride, const TSize& aSrcSize, const TRect& aSrcRect, TDisplayMode aSrcDisplayMode, const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize, const TPoint &aMaskSrcPos, TBool aInvertMask, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode)
	{
	if (!FastBlendSupported(aSrcDisplayMode, aDrawMode, aShadowMode, aSrcStride) ||
		!FastBlendMaskSupported(aMaskDisplayMode, aMaskStride))
		return(KErrNotSupported);
	TRect srcRect;
	GetBlendPosAndRect(srcRect,aSrcRect,aSrcSize,TPoint(0,0));
	const TInt KDestModeShift=8;
	const TInt KMaskModeShift=16;
	TInt dstStride=iScanLineWords*4;
#define MASKED_MODE_SWITCH(src,dest,mask) case src|(dest<<KDestModeShift)|(mask<<KMaskModeShift)
	TInt switchValue=aSrcDisplayMode|(iDispMode<<KDestModeShift);
	switchValue|=aMaskDisplayMode<<KMaskModeShift;
	const TUint8* srcBase=reinterpret_cast<const TUint8*>(aSrcBase);
	TUint8 *dstBits=(TUint8*)iBits;
	const TUint8 *maskBits=reinterpret_cast<const TUint8*>(aMaskBase);
	if (iOrientation==EOrientationNormal)
		{
		switch (switchValue)
			{
// 16MA source
			MASKED_MODE_SWITCH(EColor16MA,EColor16MU,EGray2):
				UnscaledFastBlitMaskedG2<TMaToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MAP,EGray2):
				UnscaledFastBlitMaskedG2<TMaToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor64K,EGray2):
				UnscaledFastBlitMaskedG2<TMaTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MU,EGray256):
				UnscaledFastBlitMaskedG256<TMaToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedG256<TMaToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor64K,EGray256):
				UnscaledFastBlitMaskedG256<TMaTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
// 16MU source
			MASKED_MODE_SWITCH(EColor16MU,EColor16MU,EGray2):
				UnscaledFastBlitMaskedG2<TMuToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MAP,EGray2):
				UnscaledFastBlitMaskedG2<TMuToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor64K,EGray2):
				UnscaledFastBlitMaskedG2<TMuTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16M,EColor64K,EGray2):
				UnscaledFastBlitMaskedG2<TMTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MU,EGray256):
				UnscaledFastBlitMaskedG256<TMuToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedG256<TMuToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor64K,EGray256):
				UnscaledFastBlitMaskedG256<TMuTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16M,EColor64K,EGray256):
				UnscaledFastBlitMaskedG256<TMTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
// 16MAP source
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MU,EGray2):
				UnscaledFastBlitMaskedG2<TMapToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MAP,EGray2):
				UnscaledFastBlitMaskedG2<TMapToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor64K,EGray2):
				UnscaledFastBlitMaskedG2<TMapTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MU,EGray256):
				UnscaledFastBlitMaskedG256<TMapToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedG256<TMapToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor64K,EGray256):
				UnscaledFastBlitMaskedG256<TMapTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
// 64K source
			MASKED_MODE_SWITCH(EColor64K,EColor16MU,EGray2):
				UnscaledFastBlitMaskedG2<T64KToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MAP,EGray2):
				UnscaledFastBlitMaskedG2<T64KToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor64K,EGray2):
				UnscaledFastBlitMaskedG2<T64KTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MU,EGray256):
				UnscaledFastBlitMaskedG256<T64KToMu>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedG256<T64KToMap>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor64K,EGray256):
				UnscaledFastBlitMaskedG256<T64KTo64K>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
#if defined(__SUPPORT_16MA_TARGET__)
			MASKED_MODE_SWITCH(EColor16MA,EColor16MA,EGray2):
				UnscaledFastBlitMaskedG2<TMaToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MA,EGray256):
				UnscaledFastBlitMaskedG256<TMaToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MA,EGray2):
				UnscaledFastBlitMaskedG2<TMuToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MA,EGray256):
				UnscaledFastBlitMaskedG256<TMuToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MA,EGray2):
				UnscaledFastBlitMaskedG2<TMapToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MA,EGray256):
				UnscaledFastBlitMaskedG256<TMapToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MA,EGray2):
				UnscaledFastBlitMaskedG2<T64KToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, aInvertMask, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor64K,EColor16MA,EGray256):
				UnscaledFastBlitMaskedG256<T64KToMa>(srcBase, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, aMaskSrcPos, aMaskSize);
				break;
#endif
			default:
				return KErrNotSupported;
			}
		}
	else
		{
		switch (switchValue)
			{
	// 16MA Source
			MASKED_MODE_SWITCH(EColor16MA,EColor16MU,EGray256):
				UnscaledFastBlitMaskedRotG256<TMaToMu>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MA,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedRotG256<TMaToMap>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
	// 16MAP Source
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MU,EGray256):
				UnscaledFastBlitMaskedRotG256<TMapToMu>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MAP,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedRotG256<TMapToMap>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
	// 16MU Source
			MASKED_MODE_SWITCH(EColor16MU,EColor16MU,EGray256):
				UnscaledFastBlitMaskedRotG256<TMuToMu>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
			MASKED_MODE_SWITCH(EColor16MU,EColor16MAP,EGray256):
				UnscaledFastBlitMaskedRotG256<TMuToMap>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
	// 64K Source
			MASKED_MODE_SWITCH(EColor64K,EColor64K,EGray256):
				UnscaledFastBlitMaskedRotG256<T64KTo64K>(srcBase, iOrientation, aSrcStride, srcRect, maskBits, aMaskStride, dstBits, dstStride, aDest, iSize, aMaskSrcPos, aMaskSize);
				break;
			default:
				return KErrNotSupported;
			}
		}
	return KErrNone;
	}
