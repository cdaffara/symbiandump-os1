
// ECamImageProcessingTest.cpp

// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ECamImageProcessingTest.h"
#include <ecamadvsettings.h>
#include <ecam/ecamplugin.h>
#include <ecamuids.hrh>
#include <ecom/ecomresolverparams.h>
#include "ECamUnitTestPluginUids.hrh"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamimageprocessingconst.h>
#endif



//const TRect KRect1(100, 50, 600, 200);
const TInt  KValue = 100; 
const TInt  KBaselinedEffects = 0x000001FF;
//
// RECamImageProcessingTest
//

RECamImageProcessingTest* RECamImageProcessingTest::NewL(TBool aAllocTest)
	{
	RECamImageProcessingTest* self = new (ELeave) RECamImageProcessingTest(aAllocTest);
	return self;	
	}
	
RECamImageProcessingTest::RECamImageProcessingTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-007-HP");
	}

TVerdict RECamImageProcessingTest::DoTestStepL()	
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1(_L("Alloc test"));
	TInt i;
	TInt err;
	for (i = 1 ; ; i++)
		{
		__MM_HEAP_MARK;
		
		if (i % 5 == 0)
			{
			INFO_PRINTF2(_L("Fail count = %d"), i);
			}

		__UHEAP_SETFAIL(RHeap::EFailNext, i);

		TRAP(err, verdict = DoImageProcessingTestStepL());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = (testAlloc == NULL) && (err==KErrNone);
		User::Free(testAlloc);

		__UHEAP_RESET;
		__MM_HEAP_MARKEND;

		if ((err != KErrNoMemory ) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}
		}
	return verdict;
	}
	
TVerdict RECamImageProcessingTest::DoImageProcessingTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraImageProcessing* imageprocess = NULL;
	
	// using observer 2 
	MCameraObserver2* observer2 = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		imageprocess = 	static_cast<CCamera::CCameraImageProcessing*> (camera->CustomInterface(KECamImageProcessingUid));
	    if (imageprocess!= NULL)
	    	{
	    	CleanupStack::PushL(imageprocess);
	    	INFO_PRINTF1(_L("CCameraImageProcessing object was created")); 
	    	CleanupStack::PopAndDestroy(imageprocess);
	    	}
	    else
    		{
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		User::Leave(KErrNoMemory);
		}
	__MM_HEAP_MARKEND;

	// create a imageprocessing object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, imageprocess  = CCamera::CCameraImageProcessing::NewL(*camera));
		
		if (error==KErrNone)	
	    	{
	    	CleanupStack::PushL(imageprocess);
	  		INFO_PRINTF1(_L("CCameraImageProcessing object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(imageprocess);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraImageProcessing object was not created using NewL")); 
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		User::Leave(KErrNoMemory);
		}
	__MM_HEAP_MARKEND;
   
	return result;
	}
	
//
// RECamImageProcDefaultsTest //
//

RECamImageProcDefaultsTest* RECamImageProcDefaultsTest::NewL(TBool aAllocTest)
	{
	RECamImageProcDefaultsTest* self = new (ELeave) RECamImageProcDefaultsTest(aAllocTest);
	return self;	
	}
	
RECamImageProcDefaultsTest::RECamImageProcDefaultsTest(TBool)
	{
	iTestStepName = _L("MM-ECM-ADV-U-012-HP");
	}
	
TBool RECamImageProcDefaultsTest::CompareUidArrays(RArray<TUid>& firstArray, RArray<TUid>& secondArray)
	{
	TBool result = ETrue;
	
	if (firstArray.Count() != secondArray.Count())
		{
		result = EFalse;
		}
	else
		{
		for (TInt expected = 0; expected < firstArray.Count(); expected++)
		    {
			result = EFalse;
			for (TInt actual = 0; actual < secondArray.Count(); actual++)
				{
				if (secondArray[actual] == firstArray[expected])
					{
					result = ETrue;
					break;
					}				
				}
			}
		}
	return result;
	}	
	
TVerdict RECamImageProcDefaultsTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraImageProcessing* imageprocess = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		imageprocess = 	static_cast<CCamera::CCameraImageProcessing*> (camera->CustomInterface(KECamImageProcessingUid));
	    if (imageprocess!= NULL)
	    	{
	    	CleanupStack::PushL(imageprocess);

	    	// Supported Transformations
			RArray<TUid> expectedTransfUids;	    	
			CleanupClosePushL(expectedTransfUids);
	    	expectedTransfUids.Reset();	    			
	    	// set the expected UIDS
			expectedTransfUids.AppendL(KUidECamEventImageProcessingAdjustBrightness);
			expectedTransfUids.AppendL(KUidECamEventImageProcessingAdjustContrast);
			expectedTransfUids.AppendL(KUidECamEventImageProcessingEffect);	    	
	    	RArray<TUid> suppTransfUids; 
	    	CleanupClosePushL(suppTransfUids);
	    	imageprocess->GetSupportedTransformationsL(suppTransfUids);
	    	if (CompareUidArrays(expectedTransfUids, suppTransfUids) == EFalse)
				{
				INFO_PRINTF1(_L("GetSupportedTransformationsL failed"));
				result = EFail;
				}
	    	expectedTransfUids.Reset();
	    	suppTransfUids.Reset();	    		    	
			CleanupStack::PopAndDestroy(2, &expectedTransfUids); //suppTransfUids

		    // Active Transformations
	    	RArray<TUid> expectedActTransfUids; 
	    	CleanupClosePushL(expectedActTransfUids);
	    	expectedActTransfUids.Reset();	    
	    	// set the expected UIDS
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingAdjustBrightness);
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingAdjustContrast);
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingEffect);	    	
	    	RArray<TUid> actTransfUids; 
	    	CleanupClosePushL(actTransfUids);
	    	actTransfUids.Reset();	 	    	
	    	imageprocess->GetActiveTransformationsL(actTransfUids);
 	    	if (CompareUidArrays(expectedActTransfUids, actTransfUids) == EFalse)
				{
				INFO_PRINTF1(_L("GetActiveTransformationsL failed"));
				result = EFail;
				}
			CleanupStack::PopAndDestroy(2, &expectedActTransfUids); //actTransfUids
		    
		    // Set & Get Active Transformation Sequence
	    	RArray<TUid> expectedTransfSequence; 
	    	CleanupClosePushL(expectedTransfSequence);
	    	expectedTransfSequence.Reset();	    	
			expectedTransfSequence.Append(KUidECamEventImageProcessingAdjustBrightness);	   

			imageprocess->SetActiveTransformSequenceL(expectedTransfSequence);
			
	    	RArray<TUid> actTransfSequence; 
	    	CleanupClosePushL(actTransfSequence);
	    	actTransfSequence.Reset();    		    		    	
			imageprocess->GetActiveTransformSequenceL(actTransfSequence);
 	    	if (CompareUidArrays(expectedTransfSequence, actTransfSequence) == EFalse)
				{
				INFO_PRINTF1(_L("GetActiveTransformSequenceL failed"));
				result = EFail;
				}
			CleanupStack::PopAndDestroy(2, &expectedTransfSequence); //actTransfSequence

		    // Get & Set Transformation
	    	TUid iInputEventUid = KUidECamEventImageProcessingAdjustBrightness;
	        TInt aSetValue = 4;
	    	imageprocess->SetTransformationValue(iInputEventUid, aSetValue);

	    	CheckNotification(iInputEventUid, result);
    			
		    // Check that value has actually changed.
	    	TInt aGetValue = imageprocess->TransformationValue(iInputEventUid);
 	    	if (aSetValue != aGetValue)
				{
				INFO_PRINTF3(_L("Got wrong TransformationValue set %d, got %d"), aSetValue, aGetValue);
				result = EFail;
				}
			else
				{
				INFO_PRINTF2(_L("Transformation value set to %d"), aSetValue);					
				}	    
				
			TInt aGetValue1 = 0;
			TInt err = imageprocess->GetTransformationValue(iInputEventUid, aGetValue1);
			
			if(err)
				{
				INFO_PRINTF2(_L("New method for Getting Transformation value returned error %d"), err);
				result = EFail;
				}
				
			if(aGetValue1 != aGetValue)
				{
				INFO_PRINTF1(_L("New and old method for Getting Transformation value  retrieves wrong/dissimilar values"));
				result = EFail;
				}
				
			//Set Effect Transformation with color swap
			iInputEventUid = KUidECamEventImageProcessingEffect;
			RArray<TInt> supportedEffects;
					
	    	TValueInfo info = ENotActive;
	    	imageprocess->GetTransformationSupportedValuesL(iInputEventUid, supportedEffects, info);
	    	
	    	TInt effects = supportedEffects[0];
			supportedEffects.Close();
	    	
	    	if(info != EBitField)
	    		{
	    		INFO_PRINTF1(_L("Incorrect TValueInfo provided"));
				result = EFail;
	    		}
	    		
	    	if(effects & CCamera::CCameraImageProcessing::EEffectColorSwap)
	        	{	
	        	imageprocess->SetTransformationValue(iInputEventUid, CCamera::CCameraImageProcessing::EEffectColorSwap);	
	        	
	        	CheckNotification(iInputEventUid, result);
	        	}
	       
	        // Check that value has actually changed.
	    	aGetValue = imageprocess->TransformationValue(iInputEventUid);
 	    	if (aGetValue != static_cast<TInt>(CCamera::CCameraImageProcessing::EEffectColorSwap))
				{
				INFO_PRINTF1(_L("Got wrong TransformationValue set"));
				result = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Transformation value set to EEffectColorSwap"));					
				}
			
			TInt concurrentColorSwappingSupported=0;	
			imageprocess->GetConcurrentColorSwappingsSupportedL(concurrentColorSwappingSupported);
	        
	        if(concurrentColorSwappingSupported > 0)
	        	{
	        	TestColorSwapL(imageprocess, result);
	        	}
	        	
			//Set Effect Transformation with color accent
			iInputEventUid = iInputEventUid = KUidECamEventImageProcessingEffect;
			if(effects & CCamera::CCameraImageProcessing::EEffectColorAccent)
	        	{	
	        	imageprocess->SetTransformationValue(iInputEventUid, CCamera::CCameraImageProcessing::EEffectColorAccent);	
	        
	        	CheckNotification(iInputEventUid, result);
	        	}
	        	
	        // Check that value has actually changed.
	    	aGetValue = imageprocess->TransformationValue(iInputEventUid);
 	    	if (aGetValue != static_cast<TInt>(CCamera::CCameraImageProcessing::EEffectColorAccent))
				{
				INFO_PRINTF1(_L("Got wrong TransformationValue set"));
				result = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Transformation value set to EEffectColorAccent"));					
				}
			
			TInt concurrentColorAccentSupported=0;	
			imageprocess->GetConcurrentColorAccentSupportedL(concurrentColorAccentSupported);
	        
	        if(concurrentColorAccentSupported > 0)
	        	{
	        	TestColorAccentL(imageprocess, result);
	        	}
	    	
		    // Get & Set Source Rect
			TRect setRect(100, 50, 600, 200);
			TRect getRect;
	    	imageprocess->SetSourceRect(setRect);
			imageprocess->GetSourceRect(getRect);
			if (setRect != getRect)
				{
				INFO_PRINTF1(_L("Did not get the same rect"));
				result = EFail;
				}
			
			CCamera::CCameraImageProcessing::TOrientationReference orientationReference = 
						CCamera::CCameraImageProcessing::EOrientationReferenceCameraSensorOrientation;	
			TUint supportedRelativeRotation=0;
			TUint supportedRelativeMirroring=0;
			TUint supportedRelativeFlipping=0;
			
			INFO_PRINTF1(_L("GetSupportedRelativeOrientationOptionsL method invocation"));

			TRAP(err,imageprocess->GetSupportedRelativeOrientationOptionsL(orientationReference, 
		supportedRelativeRotation, supportedRelativeMirroring, supportedRelativeFlipping));	
			
			if(err!= KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetSupportedRelativeOrientationOptionsL did not leave with KErrNotSupported"));
				result = EFail;
				}
			
			CCamera::CCameraImageProcessing::TRelativeRotation relativeRotation = 
					CCamera::CCameraImageProcessing::ERelativeRotation0Degrees;
			
			CCamera::CCameraImageProcessing::TRelativeMirror relativeMirror = 
					CCamera::CCameraImageProcessing::ERelativeMirrorNone;
					
			CCamera::CCameraImageProcessing::TRelativeFlipping relativeFlipping = 
					CCamera::CCameraImageProcessing::ERelativeFlippingNone;
			
			INFO_PRINTF1(_L("GetCurrentRelativeOrientationOptionsL method invocation"));
			TRAP(err,imageprocess->GetCurrentRelativeOrientationOptionsL(orientationReference, 
								relativeRotation, relativeMirror, relativeFlipping));	
			
			if(err!= KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetCurrentRelativeOrientationOptionsL did not leave with KErrNotSupported"));
				result = EFail;
				}
			
			INFO_PRINTF1(_L("SetRelativeOrientationOptionsL method invocation"));
			iInputEventUid = KUidECamEventImageProcessingTransformRelativeOrientation;
			imageprocess->SetRelativeOrientationOptionsL(orientationReference, 
						relativeRotation, relativeMirror, relativeFlipping);
	    	
	    	CheckNotificationNeg(iInputEventUid, result);
	    	
	    	CleanupStack::PopAndDestroy(imageprocess);
	    	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
	
