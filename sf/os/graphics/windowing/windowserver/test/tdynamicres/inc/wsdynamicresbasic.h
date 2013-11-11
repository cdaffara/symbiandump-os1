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

/**
 @file
*/

#ifndef __WSDYNAMICRESBASIC_H__
#define __WSDYNAMICRESBASIC_H__

#include "wsdynamicresbase.h"
#include <graphics/displayconfiguration.h>
#include <graphics/displaycontrol.h>
#include <graphics/displaymapping.h>
#include <e32math.h>
/**
 *
 **/

static TBool cSCLogged =EFalse;


class CWsDynamicResBasic : public CWsDynamicResBase
{
	typedef CWsDynamicResBase MyBase;
private:
	struct TBitmapRegionPair
		{
		CFbsBitmap* bitmap;
		RRegion* region;
		TRect drawRect;
		};

public:
	CWsDynamicResBasic();
	virtual ~CWsDynamicResBasic();
	virtual void SetupL();
	virtual void TearDownL();
	virtual void TearDownFromDeleteL();
	
	static CTestSuite* CreateSuiteL( const TDesC& aName );
	
	void ResetScreens();
	
protected:
//tests
	
	//BASIC
	void	GRAPHICS_WSERV_DYNAMICRES_0001L();
	void	GRAPHICS_WSERV_DYNAMICRES_0002L();
	void	GRAPHICS_WSERV_DYNAMICRES_0003L();
	void	GRAPHICS_WSERV_DYNAMICRES_0004L();
	void	GRAPHICS_WSERV_DYNAMICRES_0005L();
	void	GRAPHICS_WSERV_DYNAMICRES_0006L();
	void	GRAPHICS_WSERV_DYNAMICRES_0007L();
	void	GRAPHICS_WSERV_DYNAMICRES_0008L();
	void	GRAPHICS_WSERV_DYNAMICRES_0009L();
	void	GRAPHICS_WSERV_DYNAMICRES_0010L();
	void	GRAPHICS_WSERV_DYNAMICRES_0011L();
	void	GRAPHICS_WSERV_DYNAMICRES_0012L();
	void	GRAPHICS_WSERV_DYNAMICRES_0013L();
	void	GRAPHICS_WSERV_DYNAMICRES_0014L();
	void	GRAPHICS_WSERV_DYNAMICRES_0015L();
	void	GRAPHICS_WSERV_DYNAMICRES_0016L();
	void	GRAPHICS_WSERV_DYNAMICRES_0017L();
	void	GRAPHICS_WSERV_DYNAMICRES_0018L();

	//NEGATIVE
	void	GRAPHICS_WSERV_DYNAMICRES_0021L(TBool aScaleMode);
	
	//OOM
	void	GRAPHICS_WSERV_DYNAMICRES_0031L();
	void	GRAPHICS_WSERV_DYNAMICRES_0032L();
	void	GRAPHICS_WSERV_DYNAMICRES_0033L();
	void	GRAPHICS_WSERV_DYNAMICRES_0034L();
	
	//SCREEN CAPTURE
	void	GRAPHICS_WSERV_DYNAMICRES_0041L(TBool aIsSurface);
	void	GRAPHICS_WSERV_DYNAMICRES_0042L(TBool aIsSurface);
	
	//DYNAMIC SCREEN MODE
	void	GRAPHICS_WSERV_DYNAMICRES_0051L();
	void	GRAPHICS_WSERV_DYNAMICRES_0052L();
	void	GRAPHICS_WSERV_DYNAMICRES_0053L();

	//DEFECT
	void	GRAPHICS_WSERV_DYNAMICRES_0101L();
	void	GRAPHICS_WSERV_DYNAMICRES_0102L();
	
	
	void SetRotation (TDisplayConfiguration& aConfig, const MDisplayControl::TResolution& aResolution);
	TBool Compare(const CFbsBitmap& aBitmap, const TRect& aRect1,
			const TRect& aRect2, RArray<TBitmapRegionPair>& aRegionArray, TBool aIsSurface);
	TRgb GreatestColor(TRgb& aColor) const;

//data members:
private:
	CActiveScheduler	iScheduler;
};

class CWsDisplayEvent : public CActive
	{
	
public:
	CWsDisplayEvent(RWsSession *aWs);
	~CWsDisplayEvent();
	void Request();
	TInt ConfigSpinner()	{return iConfigSpinner;}
	TInt ResListSpinner()	{return iResListSpinner;}
	TInt ReceivedEventCount()	{return iReceivedEventCount;}
	TInt ReceivedDeviceEventCount()	{return iReceivedDeviceEventCount;}
	TInt ReceivedPointerEventCount() {return iReceivedPointerEventCount;}
	void SetExpectedPointPos(const TPoint& aPos) {iPointerPos = aPos;}
	TBool PointerTestPassed() {return iPointerPosTestPass;}
protected:
	void DoCancel();
	void RunL();
	
private:
	RWsSession *iWs;
	TInt iConfigSpinner;
	TInt iResListSpinner;
	TInt iReceivedEventCount;
	TInt iReceivedDeviceEventCount;
	TInt iReceivedPointerEventCount;	
	TPoint iPointerPos;
	TBool iPointerPosTestPass;
	};

class CEventTimer : public CActive
	{
public:
	static CEventTimer* NewL();
	~CEventTimer();
	void Wait(TInt aDelay);
	// From CActive:
	void RunL();
	void DoCancel();
	
protected:
	CEventTimer();
	void ConstructL();
	protected:
RTimer iTimer;
};



class CDSATestDrawing: public CTimer, public MDirectScreenAccess
	{
public:

	void Draw();

	
	
	static CDSATestDrawing* NewL();
	void ConstructL();
	//from MDirectScreenAccess
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	~CDSATestDrawing();
	void SetDSA(CDirectScreenAccess *aDSA){iDSA = aDSA;}
	void SetWindow(RWindow &aWindow){iWin = &aWindow;}
	void StartDrawingL(CDirectScreenAccess *aDSA);
	void RunL();
	
private:
	CDSATestDrawing();
	TRect iRect;
	RWindow* iWin;
	CDirectScreenAccess *iDSA;
	};

#endif	/*__WSDYNAMICRESBASIC_H__*/
