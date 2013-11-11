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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDIRECTGDI_TEST_STEP_BASE__
#define __TDIRECTGDI_TEST_STEP_BASE__

#include "tdisplaymode_mapping.h"
#include "tcontextbase.h"
#include <test/testexecutestepbase.h>
#include <bitdev.h>
#include <graphics/sgimage.h>
#include <graphics/directgdiimagetarget.h>
#include <graphics/directgdidrawablesource.h>
#include <graphics/directgdidriver.h>
#include <graphics/directgdiextensioninterfaces.h>
#include <test/ttmsgraphicsstep.h>

/**
Default size of the off screen image in pixels
*/
const TSize KDefaultOffScreenImageSizeInPixels(200, 200);
const TInt KTestCaseLength = 100;

_LIT(KCom, "Compressed");
_LIT(KSeparator, "_");
_LIT(KDirectGc, "DirectGdi");
_LIT(KBitGc, "BitGdi");
_LIT(KTargetString, "TGT");
_LIT(KSourceString, "SRC");

const TInt KFileNameLength = 200;
const TInt KPixelFormatNameLength = 40;

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

_LIT(KColorNameWhite,"White");
_LIT(KColorNameGray,"Gray");
_LIT(KColorNameCyan,"Cyan");
_LIT(KColorNameMagenta,"Magenta");
_LIT(KColorNameBlue,"Blue");
_LIT(KColorNameYellow,"Yellow");
_LIT(KColorNameGreen,"Green");
_LIT(KColorNameRed,"Red");
_LIT(KColorNameDarkCyan,"DarkCyan");
_LIT(KColorNameDarkMagenta,"DarkMagenta");
_LIT(KColorNameDarkBlue,"DarkBlue");
_LIT(KColorNameDarkYellow,"DarkYellow");
_LIT(KColorNameDarkGreen,"DarkGreen");
_LIT(KColorNameDarkRed,"DarkRed");
_LIT(KColorNameDarkGrey,"DarkGrey");
_LIT(KColorNameBlack,"Black");

const TPtrC KColor16TableNames[] = 
	{
		KColorNameWhite(),
		KColorNameGray(),
		KColorNameCyan(),
		KColorNameMagenta(),
		KColorNameBlue(),
		KColorNameYellow(),
		KColorNameGreen(),
		KColorNameRed(),
		KColorNameDarkCyan(),
		KColorNameDarkMagenta(),
		KColorNameDarkBlue(),
		KColorNameDarkYellow(),
		KColorNameDarkGreen(),
		KColorNameDarkRed(),
		KColorNameDarkGrey(),
		KColorNameBlack(),
	};



const DirectGdi::TBrushStyle KBrushStyleTable[] = 
	{
	DirectGdi::ENullBrush,	
	DirectGdi::ESolidBrush,
	DirectGdi::EPatternedBrush,
	DirectGdi::EVerticalHatchBrush,
	DirectGdi::EForwardDiagonalHatchBrush,
	DirectGdi::EHorizontalHatchBrush,
	DirectGdi::ERearwardDiagonalHatchBrush,
	DirectGdi::ESquareCrossHatchBrush,
	DirectGdi::EDiamondCrossHatchBrush,
	};
const TInt KNumBrushStyles = sizeof(KBrushStyleTable) / sizeof(DirectGdi::TBrushStyle);	

_LIT(KBrushStyleNullBrush,"NullBrush");
_LIT(KBrushStyleSolidBrush,"SolidBrush");
_LIT(KBrushStylePatternedBrush,"PatternedBrush");
_LIT(KBrushStyleVerticalHatchBrush,"VerticalHatchBrush");
_LIT(KBrushStyleForwardDiagonalHatchBrush,"ForwardDiagonalHatchBrush");
_LIT(KBrushStyleHorizontalHatchBrush,"HorizontalHatchBrush");
_LIT(KBrushStyleRearwardDiagonalHatchBrush,"RearwardDiagonalHatchBrush");
_LIT(KBrushStyleSquareCrossHatchBrush,"SquareCrossHatchBrush");
_LIT(KBrushStyleDiamondCrossHatchBrush,"DiamondCrossHatchBrush");

