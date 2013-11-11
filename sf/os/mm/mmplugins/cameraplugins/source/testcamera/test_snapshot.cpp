// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// testsnapshot.cpp
// 
//

#include "test_snapshot.h"
#include <ecamerrors.h>

CTestCamSnapshot::CTestCamSnapshot(CTestCamera& aOwner): iOwner(aOwner), iSnapshotBuffer(iSnapshot)
	{
	Init();
	}
	
CTestCamSnapshot::~CTestCamSnapshot()
	{
	delete iSnapshotImage;
	iSnapshotImage = NULL;
	delete iSnapshotImageGc;
	iSnapshotImage = NULL;
	delete iSnapshotImageDev;
	iSnapshotImageDev = NULL;
	
	delete iSnapshot;
	iSnapshot = NULL;
	delete iSnapshotGc;
	iSnapshotGc = NULL;
	delete iSnapshotDev;
	iSnapshotDev = NULL;
	
	iSupportedSnapshotRect.Close();
	
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	if(globalData != NULL)
		{
		if(!globalData->iTestCamPresets && !globalData->iTestCamAdvSet && !globalData->iTestCamImgProc)
			{
			delete globalData;
			Dll::FreeTls();
			}	
		else
			{
			globalData->iTestCamSnapshot = NULL;	
			Dll::SetTls(globalData);
			}
		}
	}
		
CTestCamSnapshot* CTestCamSnapshot::NewL(CTestCamera& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(globalData == NULL)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		globalData->iSnapshotReferenceCount = 0;
		globalData->iTestCamSnapshot = new (ELeave) CTestCamSnapshot(aOwner);
		CleanupStack::PushL(globalData->iTestCamSnapshot);
		globalData->iTestCamSnapshot->ConstructL();
		globalData->iTestCamSnapshot->iRefCount = 1;
		User::LeaveIfError(Dll::SetTls(globalData));
		CleanupStack::Pop(globalData->iTestCamSnapshot);
		CleanupStack::Pop(globalData);
		return globalData->iTestCamSnapshot;
		}
	else
		{
		if(globalData->iTestCamSnapshot == NULL)
			{
			globalData->iSnapshotReferenceCount = 0;
			globalData->iTestCamSnapshot = new (ELeave) CTestCamSnapshot(aOwner);
			CleanupStack::PushL(globalData->iTestCamSnapshot);
			globalData->iTestCamSnapshot->ConstructL();
			globalData->iTestCamSnapshot->iRefCount = 1;
			User::LeaveIfError(Dll::SetTls(globalData));
			CleanupStack::Pop(globalData->iTestCamSnapshot);
			return globalData->iTestCamSnapshot;
			}
		
		CTestCamSnapshot* self = globalData->iTestCamSnapshot;
		
		globalData->iSnapshotReferenceCount++;
		self->iRefCount = globalData->iSnapshotReferenceCount + 1;
		if (globalData->iSnapshotReferenceCount == KNumOfSnapshotExtensions-1 )
			{
			globalData->iTestCamSnapshot = NULL;
			if(!globalData->iTestCamPresets && !globalData->iTestCamAdvSet && !globalData->iTestCamImgProc)
				{
				delete globalData;
				Dll::FreeTls();
				}
			else
				{
				User::LeaveIfError(Dll::SetTls(globalData));	
				}
			}
		else
			{
			User::LeaveIfError(Dll::SetTls(globalData));	
			}
		return self;		
		}
	}

void CTestCamSnapshot::Release()
	{
	iRefCount--; 
	if(iRefCount == 0)
		{
		iOwner.iSnapshotImpl = NULL;
		delete this;
		}
	}	
	
void CTestCamSnapshot::Init()
	{
	iImageFormatsSupported = 0;
	iImageFormatsSupported |= CCamera::EFormatFbsBitmapColor4K;
	iImageFormatsSupported |= CCamera::EFormatFbsBitmapColor64K;
	iImageFormatsSupported |= CCamera::EFormatFbsBitmapColor16M; 
	iSnapshotPrepared = EFalse;
	iSnapshotActive = EFalse;
	iBgColor = KRgbWhite;
	}

void CTestCamSnapshot::ConstructL()
	{
	iSupportedSnapshotRect.Reset();
	iSupportedSnapshotRect.AppendL(TRect(TPoint(0,0), iOwner.iImageSizes[0]));
	iSupportedSnapshotRect.AppendL(TRect(TPoint(0,0), iOwner.iImageSizes[1]));
	}

TUint32 CTestCamSnapshot::SupportedFormats()
	{
	return iImageFormatsSupported;
	}
	
