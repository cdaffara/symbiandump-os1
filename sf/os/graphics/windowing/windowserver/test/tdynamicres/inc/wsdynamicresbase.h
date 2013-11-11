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
//

#ifndef WSDYNAMICRESBASE_H_
#define WSDYNAMICRESBASE_H_

#include "t_wsdynamicreswinbase.h"
#include <graphics/surface.h>
#include "surfaceutility.h"

//************************************
//
// Asserting wrapper around dereferencing a pointer that might be NULL.
// I don't want to fill the test code with checks that this pointer is not null before calling surface utility methods....
//
//************************************

template <class Referee,class LoggerOwner>
class	SafePointer
	{
public:
	SafePointer(LoggerOwner* aOwner,Referee* aReferee=NULL):
		iOwner(aOwner),	iReferee(aReferee)
		{}
	Referee*	operator=(Referee* aReferee)
		{
		return iReferee=aReferee;
		}
	Referee*&	operator()()
		{
		return iReferee;
		}
	Referee*	operator->()
		{
		if (iReferee==NULL)
			{
			iOwner -> INFO_PRINTF1(_L("Tried to dereference a pointer that is null!"));
			User::Panic(_L("null pointer"),__LINE__);
			}
		return iReferee;
		}
private:
	LoggerOwner* iOwner;
	Referee* iReferee;
	};
	
/**
 * Base class test harness that provides facilities for windowing and surface testing
 * 
 **/
class CWsDynamicResBase : public CWsDynamicResWinBase
{
public:
	CWsDynamicResBase();
	virtual ~CWsDynamicResBase();
	
	virtual void SetupL();
	virtual void SetupL(TBool aUseOtherScreenForInfo);
	virtual void TearDownL();
	virtual void TearDownFromDeleteL();
	/* Note that each derived test class needs to declare a static function to register tests.
	 * This should match the following prototype:
	static CTestSuite* CreateSuiteL( const TDesC& aName );
	*/ 

	protected:
	void UISurfaceL(TSurfaceId& aSurfaceId) const;
	void CommonSurfaceWindowSetupL(RWindow& aWindow, TSurfaceId& aSurface, const TRgb& aColor);
	void CommonOverlayWindowSetupL(RWindow& aWindow, const TRgb& aColor);
	void ResizeTestCommonSetupL(RWindow& aWindow, const TRgb& aColor);

	void LargerTestWindow(TInt aPercentOfBack=80);
	void MakeExtraChildWindowL(const RWindowBase& aFromParent,TRect aChildRect,TRgb aChildColor);
	struct LoopingGcPtr;
	LoopingGcPtr	LoopBeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor);
	CWindowGc*	BeginActivateWithWipe(TBool aInvalidate,RWindow& aWin,TRgb aColor=TRgb(0,0));
	CWindowGc*	BeginActivateWithWipe(TBool aInvalidate,TRect aRect,RWindow& aWin,TRgb aColor=TRgb(0,0));
	CWindowGc*	BeginActivateWithWipe(const TRegion& aRegion,RWindow& aWin,TRgb aColor);
	static CFbsBitmap* RotateBitmapLC(const CFbsBitmap* aSrcBitmap);

	void Pause(TInt aMilliseconds);
	TInt	RegionDiffForUiLayer(TInt aUiLayer);	//returns values from TRegionExtend::TOverlapFlags enumeration
	enum	 FastPathMode
		{	//Tereat this as bit flags or distinct states as you wish.
			EFpExternalOpaque=0,
			EFpUiOpaque=1,
			EFpUiBlended=2,
			EFpUiComplex=3,			//If ony this is set then both blended and opaque are full=screen
			EFpUiRegions=4,			//If this FLAG is set then the regions are less than full-screen. Never occurs on its own
			EFpUiRegionsOpaque=5,			//If this is set then the regions are less than full-screen
			EFpUiRegionsBlended=6,			//If this is set then the regions are less than full-screen
			EFpUiRegionsComplex=7	//This is the expected setting for windowed cr1108 optimisation. 
		};
	FastPathMode	DeduceUiFastPathMode();
protected:
	SafePointer<CSurfaceUtility,CWsDynamicResBase> iUtility;
	class TPostTestCleanup
		{
		protected:
		TPostTestCleanup()	{}
		public:
		mutable CSurfaceUtility*	iSharedUtility;
		mutable TBool				iCleanedUpOnExit;
		void				CreateSharedUtilityL()const ;
		};
	static const TPostTestCleanup& PostTestCleanupInstance();
	RBlankWindow	iTestBack;
	RWindow	iTestFront;
	RWindow	iTestChild;
	RWindow	iTestSecondChild;
	static const TUidPixelFormat KSurfaceFormat = EUidPixelFormatXRGB_8888;
	static const TInt KBytesPerPixel = 4;	// Four bytes per pixel for the format above.
private:

};

struct CWsDynamicResBase::LoopingGcPtr
	{
	LoopingGcPtr(const TRegion& aRegion,RWindow& aWin,TRgb aColor,CWindowGc *aGc)
		:	iRegion(aRegion),	iWin(aWin),	iColor(aColor), iGc(aGc), iPass(-1)	
		{ operator++();	}
	LoopingGcPtr(const LoopingGcPtr& aRhs)
		:	iRegion(aRhs.iRegion),	iWin(aRhs.iWin),	
			iColor(aRhs.iColor), iGc(aRhs.iGc),
			iPass(aRhs.iPass)	
		{}
	void operator ++();	
	operator bool()		{ return iPass>=0?iGc:(CWindowGc *)NULL;	}
	CWindowGc * operator ->()	{ return iPass>=0?iGc:(CWindowGc *)NULL;	}
	const TRegion& iRegion;
	RWindow& iWin;
	TRgb iColor;
	CWindowGc *iGc;
	TInt iPass;
	};

#define LOG_AND_PANIC_IF_NOT_GCE											\
		{																	\
		if (!GCEIsSupported())												\
			{																\
			INFO_PRINTF1(_L("Test skipped: GCE support is not loaded"));	\
			User::Panic(_L("GCE.Wrong.Mode"),1);							\
			return;															\
			}																\
		}

#endif /*WSDYNAMICRESBASE_H_*/
