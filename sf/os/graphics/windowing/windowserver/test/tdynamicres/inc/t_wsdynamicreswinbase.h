// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// twsgcetestwinbase.h
// 
//

#ifndef TWSDYNAMICRESWINBASE_H_
#define TWSDYNAMICRESWINBASE_H_

#include <gdi.h>
#include <test/tefunit.h>
#include <w32std.h>
#include <w32debug.h>
#include "wsgcedebugsession.h"
struct TWsDebugWindowId;

/**
 * Base class sets up the tryptich of test windows.
 * 
 * 
 **/
class CWsDynamicResWinBase : public CTestFixture
{
public:
	CWsDynamicResWinBase();
	virtual ~CWsDynamicResWinBase();
	virtual void SetupL();
	virtual void SetupL(TBool aUseOtherScreenForInfo);
	
	virtual void TearDownL();
	virtual void TearDownFromDeleteL();
	
	static TBool	ActivateWithWipe(CWindowGc* aGc,RWindow& aWin,TRgb aColor=TRgb(0,0));
	
	/* Note that each derived test class needs to declare a static function to register tests.
	 * This should match the following prototype:
	static CTestSuite* CreateSuiteL( const TDesC& aName );
	*/ 
	
protected:
	static void Pause(TInt aMilliseconds=200);

	TBool GCEIsSupported() const;
	static TBool GCEIsSupportedStatic();

	TBool DisplayHasAlpha() const;
	TBool SelectChromaCompositionMode();
	TBool SelectAlphaCompositionMode(TDisplayMode aMode = EColor16MA);
	TRgb  GceHoleColor(RWindowBase& aWin)const;
	void DrawUIContent(RWindow& aWindow);
	void DrawPlainUI(RWindow& aWindow,TBool aInvalidate,TRgb aWipeColor=TRgb(0,0));
	void DrawCross(RWindow& aWindow, TRgb aColor, TInt aThickness=1);
	void TestPixelL(TPoint aPt, TRgb aColor, TBool aMatch);
	TBool TestRectL(TRect aRect,TRgb aInnerColor,TInt aOtherInnerColors,TRgb aOuterColor,TInt aOtherOuterColors,TInt aCornerSize);
	void MakeTitleAndCompareWindowsL(TRefByValue<const TDesC16> aTitle,TRefByValue<const TDesC16> aDetail=_L(""));
	void LargerCompareWindow(TBool aGoLarger=ETrue);
	void UpdateTitleWindowL(TRefByValue<const TDesC16> aDetail,TInt aIndex=0);
	void RepaintTitleWindowL();
	void TefUnitFailLeaveL();
	CWindowGc*	GcForWindow(RWindow& aWin);
	TBool		InvalidateRegion(const TRegion& aRegion,RWindow& aWin);
	CWindowGc*	BeginActivateWithWipe(TBool aInvalidate,RWindow& aWin,TRgb aColor=TRgb(0,0));
	CWindowGc*	BeginActivateWithWipe(TBool aInvalidate,TRect aRect,RWindow& aWin,TRgb aColor=TRgb(0,0));
	CWindowGc*	BeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor);
	/** Helper method to find a named screen region
	 *	start/end letter should be 'a' to 'y' (yes that includes 'i')
	 **/
	static TRect PentCellRect(const TRect& aFullRect,char aStartLetter,char aEndLetter=0);
	void CheckAndConnectScreen();

protected:
	static const TInt KSurfaceWidth = 50;
	static const TInt KSurfaceHeight = 50;
	TBool	iDoTearDown;
	RWsDebugSession iSession;
	RWindowGroup iGroup;
	RWindowGroup iInfoGroupInstance;
	RWindowGroup* iInfoGroup;
	CWsScreenDevice* iScreenDevice;
	CWsScreenDevice* iInfoScreenDevice;
	CWindowGc* iGc;
	CWindowGc* iInfoGc;
	TInt iWindowHandle;
	static TBool	iTransparencyEnabled;
	TRgb iRed, iGreen, iBlue, iCyan, iMagenta, iYellow, iWhite;
	TRgb TITLE_BACKGROUND;
	TRgb COMPARE_BACKGROUND;
	RBlankWindow iBackground;
	RWindow iCompare;
	RWindow iTitle;
	TRect 	iTestPos;
	TRect   iCenteredFrontWinRect;
	TPoint	iTestPointCentre;
	TDisplayMode iDisplayMode;
	static const TInt KMaxTestName = 80;
	typedef TBuf<KMaxTestName> TTestName;
	mutable TRgb	iLastGceHoleColor;
	TTestName iTestName;
	static const TInt KMaxInfoLines=5;
	TTestName iTestInfo[KMaxInfoLines];
	static CActiveScheduler	iScheduler;
};

// This little helper class checks that the given bytes contain the expected pattern.
namespace Pattern
	{
		void Fill(void* aTrg,TInt aOffset,TInt aLength);
		TBool Check(void* aTrg,TInt aOffset,TInt aLength);
		TBool CheckVal(void* aTrg,TInt aOffset,TInt aLength,char val=0);
	}

//This wrapper for the configuration class allows stress testing of the configuration object

template <class TXxxConfiguration,TInt aExtraSize,TInt aFalseSize> 
class TestExtend:	public TXxxConfiguration
	{
public:
	char iExtraData[aExtraSize];
	TestExtend():
		TXxxConfiguration(aFalseSize)
		{
		TXxxConfiguration::iSize=aFalseSize;
		Pattern::Fill(this,aFalseSize,sizeof(*this)-aFalseSize);
		}
	void* operator new(unsigned int,TestExtend* inplace)
	{	//Use this inplace operator new to pre-initialise the memory
	return inplace;
	}
	void operator delete(void*,TestExtend*)
	{	//Stops the warning!
	}
	TestExtend(const TXxxConfiguration&i):
		TXxxConfiguration(i)
		{
		Pattern::Check(this,sizeof(TXxxConfiguration),aExtraSize);
		}
	TBool WasExtraOverwritten()
		{
		return Pattern::Check(this,sizeof(TXxxConfiguration),aExtraSize);
		}
	TBool WasFalseSizeOverwritten()
		{
		return Pattern::Check(this,aFalseSize,sizeof(*this)-aFalseSize);
		}
	TBool WasFalseSizeZero()
		{
		return Pattern::CheckVal(this,aFalseSize,sizeof(TXxxConfiguration)-aFalseSize);
		}
	void	StompValue(TInt aOffsetInLongs,TInt aNewValue)
		{
		*((TInt*)this+aOffsetInLongs)=aNewValue;
		}
	TInt	ReadValue(TInt aOffsetInLongs)
		{
		return *((TInt*)this+aOffsetInLongs);
		}
	};
#endif /*TWSDYNAMICRESWINBASE_H_*/
