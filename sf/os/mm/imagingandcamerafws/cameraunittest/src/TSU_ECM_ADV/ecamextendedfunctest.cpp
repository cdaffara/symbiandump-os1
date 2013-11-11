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
//

#include <ecamuids.hrh>
#include <w32std.h>
#include <ecom/ecomresolverparams.h>
#include "ecamextendedfunctest.h"
#include "ECamUnitTestPluginUids.hrh"
#include "ECamUnitTestPlugin.h"
#include <ecam/ecamcommonuids.hrh>
#include <fbs.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamconst.h>
#include "ecamdef.h"
#include <ecamcapturecontrolconst.h>
#endif
/*------------------------------------------------------
 	Client ViewFinder : RECamClientViewFinderTest
 *------------------------------------------------------*/
RECamClientViewFinderTest* RECamClientViewFinderTest::NewL(const TDesC& aTestStep)
	{
	RECamClientViewFinderTest* self = new (ELeave) RECamClientViewFinderTest(aTestStep);
	return self;	
	}
	
RECamClientViewFinderTest::RECamClientViewFinderTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamClientViewFinderTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-050-HP")))
		{
		return DoTestStep_50L();
		}
	return EInconclusive;	
	}
	
TVerdict RECamClientViewFinderTest::DoTestStep_50L()
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

		TRAP(err, verdict = DoClientVFTestStepL());

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
	
TVerdict RECamClientViewFinderTest::DoClientVFTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	MClientViewFinderObserver *clientVFObserver = NULL;
	TInt error = KErrNone;
	CCamera::CCameraClientViewFinder* clientVF = NULL;
	
	MCameraObserver* observer = NULL; 
		
	// using observer 	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver"));
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create client viewfinder from CCamera object"));
		
		TRAP(error, clientVF  = CCamera::CCameraClientViewFinder::NewL(*camera, *clientVFObserver));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraClientViewFinder object creation using old Observer failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraClientViewFinder object creation using old Observer gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// using observer2 with NewL 
	MCameraObserver2* observer2 = NULL;	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create client viewfinder from CCamera object"));
		
		TRAP(error, clientVF  = CCamera::CCameraClientViewFinder::NewL(*camera, *clientVFObserver));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraClientViewFinder object creation using Observer2 with NewL failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraClientViewFinder object creation using Observer2 with NewL gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
		
	// create client viewfinder object using New2L
	__MM_HEAP_MARK;
	
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, clientVF  = CCamera::CCameraClientViewFinder::NewL(*camera, *clientVFObserver));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(clientVF);
	  		INFO_PRINTF1(_L("CCameraClientViewFinder object was created using NewL")); 
	    	
	    	INFO_PRINTF1(_L("Create Histogram for Client ViewFinder"));
	  		CCamera::CCameraV2Histogram* histogram_CVF = clientVF->CreateHistogramHandleL();
	  		if(!histogram_CVF)
	  			{
	  			INFO_PRINTF1(_L("Histogram creation for Client ViewFinder unsuccessful"));
	  			result = EFail;
	  			User::Leave(KErrNoMemory);
	  			}
	  			
	  		CleanupStack::PushL(histogram_CVF);
	  		INFO_PRINTF1(_L("Histogram object was created for Client ViewFinder"));
	  		CleanupStack::PopAndDestroy(histogram_CVF);
	    	
	    	CleanupStack::PopAndDestroy(clientVF);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraClientViewFinder object was not created using NewL")); 
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
	
/*------------------------------------------------------
 	Client ViewFinder : RECamClientVFNotificationTest
 *------------------------------------------------------*/
RECamClientVFNotificationTest* RECamClientVFNotificationTest::NewL(TBool aAllocTest)
	{
	RECamClientVFNotificationTest* self = new (ELeave) RECamClientVFNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamClientVFNotificationTest::ConstructL()
	{
	}
	
RECamClientVFNotificationTest::RECamClientVFNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-051-HP");
	}
	
void RECamClientVFNotificationTest::ViewFinderBufferReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode)
	{
	aClientViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}

void RECamClientVFNotificationTest::DirectHistogramDisplayed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt aErrorCode)
	{
	aClientViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}

void RECamClientVFNotificationTest::ClientHistogramReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt aErrorCode)
	{
	aClientViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}
	
void RECamClientVFNotificationTest::ImageProcessingFailed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode)
	{
	aClientViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;	
	}
	
TInt RECamClientVFNotificationTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

void RECamClientVFNotificationTest::CheckViewFinderNegNotification(TInt aVFHandle, TVerdict& aResult)	
	{
	INFO_PRINTF4(_L("Expected ClientViewFinder Id %d, received event %x. Error %d."), aVFHandle, iVFHandle, iError); 
	if (aVFHandle != iVFHandle || iError != KErrNotSupported)
		{
	    aResult = EFail;
	    }
	}
	
TVerdict RECamClientVFNotificationTest::DoTestStepL()
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

		TRAP(err, verdict = DoTestStepL_51L());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = (testAlloc == NULL) && (err==KErrNone);
		User::Free(testAlloc);

		__UHEAP_RESET;
		__MM_HEAP_MARKEND;

		if ((err != KErrNoMemory && verdict == EPass ) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}
		}
	return verdict;
	}
	
TVerdict RECamClientVFNotificationTest::DoTestStepL_51L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraClientViewFinder* clientVF = NULL;

/**************************************************************/	
	MCameraObserver2* observer2 = NULL;	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create client viewfinder from CCamera object"));
		
		TRAP(error, clientVF  = CCamera::CCameraClientViewFinder::NewL(*camera, *this));	
 		
 		if (error == KErrNone)
 			{
 			INFO_PRINTF1(_L("CCameraClientViewFinder object creation using Observer2 with NewL is successful."));
 			}
 		else if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraClientViewFinder object creation using Observer2 with NewL failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraClientViewFinder object creation using Observer2 with NewL gave unexpected result"));
    		result = EFail;
    		}
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
/**************************************************************/
	
	// create client viewfinder object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this, 0, 0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, clientVF  = CCamera::CCameraClientViewFinder::NewL(*camera, *this));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(clientVF);
	  		INFO_PRINTF1(_L("CCameraClientViewFinder object was created using NewL")); 
	  		//Test Client ViewFinder APIs
	  		INFO_PRINTF1(_L("Create Histogram for Client ViewFinder"));
	  		CCamera::CCameraV2Histogram* histogram_CVF = clientVF->CreateHistogramHandleL();
	  		if(!histogram_CVF)
	  			{
	  			INFO_PRINTF1(_L("Histogram creation for Client ViewFinder unsuccessful"));
	  			result = EFail;
	  			}
	  			
	  		CleanupStack::PushL(histogram_CVF);
	  			
	  		INFO_PRINTF1(_L("Create ImageProcessing for Client ViewFinder"));
	  		
	  		TRAPD(err, clientVF->GetTransformationHandleL());
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("ImageProcessing creation for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		//set properties for client VF (base class)
	  		INFO_PRINTF1(_L("GetViewFinderFadingCapabilitiesL for Client ViewFinder"));
	  		CCameraViewFinder::TViewFinderFadingCapabilities fadingCapabilities;
	  		TRAP(err,clientVF->GetViewFinderFadingCapabilitiesL(fadingCapabilities));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderFadingCapabilitiesL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("GetViewFinderFadingEffectL for Client ViewFinder"));
	  		CCameraViewFinder::TViewFinderFadingEffect fadingEffect;
	  		TRAP(err,clientVF->GetViewFinderFadingEffectL(fadingEffect));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderFadingEffectL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	
			INFO_PRINTF1(_L("SetViewFinderFadingEffectL for Client ViewFinder"));
			iInputEventUid = KUidECamEvent2ViewFinderFadingEffect;
			clientVF->SetViewFinderFadingEffectL(fadingEffect);
			CheckNotificationNeg(iInputEventUid, result);
			
			TInt vfHandle=-1;
			clientVF->GetViewFinderHandleL(vfHandle);
	  	
	  		//set properties for histogram (for viewfinder)
	  		TUint supportedHistogramType=0;
	  		histogram_CVF->GetSupportedHistogramsL(supportedHistogramType);
	  		if(supportedHistogramType!=0)
	  			{
	  			INFO_PRINTF1(_L("unexpected GetSupportedHistogramsL"));
	  			result = EFail;
	  			}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Client ViewFinder"));
	  		TBool directHistogramSupported = EFalse;
	  		TRAP(err,histogram_CVF->GetDirectHistogramSupportInfoL(directHistogramSupported));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Client ViewFinder"));
	  		CCamera::CCameraV2Histogram::THistogramType histogramType = CCamera::CCameraV2Histogram::EHistNone;
	  		TRAP(err,histogram_CVF->PrepareClientHistogramL(histogramType));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Client ViewFinder"));
	  		CCamera::CCameraV2Histogram::TDirectHistogramParameters histogramParameters;
	  		TRAP(err,histogram_CVF->PrepareDirectHistogramL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Client ViewFinder"));
	   		TRAP(err,histogram_CVF->UpdateDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Client ViewFinder"));
	   		TRAP(err,histogram_CVF->GetDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
			INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Client ViewFinder"));
	   		TBool histogramActive = EFalse;
	   		TRAP(err,histogram_CVF->GetHistogramStateL(histogramActive));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			
  			//viewfinder starting...
  			INFO_PRINTF1(_L("StartClientViewFinderL for Client ViewFinder"));
			TSize size(320,240);
  			TRAP(err,clientVF->StartClientViewFinderL(0, CCamera::EFormatFbsBitmapColor64K, size));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("StartClientViewFinderL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			CheckViewFinderNegNotification(vfHandle, result);
			
			INFO_PRINTF1(_L("GetViewFinderBufferL for Client ViewFinder"));
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
			CleanupStack::PushL(bitmap);
			RCamBuffer2 cameraVFBuffer(bitmap);
			TRAP(err,clientVF->GetViewFinderBufferL(cameraVFBuffer));
			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderBufferL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
  			CleanupStack::PopAndDestroy(bitmap);
			
			TRect clipRect(TPoint(0,0),size);
			TRAP(err,clientVF->StartClientViewFinderL(0, CCamera::EFormatFbsBitmapColor64K, size, clipRect));
			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("StartClientViewFinderL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			CheckViewFinderNegNotification(vfHandle, result);
			
			INFO_PRINTF1(_L("GetClientViewFinderStateL for Client ViewFinder"));
			TBool activeVF = EFalse;;
			TRAP(err,clientVF->GetClientViewFinderStateL(activeVF));
			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetClientViewFinderStateL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			
			INFO_PRINTF1(_L("StartHistogram for Client ViewFinder"));
			histogram_CVF->StartHistogram();
			CheckViewFinderNegNotification(vfHandle, result);
	  		
	  		INFO_PRINTF1(_L("StopHistogram for Client ViewFinder"));	
			histogram_CVF->StopHistogram();
			
			INFO_PRINTF1(_L("GetClientViewFinderPropertiesL for Client ViewFinder"));	
			TInt screenNumber=0;
			CCamera::TFormat imageFormat=CCamera::EFormatFbsBitmapColor64K;
			TRAP(err,clientVF->GetClientViewFinderPropertiesL(screenNumber, imageFormat, size, clipRect));
			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetClientViewFinderPropertiesL for Client ViewFinder: unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("StopClientViewFinder for Client ViewFinder"));		
			clientVF->StopClientViewFinder();
	
  			CleanupStack::PopAndDestroy(histogram_CVF);
	    	CleanupStack::PopAndDestroy(clientVF);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraClientViewFinder object not created")); 
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
	
/*------------------------------------------------------
 	Direct ViewFinder : RECamV2DirectViewFinderTest
 *------------------------------------------------------*/
RECamV2DirectViewFinderTest* RECamV2DirectViewFinderTest::NewL(const TDesC& aTestStep)
	{
	RECamV2DirectViewFinderTest* self = new (ELeave) RECamV2DirectViewFinderTest(aTestStep);
	return self;	
	}
	
RECamV2DirectViewFinderTest::RECamV2DirectViewFinderTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamV2DirectViewFinderTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-052-HP")))
		{
		return DoTestStep_52L();
		}
	return EInconclusive;	
	}
	
TVerdict RECamV2DirectViewFinderTest::DoTestStep_52L()
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

		TRAP(err, verdict = DoV2DirectVFTestStepL());

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
	
TVerdict RECamV2DirectViewFinderTest::DoV2DirectVFTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	MCameraObserver* observer = NULL;
	MDirectViewFinderObserver* directVFObserver = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraV2DirectViewFinder* directVF = NULL;
	
	// using observer 	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver"));
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create v2 direct viewfinder from CCamera object"));
		
		TRAP(error, directVF  = CCamera::CCameraV2DirectViewFinder::NewL(*camera, *directVFObserver));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraV2DirectViewFinder object creation using old Observer failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object creation using old Observer gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// using observer2 with NewL 
	MCameraObserver2* observer2 = NULL;	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create v2 direct viewfinder from CCamera object"));
		
		TRAP(error, directVF  = CCamera::CCameraV2DirectViewFinder::NewL(*camera, *directVFObserver));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraV2DirectViewFinder object creation using Observer2 with NewL failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object creation using Observer2 with NewL gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// create v2 direct viewfinder object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, directVF = CCamera::CCameraV2DirectViewFinder::NewL(*camera, *directVFObserver));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(directVF);
	  		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(directVF);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object was not created using NewL")); 
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

/*------------------------------------------------------
 	Direct ViewFinder : RECamV2DirectVFNotificationTest
 *------------------------------------------------------*/
RECamV2DirectVFNotificationTest* RECamV2DirectVFNotificationTest::NewL(TBool aAllocTest)
	{
	RECamV2DirectVFNotificationTest* self = new (ELeave) RECamV2DirectVFNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamV2DirectVFNotificationTest::ConstructL()
	{
	}
	
RECamV2DirectVFNotificationTest::RECamV2DirectVFNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-053-HP");
	}
	
void RECamV2DirectVFNotificationTest::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode)
	{
	aDirectViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}

void RECamV2DirectVFNotificationTest::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt aErrorCode)
	{
	aDirectViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}

void RECamV2DirectVFNotificationTest::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt aErrorCode)
	{
	aDirectViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}
	