void RECamImageProcDefaultsTest::TestColorSwapL(CCamera::CCameraImageProcessing* aImageProcess, TVerdict& aResult)
	{
	CCamera::CCameraImageProcessing::TColorOperationCapabilities colorSwapCapabilities;
	aImageProcess->GetColorSwapCapabilitiesL(0, colorSwapCapabilities);
	
	if(!colorSwapCapabilities.iIsCapabilityUniform)
		{
		INFO_PRINTF1(_L("Non Uniform entries for color swap"));
		}
		        	
	//fill the parameters for color swap entry 
	CCamera::CCameraImageProcessing::TColorOperationEntry colorSwapParameters;
	
	colorSwapParameters.iSourceColor.SetGreen(KValue);
	colorSwapParameters.iTargetColor.SetBlue(KValue);

	colorSwapParameters.iSourceColorRepresentation = CCamera::CCameraImageProcessing::ERepresentationSingle;
	colorSwapParameters.iTargetColorRepresentation = CCamera::CCameraImageProcessing::ERepresentationSingle;
	
	colorSwapParameters.iColorOperationSourceRgbGroupingMode = CCamera::CCameraImageProcessing::ERgbGroupingFixed;
	colorSwapParameters.iColorOperationTargetRgbGroupingMode = CCamera::CCameraImageProcessing::ERgbGroupingFixed;
	
	//set the entry
	iInputEventUid = KUidECamEvent2CIPSetColorSwapEntry;
	aImageProcess->SetColorSwapEntryL(0, colorSwapParameters);
	
	CheckNotificationImgProc(iInputEventUid, 0, aResult);
	
	//check the entry status
	CCamera::CCameraImageProcessing::TColorOperationEntry getColorSwapParameters;
	aImageProcess->GetColorSwapEntryL(0, getColorSwapParameters);
	
	if(getColorSwapParameters.iEntryStatus != EDiscreteSteps)
		{
		INFO_PRINTF1(_L("Entry status set incorrectly"));
		aResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Entry status set correctly"));	
		}
	
	//start the color swap process
	iInputEventUid = KUidECamEventCIPStartColorSwap;
	aImageProcess->StartColorSwappingL();
	
	//try to cancel it
	aImageProcess->CancelColorSwappingL();
	
	CheckNotification(iInputEventUid, aResult);
	
	//remove the entry
	iInputEventUid = KUidECamEventCIPRemoveColorSwapEntry;
	aImageProcess->RemoveColorSwapEntryL(0);
	CheckNotificationImgProc(iInputEventUid, 0, aResult);
	
	//check the entry status
	aImageProcess->GetColorSwapEntryL(0, getColorSwapParameters);
	
	if(getColorSwapParameters.iEntryStatus != ENotActive)
		{
		INFO_PRINTF1(_L("Entry status set incorrectly"));
		aResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Entry status set correctly"));	
		}
	}
	