const TPtrC KBrushStyleTableNames[] =
	{
		KBrushStyleNullBrush(),
		KBrushStyleSolidBrush(),
		KBrushStylePatternedBrush(),
		KBrushStyleVerticalHatchBrush(),
		KBrushStyleForwardDiagonalHatchBrush(),
		KBrushStyleHorizontalHatchBrush(),
		KBrushStyleRearwardDiagonalHatchBrush(),
		KBrushStyleSquareCrossHatchBrush(),
		KBrushStyleDiamondCrossHatchBrush(),
	};

const DirectGdi::TPenStyle KPenStyleTable[] =
	{
	DirectGdi::ENullPen,
	DirectGdi::ESolidPen,
	DirectGdi::EDottedPen,
	DirectGdi::EDashedPen,
	DirectGdi::EDotDashPen,
	DirectGdi::EDotDotDashPen,
	};

const DirectGdi::TTextAlign KTextAlignTable[] =
	{
	DirectGdi::ELeft,
	DirectGdi::ECenter,
	DirectGdi::ERight,
	};

_LIT(KTextAlignNameLeft,"Left");
_LIT(KTextAlignNameCenter,"Center");
_LIT(KTextAlignNameRight,"Right");

const TPtrC KTextAlignTableNames[] =
	{
		KTextAlignNameLeft(),
		KTextAlignNameCenter(),
		KTextAlignNameRight(),
	};

// Look-up table for pen-names.
_LIT(KPenNameNull,"NullPen");
_LIT(KPenNameSolid,"SolidPen");
_LIT(KPenNameDotted,"DottedPen");
_LIT(KPenNameDashed,"DashedPen");
_LIT(KPenNameDotDash,"DotDashPen");
_LIT(KPenNameDotDotDash,"DotDotDashPen");

const TPtrC KPenNames[] = 
	{
	KPenNameNull(),
	KPenNameSolid(),
	KPenNameDotted(),
	KPenNameDashed(),
	KPenNameDotDash(),
	KPenNameDotDotDash()
	};

// Look-up table for all available pen-styles.
const DirectGdi::TPenStyle KPenStyles[] = 
	{ 
	DirectGdi::ENullPen,
	DirectGdi::ESolidPen,
	DirectGdi::EDottedPen,
	DirectGdi::EDashedPen,
	DirectGdi::EDotDashPen,
	DirectGdi::EDotDotDashPen
	};
const TInt KNumPenStyles = sizeof(KPenStyles)/sizeof(DirectGdi::TPenStyle);

// Look-up table for fill rule names.
_LIT(KFillRuleAlternate,"Alternate");
_LIT(KFillRuleWinding,"Winding");

const TPtrC KFillRuleNames[] = 
	{
	KFillRuleAlternate(),
	KFillRuleWinding()
	};

/**
Possible use cases of contexts and targets. 
*/
enum TContextTestCase
	{
	EOneContextOneTarget,
	EOneContextTwoTargets_SamePixelType,
	EOneContextTwoTargets_DifferentPixelType,
	ETwoContextsOneTarget,
	ETwoContextsTwoTargets_WithoutSharing_SamePixelType,
	ETwoContextsTwoTargets_WithoutSharing_DifferentPixelType,
	ETwoContextsTwoTargets_WithSharing_SamePixelType,
	ETwoContextsTwoTargets_WithSharing_DifferentPixelType,
	EOneContextOneTarget_TwiceActivate,	
	ELast
	};

/**
Position iterator helper class.
Position iterator generates all possible positions from given start position to end position
with step. All combinations of horizontal and vertical positions are generated.
Additionaly x and y position index is iterated.
 */
