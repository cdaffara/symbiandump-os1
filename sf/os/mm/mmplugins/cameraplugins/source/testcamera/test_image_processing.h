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
// testimageprocessing.h
// 
//

#ifndef TESTIMAGEPROCESSING_H
#define TESTIMAGEPROCESSING_H

#include "TestCamera.h"
#include <ecam/ecamadvsettingsintf.h>

const TInt KNumOfImgProcExtensions = 2;
const TInt KMinBrightness = -100;
const TInt KMaxBrightness = 100;
const TInt KMinContrast = -100;
const TInt KMaxContrast = 100;

// Implementations of the MCameraImageProcessing class
class CTestCamImgProc : 	public CBase, 
							public MCameraImageProcessing,
							public MCameraImageProcessing2
	{
	friend class CTestCamera;
public:
	static CTestCamImgProc* NewL(CTestCamera& aOwner);
	~CTestCamImgProc();
	
protected:
	//from MCameraImageProcessing
	void Release();
	void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const;
	void GetActiveTransformationsL(RArray<TUid>& aTransformations) const;
	void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const;
	TInt TransformationValue(TUid aTransformation) const;
	TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const;
	void SetTransformationValue(TUid aTransformation, TInt aValue);
	void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const;
	void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence);
	void SetSourceRect(const TRect& aRect);
	void GetSourceRect(TRect& aRect) const;
	
	//from MCameraImageProcessing2
	void GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const;
	void GetColorSwapCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorSwapCapabilities) const;
	void SetColorSwapEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters); 
	void RemoveColorSwapEntry(TInt aIndex);
	void GetColorSwapEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const; 
	void StartColorSwapping();
	void CancelColorSwappingL();
	void GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const;
	void GetColorAccentCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorAccentCapabilities) const;
	void SetColorAccentEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters); 
	void RemoveColorAccentEntry(TInt aIndex);
	void GetColorAccentEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const; 
	void StartColorAccent();
	void CancelColorAccentL();

private:
	CTestCamImgProc(CTestCamera& aOwner);
	void ConstructL();
	
	TBool IsTransform(TUid aTransformation);
	
private:
	CTestCamera& iOwner;
	
	TInt iRefCount;
	
	RArray<TUid> iSupportedTranformations;
	RArray<TUid> iActiveTransformations;
	RArray<TUid> iActiveTransformSequence;
	};
#endif // TESTIMAGEPROCESSING_H
