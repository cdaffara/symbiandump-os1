// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "offscreenbitmap.h"
#include "inifile.h"
#include "panics.h"
#include "ScrDev.H"
#include "server.h"
#include "wstop.h"
#include "rootwin.h"
#if defined(__WINS__) && defined(_DEBUG)
#include "../../debuglog/osbwin.h"
#endif

GLREF_D TDisplayMode ParseDisplayMode(const TDesC& aModeName);

CWsOffScreenBitmap* CWsOffScreenBitmap::NewL(CScreen* aScreen)
	{
	CWsOffScreenBitmap* self=new(ELeave) CWsOffScreenBitmap(aScreen);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

inline CWsOffScreenBitmap::CWsOffScreenBitmap(CScreen* aScreen) :iScreen(aScreen)
	{}

CWsOffScreenBitmap::~CWsOffScreenBitmap()
	{
	delete iBitmapGc;
	delete iBitmap;
	iObserver = NULL;
#if defined(__WINS__) && defined(_DEBUG)
	delete iOsbWin;
#endif
	delete iBitmapDevice;	
	}

template <class T> 
LOCAL_C inline void Swap(T& aLeft, T& aRight)
		{ T temp=aLeft;aLeft=aRight;aRight=temp;}
template <class T> 
LOCAL_C inline TBool SwapIfLeftIsBigger(T& aLeft, T& aRight)
		{if (aLeft>aRight) { ::Swap(aLeft, aRight); return (aRight>aLeft);} return (aLeft>aRight);}

void CWsOffScreenBitmap::ConstructL()
	{
	_LIT(KFlickerBufferMode,"FLICKERBUFFERMODE");
	TPtrC flickerBufferModeName;
	TDisplayMode displayMode = ENone;
	if (WsIniFile->FindVar(iScreen->ScreenNumber(), KFlickerBufferMode, flickerBufferModeName))
		displayMode = ParseDisplayMode(flickerBufferModeName);
	if (displayMode == ENone)
		displayMode = iScreen->DisplayMode();
	
	WS_ASSERT_DEBUG(displayMode!=ENone, EWsPanicNoDisplayModeFound);
	iBitmap=new(ELeave) CFbsBitmap;
	TSize screenSize=iScreen->ScreenDevice()->SizeInPixels();
	TBool haveSwapped=SwapIfLeftIsBigger(screenSize.iWidth, screenSize.iHeight);
	User::LeaveIfError(iBitmap->Create(screenSize,displayMode));
	if (haveSwapped)
		{
		iBitmap->SwapWidthAndHeight();
		}
	User::LeaveIfError(iBitmap->SetDisplayMode(iScreen->DisplayMode()));
	iBitmapDevice=CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapGc));
	
	TSizeMode sizeMode = iScreen->ScreenSizeModeData(iScreen->ScreenSizeMode());
	TSize osbSize(iBitmap->SizeInPixels());
	TSize osbTwips(sizeMode.iScreenTwipsSize);
	if (osbSize!=sizeMode.iScreenSize)
		{
		// The specified screen twips size is for the specified screen pixel size, however the OSB
		// is potentially larger as it needs to hold the maximum possible screen size, so we need
		// to scale the twips size up correspondingly.
		osbTwips.iWidth=sizeMode.iScreenTwipsSize.iWidth*osbSize.iWidth/sizeMode.iScreenSize.iWidth;
		osbTwips.iHeight=sizeMode.iScreenTwipsSize.iHeight*osbSize.iHeight/sizeMode.iScreenSize.iHeight;
		}
	iBitmap->SetSizeInTwips(osbTwips);
	
#	if defined(__WINS__) && defined(_DEBUG)
	_LIT(KDebugOsb,"DEBUGOSB");
	if (WsIniFile->FindVar(iScreen->ScreenNumber(),KDebugOsb))
		{
		_LIT(KDebugOsbTitleFormat, "Screen %d, offscreen bitmap");
		TBuf<32> title;
		title.Format(KDebugOsbTitleFormat, iScreen->ScreenNumber());
		iOsbWin = CDebugOsbWin::NewL(title, iBitmap->SizeInPixels());
		}
#	endif
	}

