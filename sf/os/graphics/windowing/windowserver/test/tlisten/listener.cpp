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

#include "listener.h"
#include "wsgraphicdrawercontext.h"
#include <graphics/wsscreendevice.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#ifdef __WINS__
#include "../debuglog/osbwin.h"
#endif

const TInt KDefaultScreen 	= 0;
const TInt KListenDisable 	= 0;
const TInt KListenEnable  	= 1;
const TInt KCmdQuery		= 2;

const TUint8 KListenerInfoSig = 0xaa;

NONSHARABLE_STRUCT(TListenerInfo)
	{
	TUint8 iSignature;
	TInt iNumRect;
	TRect iRect;
	TInt iWindowGroupId;
	};

const TInt KMaxColors = 14;
const TInt KColorList[KMaxColors] =
	{
	0x555555,
	0x000080,
	0x008000,
	0x008080,
	0x800000,
	0x800080,
	0x808000,
	0x0000ff,
	0x00ff00,
	0x00ffff,
	0xff0000,
	0xff00ff,
	0xffff00,
	0xaaaaaa
	};
	
CWsEventListener* CWsEventListener::NewL()
	{
	return new(ELeave) CWsEventListener;	
	}
	
CWsEventListener::~CWsEventListener()
	{
	if (iEnabled)
		Env().UnregisterEventHandler(this);

#ifdef __WINS__
	if (!iDisableWin)
		delete iWin;
#endif	
	delete iGc;
	delete iDev;
	delete iBit;

	if (iDrawerContext)
		{
		iDrawerContext->Destroy();
		iDrawerContext = NULL;
		}
	}

void CWsEventListener::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData)
	{
	BaseConstructL(aEnv, aId, aOwner);

#ifdef __WINS__
	if (aData.Length()>0)
		iDisableWin = aData[0]==1;
#endif

	iScreen = aEnv.Screen(KDefaultScreen);
	MWsScreenConfig* screenConfig = iScreen->ObjectInterface<MWsScreenConfig>();
	if (screenConfig)
		{// Non NGA
		iSize = screenConfig->SizeInPixels();
		iDisplayMode = screenConfig->DisplayMode();
		iDrawerContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{// NGA
		MWsScreenDevice* screenDevice = iScreen->ObjectInterface<MWsScreenDevice>();
		User::LeaveIfNull(screenDevice);
		iSize = screenDevice->SizeInPixels();
		iDisplayMode = screenDevice->DisplayMode();
		iDrawerContext = CWsGraphicDrawerNgaContext::NewL();
		}

	iWindowGroupId = KErrNotFound;

	iBit = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBit->Create(iSize,iDisplayMode));
	iDev = CFbsBitmapDevice::NewL(iBit);
	User::LeaveIfNull(iDev);
	User::LeaveIfError(iDev->CreateContext(iGc));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);

#ifdef __WINS__
	if (!iDisableWin)	
		{
		_LIT(KListener, "EventListener");
		iWin = CDebugOsbWin::NewL(KListener, iBit->SizeInPixels());
		}
#endif	
	iReady = ETrue;
	}

void CWsEventListener::HandleMessage(const TDesC8& aData)
	{
	// wserv already check data size, and won't invoke this handler if it's empty
	switch (aData[0])
		{
		case KListenEnable:
		if (!iEnabled)
			{
			Env().RegisterEventHandler(this, this, TWservCrEvent::EScreenSizeModeChanged|TWservCrEvent::EWindowVisibilityChanged|TWservCrEvent::EWindowGroupChanged);
			iEnabled = ETrue;
			}
		break;
		
		case KListenDisable:
		if (iEnabled)
			{
			iEnabled = EFalse;
			Env().UnregisterEventHandler(this);
			}
		break;
		
		case KCmdQuery:
		SendInfo();
		break;
		}
	}
	
void CWsEventListener::DoHandleEvent(const TWservCrEvent& aEvent)
	{
	if (!iReady)
		return;
	
	switch (aEvent.Type())
		{
	case TWservCrEvent::EScreenSizeModeChanged:
		OnScreenSizeModeChanged(aEvent.SizeMode());
		break;
		
	case TWservCrEvent::EWindowVisibilityChanged:
		OnWindowVisibilityChanged(aEvent.VisibleRegion());
		break;

	case TWservCrEvent::EWindowGroupChanged:
		OnWindowGroupChanged(aEvent.ScreenNumber(), aEvent.WindowGroupIdentifier());
		break;
		}
	}