void RECamV2DirectVFNotificationTest::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode)
	{
	aDirectViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;	
	}
	
void RECamV2DirectVFNotificationTest::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt aErrorCode)
	{
	aDirectViewFinderHandle.GetViewFinderHandleL(iVFHandle);
	iError = aErrorCode;
	}
	
TInt RECamV2DirectVFNotificationTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

void RECamV2DirectVFNotificationTest::CheckViewFinderNegNotification(TInt aVFHandle, TVerdict& aResult)	
	{
	INFO_PRINTF4(_L("Expected DirectViewFinder Id %d, received event %x. Error %d."), aVFHandle, iVFHandle, iError); 
	if (aVFHandle != iVFHandle || iError != KErrNotSupported)
		{
	    aResult = EFail;
	    }
	}
	
TVerdict RECamV2DirectVFNotificationTest::DoTestStepL()
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1(_L("Alloc test"));
	
	TInt i, err;
	
	for (i =1; ;i++)
		{
		if (i % 5 == 0)
			{
			INFO_PRINTF2(_L("Fail count = %d"), i);
			}

		__UHEAP_SETFAIL(RHeap::EFailNext,i);
		__MM_HEAP_MARK;

		TRAP(err,verdict = DoTestStep_53L());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = ((testAlloc == NULL ) && (err == KErrNone));
		User::Free(testAlloc);

		if ((err != KErrNoMemory && verdict == EPass) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}

		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		}
	return verdict;	
	}