class TPositionIterator
	{
public:
	TPositionIterator(TInt aStartX, TInt aEndX, TInt aStepX,
		TInt aStartY, TInt aEndY, TInt aStepY);
	void Begin();
	TBool Next();
public:
	TInt iStartX;
	TInt iEndX;
	TInt iStepX;
	TInt iStartY;
	TInt iEndY;
	TInt iStepY;
	TInt iPosX; // current x position
	TInt iPosY; // current y position
	TInt iIndexX; // current x position index
	TInt iIndexY; // current y position index
	};
	
/**
Base class for creating an off screen image target
*/
class CTImageTarget : public CBase
	{
public:
	virtual TSize SizeInPixels() const = 0;
	virtual CFbsBitmap* GetTargetFbsBitmapL() = 0;
	virtual TInt CreateContext(CTContextBase*& aGc, TBool aActivate=ETrue) = 0;
	virtual void Finish() = 0;
	virtual TInt Activate(CTContextBase*& aGc) = 0;
	virtual void Close() = 0;
	virtual CBitmapDevice* BitmapDevice() const;

protected:
	CBitmapDevice* iBitmapDevice;	// The virtual device
	CFbsBitmap* iBitmap;			// Virtual bitmap
	};

/**
Create an image target using CFbsBitGc
*/
class CTBitGdiTarget : public CTImageTarget 
	{
public:
	virtual ~CTBitGdiTarget();
	static CTBitGdiTarget* NewL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	virtual TSize SizeInPixels() const;
	virtual CFbsBitmap* GetTargetFbsBitmapL();
	virtual TInt CreateContext(CTContextBase*& aGc, TBool aActivate=ETrue);
	virtual void Finish();	
	virtual TInt Activate(CTContextBase*& aGc);
	virtual void Close();
private:
	void ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	};

/**
Create an image target using DirectGDiGc
 */
class CTDirectGdiTarget : public CTImageTarget
	{
public:
	CTDirectGdiTarget();
	virtual ~CTDirectGdiTarget();
	static CTDirectGdiTarget* NewL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	virtual TSize SizeInPixels() const;
	virtual CFbsBitmap* GetTargetFbsBitmapL();
	virtual TInt CreateContext(CTContextBase*& aGc, TBool aActivate=ETrue);
	virtual void Finish();	
	virtual TInt Activate(CTContextBase*& aGc);
	virtual void Close();
private:
	void ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize);

public:
	CDirectGdiDriver*      iDGdiDriver;  // Driver for Direct GDI support
	RDirectGdiImageTarget* iDGdiImageTarget;
	RSgImage              iRSgImage;
	TSgImageInfo	iImageInfo;	// image info for RSgImage
	};