void CWsEventListener::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& /*aData*/) const
	{
	iDrawerContext->DrawEllipse(aGc, aRect, TRgb(0,255,0,128));
	}

void CWsEventListener::OnScreenSizeModeChanged(TInt /*aMode*/)
	{
	iGc->SetBrushColor(KRgbWhite);
	iGc->Clear();

#ifdef __WINS__	
	if (!iDisableWin)
		{
		iBit->LockHeap();
		iWin->Refresh(iBit->SizeInPixels(), iBit->DisplayMode(), iBit->DataAddress());
		iBit->UnlockHeap();
		}
#endif	
	}

TRgb CWsEventListener::CurrentColor()
	{
	iColorIdx = (iColorIdx+1) % KMaxColors;
	return TRgb(KColorList[iColorIdx]);
	}

void CWsEventListener::OnWindowVisibilityChanged(const RRegion* aRegion)
	{
	if (!aRegion)
		return;
	
	TInt n = aRegion->Count();
	if (n==0)
		return;
	iGc->SetBrushColor(CurrentColor());
	for (TInt i=0; i<n; ++i)
		{
		iGc->DrawRect((*aRegion)[i]);
		}
		
	iSaveNumRect = n;
	iSaveRect = (*aRegion)[0];
	
#ifdef __WINS__
	if (!iDisableWin)
		{
		iBit->LockHeap();
		iWin->Refresh(iBit->SizeInPixels(), iBit->DisplayMode(), iBit->DataAddress());
		iBit->UnlockHeap();
		}
#endif	
	}

void CWsEventListener::OnWindowGroupChanged(TInt /*aScreenNumber*/, TInt aWindowGroupId)
	{
	iWindowGroupId = aWindowGroupId;
	}

void CWsEventListener::SendInfo()
	{
	TPckgBuf<TListenerInfo> buf;
	buf().iSignature = KListenerInfoSig;
	buf().iNumRect = iSaveNumRect;
	buf().iRect = iSaveRect;
	buf().iWindowGroupId = iWindowGroupId;
	TInt err = SendMessage(buf);
	__ASSERT_ALWAYS(err>=KErrNone, User::Invariant());
	iSaveNumRect = 0;
	iSaveRect = TRect();
	}

CWsEventNotifier* CWsEventNotifier::NewL()
	{
	return new(ELeave) CWsEventNotifier;	
	}
	
CWsEventNotifier::~CWsEventNotifier()
	{
	Env().UnregisterEventHandler(this);
	}

void CWsEventNotifier::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
	iEnv = &aEnv;
	iTestGraphicDrawerEnvironment = EFalse;
	iReady = ETrue;
	}

void CWsEventNotifier::HandleMessage(const TDesC8& aData)
	{
	// wserv already checked data size, and won't invoke this handler if it's empty
	switch (aData[0])
		{				//Set how it will handle the first visibility event
		case KNotifyRemoveSelf:
		case KNotifyRemoveThenAddSelf:
		case KNotifyAddSelf:
		case KNotifyRemoveOther:
		case KNotifyRemoveThenAddOther:
		case KNotifyAddOther:
		case KNotifyRemoveSelfAndOther:
		case KNotifyDoNothing:
			Env().RegisterEventHandler(this, this, TWservCrEvent::EWindowVisibilityChanged);
			iHandleMethod = aData[0];
			break;
		case KNotifyDisable:
			Env().UnregisterEventHandler(this);
			iHandleMethod = KNotifyDoNothing;
			break;
		default:
			break;
		}
	}
	
void CWsEventNotifier::DoHandleEvent(const TWservCrEvent& /*aEvent*/)
	{
	if(!iTestGraphicDrawerEnvironment)
		{
		iTestGraphicDrawerEnvironment = ETrue;
		TestGraphicDrawerEnvironment();
		}
	
	if (iHandleMethod == KNotifyRemoveSelf)
		{
		Env().UnregisterEventHandler(this);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyRemoveThenAddSelf)
		{
		Env().UnregisterEventHandler(this);
		Env().RegisterEventHandler(this, this, TWservCrEvent::EWindowVisibilityChanged);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyAddSelf)
		{
		Env().RegisterEventHandler(this, this, TWservCrEvent::EWindowVisibilityChanged);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyRemoveOther)
		{
		iAnotherPluginID.iId = 0x102754c5;		//First method that needs the address of the other plugin
		iAnotherPluginID.iIsUid = ETrue;
		iAnotherPlugin = const_cast<CWsGraphicDrawer*>(Env().ResolveGraphic(iAnotherPluginID));
		Env().UnregisterEventHandler(iAnotherPlugin);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyRemoveThenAddOther)
		{
		Env().UnregisterEventHandler(iAnotherPlugin);
		Env().RegisterEventHandler(iAnotherPlugin, this, TWservCrEvent::EWindowVisibilityChanged);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyAddOther)
		{
		Env().RegisterEventHandler(iAnotherPlugin, this, TWservCrEvent::EWindowVisibilityChanged);
		iHandleMethod = KNotifyDoNothing;
		}
	else if (iHandleMethod == KNotifyRemoveSelfAndOther)
		{
		Env().UnregisterEventHandler(this);
		Env().UnregisterEventHandler(iAnotherPlugin);
		iHandleMethod = KNotifyDoNothing;
		}
	}

