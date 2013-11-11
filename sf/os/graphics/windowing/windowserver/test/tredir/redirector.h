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

#ifndef __REDIRECTOR_H__
#define __REDIRECTOR_H__

#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;
#ifdef __WINS__
class CDebugOsbWin;
#endif

class TWsBackBuffer;

NONSHARABLE_CLASS(CWsGcRedirector): public CWsGraphicDrawer, public MWsEventHandler
	{
public:
	enum {EImplUid = 0x10281e1e};	
		
public:		
	static CWsGcRedirector* NewL();
	virtual ~CWsGcRedirector();

	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
	virtual void DoHandleEvent(const TWservCrEvent& aEvent);
private:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
	void SendInfo();
	void OnScreenUpdated();
#ifdef __WINS__
	static TInt UpdateDebugWindowsCallback(TAny* aSelf);
#endif
	
private:
	MWsFrontBuffer* iFrontBuf;
	MWsBackBuffer* iBackBuf;
	CFbsBitmap* iFrontBit;
	CFbsBitmapDevice* iFrontDev;
	CFbsBitGc* iFrontGc;
	CFbsBitmap* iBackBit;
	CFbsBitmapDevice* iBackDev;
	CFbsBitGc* iBackGc;
	CFbsBitGc* iBadGc;
	TWsBackBuffer* iBackObj;
#ifdef __WINS__
	CDebugOsbWin* iFrontWin;
	CDebugOsbWin* iBackWin;
	CPeriodic* iUpdateDebugWinsTimer;
	TBool iDisableWin;
#endif
	TInt iScreenId;
	TInt iUpdateCounter;
	TSize iDefaultSize;
	};

#endif