TInt CWsOffScreenBitmap::DisplayModeChanged(TBool aSwapWidthAndHeight)
	{
	TInt err=KErrNone;
	if (aSwapWidthAndHeight)
		{
		err=iBitmapDevice->SwapWidthAndHeight();
		WS_ASSERT_DEBUG(err==KErrNone, EWsCreatedOffScreenBitmapInWrongDimensions);
		iBitmapGc->Activate(iBitmapDevice);
		}
	return err;
	}

void CWsOffScreenBitmap::SetObserver(MWsFlickerFreeBufferObserver* aObserver)
	{
	iObserver = aObserver;
	}

MWsFlickerFreeBufferObserver * CWsOffScreenBitmap::Observer()
	{
	return iObserver;
	}

CFbsBitmap* CWsOffScreenBitmap::Bitmap()
	{
	CFbsBitmap* bmp = iRedirectBuffer? iRedirectBuffer->GetBitmap() : iBitmap;
	WS_ASSERT_DEBUG(bmp!=NULL, EWsPanicTemp);
	return bmp;
	}

CFbsDevice* CWsOffScreenBitmap::BitmapDevice()
	{
	CFbsDevice* device = static_cast<CFbsDevice*>(GetBitGcCurrent()->Device());
	WS_ASSERT_DEBUG(device!=NULL, EWsPanicNullDeviceHandle);
	return device;
	}

// This function updates the OffScreenBitmap and TransparencyManager's device and GC
// Note: Here the OffScreenBitmap's scale is not considered.
void CWsOffScreenBitmap::UpdateGc(const TBool aSwapWidthAndHeight)
	{
	if (aSwapWidthAndHeight)
		{
		iBitmapDevice->SwapWidthAndHeight();
		}
	iBitmapDevice->SetScalingFactor(iScreen->CurrentScreenModeScaledOrigin(),1,1,1,1);
	iBitmapGc->Activate(iBitmapDevice);
	}

#if defined(__WINS__) && defined(_DEBUG)
void CWsOffScreenBitmap::Update()
	{
	if (iOsbWin)
		{
		iBitmap->LockHeap();
		iOsbWin->Refresh(iBitmap->SizeInPixels(), iBitmap->DisplayMode(), iBitmap->DataAddress());
		iBitmap->UnlockHeap();
		}
	}
#endif

/**
 Implementing MWsBackBuffer interface
*/
CFbsBitmap* CWsOffScreenBitmap::GetBitmap()
	{
	return iBitmap;
	}

CFbsBitGc* CWsOffScreenBitmap::GetBitGc()
	{
	return iBitmapGc;
	}

CFbsBitGc* CWsOffScreenBitmap::GetBitGcCurrent()
	{
	CFbsBitGc* gc = iRedirectGc? iRedirectGc : (iRedirectBuffer? iRedirectBuffer->GetBitGc() : iBitmapGc);
	WS_ASSERT_DEBUG(gc!=NULL, EWsPanicTemp);
	return gc;
	}
	
TInt CWsOffScreenBitmap::SetBitGc(CFbsBitGc* aBitGc)
	{
	if (iRedirectBuffer)
		return KErrInUse;

	if (aBitGc && (aBitGc==iBitmapGc || aBitGc==iRedirectGc))
		return KErrAlreadyExists;
	
	if (aBitGc && !aBitGc->Device())
		return KErrArgument;
	
	iRedirectGc = aBitGc;
	return KErrNone;
	}

TInt CWsOffScreenBitmap::RedirectTo(MWsBackBuffer* aTarget)
	{
	if (iRedirectGc)
		return KErrInUse;

	if (aTarget && aTarget==iRedirectBuffer)
		return KErrAlreadyExists;
	
	if ( aTarget && iScreen->HasVisibleDirectOnQueue() )
		return KErrInUse;	
	if (aTarget && 
			(
			!aTarget->GetBitmap()|| 
			!aTarget->GetBitGc() || 
			(aTarget->GetBitGc() && !aTarget->GetBitGc()->Device())
			)
		)
		return KErrArgument;

	iRedirectBuffer = aTarget;
	return KErrNone;
	}

TAny* CWsOffScreenBitmap::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case MWsBackBuffer::EWsObjectInterfaceId:
			return static_cast<MWsBackBuffer *>(this);
		}
	return NULL;
	}