void RECamImageProcDefaultsTest::TestColorAccentL(CCamera::CCameraImageProcessing* aImageProcess, TVerdict& aResult)
	{
	CCamera::CCameraImageProcessing::TColorOperationCapabilities colorAccentCapabilities;
	aImageProcess->GetColorAccentCapabilitiesL(0, colorAccentCapabilities);
	
	if(!colorAccentCapabilities.iIsCapabilityUniform)
		{
		INFO_PRINTF1(_L("Non Uniform entries for color accent"));
		}
	
	//fill the parameters for color swap entry 
	CCamera::CCameraImageProcessing::TColorOperationEntry colorAccentParameters;
	
	colorAccentParameters.iSourceColor.SetGreen(KValue);
	colorAccentParameters.iSourceColorRepresentation = CCamera::CCameraImageProcessing::ERepresentationSingle;
	colorAccentParameters.iColorOperationSourceRgbGroupingMode = CCamera::CCameraImageProcessing::ERgbGroupingFixed;
	
	//set the entry
	iInputEventUid = KUidECamEventCIPSetColorAccentEntry;
	aImageProcess->SetColorAccentEntryL(0, colorAccentParameters);
	
	CheckNotificationImgProc(iInputEventUid, 0, aResult);
	
	//check the entry status
	CCamera::CCameraImageProcessing::TColorOperationEntry getColorAccentParameters;
	aImageProcess->GetColorAccentEntryL(0, getColorAccentParameters);
	
	if(getColorAccentParameters.iEntryStatus != EDiscreteSteps)
		{
		INFO_PRINTF1(_L("Entry status set incorrectly"));
		aResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Entry status set correctly"));	
		}
	
	//start the color accent process
	iInputEventUid = KUidECamEventCIPStartColorAccent;
	aImageProcess->StartColorAccentL();
	
	//try to cancel it
	aImageProcess->CancelColorAccentL();
	
	CheckNotification(iInputEventUid, aResult);
	
	//remove the entry
	iInputEventUid = KUidECamEventCIPRemoveColorAccentEntry;
	aImageProcess->RemoveColorAccentEntryL(0);
	CheckNotificationImgProc(iInputEventUid, 0, aResult);
	
	//check the entry status
	aImageProcess->GetColorAccentEntryL(0, getColorAccentParameters);
	
	if(getColorAccentParameters.iEntryStatus != ENotActive)
		{
		INFO_PRINTF1(_L("Entry status set incorrectly"));
		aResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Entry status set correctly"));	
		}
	}
	
