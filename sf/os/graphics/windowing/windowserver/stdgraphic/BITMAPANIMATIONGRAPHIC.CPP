// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "W32STDGRAPHIC.H"
#include <s32mem.h>

LOCAL_C const TUid KStdBitmapAnimationType = {0x10281AAE};

// CWsGraphicBitmapAnimation::CFrame  \\\\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C CWsGraphicBitmapAnimation::CFrame* CWsGraphicBitmapAnimation::CFrame::NewL()
	{
	CFrame* self = new(ELeave) CFrame;
	return self;
	}

CWsGraphicBitmapAnimation::CFrame::CFrame()
	{
	}

EXPORT_C CWsGraphicBitmapAnimation::CFrame::~CFrame()
	{
	delete iBitmap;
	delete iMask;
	}

// public getters/setters

EXPORT_C TFrameInfo CWsGraphicBitmapAnimation::CFrame::FrameInfo() const
	{
	return iFrameInfo;
	}

EXPORT_C void CWsGraphicBitmapAnimation::CFrame::SetFrameInfo(const TFrameInfo& aFrameInfo)
	{
	iFrameInfo = aFrameInfo;
	}

EXPORT_C const CFbsBitmap* CWsGraphicBitmapAnimation::CFrame::Bitmap() const
	{
	return iBitmap;
	}

EXPORT_C void CWsGraphicBitmapAnimation::CFrame::SetBitmap(CFbsBitmap* aBitmap)
	{
	if(iBitmap != aBitmap)
		{
		delete iBitmap;
		iBitmap = aBitmap;
		}
	}

EXPORT_C const CFbsBitmap* CWsGraphicBitmapAnimation::CFrame::Mask() const
	{
	return iMask;
	}

EXPORT_C void CWsGraphicBitmapAnimation::CFrame::SetMask(CFbsBitmap* aMask)
	{
	if(iMask != aMask)
		{
		delete iMask;
		iMask = aMask;
		}
	}

// CWsGraphicBitmapAnimation  \\\\\\\\\\\\\\\\\\\\\\\\\\

CWsGraphicBitmapAnimation::CWsGraphicBitmapAnimation()
	{
	}

EXPORT_C CWsGraphicBitmapAnimation::~CWsGraphicBitmapAnimation()
	{
	}

EXPORT_C CWsGraphicBitmapAnimation* CWsGraphicBitmapAnimation::NewL(const TFrames& aFrames)
	{
	CWsGraphicBitmapAnimation* self = new(ELeave) CWsGraphicBitmapAnimation;
	CleanupStack::PushL(self);
	HBufC8* data = PackLC(aFrames);
	self->BaseConstructL(KStdBitmapAnimationType,*data);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicBitmapAnimation* CWsGraphicBitmapAnimation::NewL(TUid aUid,const TFrames& aFrames)
	{
	CWsGraphicBitmapAnimation* self = new(ELeave) CWsGraphicBitmapAnimation;
	CleanupStack::PushL(self);
	HBufC8* data = PackLC(aFrames);
	self->BaseConstructL(aUid,KStdBitmapAnimationType,*data);
	CleanupStack::PopAndDestroy(data);	
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicBitmapAnimation* CWsGraphicBitmapAnimation::NewL(const TWsGraphicId& aReplace,const TFrames& aFrames)
	{
	CWsGraphicBitmapAnimation* self = new(ELeave) CWsGraphicBitmapAnimation;
	CleanupStack::PushL(self);
	HBufC8* data = PackLC(aFrames);
	self->BaseConstructL(aReplace,KStdBitmapAnimationType,*data);
	CleanupStack::PopAndDestroy(data);	
	CleanupStack::Pop(self);
	return self;
	}

// protected virtuals from CWsGraphic promoted to public

EXPORT_C TInt CWsGraphicBitmapAnimation::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

EXPORT_C TInt CWsGraphicBitmapAnimation::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

EXPORT_C TInt CWsGraphicBitmapAnimation::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

EXPORT_C TInt CWsGraphicBitmapAnimation::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}

void CWsGraphicBitmapAnimation::HandleMessage(const TDesC8& /*aData*/)
	{
	}

void CWsGraphicBitmapAnimation::OnReplace()
	{
	}

HBufC8* CWsGraphicBitmapAnimation::PackLC(const TFrames& aFrames)
	{
	__ASSERT_COMPILE(sizeof(TInt) == sizeof(TInt32));
	const TInt count = aFrames.Count();
	HBufC8* buf = HBufC8::NewL(sizeof(TInt) + ((sizeof(TFrameInfo)+sizeof(TInt)+sizeof(TInt))*count));
	CleanupStack::PushL(buf);
	TPtr8 des = buf->Des();
	RDesWriteStream out(des);
	out.PushL();
	out.WriteInt32L(count);
	for(TInt i=0; i<count; i++)
		{
		const CFrame* frame = aFrames[i];
		if(!frame)
			{
			User::Leave(KErrArgument);
			}
		out.WriteL(reinterpret_cast<const TUint8*>(&(frame->iFrameInfo)),sizeof(TFrameInfo));
		out.WriteInt32L(frame->Bitmap()? frame->Bitmap()->Handle(): 0);
		out.WriteInt32L(frame->Mask()? frame->Mask()->Handle(): 0);
		}
	out.Pop();
	out.CommitL();
	return buf;
	}
