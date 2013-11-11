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
// testimageprocessing.cpp
// 
//

#include "test_image_processing.h"
#include <ecamerrors.h>
#include "ecamversion.h"

CTestCamImgProc::CTestCamImgProc(CTestCamera& aOwner): iOwner(aOwner)
	{}
	
CTestCamImgProc::~CTestCamImgProc()
	{
	iSupportedTranformations.Close();
	iActiveTransformations.Close();
	iActiveTransformSequence.Close();
	
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());	
	if(globalData != NULL)
		{
		if(!globalData->iTestCamAdvSet && !globalData->iTestCamPresets && !globalData->iTestCamSnapshot)
			{
			delete globalData;
			Dll::FreeTls();
			}	
		else
			{
			globalData->iTestCamImgProc = NULL;	
			Dll::SetTls(globalData);
			}
		}
	}
		
CTestCamImgProc* CTestCamImgProc::NewL(CTestCamera& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(globalData == NULL)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		globalData->iImgProcReferenceCount = 0;
		globalData->iTestCamImgProc = new (ELeave) CTestCamImgProc(aOwner);
		CleanupStack::PushL(globalData->iTestCamImgProc);
		globalData->iTestCamImgProc->ConstructL();
		globalData->iTestCamImgProc->iRefCount = 1;
		User::LeaveIfError(Dll::SetTls(globalData));
		CleanupStack::Pop(globalData->iTestCamImgProc);
		CleanupStack::Pop(globalData);
		return globalData->iTestCamImgProc;
		}
	else
		{
		if(globalData->iTestCamImgProc == NULL)
			{
			globalData->iImgProcReferenceCount = 0;
			globalData->iTestCamImgProc = new (ELeave) CTestCamImgProc(aOwner);
			CleanupStack::PushL(globalData->iTestCamImgProc);
			globalData->iTestCamImgProc->ConstructL();
			globalData->iTestCamImgProc->iRefCount = 1;
			User::LeaveIfError(Dll::SetTls(globalData));
			CleanupStack::Pop(globalData->iTestCamImgProc);
			return globalData->iTestCamImgProc;
			}
			
		CTestCamImgProc* self = globalData->iTestCamImgProc;
		
		globalData->iImgProcReferenceCount++;
		self->iRefCount = globalData->iImgProcReferenceCount + 1;
		if (globalData->iImgProcReferenceCount == KNumOfImgProcExtensions-1)
			{
			globalData->iTestCamImgProc = NULL;
			if(!globalData->iTestCamAdvSet && !globalData->iTestCamPresets && !globalData->iTestCamSnapshot)
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

void CTestCamImgProc::Release()
	{
	iRefCount--; 
	if(iRefCount == 0)
		{
		iOwner.iImgProcImpl = NULL;
		delete this;
		}
	}	
	
void CTestCamImgProc::ConstructL()
	{
	iSupportedTranformations.Reset();
	iSupportedTranformations.AppendL(KUidECamEventImageProcessingAdjustBrightness);
	iSupportedTranformations.AppendL(KUidECamEventImageProcessingAdjustContrast);
	
	iActiveTransformations.Reset();
	
	iActiveTransformSequence.Reset();
	}
	
void CTestCamImgProc::GetSupportedTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	for(TInt index=0; index < iSupportedTranformations.Count(); index++)
		{
		aTransformations.AppendL(iSupportedTranformations[index]);
		}
	}

void CTestCamImgProc::GetActiveTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	for(TInt index=0; index < iActiveTransformations.Count(); index++)
		{
		aTransformations.AppendL(iActiveTransformations[index]);
		}	
	}
	