//
// RECamImageProcRangesTest //
//

RECamImageProcRangesTest* RECamImageProcRangesTest::NewL(TBool aAllocTest)
	{
	RECamImageProcRangesTest* self = new (ELeave) RECamImageProcRangesTest(aAllocTest);
	return self;	
	}
	
RECamImageProcRangesTest::RECamImageProcRangesTest(TBool)
	{
	iTestStepName = _L("MM-ECM-ADV-U-013-HP");
	}
	
TVerdict RECamImageProcRangesTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraImageProcessing* imageprocess = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		imageprocess = 	static_cast<CCamera::CCameraImageProcessing*> (camera->CustomInterface(KECamImageProcessingUid));
	    if (imageprocess!= NULL)
	    	{
	    	CleanupStack::PushL(imageprocess);
	    
		    // Get Transformation Supported Values for brightness
	    	iInputEventUid = KUidECamEventImageProcessingAdjustBrightness;
	    	RArray<TInt> transfSuppValues; 
	    	CleanupClosePushL(transfSuppValues);
	    	transfSuppValues.Reset();	 	    	
			TValueInfo valueInfo;
			imageprocess->GetTransformationSupportedValuesL(iInputEventUid, transfSuppValues, valueInfo);

		    // Set a non supported Transformation value
	        TInt aSetValue = 0;
	    	imageprocess->SetTransformationValue(iInputEventUid, aSetValue);

	        CheckNotification(iInputEventUid, result);
    				    	
	    	TInt aGetValue =imageprocess->TransformationValue(iInputEventUid);
 	    	if (aSetValue != aGetValue)
				{
				INFO_PRINTF3(_L("Set unsupported TransformationValue set %d, get %d"), aSetValue, aGetValue);
				result = EFail;
				}	    
			CleanupStack::PopAndDestroy(&transfSuppValues); 
				
	    	CleanupStack::PopAndDestroy(imageprocess);
		  	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
	
//
// RECamPresetsNegTest //
//
RECamImageProcNegTest* RECamImageProcNegTest::NewL(TBool aAllocTest)
	{
	RECamImageProcNegTest* self = new (ELeave) RECamImageProcNegTest(aAllocTest);
	return self;	
	}
	
RECamImageProcNegTest::RECamImageProcNegTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-0103-HP");
	}
	
