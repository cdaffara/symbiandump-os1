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
 @internalTechnology
*/

#ifndef MMDIRECTVIEWFINDER_H
#define MMDIRECTVIEWFINDER_H

#include "mmcameraclientsession.h"

#include <ecam/ecamviewfinderintf.h>

class CMMDirectViewFinder : public CBase, public MCameraV2DirectViewFinder, public MCameraViewFinder
	{
friend class CMMCameraClientSession;

public:
	static CMMDirectViewFinder* NewL(CMMCameraClientSession& aOwner);
	~CMMDirectViewFinder();

	// From MCameraV2DirectViewFinder
	void Release(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle);

	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	TAny* GetDirectSnapshotImpl(TUid aInterface) const;

	void SetDirectViewFinderObserver(MDirectViewFinderObserver& aDirectViewFinderObserver);
	void SetDirectViewFinderHandle(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle);

	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect);
	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice,
											RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect);

	void GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, TRect& aClipRect) const;
	void PauseViewFinderDirect();
	void ResumeViewFinderDirect();
	void GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& aViewFinderState) const;
	void StopDirectViewFinder();

	// From MCameraViewFinder
	void Release();

	void GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const;
	void GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const;
	void SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& aVFFadingEffect);
	void GetViewFinderHandleL(TInt& aVFHandle) const;

private:
	CMMDirectViewFinder(CMMCameraClientSession& aOwner);
	void ConstructL();

private:
	CMMCameraClientSession& iOwner;
	CCamera::CCameraV2DirectViewFinder* iDirectViewFinderHandle;

	MDirectViewFinderObserver* iDirectViewFinderObserver;

	TBool iPrepareCompleted;
	TInt iReferenceCount;
	};

#endif // MMDIRECTVIEWFINDER_H