TVerdict RECamV2DirectVFNotificationTest::DoTestStep_53L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraV2DirectViewFinder* directVF = NULL;
	
	// create client viewfinder object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this, 0, 0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, directVF  = CCamera::CCameraV2DirectViewFinder::NewL(*camera, *this));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(directVF);
	  		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object was created using NewL")); 
	  		//Test Direct ViewFinder APIs
	  		INFO_PRINTF1(_L("Create Histogram for Direct ViewFinder"));
	  		CCamera::CCameraV2Histogram* histogram_DVF = directVF->CreateHistogramHandleL();
	  		if(!histogram_DVF)
	  			{
	  			INFO_PRINTF1(_L("Histogram creation for Direct ViewFinder unsuccessful"));
	  			result = EFail;
	  			}
	  			
	  		CleanupStack::PushL(histogram_DVF);
	  			
	  		INFO_PRINTF1(_L("Create ImageProcessing for Direct ViewFinder"));
	  		
	  		TRAPD(err, directVF->GetTransformationHandleL());
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("ImageProcessing creation for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		//set properties for direct VF (base class)
	  		INFO_PRINTF1(_L("GetViewFinderFadingCapabilitiesL for Direct ViewFinder"));
	  		CCameraViewFinder::TViewFinderFadingCapabilities fadingCapabilities;
	  		TRAP(err,directVF->GetViewFinderFadingCapabilitiesL(fadingCapabilities));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderFadingCapabilitiesL for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("GetViewFinderFadingEffectL for Direct ViewFinder"));
	  		CCameraViewFinder::TViewFinderFadingEffect fadingEffect;
	  		TRAP(err,directVF->GetViewFinderFadingEffectL(fadingEffect));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderFadingEffectL for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	
			INFO_PRINTF1(_L("SetViewFinderFadingEffectL for Direct ViewFinder"));
			iInputEventUid = KUidECamEvent2ViewFinderFadingEffect;
			directVF->SetViewFinderFadingEffectL(fadingEffect);
			CheckNotificationNeg(iInputEventUid, result);
			
			TInt vfHandle=-1;
			directVF->GetViewFinderHandleL(vfHandle);
	  	
	  		//set properties for histogram (for viewfinder)
	  		TUint supportedHistogramType=0;
	  		histogram_DVF->GetSupportedHistogramsL(supportedHistogramType);
	  		if(supportedHistogramType!=0)
	  			{
	  			INFO_PRINTF1(_L("unexpected GetSupportedHistogramsL"));
	  			result = EFail;
	  			}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Direct ViewFinder"));
	  		TBool directHistogramSupported = EFalse;
	  		TRAP(err,histogram_DVF->GetDirectHistogramSupportInfoL(directHistogramSupported));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Direct ViewFinder"));
	  		CCamera::CCameraV2Histogram::THistogramType histogramType = CCamera::CCameraV2Histogram::EHistNone;
	  		TRAP(err,histogram_DVF->PrepareClientHistogramL(histogramType));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Direct ViewFinder"));
	  		CCamera::CCameraV2Histogram::TDirectHistogramParameters histogramParameters;
	  		TRAP(err,histogram_DVF->PrepareDirectHistogramL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Direct ViewFinder"));
	   		TRAP(err,histogram_DVF->UpdateDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Direct ViewFinder"));
	   		TRAP(err,histogram_DVF->GetDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
	  			
			INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Direct ViewFinder"));
	   		TBool histogramActive = EFalse;
	   		TRAP(err,histogram_DVF->GetHistogramStateL(histogramActive));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
		
  			//viewfinder starting...
  			INFO_PRINTF1(_L("StartViewFinderDirectL for Direct ViewFinder"));
  			TSize size(320,240);
			TRect rect(TPoint(0,0),size);
			TRect cliprect(rect);
			RWsSession ws;
			CWsScreenDevice* screenDevice = NULL;
			RWindow window;
  			TRAP(err,directVF->StartViewFinderDirectL(ws, *screenDevice, window, rect));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("StartViewFinderDirectL for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			CheckViewFinderNegNotification(vfHandle, result);
			
			TRAP(err,directVF->StartViewFinderDirectL(ws, *screenDevice, window, rect, cliprect));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("StartViewFinderDirectL (with cliprect) for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
			CheckViewFinderNegNotification(vfHandle, result);
			
			INFO_PRINTF1(_L("GetDirectViewFinderPropertiesL for Direct ViewFinder"));
			TInt screenNumber=0;
			TRAP(err,directVF->GetDirectViewFinderPropertiesL(screenNumber, rect, cliprect));
			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectViewFinderPropertiesL for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("PauseViewFinderDirect for Direct ViewFinder"));
			directVF->PauseViewFinderDirect();
			
			INFO_PRINTF1(_L("ResumeViewFinderDirect for Direct ViewFinder"));
			directVF->ResumeViewFinderDirect();
			CheckViewFinderNegNotification(vfHandle, result);
			
			INFO_PRINTF1(_L("StartHistogram for Direct ViewFinder"));
			histogram_DVF->StartHistogram();
			CheckViewFinderNegNotification(vfHandle, result);
	  		
	  		INFO_PRINTF1(_L("StopHistogram for Direct ViewFinder"));	
			histogram_DVF->StopHistogram();
			
			INFO_PRINTF1(_L("GetViewFinderStateL for Direct ViewFinder"));
			CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState = CCamera::CCameraV2DirectViewFinder::EViewFinderInActive;
			TRAP(err,directVF->GetViewFinderStateL(vfState));
			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetViewFinderStateL for Direct ViewFinder: unexpected result!"));
  				result = EFail;
  				}
  				
  			//access direct snapshot functionality
  			INFO_PRINTF1(_L("Access direct snapshot functionality"));
  			CCamera::CCameraDirectSnapshot* directSnapshot = directVF->GetDirectSnapshotHandleL();
  			if(directSnapshot == NULL)
	  			{
	  			INFO_PRINTF1(_L("Direct Snapshot creation for Direct ViewFinder unsuccessful"));
	  			result = EFail;
	  			}
	  			
	  		CleanupStack::PushL(directSnapshot);
	  		
	  		INFO_PRINTF1(_L("Get parent viewfinder for direct snapshot"));
	  		CCamera::CCameraV2DirectViewFinder* directViewFinder = NULL;
	  		TRAP(err, directSnapshot->GetDirectViewFinderL(directViewFinder));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectViewFinderL for Direct ViewFinder Snapshot: unexpected result!"));
  				result = EFail;
  				}
  			
  			TInt viewfinderHandle = -1;
  			directViewFinder->GetViewFinderHandleL(viewfinderHandle);
  			if(vfHandle != viewfinderHandle)
  				{
  				INFO_PRINTF1(_L("Parent Viewfinder for Direct ViewFinder Snapshot: unexpected result!"));
  				result = EFail;
  				}
  			
  			CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters directSnapshotParameters;
  			
  			if(directSnapshotParameters.Size() != sizeof(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters) ||
  				directSnapshotParameters.Version() != KECamDirectSnapshotParametersCurrentVersion)
  				{
  				INFO_PRINTF1(_L("TDirectSnapshotParameters has different version for Direct Snapshot: unexpected result!"));
  				result = EFail;
  				}
  			
   			INFO_PRINTF1(_L("SetAspectRatioState for Direct Snapshot"));
			directSnapshotParameters.SetAspectRatioState(ETrue);
			TBool aspectRatioState = directSnapshotParameters.IsAspectRatioMaintained();
			if(!aspectRatioState)
				{
				INFO_PRINTF1(_L("AspectRatioState for Direct Snapshot: unexpected result!"));
  				result = EFail;
				}
				
			directSnapshotParameters.SetAspectRatioState(EFalse);
			aspectRatioState = directSnapshotParameters.IsAspectRatioMaintained();
			if(aspectRatioState != EFalse)
				{
				INFO_PRINTF1(_L("AspectRatioState for Direct Snapshot: unexpected result!"));
  				result = EFail;
				}
			
			INFO_PRINTF1(_L("EnableDirectSnapshotL for Direct Snapshot"));	
  			TRAP(err, directSnapshot->EnableDirectSnapshotL(directSnapshotParameters));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("EnableDirectSnapshotL for DirectSnapshot: unexpected result!"));
  				result = EFail;
  				}
  			
  			INFO_PRINTF1(_L("GetDirectSnapshotStateL for Direct Snapshot"));
  			CCamera::CCameraDirectSnapshot::TDirectSnapshotState directSnapshotState;	
  			TRAP(err, directSnapshot->GetDirectSnapshotStateL(directSnapshotState));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectSnapshotStateL for DirectSnapshot: unexpected result!"));
  				result = EFail;
  				}
  				
  			if(CCamera::CCameraDirectSnapshot::EDirectSnapshotEnabled != directSnapshotState)
  				{
  				INFO_PRINTF1(_L("DirectSnapshotState not set properly: unexpected result!"));
  				result = EFail;
  				}
  			
  			INFO_PRINTF1(_L("GetDirectSnapshotParametersL for Direct Snapshot"));
  			CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters directSnapshotParam;	
  			TRAP(err, directSnapshot->GetDirectSnapshotParametersL(directSnapshotParam));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectSnapshotParametersL for DirectSnapshot: unexpected result!"));
  				result = EFail;
  				}
  				
  			if(directSnapshotParam.IsAspectRatioMaintained() != directSnapshotParameters.IsAspectRatioMaintained())
  				{
  				INFO_PRINTF1(_L("DirectSnapshotParameters for DirectSnapshot wrongly set: unexpected result!"));
  				result = EFail;
  				}
  			
  			INFO_PRINTF1(_L("SetDirectSnapshotParametersL for Direct Snapshot"));
  			directSnapshotParameters.SetAspectRatioState(ETrue);	
  			TRAP(err, directSnapshot->SetDirectSnapshotParametersL(directSnapshotParameters));
			if(err != KErrNone)
				{
				INFO_PRINTF1(_L("SetDirectSnapshotParametersL for DirectSnapshot: unexpected result!"));
  				result = EFail;	
				}
			
			INFO_PRINTF1(_L("DisableDirectSnapshot for Direct Snapshot"));
			directSnapshot->DisableDirectSnapshot();
			TRAP(err, directSnapshot->GetDirectSnapshotStateL(directSnapshotState));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectSnapshotStateL for DirectSnapshot: unexpected result!"));
  				result = EFail;
  				}
  			
  			if(CCamera::CCameraDirectSnapshot::EDirectSnapshotDisabled != directSnapshotState)
  				{
  				INFO_PRINTF1(_L("GetDirectSnapshotStateL for DirectSnapshot: unexpected result!"));
  				result = EFail;
  				}
							
			INFO_PRINTF1(_L("StopDirectViewFinder for Direct ViewFinder"));
			directVF->StopDirectViewFinder();
			
			CleanupStack::PopAndDestroy(directSnapshot);
			CleanupStack::PopAndDestroy(histogram_DVF);
	    	CleanupStack::PopAndDestroy(directVF);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraV2DirectViewFinder object not created")); 
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
/*------------------------------------------------------------------
 	Pre Image Capture Control : RECamPreImgCaptureNotificationTest
 *-----------------------------------------------------------------*/
RECamPreImgCaptureNotificationTest* RECamPreImgCaptureNotificationTest::NewL(TBool aAllocTest)
	{
	return new (ELeave)RECamPreImgCaptureNotificationTest(aAllocTest);
	}
	
RECamPreImgCaptureNotificationTest::RECamPreImgCaptureNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-054-HP");
	iCaptureImageHandle = NULL;
	}
	
void RECamPreImgCaptureNotificationTest::PrepareImageComplete(CCamera::CCameraImageCapture* aCaptureImageHandle, TInt aErrorCode)
	{
	iCaptureImageHandle = aCaptureImageHandle;
	iError = aErrorCode;
	}

TInt RECamPreImgCaptureNotificationTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

void RECamPreImgCaptureNotificationTest::CheckCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult)
	{
	INFO_PRINTF2(_L("Pre Image Capture Control Error %d, "), iError); 
	if (iError != KErrNone)
		{
	    aResult = EFail;
	    }
	else
		{
		aImageCapture = iCaptureImageHandle;
		}
	}
	
TVerdict RECamPreImgCaptureNotificationTest::DoTestStepL()
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1(_L("Alloc test"));
	
	TInt i, err;
	
	for (i =1; ;i++)
		{
		if (i % 5 == 0)
			{
			INFO_PRINTF2(_L("Fail count = %d"), i);
			}

		__UHEAP_SETFAIL(RHeap::EFailNext,i);
		__MM_HEAP_MARK;

		TRAP(err,verdict = DoPreImgCaptureTestStepL());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = ((testAlloc == NULL ) && (err == KErrNone));
		User::Free(testAlloc);

		if ((err != KErrNoMemory && verdict == EPass) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}

		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		}
	return verdict;
	}

