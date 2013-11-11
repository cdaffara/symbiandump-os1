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
 @internalComponent
*/

#include "mmdirectviewfinder.h"

#include <graphics/surfaceconfiguration.h>
#include "w32std.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamdef.h>
#endif

CMMDirectViewFinder* CMMDirectViewFinder::NewL(CMMCameraClientSession& aOwner)
	{
	CMMDirectViewFinder* self = new (ELeave) CMMDirectViewFinder(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMMDirectViewFinder::ConstructL()
	{
	}

CMMDirectViewFinder::CMMDirectViewFinder(CMMCameraClientSession& aOwner)
	: iOwner(aOwner), iDirectViewFinderObserver(NULL), iPrepareCompleted(EFalse)
	{
	iReferenceCount++;
	}

CMMDirectViewFinder::~CMMDirectViewFinder()
	{
	}

void CMMDirectViewFinder::Release(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle)
	{
	THashMapIter<TInt, CCamera::CCameraV2DirectViewFinder*> iter(iOwner.iHashMap);
	for (CCamera::CCameraV2DirectViewFinder* const* vF = iter.NextValue(); vF; vF = iter.NextValue())
		{
		if (*vF == aDirectViewFinderHandle)
			{
			iOwner.iHashMap.Remove(*iter.CurrentKey());
			break;
			}
		}
	}

void CMMDirectViewFinder::CreateHistogramImplFactoryL(MImplementationFactory*& /*aImplFactoryPtr*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CMMDirectViewFinder::GetImageProcessingImplFactoryL(MImplementationFactory*& /*aImplFactoryPtr*/) const
	{
	User::Leave(KErrNotSupported);
	}

TAny* CMMDirectViewFinder::GetDirectSnapshotImpl(TUid /*aInterface*/) const
	{
	return NULL;
	}

void CMMDirectViewFinder::SetDirectViewFinderObserver(MDirectViewFinderObserver& aDirectViewFinderObserver)
	{
	iDirectViewFinderObserver = &aDirectViewFinderObserver;
	}

void CMMDirectViewFinder::SetDirectViewFinderHandle(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle)
	{
	TInt position = iOwner.iHashMap.Count();
	iDirectViewFinderHandle = aDirectViewFinderHandle;
	iOwner.iHashMap.Insert(position, iDirectViewFinderHandle);
	}

void CMMDirectViewFinder::StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect)
	{
	TRect clip;
	clip.SetWidth(0);
	clip.SetHeight(0);

	StartViewFinderDirectL(aWs, aScreenDevice, aWindow, aScreenRect, clip);
	}

void CMMDirectViewFinder::StartViewFinderDirectL(RWsSession& /*aWs*/, CWsScreenDevice& aScreenDevice,
											RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect)
	{
	// Make sure device is powered up (implies it has been reserved)
	if (!iOwner.iPoweredUp)
		{
		User::Leave(KErrNotReady);
		}

	// Ensure start is not called twice
	CCamera::CCameraV2DirectViewFinder::TViewFinderState state;
	GetViewFinderStateL(state);
	if (state != CCamera::CCameraV2DirectViewFinder::EViewFinderInActive)
		{
		User::Leave(KErrNotReady);
		}

	TInt error = KErrNone;
	if (!iPrepareCompleted)
		{
		TDirectViewFinderInfo directViewFinderInfo;
		// Retrieve screen number from the screen device
		directViewFinderInfo.iScreenNum = aScreenDevice.GetScreenNumber();
		directViewFinderInfo.iScreenRect = aScreenRect;
		directViewFinderInfo.iClipRect = aClipRect;
		TDirectViewFinderInfoPckg directViewFinderInfoPckg(directViewFinderInfo);

		// Prepare viewfinder for playback and retrieve the surface id from the graphics sink
		error = iOwner.iCameraSession.SendMessage(ECamPrepareDirectViewFinder, directViewFinderInfoPckg);
		if (error == KErrNone)
			{
			TSurfaceConfiguration surfaceConfig;
			directViewFinderInfo = directViewFinderInfoPckg();

			error = surfaceConfig.SetSurfaceId(directViewFinderInfo.iSurfaceId);
			if (error == KErrNone)
				{
				//	User::LeaveIfError(surfaceConfig.SetOrientation(CFbsBitGc::EGraphicsOrientationRotated90));
				error = surfaceConfig.SetExtent(aScreenRect);
				if (error == KErrNone)
					{
					//	User::LeaveIfError(aWindow.SetRequiredDisplayMode(EColor64K));
					error = aWindow.SetBackgroundSurface(surfaceConfig, ETrue);
					if (error == KErrNone)
						{
						iPrepareCompleted = ETrue;
						}
					}
				}
			}
		}

	if (error != KErrNone)
		{
		User::Leave(error);
		}

	// Start the viewfinder
	error = iOwner.iCameraSession.SendMessage(ECamStartDirectViewFinder);
	if (error != KErrNone)
		{
		User::Leave(error);
		}

	// HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!!
	// TODO: Here to let tests run. Will be removed once foundation provide event registration/notification APIs
	iDirectViewFinderObserver->DirectViewFinderFirstFrameDisplayed(*iDirectViewFinderHandle, KErrNone);
	}

void CMMDirectViewFinder::GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, TRect& aClipRect) const
	{
	if (!iPrepareCompleted)
		{
		User::Leave(KErrNotReady);
		}

	TDirectViewFinderInfo info;
	TDirectViewFinderInfoPckg pckg(info);

	iOwner.iCameraSession.SendRxMessage(ECamDirectViewFinderProperties, pckg);
	info = pckg();
	aScreenNumber = info.iScreenNum;
	aScreenRect = info.iScreenRect;
	aClipRect = info.iClipRect;
	}

