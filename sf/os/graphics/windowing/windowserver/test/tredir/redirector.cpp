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

#include "redirector.h"
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <gdi.h>
#ifdef __WINS__
#include "../debuglog/osbwin.h"
#endif

const TInt KDefaultScreen = 0;

const TUint8 KCmdQuery			= 0;
const TUint8 KCmdSetGcFront 	= 1;
const TUint8 KCmdSetGcBack  	= 2;
const TUint8 KCmdResetGcFront	= 3;
const TUint8 KCmdResetGcBack 	= 4;
const TUint8 KCmdSetBackObject 	= 5;
const TUint8 KCmdResetBackObject= 6;

const TUint8 KRedirectorInfoSig = 0x7e;

NONSHARABLE_STRUCT(TRedirectorInfo)
	{
	TUint8 iSignature;
	TAny* iFrontBufferInterface;
	TAny* iBackBufferInterface;
	TAny* iScreenConfigInterface;
	TInt iScreenBitmapHandle;
	TInt iFlickerBitmapHandle; 
	TInt iUpdateCounter;
	};

NONSHARABLE_STRUCT(TWsBackBuffer): public MWsBackBuffer
	{
public:
	TWsBackBuffer(CFbsBitmap* aBit,CFbsBitGc* aGc): iBit(aBit), iGc(aGc)
		{}
	virtual CFbsBitmap* GetBitmap() 
		{return iBit;}
	virtual CFbsBitGc* GetBitGc() 
		{return iGc;}
	virtual TInt SetBitGc(CFbsBitGc*)
		{return KErrNotSupported;}
	virtual TInt RedirectTo(MWsBackBuffer*)
		{return KErrNotSupported;}
	virtual void SetObserver(MWsFlickerFreeBufferObserver* /*aObserver*/)
		{ASSERT(0);}
	virtual MWsFlickerFreeBufferObserver* Observer()
		{
		 ASSERT(0);
		 return NULL;
		}
	virtual CFbsBitGc* GetBitGcCurrent()
		{
		ASSERT(0);
		return NULL;
		}

private:
	CFbsBitmap* iBit;
	CFbsBitGc* iGc;	
	};

//
CWsGcRedirector* CWsGcRedirector::NewL()
	{
	return new(ELeave) CWsGcRedirector;	
	}
	
CWsGcRedirector::~CWsGcRedirector()
	{
#ifdef __WINS__
	delete iFrontWin;
	delete iBackWin;
	delete iUpdateDebugWinsTimer;
#endif

	if (iFrontBuf)
		iFrontBuf->SetBitGc(NULL);
	if (iBackBuf)
		iBackBuf->SetBitGc(NULL);
	
	delete iFrontGc;
	delete iFrontDev;
	delete iFrontBit;
	delete iBackGc;
	delete iBackDev;
	delete iBackBit;
	delete iBadGc;
	delete iBackObj;
	}
	
void CWsGcRedirector::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData)
	{
	BaseConstructL(aEnv, aId, aOwner);

#ifdef __WINS__
	if (aData.Length()>1)
		iDisableWin = aData[1]==1;
#endif
	iScreenId = KDefaultScreen;
	if (aData.Length()>0)
		iScreenId = aData[0];
	MWsScreen* scr = aEnv.Screen(iScreenId);
	User::LeaveIfNull(scr);

	MWsScreenConfig* cfg = scr->ObjectInterface<MWsScreenConfig>();
	User::LeaveIfNull(cfg);

	iFrontBuf = scr->ObjectInterface<MWsFrontBuffer>();
	User::LeaveIfNull(iFrontBuf);
	iBackBuf = scr->ObjectInterface<MWsBackBuffer>();
	
	iFrontBit = new(ELeave) CFbsBitmap;
	iDefaultSize=cfg->SizeInPixels();
	User::LeaveIfError(iFrontBit->Create(iDefaultSize,EColor64K));
	iFrontDev = CFbsBitmapDevice::NewL(iFrontBit);
	User::LeaveIfNull(iFrontDev);
	User::LeaveIfError(iFrontDev->CreateContext(iFrontGc));
	
	User::LeaveIfError(iFrontDev->CreateContext(iBadGc));
	iBadGc->ChangeDevice(NULL);

#ifdef __WINS__
	if (!iDisableWin)
		{
		iUpdateDebugWinsTimer = CPeriodic::NewL(0);
		iUpdateDebugWinsTimer->Start(0, 300 * 1000, TCallBack(UpdateDebugWindowsCallback, this));

		_LIT(KRedFront, "RedFront");
		iFrontWin = CDebugOsbWin::NewL(KRedFront, iFrontBit->SizeInPixels());
		}
#endif	

	if (iBackBuf)
		{
		CFbsBitmap* bit = iBackBuf->GetBitmap();
		User::LeaveIfNull(bit);
		iBackBit = new(ELeave) CFbsBitmap;
		User::LeaveIfError(iBackBit->Create(bit->SizeInPixels(),EColor64K));
		iBackDev = CFbsBitmapDevice::NewL(iBackBit);
		User::LeaveIfNull(iBackDev);
		User::LeaveIfError(iBackDev->CreateContext(iBackGc));
		
		iBackObj = new(ELeave) TWsBackBuffer(iBackBit,iBackGc);
#ifdef __WINS__		
		if (!iDisableWin)
			{
			_LIT(KRedBack, "RedBack");
			iBackWin = CDebugOsbWin::NewL(KRedBack, iBackBit->SizeInPixels());
			}
#endif		
		}
		
	// compile check, non-const interface access
	MWsGraphicDrawerEnvironment& ee = Env();
	MWsScreen* ss = ee.Screen(iScreenId);
	MWsScreenConfig* cc = ss->ObjectInterface<MWsScreenConfig>();
	}