TVerdict RECamPreImgCaptureNotificationTest::DoPreImgCaptureTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPreImageCaptureControl* preImgCapture = NULL;
	
	// using observer 	
	__MM_HEAP_MARK;
	MCameraObserver* observer=NULL;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver"));
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create PreImageCaptureControl from CCamera object"));
		
		TRAP(error, preImgCapture  = CCamera::CCameraPreImageCaptureControl::NewL(*camera, *this));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraPreImageCaptureControl object creation using old Observer failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object creation using old Observer gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// using observer2 with NewL 
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create PreImageCaptureControl from CCamera object"));
		
		TRAP(error, preImgCapture  = CCamera::CCameraPreImageCaptureControl::NewL(*camera, *this));		
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraPreImageCaptureControl object creation using Observer2 with NewL failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object creation using Observer2 with NewL gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// create pre image control object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, preImgCapture = CCamera::CCameraPreImageCaptureControl::NewL(*camera, *this));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(preImgCapture);
	  		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object was created using NewL")); 
	  		
	  		INFO_PRINTF1(_L("GetDirectSnapshotSupportInfoL for PreImageCaptureControl"));
	  		TUint directSnapshotSupportInfo = 0;
	  		TRAPD(err, preImgCapture->GetDirectSnapshotSupportInfoL(directSnapshotSupportInfo));
	  		if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectSnapshotSupportInfoL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
	    	
	    	INFO_PRINTF1(_L("GetSupportedEmbeddedStillCaptureSettingsL for PreImageCaptureControl"));
	  		RArray<TUid> supportedEmbeddedStillCaptureSettings;
	  		TRAP(err, preImgCapture->GetSupportedEmbeddedStillCaptureSettingsL(supportedEmbeddedStillCaptureSettings));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetSupportedEmbeddedStillCaptureSettingsL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  			
  			if(supportedEmbeddedStillCaptureSettings.Count() != 0)
  				{
  				INFO_PRINTF1(_L("Number of SupportedEmbeddedStillCaptureSettings for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
	    	
	    	INFO_PRINTF1(_L("GetSupportedDirectSavingTypeL for PreImageCaptureControl"));
	  		CCamera::CCameraPreImageCaptureControl::TDirectSavingType supportedDirectSavingType 
	  				= CCamera::CCameraPreImageCaptureControl::EDirectSavingNotUsed;
	  		TRAP(err, preImgCapture->GetSupportedDirectSavingTypeL(supportedDirectSavingType));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetSupportedDirectSavingTypeL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
			INFO_PRINTF1(_L("SetSequentialImageFilenameL for PreImageCaptureControl"));
	  		_LIT8(KDirectSavingFileName, "C:\\ECam.jpg");
	  		TInt seqNum = 0;
	  		TRAP(err, preImgCapture->SetSequentialImageFilenameL(KDirectSavingFileName, seqNum));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("SetSequentialImageFilenameL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
			
			INFO_PRINTF1(_L("SetLowerResolutionSequentialImageFilenameL for PreImageCaptureControl"));
	  		_LIT8(KLowResDirectSavingFileName, "C:\\ECamLowRes.jpg");
	  		TRAP(err, preImgCapture->SetLowerResolutionSequentialImageFilenameL(KLowResDirectSavingFileName, seqNum));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("SetLowerResolutionSequentialImageFilenameL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("GetDirectSavingTypeL for PreImageCaptureControl"));
	  		CCamera::CCameraPreImageCaptureControl::TDirectSavingType directSavingType 
	  				= CCamera::CCameraPreImageCaptureControl::EDirectSavingNotUsed;
	  		TRAP(err, preImgCapture->GetDirectSavingTypeL(directSavingType));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetDirectSavingTypeL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("SetDirectSavingTypeL for PreImageCaptureControl"));
  			TRAP(err, preImgCapture->SetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::EDirectSavingHighResolutionFileOnly));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("SetDirectSavingTypeL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("GetCaptureEventSupportInfoL for PreImageCaptureControl"));
  			TUint supportedDriveModes = 0;
  			TRAP(err, preImgCapture->GetCaptureEventSupportInfoL(supportedDriveModes));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetCaptureEventSupportInfoL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("GetImageFormatsSupportedL for PreImageCaptureControl"));
  			TSize size(640,480);
  			TUint imageFormatsSupported = 0;
  			TRAP(err, preImgCapture->GetImageFormatsSupportedL(imageFormatsSupported, size));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetImageFormatsSupportedL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("GetPixelAspectsSupportedL for PreImageCaptureControl"));
    			TUint pixelAspectsSupported = 0;
  			TRAP(err, preImgCapture->GetPixelAspectsSupportedL(pixelAspectsSupported, CCamera::EFormatFbsBitmapColor16M, size));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetPixelAspectsSupportedL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
			
			INFO_PRINTF1(_L("GetImageMaxMemorySizeSettingSupportInfoL for PreImageCaptureControl"));
  			TBool maxMemorySizeSettingSupported = EFalse;
  			TRAP(err, preImgCapture->GetImageMaxMemorySizeSettingSupportInfoL(maxMemorySizeSettingSupported));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetImageMaxMemorySizeSettingSupportInfoL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}
  			
  			INFO_PRINTF1(_L("GetImageMaxMemorySizeL for PreImageCaptureControl"));
  			TUint memorySize = 0;
  			TRAP(err, preImgCapture->GetImageMaxMemorySizeL(memorySize));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetImageMaxMemorySizeL for PreImageCaptureControl : unexpected result!"));
  				result = EFail;
  				}	
			
			INFO_PRINTF1(_L("GetSupportedProcessingOptionsL for PreImageCaptureControl"));
			TUint processingOptions = 0;
			TRAP(err, preImgCapture->GetSupportedProcessingOptionsL(processingOptions));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetSupportedProcessingOptionsL for PreImageCaptureControl : unexpected result!"));
				}
			
  			INFO_PRINTF1(_L("PrepareImageCapture for PreImageCaptureControl"));
  			CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters prepareImageParameters;
 
  			INFO_PRINTF1(_L("Setting processing options member variable"));
  			TUint options = 0;
  			options |= CCamera::CCameraPreImageCaptureControl::EEcamNormalProcessing;
  			prepareImageParameters.SetImageProcessingOptions(options);
  			INFO_PRINTF1(_L("Getting processing options member variable"));
  			TUint retrievedOptions = 0;
  			prepareImageParameters.GetImageProcessingOptions(retrievedOptions);
  			if(retrievedOptions != options)
  				{
  				INFO_PRINTF1(_L("Set image processing option does not match retrieved image processing option"));
  				result = EFail;
  				}
  			else
  				{
  				INFO_PRINTF1(_L("Set image processing option matches retrieved image processing option"));
  				}
 
  			MCaptureImageObserver* 	captureImageObserver = NULL;
  			preImgCapture->PrepareImageCapture(prepareImageParameters, *captureImageObserver);
  			CCamera::CCameraImageCapture* imageCapture = NULL;
			CheckCaptureNotification(imageCapture, result);	
			
			if(imageCapture == NULL)
				{
				INFO_PRINTF1(_L("CCameraImageCapture object not created!"));
  				result = EFail;
				}
			else
			    {
				delete imageCapture;
				}
	    
	    	CleanupStack::PopAndDestroy(preImgCapture);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object was not created using NewL")); 
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
/*------------------------------------------------------------------
 	Image Capture Control : RECamImgCaptureNotificationTest
 *-----------------------------------------------------------------*/
RECamImgCaptureNotificationTest* RECamImgCaptureNotificationTest::NewL(TBool aAllocTest)
	{
	return new (ELeave)RECamImgCaptureNotificationTest(aAllocTest);
	}
	
RECamImgCaptureNotificationTest::RECamImgCaptureNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-055-HP");
	iCaptureImageHandle = NULL;
	}
	
void RECamImgCaptureNotificationTest::PrepareImageComplete(CCamera::CCameraImageCapture* aCaptureImageHandle, TInt aErrorCode)
	{
	iCaptureImageHandle = aCaptureImageHandle;
	iError = aErrorCode;
	}

TInt RECamImgCaptureNotificationTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

void RECamImgCaptureNotificationTest::CheckPreCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult)
	{
	INFO_PRINTF2(_L("Pre Image Capture Control Error %d, "), iError); 
	if (iError != KErrNone)
		{
	    aResult = EFail;
	    }
	else
		{
		aImageCapture = iCaptureImageHandle;
		}
	}
	
void RECamImgCaptureNotificationTest::IndividualImageControlHandle(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	}
	
void RECamImgCaptureNotificationTest::ImageBufferReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;
	}
	
void RECamImgCaptureNotificationTest::ClientSnapshotForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MCameraBuffer2* /*aSnapshotBuffer*/, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;
	}
	
void RECamImgCaptureNotificationTest::DirectSnapshotForImageDisplayed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;
	}
	
void RECamImgCaptureNotificationTest::CutDownImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;	
	}
	
void RECamImgCaptureNotificationTest::ImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;	
	}
	
void RECamImgCaptureNotificationTest::ImageCaptureComplete(CCamera::CCameraImageCapture& aCaptureImageHandle, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iError = aErrorCode;	
	}
	
void RECamImgCaptureNotificationTest::ClientHistogramForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;		
	}
	
void RECamImgCaptureNotificationTest::ClientHistogramForSnapshotReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;	
	}
	
void RECamImgCaptureNotificationTest::DirectHistogramForSnapshotDisplayed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iError = aErrorCode;		
	}
	
void RECamImgCaptureNotificationTest::ProcessingFailed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TUint aProcessingTypes, TInt aErrorCode)
	{
	iCaptureImageHandle = &aCaptureImageHandle;
	iPostCaptureControlId = aPostCaptureControlId;
	iProcessingTypes = aProcessingTypes;
	iError = aErrorCode;	
	}

void RECamImgCaptureNotificationTest::CheckCaptureNotification(CCamera::CCameraImageCapture*& aImageCapture, TPostCaptureControlId& aPostCaptureControlId, TVerdict& aResult)
	{
	INFO_PRINTF2(_L("Image Capture Control Error %d, "), iError); 
	if (iError != KErrNotSupported)
		{
	    aResult = EFail;
	    }
	else
		{
		aImageCapture = iCaptureImageHandle;
		aPostCaptureControlId = iPostCaptureControlId;
		}
	}
	
void RECamImgCaptureNotificationTest::CheckCaptureCompletion(CCamera::CCameraImageCapture*& aImageCapture, TVerdict& aResult)
	{
	INFO_PRINTF2(_L("Image Capture Control Error %d, "), iError); 
	if (iError != KErrNotSupported)
		{
	    aResult = EFail;
	    }
	else
		{
		aImageCapture = iCaptureImageHandle;
		}
	}
	
