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

#ifndef TCWGCECSCBASE_H_
#define TCWGCECSCBASE_H_

#include <test/tefunit.h>
#include <w32debug.h>
#include "surfaceutility.h"
#include <e32property.h>

class TPerfProperties
{
public:
	TBool GetValue(TUint aKey);
	void PrintAllL(CTestFixture& aLogger);
	TBool ResetValue(TUint aKey);
	void UpdateAll();
	void ResetAll();
private:
#ifdef GCE_BACKEND_PROFILING
	TInt iValue[EPerfLastKey];
	TInt iPrevValue[EPerfLastKey];
	const static TBuf<100> propertyKeysTxt[EPerfLastKey];
#endif
};

class CWsGceCscBase: public CTestFixture
{

public:
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
	
public:
	CWsGceCscBase();
	virtual ~CWsGceCscBase();
	virtual void SetupL();
	virtual void TearDownL();
	virtual void TearDownFromDeleteL();

protected:
	typedef TBuf<KMaxTestName> TTestName;
	
	class TPostTestCleanup
		{
		protected:
		TPostTestCleanup()	{}
		public:
		CSurfaceUtility*	iSharedUtility;
		TBool				iCleanedUpOnExit;
		};


	typedef enum TOptimizationLevel
		{
		EDisable,
		EDetectFrontmostFullScreenOpaqueLayer,
		EMaximum,
		} TOptimizationLevel;

		
protected:
	void SetCompositionMode(RWindow& aWindow, TDisplayMode aMode);
	
	TBool Compare(CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2);

	void ConstructOpaqueWindowLC(RWindow& aWindow, 
			                     const TRgb& aColor,
			                     TInt aPos,
			                     const TPoint& aPoint,
			                     const TSize& aSize);
	
	void ConstructOpaqueWindowL(RWindow& aWindow, 
			                    const TRgb& aColor,
			                    TInt aPos,
			                    const TPoint& aPoint,
			                    const TSize& aSize);
	
	void SetBackgroundColorWindow(RWindow& aWindow, const TRgb& aColor);
	
	void ConstructOpaqueSurfacedWindowLC(RWindow& aWindow,
		                                 const TRgb& aColor,
		                                 TInt aPos,
		                                 const TPoint& aPoint,
		                                 const TSize& aSize,
		                                 TSurfaceId& aSurfaceID);
	void ConstructOpaqueSurfacedWindowL(RWindow& aWindow,
		                                const TRgb& aColor,
		                                TInt aPos,
		                                const TPoint& aPoint,
		                                const TSize& aSize,
		                                const TSurfaceId& aSurfaceID);
	
protected:
	inline TInt ObjIndScr(TInt aScreen,TInt aWin=0,TInt aLayer=0,TInt aExtra=0);
	template <class DataType>	// DataType may be TSurfaceId, TInt,TRect, TSurfaceConfiguration (not TRegion)
	TInt	DebugInfo(TWsDebugInfoFunc aFunction, TInt aObjectIndex, TDes8& aHostBuffer,const DataType*&aReturnedObject)const
	{
	return DebugInfo(aFunction,aObjectIndex,aHostBuffer,(const void*&)aReturnedObject,sizeof(DataType));
	}
	TInt DebugInfo(TWsDebugInfoFunc aFunction, 
			       TInt aParam, 
			       TDes8& aHostBuffer,
			       const void*&aReturnedObject,
			       TInt aObjectSize)const;
	void TefUnitFailLeaveL();
	static void Pause(TInt aMilliseconds=200);
	TBool GCEIsSupported() const;
	static TBool GCEIsSupportedStatic();
	static TPostTestCleanup& PostTestCleanupInstance();
	
protected:
	TBool	iDoTearDown;
	RWsSession iSession;
	RWindowGroup iGroup;
	CWsScreenDevice* iScreenDevice;
	CWindowGc* iGc;
	TInt iWindowHandle;
	TBool	iTransparencyEnabled;
	TRgb iRed, iGreen, iBlue, iCyan, iMagenta, iYellow, iWhite;
	TDisplayMode iDisplayMode;
	SafePointer<CSurfaceUtility,CWsGceCscBase> iUtility;
	static const TInt KMaxInfoLines=5;
	TTestName iTestInfo[KMaxInfoLines];
	TTestName iTestName;
	static const TUidPixelFormat KSurfaceFormat = EUidPixelFormatXRGB_8888;
	static const TInt KBytesPerPixel = 4;	// Four bytes per pixel for the format above.
	TPerfProperties iProperties;
};

inline TInt CWsGceCscBase::ObjIndScr(TInt aScreen,
		                                     TInt aWin,
		                                     TInt aLayer,
		                                     TInt aExtra)
	{
	return 	(aScreen<<EWsDebugArgScreenShift)
		|	(aWin<<EWsDebugArgWindowShift)
		|	(aLayer<<EWsDebugArgLayerShift)
		|	(aExtra<<EWsDebugArgExtraShift);
	}

#endif /*TCWGCECSCBASE_H_*/