void CTestCamSnapshot::PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool /*aMaintainAspectRatio*/)
	{
	iOwner.CheckReserveAndPowerL();
		
	if (iOwner.iVideoCaptureActive || iOwner.iImageCaptureActive)
		{
		User::Leave(KErrNotReady);
		}
		
	if (!(aFormat & iImageFormatsSupported) )
		{
		User::Leave(KErrNotSupported);
		}
	
	//check if the snapshot size is supported
	TRect snapshot_rect = TRect(aPosition, aSize);
	
	if(snapshot_rect != iSupportedSnapshotRect[0] && snapshot_rect != iSupportedSnapshotRect[1])
		{
		User::Leave(KErrNotSupported);
		}

	if (iSnapshotPrepared
		&& iSnapshotFormat == aFormat
		&& iSnapshotImageRect == snapshot_rect
		&& iBgColor == aBgColor
		)
		{
		return;
		}

	TDisplayMode displayMode = ENone;
	switch (aFormat)
		{
		case CCamera::EFormatFbsBitmapColor4K :
			displayMode = EColor4K;
			break;
		case CCamera::EFormatFbsBitmapColor64K :
			displayMode = EColor64K;
			break;
		case CCamera::EFormatFbsBitmapColor16M :
			displayMode = EColor16M;
			break;
		default :
			User::Leave(KErrNotSupported);
			break;
		}

	delete iSnapshotImage;
	iSnapshotImage = NULL;
	delete iSnapshotImageGc;
	iSnapshotImageGc = NULL;
	delete iSnapshotImageDev;
	iSnapshotImageDev = NULL;

	iSnapshotImage = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iSnapshotImage->Create(aSize, displayMode));
	iSnapshotImageDev = CFbsBitmapDevice::NewL(iSnapshotImage);
	User::LeaveIfError(iSnapshotImageDev->CreateContext(iSnapshotImageGc));
	iSnapshotImageGc->SetPenColor(KRgbBlack);
	iSnapshotImageGc->SetPenSize(TSize(KFrameFeatureBorderThickness, KFrameFeatureBorderThickness));
	iSnapshotImageGc->SetBrushColor(aBgColor);
	iSnapshotImageGc->SetFaded(ETrue);
	
	iSnapshotPrepared = ETrue;
	iSnapshotImageRect = snapshot_rect;
	iSnapshotFormat = aFormat;
	iBgColor = aBgColor;
	}
	
void CTestCamSnapshot::PrepareSnapshotL(CCamera::TFormat /*aFormat*/, const TSize& /*aSize*/, TBool /*aMaintainAspectRatio*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamSnapshot::SetBgColorL(const TRgb& /*aBgColor*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamSnapshot::SetPositionL(const TPoint& /*aPosition*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool CTestCamSnapshot::IsSnapshotActive() const
	{
	return iSnapshotActive;
	}

void CTestCamSnapshot::StartSnapshot()
	{
	TInt error = iOwner.CheckReserveAndPower();
	
	if(error == KErrNone)
		{
		if(!iSnapshotPrepared)
			{
			error = KErrNotReady;
			}
		}
	
	if(error != KErrNone)
		{
		TECAMEvent ecamevent(KUidECamEventCameraSnapshot, error);
		
		iOwner.iECamEvent = ecamevent;
		iOwner.iHandleEventAsync.CallBack();	
		return;	
		}
		
	delete iSnapshot;
	iSnapshot = NULL;
	delete iSnapshotGc;
	iSnapshot = NULL;
	delete iSnapshotDev;
	iSnapshotDev = NULL;
	
	TRAP(error, DoStartSnapshotL());
	
	if(error != KErrNone)
		{
		TECAMEvent ecamevent(KUidECamEventCameraSnapshot, error);
		
		iOwner.iECamEvent = ecamevent;
		iOwner.iHandleEventAsync.CallBack();
		}
	}
	
void CTestCamSnapshot::DoStartSnapshotL()
	{
	CFbsBitmap* snapshot = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(snapshot);
	User::LeaveIfError(snapshot->Create(iSnapshotImageRect.Size(), iSnapshotImage->DisplayMode()));
	CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(snapshot);
	CleanupStack::PushL(dev);
	CFbsBitGc* gc = NULL;
	User::LeaveIfError(dev->CreateContext(gc));
	CleanupStack::Pop(dev);
	CleanupStack::Pop(snapshot);
	
	iSnapshot = snapshot;
	iSnapshotDev = dev;
	iSnapshotGc = gc;
		
	iSnapshotActive = ETrue;
	}

void CTestCamSnapshot::StopSnapshot()
	{
	delete iSnapshotGc;
	iSnapshotGc = NULL;
	delete iSnapshotDev;
	iSnapshotDev = NULL;
	delete iSnapshot;
	iSnapshot = NULL;
	
	iSnapshotActive = EFalse;
	}
        	
MCameraBuffer& CTestCamSnapshot::SnapshotDataL(RArray<TInt>& /*aFrameIndexOrder*/)
	{
	iSnapshotBuffer.iAvailable = EFalse;
	return iSnapshotBuffer;
	}
	
void CTestCamSnapshot::ServiceSnapshot(const CFbsBitGc& aSourceBitmapGc)
	{
	TInt error = KErrNone;
	if(!iSnapshotBuffer.iAvailable)
		{
		error = KErrInUse;
		}
	else
		{
		iSnapshotGc->BitBlt(TPoint(0,0), aSourceBitmapGc);
		}
			
	iOwner.iObserver2->HandleEvent(TECAMEvent(KUidECamEventCameraSnapshot, error));
	}
	
void CTestCamSnapshot::ServiceBurstImageSnapshot(const CFbsBitGc& aSourceBitmapGc)
	{
	TInt error = KErrNone;
	if(!iSnapshotBuffer.iAvailable)
		{
		error = KErrInUse;
		}
	else
		{
		iSnapshotGc->BitBlt(TPoint(0,0), aSourceBitmapGc);
		}
			
	iOwner.iObserver2->HandleEvent(TECAMEvent(KUidECamEventCameraSnapshot, error));
	
	User::After(500000);
	
	iOwner.iStillImagePos.iX += KFramePosIncrement;
	iOwner.iStillImagePos.iY += KFramePosIncrement;

	if (iOwner.iStillImagePos.iX >= iOwner.iStillImageSize.iWidth)
		{
		iOwner.iStillImagePos.iX = 0;
		}
		
	if (iOwner.iStillImagePos.iY >= iOwner.iStillImageSize.iHeight)
		{
		iOwner.iStillImagePos.iY = 0;
		}
	}

