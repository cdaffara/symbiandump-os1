// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LISTENER_H__
#define __LISTENER_H__

#include <bitstd.h>
#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;
class MWsGraphicDrawerContext;
#ifdef __WINS__
class CDebugOsbWin;
#endif

const TInt KNotifyRemoveSelf = 0;
const TInt KNotifyRemoveThenAddSelf = 1;
const TInt KNotifyAddSelf = 2;
const TInt KNotifyRemoveOther = 3;
const TInt KNotifyRemoveThenAddOther = 4;
const TInt KNotifyAddOther = 5;
const TInt KNotifyRemoveSelfAndOther = 6;
const TInt KNotifyDoNothing = 7;
const TInt KNotifyDisable = 8;

NONSHARABLE_CLASS(CWsEventListener): public CWsGraphicDrawer, public MWsEventHandler
	{
public:
	enum {EImplUid = 0x10281fb7};	
		
public:		
	static CWsEventListener* NewL();
	virtual ~CWsEventListener();

	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
	// override MWsEventHandler
	virtual void DoHandleEvent(const TWservCrEvent& aEvent);
private:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;

	void OnScreenSizeModeChanged(TInt aMode);
	void OnWindowVisibilityChanged(const RRegion* aRegion);
	TRgb CurrentColor();
	void SendInfo();
	void OnWindowGroupChanged(TInt aScreenNumber, TInt aWindowGroupId);
private:
	MWsGraphicDrawerContext* iDrawerContext;
	CFbsBitmap* iBit;
	CFbsBitmapDevice* iDev;
	CFbsBitGc* iGc;
#ifdef __WINS__	
	CDebugOsbWin* iWin;
	TBool iDisableWin;	
#endif
	TBool iEnabled;
	TBool iReady;
	TSize iSize;
	TDisplayMode iDisplayMode;
	MWsScreen* iScreen;
	TInt iColorIdx;
	TInt iSaveNumRect;
	TRect iSaveRect;
	TInt iWindowGroupId;	
	};

NONSHARABLE_CLASS(CWsEventNotifier): public CWsGraphicDrawer, public MWsEventHandler
	{
public:
	enum {	EImplUid1 = 0x102754c4,
			EImplUid2 = 0x102754c6};
		
public:		
	static CWsEventNotifier* NewL();
	virtual ~CWsEventNotifier();

	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
	// override MWsEventHandler
	virtual void DoHandleEvent(const TWservCrEvent& aEvent);
private:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
	void TestGraphicDrawerEnvironment();
private:
	TBool iReady;
	TInt iHandleMethod;
	TGraphicDrawerId iAnotherPluginID;
	CWsGraphicDrawer* iAnotherPlugin;
	TBool iTestGraphicDrawerEnvironment;
	MWsGraphicDrawerEnvironment* iEnv;
	};

#endif