void CWsGcRedirector::HandleMessage(const TDesC8& aData)
	{
	// wserv already check data size, and won't invoke this handler if it's empty
	TBuf8<1> ack;
	ack.Append(KRedirectorInfoSig);
	
	switch (aData[0])
		{
		case KCmdQuery:
		SendInfo();
		break;
		
		case KCmdSetGcFront:
		Env().RegisterEventHandler(this,this,TWservCrEvent::EScreenUpdated|TWservCrEvent::EScreenOrientationChanged|TWservCrEvent::EDeviceOrientationChanged);

		// -test bad gc
		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(iBadGc)==KErrArgument, User::Invariant());	
		// -test bad gc
		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(iBadGc,ETrue)==KErrArgument, User::Invariant());
		// +test
		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(iFrontGc,EFalse)==KErrNone, User::Invariant());
		// -test duplicate calls
		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(iFrontGc)==KErrAlreadyExists, User::Invariant());
		
		SendMessage(ack);
		break;
		
		case KCmdSetGcBack:
		if (iBackBuf)
			{
			// -test bad gc
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(iBadGc)==KErrArgument, User::Invariant());
			// +test
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(iBackGc)==KErrNone, User::Invariant());
			// -test duplicate calls
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(iBackGc)==KErrAlreadyExists, User::Invariant());
			// -test double redirection
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(iBackObj)==KErrInUse, User::Invariant());
			}
		SendMessage(ack);
		break;
		
		case KCmdResetGcFront:
		Env().UnregisterEventHandler(this);
		iUpdateCounter = 0;

		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(NULL,EFalse)==KErrNone, User::Invariant());
		// -test duplicate calls
		__ASSERT_ALWAYS(iFrontBuf->SetBitGc(NULL,EFalse)==KErrNone, User::Invariant());
		SendMessage(ack);
		break;
		
		case KCmdResetGcBack:
		if (iBackBuf)
			{
			// +test
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(NULL)==KErrNone, User::Invariant());
			// -test duplicate calls
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(NULL)==KErrNone, User::Invariant());
			}
		SendMessage(ack);
		break;

		case KCmdSetBackObject:
		if (iBackBuf)
			{
			TWsBackBuffer badObj1(NULL,iBackGc);
			// -test bad obj (null bitmap)
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(&badObj1)==KErrArgument, User::Invariant());
			// -test bad obj (null gc)
			TWsBackBuffer badObj2(iBackBit,NULL);
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(&badObj2)==KErrArgument, User::Invariant());			
			// -test bad obj (null device)
			TWsBackBuffer badObj3(iBackBit,iBadGc);			
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(&badObj3)==KErrArgument, User::Invariant());
			// +test
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(iBackObj)==KErrNone, User::Invariant());
			// -test duplicate calls
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(iBackObj)==KErrAlreadyExists, User::Invariant());
			// -test double redirection
			__ASSERT_ALWAYS(iBackBuf->SetBitGc(iBackGc)==KErrInUse, User::Invariant());
			}
		SendMessage(ack);
		break;

		case KCmdResetBackObject:
		if (iBackBuf)
			{
			// +test
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(NULL)==KErrNone, User::Invariant());
			// -test duplicate calls
			__ASSERT_ALWAYS(iBackBuf->RedirectTo(NULL)==KErrNone, User::Invariant());
			}
		SendMessage(ack);
		break;
		}
	}
	