void CTestCamImgProc::GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const
	{
	TInt err = iActiveTransformations.Find(aTransformation);
	if(err == KErrNotFound)
		{
		User::Leave(err);
		}
		
	switch(aTransformation.iUid)
		{
		case KUidECamEventImageProcessingAdjustBrightnessUidValue:
			{
			aInfo = EContinuousRangeMinMax;
			aValues.Reset();
			aValues.AppendL(KMinBrightness);
			aValues.AppendL(KMaxBrightness);	
			break;
			}
			
		case KUidECamEventImageProcessingAdjustContrastUidValue:
			{
			aInfo = EContinuousRangeMinMax;
			aValues.Reset();
			aValues.AppendL(KMinContrast);
			aValues.AppendL(KMaxContrast);	
			break;
			}
			
		default:
			User::Leave(KErrNotSupported);
		}
	}
	
TInt CTestCamImgProc::TransformationValue(TUid aTransformation) const
	{
	switch(aTransformation.iUid)
		{
		case KUidECamEventImageProcessingAdjustBrightnessUidValue:
			{
			return iOwner.iImgProcBrightness;
			}
			
		case KUidECamEventImageProcessingAdjustContrastUidValue:
			{
			return iOwner.iImgProcContrast;
			}
			
		default:
			return 0;
		}	
	}
	
TInt CTestCamImgProc::GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const
	{
	switch(aTransformation.iUid)
		{
		case KUidECamEventImageProcessingAdjustBrightnessUidValue:
			{
			aTransformationValue = iOwner.iImgProcBrightness;
			return KErrNone;
			}
			
		case KUidECamEventImageProcessingAdjustContrastUidValue:
			{
			aTransformationValue = iOwner.iImgProcContrast;
			return KErrNone;
			}
			
		default:
			return KErrNotSupported;
		}	
	}
	
