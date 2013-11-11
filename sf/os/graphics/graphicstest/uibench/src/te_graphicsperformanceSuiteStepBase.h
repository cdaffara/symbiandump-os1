// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TE_GRAPHICSPERFORMANCE_STEP_BASE__
#define __TE_GRAPHICSPERFORMANCE_STEP_BASE__

#include <e32math.h>
#include <bitdev.h>
#include <test/tprofiler.h>
#include <test/testexecutestepbase.h>
#include <test/ttmsgraphicsstep.h>
#include "te_graphicsperformanceSuiteDefs.h"
#include "tdisplaymode_mapping.h"

TSize GetPixmapSizeInPixels();

// Define TRgb constants to be used in test cases
#define TRANSPARENT_BLACK TRgb(0,0,0,0)
#define BLACK_SEMI_TRANSPARENT TRgb(0,0,0,0x40)
#define RED_SEMI_TRANSPARENT TRgb(0xff,0,0,0x40)
#define GREEN_SEMI_TRANSPARENT TRgb(0,0xff,0,0x40)
#define BLUE_SEMI_TRANSPARENT TRgb(0,0,0xff,0x40)
#define YELLOW_SEMI_TRANSPARENT TRgb(0xff,0xff,0,0x40)

/**
Creates a virtual bitmap device if a real screen device of the specified display mode is not supported by the hardware
*/
class CVirtualBitmapDevice : public CBase
	{
public:
	virtual ~CVirtualBitmapDevice();
	static CVirtualBitmapDevice* NewL(TDisplayMode aDisplayMode, TBool aForceOffscreen);
	void Update();
	CBitmapDevice& BitmapDevice();
	CFbsBitmap& Bitmap();
	virtual int CreateContext(CGraphicsContext *&aGc);
	virtual TSize SizeInPixels() const;
	TInt isScreenDevice();
private:
	void ConstructL(TDisplayMode aDisplayMode, TBool aForceOffscreen);
	
private:
	CBitmapDevice* iBitmapDevice;	// The virtual or real screen device
	CFbsBitmap* iBitmap;			// Virtual bitmap
	TBool iIsScreenDevice; 			// ETrue if the hardware supports the screen device at the specified display mode	
	};
	
/**
Creates a virtual bitmap device if a real screen device of the specified display mode is not supported by the hardware.
It is similar to CVirtualBitmapDevice but it uses CFbsDrawDevice directly.
*/
class CVirtualDrawDevice : public CBase
	{
public:
	virtual ~CVirtualDrawDevice();
	static CVirtualDrawDevice* NewL(TDisplayMode aDisplayMode);
	CFbsDrawDevice& DrawDevice();
	TBool IsDrawDevice();
private:
	void ConstructL(TDisplayMode aDisplayMode);
	TInt ByteSize(const TSize& aSize, TDisplayMode aDisplayMode);
	
private:
	CFbsDrawDevice* iDrawDevice;	// The virtual or real screen device
	TUint8* iDeviceMemory;			// Used for virtual draw device
	TSize	iSize;					// Used for Clear
	TBool iIsDrawDevice;		    // ETrue if the hardware supports the screen device at the specified display mode	
	};

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_graphicsperformanceSuiteStepBase : public CTTMSGraphicsStep
	{
public:
	static const TDesC& ColorModeName(TDisplayMode aMode);
	virtual ~CTe_graphicsperformanceSuiteStepBase();
	CTe_graphicsperformanceSuiteStepBase();
	CVirtualBitmapDevice* ScreenDevice();

	void SetScreenModeL(TDisplayMode aScreenMode, TBool aForceUseOffscreen=EFalse);
	CFbsBitmap* CreateSoftwareBitmapLC(const TSize& aSize, TDisplayMode aMode);
	void CopyBitmapL(CFbsBitmap* aDst, CFbsBitmap* aSrc);
	CFbsBitmap* CopyIntoNewBitmapL(CFbsBitmap* aSrc, TDisplayMode aDisplayMode);
	CFbsBitmap* LoadBitmapL(const TDesC& aName, TInt aIndex);
	static TRgb InterpolateColour(TRgb aLo, TRgb aHi, TInt aX, TInt aN);
	static void VerticalGradientAlphaL(CFbsBitmap* aBitmap, TRgb aLo, TRgb aHi);
	void SetDrawDeviceModeL(TDisplayMode aScreenMode);
	TInt GetDrawDeviceInterfaceL(TInt aInterfaceId, TAny *&aInterface);
	void ClearDrawDeviceL(TRgb aColor);
	CFbsBitmap* CreateCheckedBoardL(TDisplayMode aDisplayMode, TSize aSize, TSize aChecksPerAxis) const;
	virtual CFbsBitmap* GetTargetAsBitmapL();
	TInt WriteTargetOutput(TPtrC aName);
	static void ExtractListL(TPtrC aList, RArray<TPtrC>& aListItems);

protected:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();	
	TUint32 TimeMax();
	TUint32 TimeMin();
	
private:
	//TBool IsFoundScreenMode(TDisplayMode aScreenMode);

protected:
	CVirtualBitmapDevice * iScreenDevice;	
	CFbsBitGc* iGc;
	CVirtualDrawDevice* iDrawDevice;
    CTProfiler*         iProfiler;
		
private:
	TSize 				iScreenSize;
	TInt 				iDoProfiling;
	TInt				iCPUSpeed;
	CActiveScheduler* 	iScheduler;
	
	// Sanity checking flag as read in from the ini file. If ETrue, WriteTargetOutput() will generate
	// a bitmap of the target, otherwise it will perform no operation.
	TBool iShowBitmaps;
	__PROFILERMEMBERS
	};

	// Define 16 RGB colors
#define KRgbBlack		TRgb(0x000000)
#define KRgbDarkGray	TRgb(0x555555)
#define KRgbDarkRed		TRgb(0x000080)
#define KRgbDarkGreen	TRgb(0x008000)
#define KRgbDarkYellow	TRgb(0x008080)
#define KRgbDarkBlue	TRgb(0x800000)
#define KRgbDarkMagenta	TRgb(0x800080)
#define KRgbDarkCyan	TRgb(0x808000)
#define KRgbRed			TRgb(0x0000ff)
#define KRgbGreen		TRgb(0x00ff00)
#define KRgbYellow		TRgb(0x00ffff)
#define KRgbBlue		TRgb(0xff0000)
#define KRgbMagenta		TRgb(0xff00ff)
#define KRgbCyan		TRgb(0xffff00)
#define KRgbGray		TRgb(0xaaaaaa)
#define KRgbWhite		TRgb(0xffffff)

// A 16 RGB color table
const TRgb KColor16Table[] = 
	{		
	KRgbWhite,		
	KRgbGray,		
	KRgbCyan,		
	KRgbMagenta,		
	KRgbBlue,		
	KRgbYellow,		
	KRgbGreen,		
	KRgbRed,			
	KRgbDarkCyan,	
	KRgbDarkMagenta,	
	KRgbDarkBlue,	
	KRgbDarkYellow,	
	KRgbDarkGreen,	
	KRgbDarkRed,	
	KRgbDarkGray,	
	KRgbBlack,					
	};

#endif