void CWsEventNotifier::DoDraw(MWsGc& /*aGc*/, const TRect& /*aRect*/, const TDesC8& /*aData*/) const
	{
	}

void CWsEventNotifier::TestGraphicDrawerEnvironment()
	{
	const MWsGraphicDrawerEnvironment* aEnvConst = iEnv; 
	TInt lowerbound = -1;
	//-tests Screen methods (const and non const)
	__ASSERT_ALWAYS(iEnv->Screen(lowerbound)==NULL, SendMessage(_L8("Error:iEnv->Screen(lowerbound) - expected: NULL, actual: !NULL (listener.cpp)")));
	__ASSERT_ALWAYS(iEnv->Screen(iEnv->ScreenCount())==NULL, SendMessage(_L8("Error:iEnv->Screen(iEnv->ScreenCount()) - expected: NULL, actual: !NULL (listener.cpp)")));
	__ASSERT_ALWAYS(aEnvConst->Screen(lowerbound)==NULL, SendMessage(_L8("Error:aEnvConst->Screen(lowerbound) - expected: NULL, actual: !NULL (listener.cpp)")));
	__ASSERT_ALWAYS(aEnvConst->Screen(aEnvConst->ScreenCount())==NULL, SendMessage(_L8("Error:aEnvConst->ScreenCount()) - expected: NULL, actual: !NULL (listener.cpp)")));
	//+test const screen method
	const MWsScreen* constScreen = aEnvConst->Screen(aEnvConst->ScreenCount()-1);
	__ASSERT_ALWAYS(constScreen!=NULL, SendMessage(_L8("Error:constScreen - expected: !NULL, actual: NULL (listener.cpp)")));
	//+test resolveobjectinterface method
	MWsActiveSchedulerDebug* scheduler = iEnv->ObjectInterface<MWsActiveSchedulerDebug>();
	__ASSERT_ALWAYS(scheduler!=NULL, SendMessage(_L8("Error:scheduler - expected: !NULL, actual: NULL (listener.cpp)")));
	TUint32 eventMask = 0;
	//-tests for registereventhandler and unregistereventhandler
	__ASSERT_ALWAYS(iEnv->RegisterEventHandler(NULL,this, TWservCrEvent::EWindowClosing)==KErrArgument, SendMessage(_L8("Error:iEnv->RegisterEventHandler(NULL,this, TWservCrEvent::EWindowClosing) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
	__ASSERT_ALWAYS(iEnv->RegisterEventHandler(this,NULL, TWservCrEvent::EWindowClosing)==KErrArgument, SendMessage(_L8("Error:iEnv->RegisterEventHandler(this,NULL, TWservCrEvent::EWindowClosing) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
	__ASSERT_ALWAYS(iEnv->RegisterEventHandler(this,this, eventMask)==KErrArgument, SendMessage(_L8("Error:iEnv->RegisterEventHandler(this,this, eventMask) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
	__ASSERT_ALWAYS(iEnv->UnregisterEventHandler(NULL)==KErrArgument, SendMessage(_L8("Error:iEnv->UnregisterEventHandler(NULL) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
	//-tests for registerwseventhandler
#ifdef SYMBIAN_GRAPHICS_GCE	
	__ASSERT_ALWAYS(iEnv->RegisterWsEventHandler(this, eventMask)==KErrArgument, SendMessage(_L8("Error:iEnv->RegisterWsEventHandler(this, eventMask) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
	__ASSERT_ALWAYS(iEnv->RegisterWsEventHandler(NULL, TWservCrEvent::EWindowClosing)==KErrArgument, SendMessage(_L8("Error:iEnv->RegisterWsEventHandler(NULL, TWservCrEvent::EWindowClosing) - expected: KErrArgument, actual: !KErrArgument (listener.cpp)")));
#endif //SYMBIAN_GRAPHICS_GCE
	}
