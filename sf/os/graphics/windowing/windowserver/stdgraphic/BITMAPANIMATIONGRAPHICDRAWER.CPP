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

#include "stdgraphicdrawer.h"
#include "wsgraphicdrawercontext.h"
#include "graphics/WSGRAPHICMSGBUF.H"
#include "graphics/W32STDGRAPHICTEST.H"
#include <s32mem.h>
#include <fbs.h>
#include "W32STDGRAPHIC.H"

// CWsGraphicDrawerBitmapAnimation::CFrame \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 

NONSHARABLE_STRUCT(CWsGraphicDrawerBitmapAnimation::CFrame): public CBase
	{
	~CFrame();
	TFrameInfo iFrameInfo;
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;
	mutable RRegionBuf<12> iVisibleRegion;
	};

CWsGraphicDrawerBitmapAnimation::CFrame::~CFrame()
	{
	delete iBitmap;
	delete iMask;
	iVisibleRegion.Close();
	}

// CWsGraphicDrawerBitmapAnimation \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 

CWsGraphicDrawerBitmapAnimation* CWsGraphicDrawerBitmapAnimation::CreateL()
	{
	return new(ELeave) CWsGraphicDrawerBitmapAnimation;
	}

CWsGraphicDrawerBitmapAnimation::CWsGraphicDrawerBitmapAnimation()
	{
	}

CWsGraphicDrawerBitmapAnimation::~CWsGraphicDrawerBitmapAnimation()
	{
	if (iContext)
		{
		iContext->Destroy();
		iContext = NULL;
		}
	iFrames.ResetAndDestroy();
	}

void CWsGraphicDrawerBitmapAnimation::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData)
	{
	__ASSERT_COMPILE(sizeof(TInt) == sizeof(TInt32));
	RDesReadStream in(aData);
	in.PushL();
	const TInt count = in.ReadInt32L();
	for(TInt i=0; i<count; i++)
		{
		CFrame* frame = new(ELeave) CFrame;
		CleanupStack::PushL(frame);
		in.ReadL(reinterpret_cast<TUint8*>(&(frame->iFrameInfo)),sizeof(TFrameInfo));
		const TInt bitmapHandle = in.ReadInt32L();
		if(bitmapHandle)
			{
			frame->iBitmap = new(ELeave) CFbsBitmap;
			User::LeaveIfError(frame->iBitmap->Duplicate(bitmapHandle));
			}
		const TInt maskHandle = in.ReadInt32L();
		if(maskHandle)
			{
			frame->iMask = new(ELeave) CFbsBitmap;
			User::LeaveIfError(frame->iMask->Duplicate(maskHandle));
			}
		iFrames.AppendL(frame);
		CleanupStack::Pop(frame);
		TInt64 delay = frame->iFrameInfo.iDelay.Int64();
		if((delay < 0) || (delay > KMaxTUint32))
			{
			User::Leave(KErrCorrupt);
			}
		iAnimationLength += delay;
		}
	in.Pop();
	BaseConstructL(aEnv,aId,aOwner);
	if (!(aEnv.Screen(0)->ResolveObjectInterface(KMWsCompositionContext) || aEnv.Screen(0)->ResolveObjectInterface(KMWsScene)))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}

void CWsGraphicDrawerBitmapAnimation::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const
	{
	const TInt count = iFrames.Count();
	if(0 >= count)
		{
		return;
		}

	TWsGraphicMsgBufParser buf(aData);
	if(KErrNone != buf.Verify())
		{
		return;
		}
			
	TWsGraphicMsgAnimation anim;
 	if (KErrNone != anim.Load(buf))
 		{
 		return;
 		}

	if (KErrNone != iContext->Push(aGc))
		{
		return;
		}

	const TInt64 now_microseconds = (iAnimationLength ? anim.AnimationTime(iContext->Now(aGc),iAnimationLength).Int64(): 0LL);
	TInt64 time_microseconds = 0LL;

	// find end frame
	TInt endFrame = 0;
	while((endFrame<count) && (time_microseconds <= now_microseconds))
		{
		// work out timing
		time_microseconds += iFrames[endFrame]->iFrameInfo.iDelay.Int64();
		endFrame++;
		}

	TBool drawError = EFalse;
	// work out visible regions
	for(TInt i = 0; (i < endFrame) && !drawError; i++)
		{
		const CFrame* frame = iFrames[i];
		const TRect frameRect(frame->iFrameInfo.iFrameCoordsInPixels);
 		frame->iVisibleRegion.Clear();

 		if((i == (endFrame - 1)) || frame->iFrameInfo.iFlags & TFrameInfo::ELeaveInPlace)	// ELeave - Enum TFrameInfo::ELeaveInPlace triggers leavescan
 			{
 			frame->iVisibleRegion.AddRect(frameRect);
 			drawError = frame->iVisibleRegion.CheckError();
 			}
 		else
   			{
   			if(frame->iFrameInfo.iFlags & TFrameInfo::ERestoreToBackground) 
 				{
 				for(TInt j = 0; j <= i; j++)
 					{
 					iFrames[j]->iVisibleRegion.SubRect(frameRect);
 					// coverity[unchecked_value]
 					drawError |= iFrames[j]->iVisibleRegion.CheckError();
 					}
 				}
 			else if(!(frame->iFrameInfo.iFlags & TFrameInfo::ERestoreToPrevious))  // if no disposal method is set, treat it as leave in place
 				{
 				frame->iVisibleRegion.AddRect(frameRect);
 				drawError = frame->iVisibleRegion.CheckError();
 				}
   			}
		}

	//draw each of the visible sub frames
	for(TInt i = 0; (i < endFrame) && !drawError; i++)
		{
		const CFrame* frame = iFrames[i];
		if(frame->iBitmap)
			{
			const TRegionFix<1> bitmapRegion(frame->iFrameInfo.iFrameCoordsInPixels);
			frame->iVisibleRegion.Intersect(bitmapRegion);
			frame->iVisibleRegion.Tidy();
			if(frame->iVisibleRegion.CheckError())
				{
				drawError = ETrue;
				break;
				}
			else
				{
				const TInt clipCount = frame->iVisibleRegion.Count();
 				for(TInt j = 0; j < clipCount; j++)
					{
 					TRect frameRect = frame->iVisibleRegion.RectangleList()[j];
 					TRect clipRect(frameRect);
 					clipRect.Move(aRect.iTl);
 					clipRect.Intersection(aRect);

 					frameRect.Move(aRect.iTl);
 					frameRect.Intersection(aRect);
 					frameRect.Move(-aRect.iTl);
 					frameRect.Move(-frame->iFrameInfo.iFrameCoordsInPixels.iTl);

					if(!clipRect.IsEmpty() && !frameRect.IsEmpty())
						{
						iContext->DrawBitmap(aGc,clipRect.iTl, frame->iBitmap, frameRect, frame->iMask);
						}
					}
				}
			}
		}

	if(0 <= buf.Find(TUid::Uid(TWsGraphicFrameRate::ETypeId)))
		{
		iContext->DrawFrameRate(aGc,aRect,iFps);
		}
	iFps.Sample();
	
	if(anim.IsPlaying(iContext->Now(aGc),iAnimationLength))
		{
		iContext->ScheduleAnimation(aGc, aRect,(time_microseconds - now_microseconds));
		}
	else if(drawError)
 		{
 		iContext->ScheduleAnimation(aGc, aRect,now_microseconds + 1000000); // retry in 1 second
 		}
 
	iContext->Pop(aGc);
	}

void CWsGraphicDrawerBitmapAnimation::HandleMessage(const TDesC8& /*aData*/)
	{
	}