TVerdict RECamImageProcNegTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraImageProcessing* imageprocess = NULL;
	
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		imageprocess = 	static_cast<CCamera::CCameraImageProcessing*> (camera->CustomInterface(KECamImageProcessingUid));
	    if (imageprocess!= NULL)
	    	{
	    	CleanupStack::PushL(imageprocess);

	    	// Supported Transformations
	    	RArray<TUid> suppTransfUids; 
	    	CleanupClosePushL(suppTransfUids);
	    	suppTransfUids.Reset();
	    	imageprocess->GetSupportedTransformationsL(suppTransfUids);
		    // Get Supported Values for chosen transformation
	 	    iInputEventUid = KUidECamEventImageProcessingAdjustBrightness;
	    	RArray<TInt> transfSuppValues; 
	    	CleanupClosePushL(transfSuppValues);
	    	transfSuppValues.Reset();	    	
			TValueInfo valueInfo;
			imageprocess->GetTransformationSupportedValuesL(iInputEventUid, transfSuppValues, valueInfo);
		    // Set a non supported Transformation value.
	        TInt aSetValue = KValue;
	    	imageprocess->SetTransformationValue(iInputEventUid, aSetValue);
				
	        CheckNotificationNeg(iInputEventUid, result);
    			
		    // Check that value has not changed.
   	    	TInt aGetValue =imageprocess->TransformationValue(iInputEventUid);
 	    	if (aSetValue == aGetValue)
				{
				INFO_PRINTF2(_L("Set unsupported TransformationValue set %d"), aSetValue);
				result = EFail;
				} 
			
			CCamera::CCameraImageProcessing::TOrientationReference orientationReference = 
						CCamera::CCameraImageProcessing::EOrientationReferenceCameraSensorOrientation;	

			CCamera::CCameraImageProcessing::TRelativeRotation relativeRotation = 
					CCamera::CCameraImageProcessing::ERelativeRotation0Degrees;
			
			CCamera::CCameraImageProcessing::TRelativeMirror relativeMirror = 
					CCamera::CCameraImageProcessing::ERelativeMirrorNone;
					
			CCamera::CCameraImageProcessing::TRelativeFlipping relativeFlipping = 
					CCamera::CCameraImageProcessing::ERelativeFlippingNone;
	
			INFO_PRINTF1(_L("SetRelativeOrientationOptionsL method invocation"));
			iInputEventUid = KUidECamEventImageProcessingTransformRelativeOrientation;
			imageprocess->SetRelativeOrientationOptionsL(orientationReference, 
						relativeRotation, relativeMirror, relativeFlipping);
	    	
	    	CheckNotificationNeg(iInputEventUid, result);
 								    	    
			CleanupStack::PopAndDestroy(2, &suppTransfUids); //transfSuppValues

	    	CleanupStack::PopAndDestroy(imageprocess);
	    	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
	
//
// RECamImageProcBaselineTest //
//

RECamImageProcBaselineTest* RECamImageProcBaselineTest::NewL(TBool aAllocTest)
	{
	RECamImageProcBaselineTest* self = new (ELeave) RECamImageProcBaselineTest(aAllocTest);
	return self;	
	}
	
RECamImageProcBaselineTest::RECamImageProcBaselineTest(TBool)
	{
	iTestStepName = _L("MM-ECM-ADV-U-047-HP");
	}
	
TBool RECamImageProcBaselineTest::CompareUidArrays(RArray<TUid>& firstArray, RArray<TUid>& secondArray)
	{
	TBool result = ETrue;
	
	if (firstArray.Count() != secondArray.Count())
		{
		result = EFalse;
		}
	else
		{
		for (TInt expected = 0; expected < firstArray.Count(); expected++)
		    {
			result = EFalse;
			for (TInt actual = 0; actual < secondArray.Count(); actual++)
				{
				if (secondArray[actual] == firstArray[expected])
					{
					result = ETrue;
					break;
					}				
				}
			}
		}
	return result;
	}	
	
TVerdict RECamImageProcBaselineTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraImageProcessing* imageprocess = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::NewL(*this,0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::NewL()"));
		imageprocess = 	static_cast<CCamera::CCameraImageProcessing*> (camera->CustomInterface(KECamImageProcessingUid));
	    if (imageprocess!= NULL)
	    	{
	    	CleanupStack::PushL(imageprocess);

	    	// Supported Transformations
			RArray<TUid> expectedTransfUids;	    	
			CleanupClosePushL(expectedTransfUids);
	    	expectedTransfUids.Reset();	    			
	    	// set the expected UIDS
			expectedTransfUids.AppendL(KUidECamEventImageProcessingAdjustBrightness);
			expectedTransfUids.AppendL(KUidECamEventImageProcessingAdjustContrast);
			expectedTransfUids.AppendL(KUidECamEventImageProcessingEffect);	    	
	    	RArray<TUid> suppTransfUids; 
	    	CleanupClosePushL(suppTransfUids);
	    	imageprocess->GetSupportedTransformationsL(suppTransfUids);
	    	if (CompareUidArrays(expectedTransfUids, suppTransfUids) == EFalse)
				{
				INFO_PRINTF1(_L("GetSupportedTransformationsL failed"));
				result = EFail;
				}
	    	expectedTransfUids.Reset();
	    	suppTransfUids.Reset();	    		    	
			CleanupStack::PopAndDestroy(2, &expectedTransfUids); //suppTransfUids

		    // Active Transformations
	    	RArray<TUid> expectedActTransfUids; 
	    	CleanupClosePushL(expectedActTransfUids);
	    	expectedActTransfUids.Reset();	    
	    	// set the expected UIDS
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingAdjustBrightness);
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingAdjustContrast);
			expectedActTransfUids.AppendL(KUidECamEventImageProcessingEffect);	    	
	    	RArray<TUid> actTransfUids; 
	    	CleanupClosePushL(actTransfUids);
	    	actTransfUids.Reset();	 	    	
	    	imageprocess->GetActiveTransformationsL(actTransfUids);
 	    	if (CompareUidArrays(expectedActTransfUids, actTransfUids) == EFalse)
				{
				INFO_PRINTF1(_L("GetActiveTransformationsL failed"));
				result = EFail;
				}
			CleanupStack::PopAndDestroy(2, &expectedActTransfUids); //actTransfUids
		    
		    // check baseline
	    	RArray<TUid> expectedTransfSequence; 
	    	CleanupClosePushL(expectedTransfSequence);
	    	expectedTransfSequence.Reset();	    	
			expectedTransfSequence.Append(KUidECamEventImageProcessingEffect);	   

			imageprocess->SetActiveTransformSequenceL(expectedTransfSequence);
			
	    	RArray<TUid> actTransfSequence; 
	    	CleanupClosePushL(actTransfSequence);
	    	actTransfSequence.Reset();    		    		    	
			imageprocess->GetActiveTransformSequenceL(actTransfSequence);
 	    	if (CompareUidArrays(expectedTransfSequence, actTransfSequence) == EFalse)
				{
				INFO_PRINTF1(_L("GetActiveTransformSequenceL failed"));
				result = EFail;
				}
			CleanupStack::PopAndDestroy(2, &expectedTransfSequence); //actTransfSequence

		    				
			//Set Effect Transformation with color swap
			iInputEventUid = KUidECamEventImageProcessingEffect;
			RArray<TInt> supportedEffects;
					
	    	TValueInfo info = ENotActive;
	    	imageprocess->GetTransformationSupportedValuesL(iInputEventUid, supportedEffects, info);
	    	
	    	TInt effects = supportedEffects[0];
			supportedEffects.Close();
	    	
	    	if(info != EBitField)
	    		{
	    		INFO_PRINTF1(_L("Incorrect TValueInfo provided"));
				result = EFail;
	    		}
	    		
	    	if(effects > KBaselinedEffects)
	        	{	
	        	INFO_PRINTF1(_L("Unfamiliar enum value returned"));
				result = EFail;
	        	}
	        	
	        imageprocess->SetTransformationValue(iInputEventUid, CCamera::CCameraImageProcessing::EEffectColorSwap);	
	        CheckNotification(iInputEventUid, result);
	       
	        // Check that value has actually changed to EEffectNone due to baselining.
	    	TInt getValue = imageprocess->TransformationValue(iInputEventUid);
 	    	if (getValue != static_cast<TInt>(CCamera::CCameraImageProcessing::EEffectNone))
				{
				INFO_PRINTF1(_L("Got unfamiliar TransformationValue"));
				result = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Transformation value set to EEffectNone due to baselining"));					
				}
			
	    	CleanupStack::PopAndDestroy(imageprocess);
	    	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
