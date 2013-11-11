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

#if !defined(__OFFSCREENBITMAP_H__)
#define __OFFSCREENBITMAP_H__

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>

#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>

class CFbsBitmapDevice;
class CWsClientWindow;
class CWsWindow;
class CWsWindowBase;
class CWsClient;
class CScreen;
class DWsBitmap;

#if defined(__WINS__) && defined(_DEBUG)
class CDebugOsbWin;
#endif

class CWsOffScreenBitmap : public CBase, public MWsBackBuffer
	{
public: // Construction / Destruction
	static CWsOffScreenBitmap* NewL(CScreen* aScreen);
	~CWsOffScreenBitmap();
protected:
	CWsOffScreenBitmap(CScreen* aScreen);
	void ConstructL();
public: // Bitmap
	TInt DisplayModeChanged(TBool aSwapWidthAndHeight);
	void UpdateGc(const TBool aSwapWidthAndHeight);
public:
	CFbsBitmap* Bitmap();
	CFbsDevice* BitmapDevice();
	
public: // implementing MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);
	
public: // implementing MWsBackBuffer
	CFbsBitmap* GetBitmap();
	CFbsBitGc* GetBitGc();
	CFbsBitGc* GetBitGcCurrent();
	TInt SetBitGc(CFbsBitGc* aBitGc);
	TInt RedirectTo(MWsBackBuffer* aTarget);
	void SetObserver(MWsFlickerFreeBufferObserver* aObserver);
	MWsFlickerFreeBufferObserver* Observer();
#if defined(__WINS__) && defined(_DEBUG)
	void Update();
#endif	

protected:	// Data
	CFbsBitGc* iBitmapGc;
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iBitmapDevice;
	CScreen* iScreen;
	
private:	// Data
#if defined(__WINS__) && defined(_DEBUG)
	CDebugOsbWin* iOsbWin;
#endif

private:	// Data
	// used by MWsBackBuffer to redirect OSB output
	CFbsBitGc* iRedirectGc;
	MWsBackBuffer* iRedirectBuffer;
	MWsFlickerFreeBufferObserver* iObserver;
	};

#endif // __OFFSCREENBITMAP__