void CMMDirectViewFinder::PauseViewFinderDirect()
	{
	iOwner.iCameraSession.SendMessage(ECamPauseDirectViewFinder);
	}

void CMMDirectViewFinder::ResumeViewFinderDirect()
	{
	iOwner.iCameraSession.SendMessage(ECamResumeDirectViewFinder);
	// HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!! HACK!!!
	// TODO: Here to let tests run. Will be removed once foundation provide event registration/notification APIs
	iDirectViewFinderObserver->DirectViewFinderFirstFrameDisplayed(*iDirectViewFinderHandle, KErrNone);	
	}

void CMMDirectViewFinder::GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& aViewFinderState) const
	{
	TDirectViewFinderState state;
	TDirectViewFinderStatePckg pckg(state);

	User::LeaveIfError(iOwner.iCameraSession.SendRxMessage(ECamDirectViewFinderState, pckg));

	state = pckg();
	aViewFinderState = state.iState;
	}

void CMMDirectViewFinder::StopDirectViewFinder()
	{
	iOwner.iCameraSession.SendMessage(ECamStopDirectViewFinder);
	}

void CMMDirectViewFinder::Release()
	{
	iReferenceCount--;
	if (iReferenceCount == 0)
		{
		delete this;
		}
	}

void CMMDirectViewFinder::GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& /*aVFFadingCapabilities*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CMMDirectViewFinder::GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& /*aCurrentVFFadingEffect*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CMMDirectViewFinder::SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& /*aVFFadingEffect*/)
	{
	TECAMEvent event(KUidECamEvent2ViewFinderFadingEffect, KErrNotSupported);
	iOwner.iCameraObserver2->HandleEvent(event);
	}

void CMMDirectViewFinder::GetViewFinderHandleL(TInt& aVFHandle) const
	{
	THashMapIter<TInt, CCamera::CCameraV2DirectViewFinder*> iter(iOwner.iHashMap);
	for (CCamera::CCameraV2DirectViewFinder* const* vF = iter.NextValue(); vF; vF = iter.NextValue())
		{
		if (*vF == iDirectViewFinderHandle)
			{
			// Handle returned is position within the hash table + 100 (arbitrary value to show uniquenness of handle)
			aVFHandle = *iter.CurrentKey() + 100;
			return;
			}
		}
	User::Leave(KErrNotFound);
	}
