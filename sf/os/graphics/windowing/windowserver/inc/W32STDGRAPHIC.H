// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Standard CWsGraphic plugins
// 
//

#ifndef __W32STDGRAPHIC_H__
#define __W32STDGRAPHIC_H__

#include <w32std.h>
#include <icl/imagedata.h>
	
class TWsGraphicAnimation: public TWsGraphicMsgFixedBase
/** data attached to a CWindowGc::DrawWsGraphic to allow the artwork to understand VCR-style
	play-stop-pause commands.
	@publishedAll
	@released
*/	{
public:
	enum
		{
		ETypeId = 0x10281AB1
		};
	IMPORT_C TWsGraphicAnimation();
	IMPORT_C void Play(TBool aLoop);
	IMPORT_C void Pause();
	IMPORT_C void Stop(TBool aImmediately);
	IMPORT_C TBool IsPlaying() const;
	IMPORT_C TBool IsStopped() const;
	IMPORT_C TBool IsStopping() const;
	IMPORT_C TBool IsPaused() const;
	IMPORT_C TBool Loops() const;
private:
	TUint State() const;
	void SetState(TUint aState);
private:
	enum 
		{
		// animation state in lowest nibble
		EStopped	= 0x00,
		EPlaying	= 0x01,
		EStopping	= 0x02,
		EPaused		= 0x03,
		EStateMask	= 0x0F,
		// other flags above
		ELoop		= 0x10,
		};
	TUint iFlags;
	TTime iPlay;
	TTime iPauseOrStopping;
	};

NONSHARABLE_CLASS(CWsGraphicBitmap): public CWsGraphic
/** Client representation of a window-server-side CFbsBitmap owned by this Client

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CWsGraphicBitmap* NewL(const CFbsBitmap* aBitmap,const CFbsBitmap* aMask);
	IMPORT_C static CWsGraphicBitmap* NewL(TUid aUid,const CFbsBitmap* aBitmap,const CFbsBitmap* aMask);
	IMPORT_C static CWsGraphicBitmap* NewL(const TWsGraphicId& aReplace,const CFbsBitmap* aBitmap,const CFbsBitmap* aMask);
	IMPORT_C ~CWsGraphicBitmap();
public: // protected virtuals from CWsGraphic promoted to public
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
private:
	CWsGraphicBitmap();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
};

NONSHARABLE_CLASS(CWsGraphicBitmapAnimation): public CWsGraphic
/** Client representation of a window-server-side CFbsBitmap owned by this Client

@publishedAll
@released
*/
	{
public:
	NONSHARABLE_CLASS(CFrame): public CBase
		{
	public:
		IMPORT_C static CFrame* NewL();
		IMPORT_C ~CFrame();
		// public getters/setters
		IMPORT_C TFrameInfo FrameInfo() const;
		IMPORT_C void SetFrameInfo(const TFrameInfo& aFrameInfo);
		IMPORT_C const CFbsBitmap* Bitmap() const;
		IMPORT_C void SetBitmap(CFbsBitmap* aBitmap);
		IMPORT_C const CFbsBitmap* Mask() const;
		IMPORT_C void SetMask(CFbsBitmap* aMask);
	private:
		CFrame();
	private:
		friend class CWsGraphicBitmapAnimation;
		TFrameInfo iFrameInfo;
		CFbsBitmap* iBitmap;
		CFbsBitmap* iMask;
		};
	typedef TArray<CFrame*> TFrames;
public:
	IMPORT_C static CWsGraphicBitmapAnimation* NewL(const TFrames& aFrames);
	IMPORT_C static CWsGraphicBitmapAnimation* NewL(TUid aUid,const TFrames& aFrames);
	IMPORT_C static CWsGraphicBitmapAnimation* NewL(const TWsGraphicId& aReplace,const TFrames& aFrames);
	IMPORT_C ~CWsGraphicBitmapAnimation();
public: // protected virtuals from CWsGraphic promoted to public
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
private:
	CWsGraphicBitmapAnimation();
	static HBufC8* PackLC(const TFrames& aFrames);
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
};

#endif //__W32STDGRAPHIC_H__