TVerdict RECamImgCaptureNotificationTest::DoTestStepL()
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1(_L("Alloc test"));
	
	TInt i,err;
	
	for (i = 1 ; ; i++)
		{
		__MM_HEAP_MARK;
		
		if (i % 5 == 0)
			{
			INFO_PRINTF2(_L("Fail count = %d"), i);
			}

		__UHEAP_SETFAIL(RHeap::EFailNext, i);

		TRAP(err, verdict = DoImgCaptureAdvFuncConstruction());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = (testAlloc == NULL) && (err==KErrNone);
		User::Free(testAlloc);

		__UHEAP_RESET;
		__MM_HEAP_MARKEND;

		if ((err != KErrNoMemory && verdict == EPass) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}
		}
	return verdict;
	}
	
TVerdict RECamImgCaptureNotificationTest::DoImgCaptureAdvFuncConstruction()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPreImageCaptureControl* preImgCapture = NULL;
	
	// create pre image control object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, preImgCapture = CCamera::CCameraPreImageCaptureControl::NewL(*camera, *this));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(preImgCapture);
	  		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object was created using NewL")); 
	    	
	    	INFO_PRINTF1(_L("PrepareImageCapture for PreImageCaptureControl"));
  			CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters prepareImageParameters;
   			preImgCapture->PrepareImageCapture(prepareImageParameters, *this);
  			CCamera::CCameraImageCapture* imageCapture = NULL;
			CheckPreCaptureNotification(imageCapture, result);	
			
			if(imageCapture == NULL)
				{
				INFO_PRINTF1(_L("CCameraImageCapture object not created!"));
  				result = EFail;
				}
			else
			    {
			    CleanupStack::PushL(imageCapture);
				// image capture opertions.
				
				INFO_PRINTF1(_L("Create Histogram for Image Capture"));
				CCamera::CCameraV2Histogram* histogram_IC = NULL;
		  		TRAP(error, histogram_IC = imageCapture->CreateHistogramHandleL());
		  		
		  		if(error != KErrNone /*|| !histogram_IC*/)
		  			{
		  			INFO_PRINTF1(_L("Histogram creation for Image Capture unsuccessful"));
		  			result = EFail;
		  			CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
		  			User::Leave(error);
		  			}
		  			
		  		CleanupStack::PushL(histogram_IC);
		  			
		  		INFO_PRINTF1(_L("Create ImageProcessing for Image Capture "));
		  		
		  		CCamera::CCameraImageProcessing* impProc = NULL;
		  		TRAP(error, impProc = imageCapture->GetTransformationHandleL());
	  			
	  			if(error != KErrNone /*|| !impProc*/)
	  				{
	  				INFO_PRINTF1(_L("ImageProcessing creation for Image Capture unsuccessful"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(error);
	  				}
	  				
	  			CleanupStack::PushL(impProc);
	  				
	  			INFO_PRINTF1(_L("Create snapshot for Image Capture"));
	  			TInt viewfinderID = 0;
	  			CCamera::CCameraSnapshot* snapshot_IC = NULL;
		  		TRAP(error,snapshot_IC = imageCapture->GetSnapshotHandleL(viewfinderID));
	  			
	  			if(error != KErrNone /*||!snapshot_IC*/)
	  				{
	  				INFO_PRINTF1(_L("snapshot creation for Image Capture : unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(error);
	  				}
				CleanupStack::PushL(snapshot_IC);
				
				INFO_PRINTF1(_L("Create histogram for snapshot for Image Capture"));
				CCamera::CCameraV2Histogram* histogram_snapshot_IC = NULL;
		  		
		  		TRAP(error, histogram_snapshot_IC = snapshot_IC->CreateHistogramHandleL());
		  		
		  		if(error != KErrNone /*|| !histogram_snapshot_IC*/)
		  			{
		  			INFO_PRINTF1(_L("Histogram creation for snapshot for Image Capture unsuccessful"));
		  			result = EFail;
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(error);
		  			}
		  		CleanupStack::PushL(histogram_snapshot_IC);
		  		
		  		//check img proc for image capture
		  		INFO_PRINTF1(_L("GetSupportedTransformationsL for imgProc for Image Capture"));
		  		RArray<TUid> transformations;
		  		TRAPD(err, impProc->GetSupportedTransformationsL(transformations));
		  		if(err != KErrNone)
		  			{
		  			INFO_PRINTF1(_L("GetSupportedTransformationsL for imgProc for Image Capture : unexpected result!"));
		  			result = EFail;
		  			CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
		  			}
		  		if(transformations.Count() != 0)
		  			{
		  			INFO_PRINTF1(_L("Number of SupportedTransformations for imgProc for Image Capture : unexpected result!"));
		  			result = EFail;
		  			}
		  		transformations.Close();
		  		
		  		INFO_PRINTF1(_L("Check New Img Proc methods for Image Capture"));
		  		CCamera::CCameraImageProcessing::TOrientationReference orientationRef 
		  				= CCamera::CCameraImageProcessing::EOrientationReferenceTiltOrientation;
		  		TUint suppRelativeRotation=0;
		  		TUint supportedRelMirroring=0;
		  		TUint supportedRelFlipping=0;
		  		TRAP(err, impProc->GetSupportedRelativeOrientationOptionsL(orientationRef, suppRelativeRotation, supportedRelMirroring, supportedRelFlipping));
		  		if(err != KErrNotSupported)
		  			{
		  			INFO_PRINTF1(_L("GetSupportedRelativeOrientationOptionsL for imgProc for Image Capture : unexpected result!"));
		  			result = EFail;
		  			}
		  		
		  		CCamera::CCameraImageProcessing::TRelativeRotation relRotation = 
		  					CCamera::CCameraImageProcessing::ERelativeRotation0Degrees;
		  			
		  		CCamera::CCameraImageProcessing::TRelativeMirror relMirror =
		  					CCamera::CCameraImageProcessing::ERelativeMirrorNone;
		  					
		  		CCamera::CCameraImageProcessing::TRelativeFlipping relFlipping =
		  					CCamera::CCameraImageProcessing::ERelativeFlippingNone;
		  		
		  		TRAP(err, impProc->GetCurrentRelativeOrientationOptionsL(orientationRef, relRotation, relMirror, relFlipping));
		  		if(err != KErrNotSupported)
		  			{
		  			INFO_PRINTF1(_L("GetCurrentRelativeOrientationOptionsL for imgProc for Image Capture : unexpected result!"));
		  			result = EFail;
		  			CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
		  			}
		  			
		  		TRAP(err, impProc->SetRelativeOrientationOptionsL(orientationRef, relRotation, relMirror, relFlipping));
		  		if(err != KErrNotSupported)
		  			{
		  			INFO_PRINTF1(_L("SetRelativeOrientationOptionsL for imgProc for Image Capture : unexpected result!"));
		  			result = EFail;
		  			CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
		  			}
												
		  		//prepare histogram for image capture
		  		INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Image Capture"));
		  		CCamera::CCameraV2Histogram::THistogramType histogramType = CCamera::CCameraV2Histogram::EHistNone;
		  		TRAP(err,histogram_IC->PrepareClientHistogramL(histogramType));
	  			if(err != KErrNotSupported)
	  				{
	  				INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Image Capture: unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
	  				}
	  				
	  			//prepare snapshot for image capture
	  			INFO_PRINTF1(_L("PrepareSnapshotL for Snapshot for Image Capture"));
				CCamera::CCameraSnapshot::TSnapshotParameters snapshotParam;
		   		TRAP(err,snapshot_IC->PrepareSnapshotL(snapshotParam));
		   		if(err != KErrNone)
		   			{
		   			INFO_PRINTF1(_L("PrepareSnapshotL for Snapshot for Image Capture: unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
		   			}
		   			
		   		//prepare histogram for snapshot for image capture
		  		INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for snapshot for Image Capture"));
		  		TRAP(err,histogram_snapshot_IC->PrepareClientHistogramL(histogramType));
	  			if(err != KErrNotSupported)
	  				{
	  				INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Snapshot for Image Capture: unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
	  				}
	  				
	  			//image capture operations
	  			INFO_PRINTF1(_L("GetPrepareImageParametersL for Image Capture"));
	  			CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters prepareImageParameters;
		  		TRAP(err,imageCapture->GetPrepareImageParametersL(prepareImageParameters));
	  			if(err != KErrNone)
	  				{
	  				INFO_PRINTF1(_L("GetPrepareImageParametersL for Image Capture: unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
	  				}
	  				
	  			INFO_PRINTF1(_L("CaptureImage for Image Capture"));
	  			imageCapture->CaptureImage();
	  			
	  			CCamera::CCameraImageCapture* captImg = NULL;
	  			CCamera::CCameraPostImageCaptureControl* postImgCapt = NULL;
	  			TPostCaptureControlId postCaptureControlId = 0;
	  			
	  			TRAP(err, iCaptureImageHandle->GetPostCaptureControlHandleL(postImgCapt, iPostCaptureControlId));
	  			if(err != KErrNone)
	  				{
	  				INFO_PRINTF1(_L("GetPostCaptureControlHandleL for Image Capture: unexpected result!"));
	  				result = EFail;
	  				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
		  			CleanupStack::PopAndDestroy(snapshot_IC);
	  				CleanupStack::PopAndDestroy(impProc);
	  				CleanupStack::PopAndDestroy(histogram_IC);
	  				CleanupStack::PopAndDestroy(imageCapture);
		  			CleanupStack::PopAndDestroy(preImgCapture);
		  			CleanupStack::PopAndDestroy(camera);
					User::Leave(err);
	  				}
	  				
	  			if(postImgCapt == NULL)
	  				{
	  				INFO_PRINTF1(_L("PostCaptureControl object was not provided for Image Capture: unexpected result!"));
	  				result = EFail;
	  				}
	  			else
	  				{
	  				CleanupStack::PushL(postImgCapt);
	  				//perform operations on post image capture control
	  				INFO_PRINTF1(_L("GetPostCaptureControlId for post imgCapt object for Image Capture"));
	  				TPostCaptureControlId postCaptCtrlId = 0;
	  				postImgCapt->GetPostCaptureControlId(postCaptCtrlId);
	  				if(postCaptCtrlId != iPostCaptureControlId)
	  					{
	  					INFO_PRINTF1(_L("PostCaptureControlId unexpected!"));
	  					result = EFail;
	  					}
	  				postCaptureControlId = postCaptCtrlId;
	  					
	  				INFO_PRINTF1(_L("ImageCaptureHandle for post imgCapt object for Image Capture"));
	  				TRAP(err, CCamera::CCameraImageCapture* captImg1 = postImgCapt->ImageCaptureHandle());
					if(err != KErrNone /*|| captImg1 == NULL*/)
						{
						INFO_PRINTF1(_L("CCameraImageCapture object from post image capture object is NULL and unexpected!"));
	  					result = EFail;
	  					CleanupStack::PopAndDestroy(postImgCapt);
						CleanupStack::PopAndDestroy(histogram_snapshot_IC);
			  			CleanupStack::PopAndDestroy(snapshot_IC);
		  				CleanupStack::PopAndDestroy(impProc);
		  				CleanupStack::PopAndDestroy(histogram_IC);
		  				CleanupStack::PopAndDestroy(imageCapture);
			  			CleanupStack::PopAndDestroy(preImgCapture);
			  			CleanupStack::PopAndDestroy(camera);
						User::Leave(err);
						}
						
					INFO_PRINTF1(_L("GetImageSequenceNumberL for post imgCapt object for Image Capture"));
	  				TUint seqNum = 0;
	  				TRAP(err, postImgCapt->GetImageSequenceNumberL(seqNum));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("GetImageSequenceNumberL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("SetImagePriorityL for post imgCapt object for Image Capture"));
	  				TECamImagePriority imagePriority = EECamImagePriorityLow;
	  				TRAP(err, postImgCapt->SetImagePriorityL(imagePriority));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("SetImagePriorityL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("GetImagePriorityL for post imgCapt object for Image Capture"));
	  				TRAP(err, postImgCapt->GetImagePriorityL(imagePriority));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("GetImagePriorityL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("PauseProcessing for post imgCapt object for Image Capture"));
	  				postImgCapt->PauseProcessing(0);
	  				
	  				INFO_PRINTF1(_L("ResumeProcessingL for post imgCapt object for Image Capture"));
	  				TRAP(err, postImgCapt->ResumeProcessingL(0));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("ResumeProcessingL for post image capture object unexpected!"));
	  					result = EFail;
						}
					
					INFO_PRINTF1(_L("GetImageStateL for post imgCapt object for Image Capture"));
	  				CCamera::CCameraPostImageCaptureControl::TImageState imgState = 
	  						CCamera::CCameraPostImageCaptureControl::EProcessingPending;
	  				TRAP(err, postImgCapt->GetImageStateL(imgState));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("GetImageStateL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("GetBufferStateL for post imgCapt object for Image Capture"));
	  				CCamera::CCameraPostImageCaptureControl::TBufferState bufferState = 
	  						CCamera::CCameraPostImageCaptureControl::EBufferNotPresent;
	  				TRAP(err, postImgCapt->GetBufferStateL(bufferState));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("GetBufferStateL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("GetImageBufferL for post imgCapt object for Image Capture"));
	  				MCameraImageBuffer* cameraImageBuffer = NULL;
	  				TRAP(err, postImgCapt->GetImageBufferL(*cameraImageBuffer));
	  				if(err != KErrNotSupported)
						{
						INFO_PRINTF1(_L("GetImageBufferL for post image capture object unexpected!"));
	  					result = EFail;
						}
						
					INFO_PRINTF1(_L("CancelImage for post imgCapt object for Image Capture"));
	  				postImgCapt->CancelImage();
	  				}
	  				
	  			CheckCaptureCompletion(captImg, result);
	  			
	  			//perform operations on image capture
	  			INFO_PRINTF1(_L("GetNumImagesExposedL for Image Capture"));
	  			TUint numImagesExposed = 0;
	  			TRAP(err, imageCapture->GetNumImagesExposedL(numImagesExposed));
	  			if(err != KErrNotSupported)
					{
					INFO_PRINTF1(_L("GetNumImagesExposedL for Image Capture unexpected!"));
  					result = EFail;
					}
					
				INFO_PRINTF1(_L("GetNumTotalImagesL for Image Capture"));
	  			TRAP(err, imageCapture->GetNumTotalImagesL(numImagesExposed));
	  			if(err != KErrNone)
					{
					INFO_PRINTF1(_L("GetNumTotalImagesL for Image Capture unexpected!"));
  					result = EFail;
					}
					
				INFO_PRINTF1(_L("SetCaptureImagePriorityL for Image Capture"));
				TECamImagePriority imagePriority = EECamImagePriorityLow;
	  			TRAP(err, imageCapture->SetCaptureImagePriorityL(imagePriority));
	  			if(err != KErrNotSupported)
					{
					INFO_PRINTF1(_L("SetCaptureImagePriorityL for Image Capture unexpected!"));
  					result = EFail;
					}
				
				INFO_PRINTF1(_L("GetCaptureImagePriorityL for Image Capture"));
	  			TRAP(err, imageCapture->GetCaptureImagePriorityL(imagePriority));
	  			if(err != KErrNotSupported)
					{
					INFO_PRINTF1(_L("GetCaptureImagePriorityL for Image Capture unexpected!"));
  					result = EFail;
					}
					
				INFO_PRINTF1(_L("PauseProcessing for Image Capture"));
	  			imageCapture->PauseProcessing(0);
	  			
	  			INFO_PRINTF1(_L("ResumeProcessingL for Image Capture"));
  				TRAP(err, imageCapture->ResumeProcessingL(0));
  				if(err != KErrNotSupported)
					{
					INFO_PRINTF1(_L("ResumeProcessingL for Image Capture unexpected!"));
  					result = EFail;
					}
				
				INFO_PRINTF1(_L("CancelCaptureImage for Image Capture"));
	  			imageCapture->CancelCaptureImage();
	  			
	  			//start histogram for image capture
	  			INFO_PRINTF1(_L("StartHistogram for Image Capture"));
				histogram_IC->StartHistogram();
				CCamera::CCameraImageCapture* camImgCapture=NULL;
				TPostCaptureControlId postCaptCtrlId = 0;
				CheckCaptureNotification(camImgCapture, postCaptCtrlId, result);
				if(camImgCapture == NULL || postCaptCtrlId != postCaptureControlId)
					{
					INFO_PRINTF1(_L("StartHistogram for Histogram for Image Capture unexpected!"));
  					result = EFail;	
					}
	  		
	  			INFO_PRINTF1(_L("StopHistogram for histogram for Image Capture"));	
				histogram_IC->StopHistogram();
				
				//start snapshot for image capture
				INFO_PRINTF1(_L("EnableSnapshotL for snapshot for Image Capture"));	
				TRAP(err,snapshot_IC->EnableSnapshotL());
				if(err != KErrNone)
					{
		   			INFO_PRINTF1(_L("EnableSnapshotL for snapshot for Image Capture: unexpected result!"));
	  				result = EFail;
		   			}
		   		camImgCapture = NULL;
		   		camImgCapture = 0;
		   		CheckCaptureNotification(camImgCapture, postCaptCtrlId, result);
				if(camImgCapture == NULL || postCaptCtrlId != postCaptureControlId)
					{
					INFO_PRINTF1(_L("EnableSnapshotL for snapshot for Image Capture unexpected!"));
  					result = EFail;	
					}
					
				//start histogram for snapshot for image capture
	  			INFO_PRINTF1(_L("StartHistogram for snapshot for Image Capture"));
				histogram_snapshot_IC->StartHistogram();
				camImgCapture=NULL;
				postCaptCtrlId = 0;
				CheckCaptureNotification(camImgCapture, postCaptCtrlId, result);
				if(camImgCapture == NULL || postCaptCtrlId != postCaptureControlId)
					{
					INFO_PRINTF1(_L("StartHistogram for Histogram for snapshot for Image Capture unexpected!"));
  					result = EFail;	
					}
	  		
	  			INFO_PRINTF1(_L("StopHistogram for histogram for snapshot for Image Capture"));	
				histogram_snapshot_IC->StopHistogram();	
				//
					
				INFO_PRINTF1(_L("DisableSnapshotL for snapshot for Image Capture"));	
				TRAP(err,snapshot_IC->DisableSnapshotL());
				if(err != KErrNone)
					{
		   			INFO_PRINTF1(_L("DisableSnapshotL for snapshot for Image Capture: unexpected result!"));
	  				result = EFail;
		   			}
	  		 	
				CleanupStack::PopAndDestroy(postImgCapt);
				CleanupStack::PopAndDestroy(histogram_snapshot_IC);
				CleanupStack::PopAndDestroy(snapshot_IC);
				CleanupStack::PopAndDestroy(impProc);
				CleanupStack::PopAndDestroy(histogram_IC);
				CleanupStack::PopAndDestroy(imageCapture);
				}
			CleanupStack::PopAndDestroy(preImgCapture);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraPreImageCaptureControl object was not created using NewL")); 
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
	
/*------------------------------------------------------
 	Video Capture Control : RECamVideoCaptureControlTest
 *------------------------------------------------------*/
RECamVideoCaptureControlTest* RECamVideoCaptureControlTest::NewL(const TDesC& aTestStep)
	{
	RECamVideoCaptureControlTest* self = new (ELeave) RECamVideoCaptureControlTest(aTestStep);
	return self;	
	}
	
RECamVideoCaptureControlTest::RECamVideoCaptureControlTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamVideoCaptureControlTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-056-HP")))
		{
		return DoTestStep_56L();
		}
	return EInconclusive;	
	}

TVerdict RECamVideoCaptureControlTest::DoTestStep_56L()
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

		TRAP(err, verdict = DoVideoCaptureTestStepL());

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

TVerdict RECamVideoCaptureControlTest::DoVideoCaptureTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;

	MCaptureVideoObserver* videoCaptureObserver = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraVideoCaptureControl* videoCapture = NULL;
	
	// using observer 	
	__MM_HEAP_MARK;
	MCameraObserver* observer=NULL;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver"));
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create VideoCaptureControl from CCamera object"));
		
		TRAP(error, videoCapture  = CCamera::CCameraVideoCaptureControl::NewL(*camera, *videoCaptureObserver));	
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraVideoCaptureControl object creation using old Observer failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraVideoCaptureControl object creation using old Observer gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// using observer2 with NewL 
	MCameraObserver2* observer2 = NULL;	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create VideoCaptureControl from CCamera object"));
		
		TRAP(error, videoCapture  = CCamera::CCameraVideoCaptureControl::NewL(*camera, *videoCaptureObserver));		
 		if (error==KErrExtensionNotSupported)
	    	{
	    	INFO_PRINTF1(_L("CCameraVideoCaptureControl object creation using Observer2 with NewL failed with correct error"));
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraVideoCaptureControl object creation using Observer2 with NewL gave unexpected result"));
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	
	// create video capture control object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, videoCapture = CCamera::CCameraVideoCaptureControl::NewL(*camera, *videoCaptureObserver));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(videoCapture);
	  		INFO_PRINTF1(_L("CCameraVideoCaptureControl object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(videoCapture);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraVideoCaptureControl object was not created using NewL")); 
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
	
/*--------------------------------------------------------------
 	Video Capture Control : RECamVideoCaptureNotificationTest
 *--------------------------------------------------------------*/
RECamVideoCaptureNotificationTest* RECamVideoCaptureNotificationTest::NewL(TBool aAllocTest)
	{
	RECamVideoCaptureNotificationTest* self = new (ELeave) RECamVideoCaptureNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamVideoCaptureNotificationTest::ConstructL()
	{
	}
	
RECamVideoCaptureNotificationTest::RECamVideoCaptureNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-057-HP");
	}
	
void RECamVideoCaptureNotificationTest::VideoBufferReady(MCameraBuffer2* /*aVideoBuffer*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	}

void RECamVideoCaptureNotificationTest::ClientSnapshotReady(MCameraBuffer2* /*aSnapshotBuffer*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	}

void RECamVideoCaptureNotificationTest::ClientHistogramReady(MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	}
	
void RECamVideoCaptureNotificationTest::ImageProcessingFailed(TInt aErrorCode)
	{
	iError = aErrorCode;	
	}
	
void RECamVideoCaptureNotificationTest::DirectVideoCaptureFailed(TInt aErrorCode)
	{
	iError = aErrorCode;
	}
	
TInt RECamVideoCaptureNotificationTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

void RECamVideoCaptureNotificationTest::CheckVideoCaptureNegNotification(TInt aErrorCode, TVerdict& aResult)	
	{
	INFO_PRINTF3(_L("Expected Video Capture Control Error %d, received Error %d."), aErrorCode, iError); 
	if (iError != aErrorCode)
		{
	    aResult = EFail;
	    }
	}
	
TVerdict RECamVideoCaptureNotificationTest::DoTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraVideoCaptureControl* videocapture = NULL;
	
	// create video capture control object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this, 0, 0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, videocapture  = CCamera::CCameraVideoCaptureControl::NewL(*camera, *this));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(videocapture);
	  		INFO_PRINTF1(_L("CCameraVideoCaptureControl object was created using NewL")); 
	  		//Test Video Capture APIs
	  		INFO_PRINTF1(_L("Create Histogram for Direct ViewFinder"));
	  		CCamera::CCameraV2Histogram* histogram_VC = videocapture->CreateHistogramHandleL();
	  		if(!histogram_VC)
	  			{
	  			INFO_PRINTF1(_L("Histogram creation for Video Capture Control unsuccessful"));
	  			result = EFail;
	  			}
	  			
	  		CleanupStack::PushL(histogram_VC);
	  			
	  		INFO_PRINTF1(_L("Create ImageProcessing for Video Capture Control "));
	  		
	  		TRAPD(err, videocapture->GetTransformationHandleL());
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("ImageProcessing creation for Video Capture Control : unexpected result!"));
  				result = EFail;
  				}
  				
  			INFO_PRINTF1(_L("Create snapshot for Video Capture Control "));
  			TInt viewfinderID=0;
	  		CCamera::CCameraSnapshot* snapshot_VC = videocapture->GetSnapshotHandleL(viewfinderID);
  			if(!snapshot_VC)
  				{
  				INFO_PRINTF1(_L("snapshot creation for Video Capture Control : unexpected result!"));
  				result = EFail;
  				}
  			
  			CleanupStack::PushL(snapshot_VC);
	  		
	  		//set properties for video capture
	  		INFO_PRINTF1(_L("GetVideoFormatsSupportedL for Video Capture Control"));
	  		TUint videoFormatsSupported=0;
	  		TSize size(320,240);
	  		TRAP(err,videocapture->GetVideoFormatsSupportedL(videoFormatsSupported, size));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetVideoFormatsSupportedL for Video Capture Control: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		if(videoFormatsSupported == 0)
	  			{
	  			INFO_PRINTF1(_L(" Number of VideoFormats Supported for Video Capture Control: unexpected result!"));
	  			}
	  		
	  		INFO_PRINTF1(_L("GetPixelAspectsSupportedL for Video Capture Control"));
	  		TUint pixelAspectsSupported=0;
	  		CCamera::TFormat format = CCamera::EFormatFbsBitmapColor16M;
	  		TRAP(err,videocapture->GetPixelAspectsSupportedL(pixelAspectsSupported, format, size));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetPixelAspectsSupportedL for Video Capture Control: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		if(pixelAspectsSupported == 0)
	  			{
	  			INFO_PRINTF1(_L(" Number of PixelAspects Supported for Video Capture Control: unexpected result!"));
	  			}	
	  			
			INFO_PRINTF1(_L("GetEmbeddedStillCaptureSupportInfoL for Video Capture Control"));
	  		TInt supportedEmbeddedStillCaptureTypes=0;
	  		TRAP(err,videocapture->GetEmbeddedStillCaptureSupportInfoL(supportedEmbeddedStillCaptureTypes));
  			if(err != KErrNone)
  				{
  				INFO_PRINTF1(_L("GetEmbeddedStillCaptureSupportInfoL for Video Capture Control: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		if(supportedEmbeddedStillCaptureTypes != 0)
	  			{
	  			INFO_PRINTF1(_L(" Supported Types of Embedded Still Capture for Video Capture Control: unexpected result!"));
	  			}	
	  			
			INFO_PRINTF1(_L("PrepareVideoCapture for Video Capture Control"));
			iInputEventUid = KUidECamEventVideoCaptureControlPrepareComplete;
			CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters prepareVideoParameters;
			
			prepareVideoParameters.SetEmbeddedStillCaptureState(ETrue);
			
			if(!prepareVideoParameters.IsEmbeddedStillCaptureEnabled())
				{
				INFO_PRINTF1(_L("IsEmbeddedStillCaptureEnabled for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
				
			prepareVideoParameters.SetEmbeddedStillCaptureState(EFalse);
			
			if(prepareVideoParameters.IsEmbeddedStillCaptureEnabled() != EFalse)
				{
				INFO_PRINTF1(_L("IsEmbeddedStillCaptureEnabled for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
			
	  		videocapture->PrepareVideoCapture(prepareVideoParameters);
	  		
	  		CheckNotification(iInputEventUid, result);	
	
    		INFO_PRINTF1(_L("GetRangeAffectedSettingsL for Video Capture Control"));
			RArray<TUid> affectedSettings;
			
			TRAP(err,videocapture->GetRangeAffectedSettingsL(affectedSettings));
			if(err != KErrNone)
				{
				INFO_PRINTF1(_L("GetRangeAffectedSettingsL for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
				
			if(affectedSettings.Count() != 0)
				{
				INFO_PRINTF1(_L("Number of RangeAffectedSettingsL for Video Capture Control: unexpected result!"));
				result = EFail;
				}
				
			INFO_PRINTF1(_L("GetValueAffectedSettingsL for Video Capture Control"));
			TRAP(err,videocapture->GetValueAffectedSettingsL(affectedSettings));
			if(err != KErrNone)
				{
				INFO_PRINTF1(_L("GetValueAffectedSettingsL for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
				
			if(affectedSettings.Count() != 0)
				{
				INFO_PRINTF1(_L("Number of ValueAffectedSettingsL for Video Capture Control: unexpected result!"));
				result = EFail;
				}
				
			INFO_PRINTF1(_L("GetDisabledSettingsL for Video Capture Control"));
			TRAP(err,videocapture->GetDisabledSettingsL(affectedSettings));
			if(err != KErrNone)
				{
				INFO_PRINTF1(_L("GetDisabledSettingsL for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
				
			if(affectedSettings.Count() != 0)
				{
				INFO_PRINTF1(_L("Number of DisabledSettingsL for Video Capture Control: unexpected result!"));
				result = EFail;
				}
				
			INFO_PRINTF1(_L("ReleaseVideoResource for Video Capture Control"));
			videocapture->ReleaseVideoResource();
			
			INFO_PRINTF1(_L("GetFadingEffectStateL for Video Capture Control"));
			CCamera::CCameraVideoCaptureControl::TFadingEffectState fadingEffectState = 
																CCamera::CCameraVideoCaptureControl::EFadingEffectDisabled;
			TRAP(err,videocapture->GetFadingEffectStateL(fadingEffectState));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetFadingEffectStateL for Video Capture Control: unexpected result!"));
  				result = EFail;
				}
			
			INFO_PRINTF1(_L("SetFadingEffectState for Video Capture Control"));
			iInputEventUid = KUidECamEventVideoCaptureControlFadingEffect;
			videocapture->SetFadingEffectState(CCamera::CCameraVideoCaptureControl::EFadingEffectEnabled);
			CheckNotificationNeg(iInputEventUid, result);
			
    		//set properties for histogram (for video capture)
	  		TUint supportedHistogramType=0;
	  		histogram_VC->GetSupportedHistogramsL(supportedHistogramType);
	  		if(supportedHistogramType!=0)
	  			{
	  			INFO_PRINTF1(_L("unexpected GetSupportedHistogramsL"));
	  			result = EFail;
	  			}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Video Capture"));
	  		TBool directHistogramSupported = EFalse;
	  		TRAP(err,histogram_VC->GetDirectHistogramSupportInfoL(directHistogramSupported));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramSupportInfoL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Video Capture"));
	  		CCamera::CCameraV2Histogram::THistogramType histogramType = CCamera::CCameraV2Histogram::EHistNone;
	  		TRAP(err,histogram_VC->PrepareClientHistogramL(histogramType));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareClientHistogramL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
	  		
	  		INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Video Capture"));
	  		CCamera::CCameraV2Histogram::TDirectHistogramParameters histogramParameters;
	  		TRAP(err,histogram_VC->PrepareDirectHistogramL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("PrepareDirectHistogramL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Video Capture"));
	   		TRAP(err,histogram_VC->UpdateDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("UpdateDirectHistogramPropertiesL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
	  			
	  		INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Video Capture"));
	   		TRAP(err,histogram_VC->GetDirectHistogramPropertiesL(histogramParameters));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetDirectHistogramPropertiesL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
	  			
			INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Video Capture"));
	   		TBool histogramActive = EFalse;
	   		TRAP(err,histogram_VC->GetHistogramStateL(histogramActive));
  			if(err != KErrNotSupported)
  				{
  				INFO_PRINTF1(_L("GetHistogramStateL for Histogram for Video Capture: unexpected result!"));
  				result = EFail;
  				}
			
			//set properties for snapshot (for video capture)
			CCamera::CCameraSnapshot::TSnapshotParameters snapshotParam;
			
			INFO_PRINTF1(_L("SetAspectRatioState for Snapshot for Video Capture"));
			snapshotParam.SetAspectRatioState(ETrue);
			TBool aspectRatioState = snapshotParam.IsAspectRatioMaintained();
			if(!aspectRatioState)
				{
				INFO_PRINTF1(_L("AspectRatioState for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
				}
			snapshotParam.SetAspectRatioState(EFalse);
			aspectRatioState = snapshotParam.IsAspectRatioMaintained();
			if(aspectRatioState != EFalse)
				{
				INFO_PRINTF1(_L("AspectRatioState for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
				}
			
			INFO_PRINTF1(_L("PrepareSnapshotL for Snapshot for Video Capture"));
			TRAP(err,snapshot_VC->PrepareSnapshotL(snapshotParam));
	   		if(err != KErrNone)
	   			{
	   			INFO_PRINTF1(_L("PrepareSnapshotL for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF1(_L("GetSnapshotParametersL for Snapshot for Video Capture"));
			TRAP(err,snapshot_VC->GetSnapshotParametersL(snapshotParam));
	   		if(err != KErrNone)
	   			{
	   			INFO_PRINTF1(_L("GetSnapshotParametersL for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   		
	   		INFO_PRINTF1(_L("SetSnapshotParametersL for Snapshot for Video Capture"));
			TRAP(err,snapshot_VC->SetSnapshotParametersL(snapshotParam));
	   		if(err != KErrNone)
	   			{
	   			INFO_PRINTF1(_L("SetSnapshotParametersL for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   				
			INFO_PRINTF1(_L("SelectSnapshotVideoFramesL for Snapshot for Video Capture"));
			CCamera::CCameraSnapshot::TSnapshotVideoFrames snapshotVideoFrames = CCamera::CCameraSnapshot::ESnapshotVideoFirstFrame;
			TRAP(err,snapshot_VC->SelectSnapshotVideoFramesL(snapshotVideoFrames));
	   		if(err != KErrNotSupported)
	   			{
	   			INFO_PRINTF1(_L("SelectSnapshotVideoFramesL for Snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
			//Start video capture and other child objects
			INFO_PRINTF1(_L("StartVideoCaptureL for Video Capture"));
			TRAP(err,videocapture->StartVideoCaptureL());
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("StartVideoCaptureL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		CheckVideoCaptureNegNotification(KErrNotSupported, result);
	   		
	   		INFO_PRINTF1(_L("StartHistogram for Video Capture"));
			histogram_VC->StartHistogram();
			CheckVideoCaptureNegNotification(KErrNotSupported, result);
	  		
	  		INFO_PRINTF1(_L("StopHistogram for Video Capture"));	
			histogram_VC->StopHistogram();
			
			INFO_PRINTF1(_L("EnableSnapshotL for snapshot for Video Capture"));	
			TRAP(err,snapshot_VC->EnableSnapshotL());
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("EnableSnapshotL for snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   		CheckVideoCaptureNegNotification(KErrNotSupported, result);
	   		
			INFO_PRINTF1(_L("GetSnapshotStatusL for snapshot for Video Capture"));	
			CCamera::CCameraSnapshot::TSnapshotState snapshotState = CCamera::CCameraSnapshot::ESnapshotInactive;
			TRAP(err,snapshot_VC->GetSnapshotStatusL(snapshotState));
			if(err != KErrNotSupported)
				{
	   			INFO_PRINTF1(_L("GetSnapshotStatusL for snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF1(_L("DisableSnapshotL for snapshot for Video Capture"));	
			TRAP(err,snapshot_VC->DisableSnapshotL());
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("DisableSnapshotL for snapshot for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF1(_L("PauseVideoCapture for Video Capture"));
	   		videocapture->PauseVideoCapture();
	   			
	   		INFO_PRINTF1(_L("ResumeVideoCaptureL for Video Capture"));
			TRAP(err,videocapture->ResumeVideoCaptureL());
			if(err != KErrNotSupported)
				{
	   			INFO_PRINTF1(_L("ResumeVideoCaptureL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF1(_L("GetVideoCaptureStateL for Video Capture"));
	   		CCamera::CCameraVideoCaptureControl::TVideoCaptureState videoCaptureState = 
	   				CCamera::CCameraVideoCaptureControl::EVideoCaptureInActive;
			TRAP(err,videocapture->GetVideoCaptureStateL(videoCaptureState));
			if(err != KErrNotSupported)
				{
	   			INFO_PRINTF1(_L("GetVideoCaptureStateL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF1(_L("StopVideoCapture for Video Capture"));
	   		videocapture->StopVideoCapture();
	   			   		
	   		//Start Direct video capture and other child objects
	   		TInt supportedDVCType = -1;
	   		INFO_PRINTF1(_L("Check Direct Video Capture Support Info"));
	   		TRAP(err,videocapture->GetVideoCaptureSupportInfoL(supportedDVCType));
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("GetVideoCaptureSupportInfoL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		if(supportedDVCType != KSupportedDirectVCType)
	   			{
	   			INFO_PRINTF1(_L("GetVideoCaptureSupportInfoL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}

			INFO_PRINTF1(_L("Test Set Client Video Capture in PrepareVideoParameters"));
	   		
	   		prepareVideoParameters.SetVideoCaptureType(CCamera::CCameraVideoCaptureControl::EClientVideoCapture);
			
			if(CCamera::CCameraVideoCaptureControl::EClientVideoCapture != prepareVideoParameters.VideoCaptureType())
				{
				INFO_PRINTF1(_L("iVideoCaptureType for TPrepareVideoParameters: unexpected result!"));
  				result = EFail;
				}
	   		
	   		
			INFO_PRINTF1(_L("Set Video Capture in PrepareVideoParameters"));
			
			prepareVideoParameters.SetVideoCaptureType(CCamera::CCameraVideoCaptureControl::EDirectVideoCapture);
			
			if(CCamera::CCameraVideoCaptureControl::EDirectVideoCapture != prepareVideoParameters.VideoCaptureType())
				{
				INFO_PRINTF1(_L("iVideoCaptureType for TPrepareVideoParameters: unexpected result!"));
  				result = EFail;
				}
				
			prepareVideoParameters.iFormat = CCamera::EFormatEncodedH264;
			prepareVideoParameters.iPixelAspectRatio = CCamera::CCameraAdvancedSettings::EEPixelAspect40To33;
			
			iInputEventUid = KUidECamEventVideoCaptureControlPrepareComplete;
			videocapture->PrepareVideoCapture(prepareVideoParameters);
	  		CheckNotification(iInputEventUid, result);
						
			INFO_PRINTF1(_L("StartVideoCaptureL for Direct Video Capture"));
			TRAP(err,videocapture->StartVideoCaptureL());
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("StartVideoCaptureL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		CheckVideoCaptureNegNotification(KErrNotSupported, result);
	   		
	   		INFO_PRINTF1(_L("StopVideoCapture for Direct Video Capture"));
	   		videocapture->StopVideoCapture();
	   		
	   		INFO_PRINTF1(_L("GetPrepareVideoParameters for Direct Video Capture"));
	   		CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters retrievedVideoParam;
			TRAP(err,videocapture->GetPrepareVideoParametersL(retrievedVideoParam));
			if(err != KErrNone)
				{
	   			INFO_PRINTF1(_L("GetPrepareVideoParametersL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		if(CCamera::EFormatEncodedH264 != retrievedVideoParam.iFormat)
				{
	   			INFO_PRINTF1(_L("GetPrepareVideoParametersL for Video Capture: unexpected result!"));
  				result = EFail;
	   			}
	   			
	   		INFO_PRINTF2(_L("retrievedVideoParam.iFormat: %d"),retrievedVideoParam.iFormat);
	   		INFO_PRINTF2(_L("retrievedVideoParam.iPixelAspectRatio: %d"),retrievedVideoParam.iPixelAspectRatio);
	   		
	   					
			CleanupStack::PopAndDestroy(snapshot_VC);
			CleanupStack::PopAndDestroy(histogram_VC);
			CleanupStack::PopAndDestroy(videocapture);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraVideoCaptureControl object not created")); 
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
