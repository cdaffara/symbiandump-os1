// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TSCDVDEVORIENTATION_H__)
#define __TSCDVDEVORIENTATION_H__

#include "TGraphicsHarness.h"
#include <graphics/surface.h>
#include <pixelformats.h>
#include <bitdrawsurface.h>


LOCAL_D const TUint KPanicThreadHeapSize=0x4000;
_LIT(KSCDVPanicCategory,"SCDV");

//
// Panic testing //
//
typedef TInt (*TPanicFunction)(TDeviceOrientation aOrientation, TDisplayMode aDisplayMode,TInt aScreenNo);

struct SPanicParams
	{
	TPanicFunction functionName;
	TDeviceOrientation* pointerDeviceOrientation;
	TDisplayMode* pointerDisplayMode;
	TInt* pointerScreenNo;
	};

enum TPanicExitReasons
	{
	EWsExitReasonBad=0x12345678,
	EWsExitReasonFinished
	};

enum TAutoPanics
	{
	EAutoPanicTestFailed=0xBAADF00D,
	EAutoPanicPanicFailed=0xF00DBAAD
	};
	
class CPanicTest : public CBase
	{
public:
	CPanicTest();
	~CPanicTest();
	TInt LaunchPanicThread(RThread &aThread, SPanicParams *aPtr);
	TInt TestInvalidModePanicL(TPanicFunction aFunction, TScreenDriverPanic aExitReason,TDisplayMode* aPtr2, TDeviceOrientation* aPtr, TInt* aPtrScreenNo, TBool* aTestFinished = NULL);

	TInt TestPanicL(SPanicParams *aPtr, TInt aExitReason, const TDesC &aCategory, TBool* aTestFinished=NULL);
	TInt TestPanicL(TPanicFunction aFunction, TInt aExitReason, const TDesC &aCategory, TDisplayMode* aPtr2,TDeviceOrientation* aPtr, TInt* aPtrScreenNo, TBool* aTestFinished=NULL);
	TInt iActualPanicCode;
	TInt iThreadNumber;
	};



class CTDevOrientation: public CTGraphicsBase
	{
public:

	CTDevOrientation(CTestStep *aTest, RArray<TInt> aArrayDispModes);
	CTDevOrientation(CTestStep *aTest);
	~CTDevOrientation();
	void RunTestCaseL(TInt aCurTestCase);
	
	TBool GetSurfaceIdInterfaceL(const TInt& aScreenNo,const TDisplayMode& aDisplayMode, CFbsDrawDevice*& aDrawScreenDevice, MSurfaceId*& aSurfaceIdInterface );

	TBool ChooseOrientationMode( MSurfaceId* aSurfaceIdInterface);
	
    void TestValidButUnavailableOrientationL(MSurfaceId* aSurfaceIdInterface,TDisplayMode aDisplayMode);
	void TestSetDeviceOrientationWithInvalidModeL(TDisplayMode aDisplayMode);
	void TestGetSurfaceWithValidDispModeL( const MSurfaceId* aSurfaceIdInterface, const TDisplayMode& aDisplayMode, TInt aScreenNo);
	void TestGetDeviceOrientation(MSurfaceId* aSurfaceIdInterface);
	void TestDeviceOrientationSettingandGetting( MSurfaceId* aSurfaceIdInterface);
	void TestHorizontalVerticalTwips(const CFbsDrawDevice* aDrawScreenDevice,  MSurfaceId* aSurfaceIdInterface, TDisplayMode aDisplayMode, TInt aScreenNo);
	void TestSetDeviceOrientationWithValidMode(const CFbsDrawDevice* aDrawScreenDevice,  MSurfaceId* aSurfaceIdInterface, TDisplayMode aDisplayMode, TInt aScreenNo);
    void TestSetDisplayMode(CFbsDrawDevice* aDrawScreenDevice,MSurfaceId* aSurfaceIdInterface);


private:
	void ConstructL();
	TBool ReqBppAndPixelFormat(TDisplayMode aDisplayMode, TInt& aBpp, TUint32& aPixelFormat);

private:	
	TSize iPhysSize;
    RArray<TDeviceOrientation> iArrayOrientation;
	// handle to initiate the panic tests
	CPanicTest *iPanicTest;
	RArray<TInt> iArrayDispModesPerScreen;
	TInt iCurrScreen;
	};

  
class CTDevOrientationStep : public CTGraphicsStep
	{
public:
	CTDevOrientationStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	};

_LIT(KTDevOrientationStep,"TDevOrientation");


#endif