void CWsGcRedirector::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const
	{
	aGc.PushBitGcSettings();
	CFbsBitGc& bc = aGc.BitGc();	
	bc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	bc.SetBrushColor(TRgb(255,0,0,128));
	bc.DrawRect(aRect);
	TGraphicDrawerId id;
	id.iId = 0x10281fb6;
	id.iIsUid = ETrue;
	const CWsGraphicDrawer* listener = Env().ResolveGraphic(id);
	if (listener)
		listener->Draw(aGc, aRect, aData);
	aGc.PopBitGcSettings();
	
	// compile check, const interface access
	const MWsGraphicDrawerEnvironment& env = Env(); // Env() const
	const MWsScreen* scr = env.Screen(iScreenId); // Screen(TInt) const
	const MWsScreenConfig* cfg = scr->ObjectInterface<MWsScreenConfig>(); // ObjectInterface() const
	}

void CWsGcRedirector::SendInfo()
	{
	TPckgBuf<TRedirectorInfo> buf;
	MWsScreen* scr = Env().Screen(iScreenId);
	if (scr)
		{
		buf().iSignature = KRedirectorInfoSig;
		buf().iFrontBufferInterface = scr->ObjectInterface<MWsFrontBuffer>();
		buf().iBackBufferInterface = scr->ObjectInterface<MWsBackBuffer>();
		buf().iScreenConfigInterface = scr->ObjectInterface<MWsScreenConfig>();
		buf().iScreenBitmapHandle = iFrontBit->Handle();
		buf().iFlickerBitmapHandle = iBackBit? iBackBit->Handle() : 0;
		buf().iUpdateCounter = iUpdateCounter;
		}
	TInt err = SendMessage(buf);
	__ASSERT_ALWAYS(err>=KErrNone, User::Invariant());
	}

void CWsGcRedirector::DoHandleEvent(const TWservCrEvent& aEvent)
	{
	if (aEvent.ScreenNumber()==iScreenId)
		{
		switch(aEvent.Type())
			{
		case TWservCrEvent::EScreenUpdated:
			++iUpdateCounter;
			break;
		case TWservCrEvent::EDeviceOrientationChanged:
		case TWservCrEvent::EScreenOrientationChanged:
			{
			CFbsBitGc::TGraphicsOrientation orientation=aEvent.Orientation();
			TSize bmpSize;
			if (orientation==CFbsBitGc::EGraphicsOrientationNormal || orientation==CFbsBitGc::EGraphicsOrientationRotated180)
				bmpSize=iDefaultSize;
			else
				bmpSize.SetSize(iDefaultSize.iHeight,iDefaultSize.iWidth);
			const TSize fBufSize(iFrontBit->SizeInPixels());
			if (fBufSize!=bmpSize)
				{
				__ASSERT_DEBUG(fBufSize.iWidth==bmpSize.iHeight && fBufSize.iHeight==bmpSize.iWidth, User::Invariant());
				iFrontDev->SwapWidthAndHeight();
				iFrontGc->Activate(iFrontDev);
				}
			}
			break;
			}
		}
	}

#ifdef __WINS__
TInt CWsGcRedirector::UpdateDebugWindowsCallback(TAny* aSelf)
	{
	CWsGcRedirector* self = static_cast<CWsGcRedirector*>(aSelf);
	if(!self)
		return KErrArgument;

	if(self->iFrontWin && self->iFrontBit)
		{
		CFbsBitmap* bitmap = self->iFrontBit;
		bitmap->LockHeap();
		self->iFrontWin->Refresh(bitmap->SizeInPixels(), bitmap->DisplayMode(), bitmap->DataAddress());
		bitmap->UnlockHeap();
		}

	if(self->iBackWin && self->iBackBit)
		{
		CFbsBitmap* bitmap = self->iBackBit;
		bitmap->LockHeap();
		self->iBackWin->Refresh(bitmap->SizeInPixels(), bitmap->DisplayMode(), bitmap->DataAddress());
		bitmap->UnlockHeap();
		}
	return KErrNone;
	}
#endif