/**
The reason to have a new step base is that it is very much possible that the all 
individual test steps have project related common variables and members 
and this is the place to define these common variable and members.
All directgdi test classes derive from this class
*/
class CTDirectGdiStepBase : public CTTMSGraphicsStep
	{

protected:
	// This structure is used to help manage parameters on a per test-case
	// basis. A pointer to an instance of this structure is passed through
	// to each test case as it executes.
	//
	struct TTestParams
	{		
	TUidPixelFormat iTargetPixelFormat;
	TUidPixelFormat iSourcePixelFormat;
	TUidPixelFormat iSourceResourcePixelFormat;
	TBool 			iDoCompressed;
	
	TTestParams () : 
		iTargetPixelFormat(EUidPixelFormatUnknown), 
		iSourcePixelFormat(EUidPixelFormatUnknown), 
		iSourceResourcePixelFormat(EUidPixelFormatUnknown),
		iDoCompressed(EFalse){};
	};
	
	virtual ~CTDirectGdiStepBase();
	CTDirectGdiStepBase();
	void SetTargetL(TUidPixelFormat aPixelFormat, const TContextTestCase aCase = EOneContextOneTarget, const TSize& aSize=KDefaultOffScreenImageSizeInPixels);
	void ConvertPixelFormats(TPtrC aPixelFormatInput, RArray<TUidPixelFormat>& aPixelFormatArray);
	void CreateFileName(TTestParams& aParams, TDes& aTestName, TPtrC& aTestCaseName, TDesC* namePostfix = NULL);
	void DisplayTargetImageL(TUidPixelFormat aPixelFormat);
	void DisplayTargetImageL(TUidPixelFormat aPixelFormat, CTImageTarget* aGdiTarget);
	TInt WriteTargetOutput(TTestParams& aParams, TPtrC aTestCaseName, TDesC* namePostfix = NULL);
	TInt WriteTargetOutput(TTestParams& aParams, TPtrC aTestCaseName, CTImageTarget* aImageTarget, TDesC* namePostfix = NULL);
	void ResetGc();
	CFbsBitmap* CreateConcentricRectsBitmapL(TUidPixelFormat aPixelFormat, const TSize& aSize);
	CFbsBitmap* CreateCheckedBoardBitmapL(TUidPixelFormat aPixelFormat,
			const TSize& aSize, const TSize& aChecksPerAxis = TSize(10, 10), TBool aGenAlpha = EFalse);
	CFbsBitmap* CreateBlackWhiteBitmapL(TUidPixelFormat aPixelFormat,
			const TSize& aSize, const TSize& aChecksPerAxis = TSize(10, 10));
	CFbsBitmap* CreateMaskingPixmapL (TUidPixelFormat aPixelFormat, const TSize& aSize);
	CFont* GetFont();
	void ReleaseFont(CFont* aFont);
	TBool TestTargetL(const TRgb& aColour);
	void CheckErrorsL(TInt aDirectGdiErrorCode, TInt aBitGdiErrorCode, const TText8* aFile, TInt aLine);

protected:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	virtual void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
	virtual void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);
	virtual void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
	virtual void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
	// Functions specific to CTDirectGdiStepBase (and derived classes)
	virtual void RunTestsL() = 0;
	void RunOomTestsL();
	static void ResetCache(TAny* aPtr);
	static void ResetPenSize(TAny* aPtr);
	
protected:	
	CTContextBase* iGc;
	CTContextBase* iGc2;
	CTImageTarget* iGdiTarget;
	CTImageTarget* iGdiTarget2;
	TBool	iMakeRefImg; // switch on (ETrue) and off the control of generating reference images, from .ini file
	TBool	iUseDirectGdi; // switch on (ETrue) and off the control of using Directgdi or old bitgdi, from .ini file
	RArray<TUidPixelFormat> iTargetPixelFormatArray; // target pixel format(s) for testing
	RArray<TUidPixelFormat> iSourcePixelFormatArray; // source pixel format(s) for testing
	RArray<TUidPixelFormat> iSourceResourcePixelFormatArray; // source resource pixel format(s) for testing (for DrawResource)
	TTestParams iTestParams;	// Parameters used for testing in derived classes
	TBool iDoOomTests; 	// switch on (ETrue) and off whether out of memory tests are run, from .ini file
	TBool iRunningOomTests;	// ETrue if out of memory tests are running

	CFbsTypefaceStore* iFontStore;
	TInt iFontId;
	MVgImageCache* iVgImageCache;
	TBool iUseSwDirectGdi;	// ETrue if software DirectGDI is being used
private:
	CActiveScheduler* 	iScheduler;
	};
	
// Macros to automatically test error condition and report when test fails.
// Essentially same as TESTE(a,b) MACRO that base class uses, except error to check is also error that is reported.
#define TESTNOERROR(a) \
	{\
	TInt b = a;\
	TBool c = (b == KErrNone);\
	testBooleanTrueWithErrorCode((c), (b), (TText8*)__FILE__, __LINE__);\
	}

#define TESTNOERRORL(a) \
	{\
	TInt b = a;\
	TBool c = (b == KErrNone);\
	testBooleanTrueWithErrorCodeL((c), (b), (TText8*)__FILE__, __LINE__);\
	}
	
#define TESTWITHFILENAMEANDLINENUMBERL(a, f, l) \
	{\
	testBooleanTrueL((a), f, l); \
	}
	
/**
Gets the size of the hardware display in pixels
@return TSize object containing the screen size
*/
inline TSize GetDisplaySizeInPixels()
	{
	return TSize(150, 150);
	}
	
#endif