void CTestCamImgProc::SetTransformationValue(TUid aTransformation, TInt aValue)
	{
	TInt eventError = iOwner.CheckReserveAndPower();
	
	if(eventError == KErrNone)
		{
		switch(aTransformation.iUid)
			{
			case KUidECamEventImageProcessingAdjustBrightnessUidValue:
				{
				if(aValue < KMinBrightness)
					{
					iOwner.iImgProcBrightness = KMinBrightness;
					}
				else
					{
					if(aValue > KMaxBrightness)	
						{
						iOwner.iImgProcBrightness = KMaxBrightness;
						}
					else
						{
						iOwner.iImgProcBrightness = aValue;	
						}
					}
				break;
				}
				
			case KUidECamEventImageProcessingAdjustContrastUidValue:
				{
				if(aValue < KMinContrast)
					{
					iOwner.iImgProcContrast = KMinContrast;
					}
				else
					{
					if(aValue > KMaxContrast)	
						{
						iOwner.iImgProcContrast = KMaxContrast;
						}
					else
						{
						iOwner.iImgProcContrast = aValue;	
						}
					}
				break;
				}
				
			default:
				eventError = KErrNotSupported;
			}	
		}
	
	if(eventError == KErrNone)	
		{
		eventError = iActiveTransformations.Find(aTransformation);
		
		if(eventError == KErrNotFound)
			{
			eventError = iActiveTransformations.Append(aTransformation);
			
			if(eventError == KErrNone)
				{
				if(IsTransform(aTransformation))
					{
					eventError = iActiveTransformSequence.Append(aTransformation);
					}
				}
			}
		else
			{
			eventError = KErrNone;	
			}	
		}
		
	TECAMEvent ecamevent(aTransformation, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamImgProc::GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const
	{
	aTransformSequence.Reset();
	for(TInt index=0; index < iActiveTransformSequence.Count(); index++)
		{
		aTransformSequence.AppendL(iActiveTransformSequence[index]);
		}	
	}
	
void CTestCamImgProc::SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence)
	{
	TInt err = KErrNone;
	for(TInt index=0; index < aTransformSequence.Count(); index++)
		{
		err = iActiveTransformations.Find(aTransformSequence[index]);
		if(err == KErrNotFound)
			{
			User::Leave(err);
			}
		}
		
	iActiveTransformSequence.Reset();
	for(TInt index=0; index < aTransformSequence.Count(); index++)
		{
		iActiveTransformSequence.AppendL(aTransformSequence[index]);
		}
	}
	
void CTestCamImgProc::SetSourceRect(const TRect& /*aRect*/)
	{
	return;
	}

void CTestCamImgProc::GetSourceRect(TRect& /*aRect*/) const
	{
	return;	
	}
	
void CTestCamImgProc::GetConcurrentColorSwappingsSupportedL(TInt& /*aConcurrentColorSwappingSupported*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamImgProc::GetColorSwapCapabilitiesL(TInt /*aIndex*/, CCamera::CCameraImageProcessing::TColorOperationCapabilities& /*aColorSwapCapabilities*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CTestCamImgProc::SetColorSwapEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& /*aColorSwapParameters*/)
	{
	TECAMEvent2 ecamevent2(KUidECamEventCIPSetColorSwapEntry, KErrNotSupported, aIndex);
	
	iOwner.iECamEvent2 = ecamevent2;
	iOwner.iHandleEvent2Async.CallBack();
	}
	
void CTestCamImgProc::RemoveColorSwapEntry(TInt aIndex)
	{
	TECAMEvent2 ecamevent2(KUidECamEventCIPRemoveColorSwapEntry, KErrNotSupported, aIndex);
	
	iOwner.iECamEvent2 = ecamevent2;
	iOwner.iHandleEvent2Async.CallBack();
	}
	
void CTestCamImgProc::GetColorSwapEntryL(TInt /*aIndex*/, CCamera::CCameraImageProcessing::TColorOperationEntry& /*aColorSwapParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CTestCamImgProc::StartColorSwapping()
	{
	TECAMEvent ecamevent(KUidECamEventCIPStartColorSwap, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamImgProc::CancelColorSwappingL()
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamImgProc::GetConcurrentColorAccentSupportedL(TInt& /*aConcurrentColorAccentSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CTestCamImgProc::GetColorAccentCapabilitiesL(TInt /*aIndex*/, CCamera::CCameraImageProcessing::TColorOperationCapabilities& /*aColorAccentCapabilities*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CTestCamImgProc::SetColorAccentEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& /*aColorAccentParameters*/)
	{
	TECAMEvent2 ecamevent2(KUidECamEventCIPSetColorAccentEntry, KErrNotSupported, aIndex);
	
	iOwner.iECamEvent2 = ecamevent2;
	iOwner.iHandleEvent2Async.CallBack();
	}
	
void CTestCamImgProc::RemoveColorAccentEntry(TInt aIndex)
	{
	TECAMEvent2 ecamevent2(KUidECamEventCIPRemoveColorAccentEntry, KErrNotSupported, aIndex);
	
	iOwner.iECamEvent2 = ecamevent2;
	iOwner.iHandleEvent2Async.CallBack();
	}
	
void CTestCamImgProc::GetColorAccentEntryL(TInt /*aIndex*/, CCamera::CCameraImageProcessing::TColorOperationEntry& /*aColorAccentParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CTestCamImgProc::StartColorAccent()
	{
	TECAMEvent ecamevent(KUidECamEventCIPStartColorAccent, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamImgProc::CancelColorAccentL()
	{
	User::Leave(KErrNotSupported);
	}

TBool CTestCamImgProc::IsTransform(TUid aTransformation)
	{
	switch(aTransformation.iUid)
		{
		//fall through
		case KUidECamEventImageProcessingTransformCropUidValue:
		case KUidECamEventImageProcessingTransformRotateUidValue:
		case KUidECamEventImageProcessingTransformMirrorUidValue:
		case KUidECamEventImageProcessingTransformScaleUidValue:
		case KUidECamEventImageProcessingSourceRectUidValue:
		case KUidECamEventImageProcessingNoiseReductionUidValue:
		case KUidECamEventImageProcessingGlareRemovalUidValue:
			{
			return ETrue;	
			}
		default:
			return EFalse;
		}
	}
