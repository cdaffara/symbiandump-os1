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
//

#include <ecamuids.hrh>
#include <ecom/ecomresolverparams.h>
#include "ECamEnhanceFuncTest.h"
#include "ECamUnitTestPluginUids.hrh"
#include "ECamUnitTestPlugin.h"
#include <ecam/ecamcommonuids.hrh>
#include <fbs.h>

/*------------------------------------------------------
 	Histogram : RECamHistTest
 *------------------------------------------------------*/
RECamHistTest* RECamHistTest::NewL(const TDesC& aTestStep)
	{
	RECamHistTest* self = new (ELeave) RECamHistTest(aTestStep);
	return self;	
	}
	
RECamHistTest::RECamHistTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamHistTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-020-HP")))
		{
		return DoTestStep_20L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-021-HP")))
		{
		return DoTestStep_21L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-022-HP")))
		{
		return DoTestStep_22L();
		}
	return EInconclusive;	
	}

TVerdict RECamHistTest::DoTestStep_20L()	
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

		TRAP(err, verdict = DoHistTestStepL());

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
	
TVerdict RECamHistTest::DoHistTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraHistogram* hist = NULL;
	
	// using observer 2 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		hist = 	static_cast<CCamera::CCameraHistogram*> (camera->CustomInterface(KECamHistogramUid));
	    if (hist!= NULL)
	    	{
	    	CleanupStack::PushL(hist);
	  		INFO_PRINTF1(_L("CCameraHistogram object was created")); 
	    	CleanupStack::PopAndDestroy(hist);
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
	
	// create a Hist object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, hist  = CCamera::CCameraHistogram::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(hist);
	  		INFO_PRINTF1(_L("CCameraHistogram object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(hist);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraHistogram object was not created using NewL")); 
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
	
TVerdict RECamHistTest::DoTestStep_21L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraHistogram* hist = NULL;
		
	// create a Hist object using New2L
	__MM_HEAP_MARK;
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, hist  = CCamera::CCameraHistogram::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraHistogram object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraHistogram object was not created using NewL")); 
    		return EFail;
    		}	
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	
	if (hist!= NULL)
    	{
    	CleanupStack::PushL(hist);
    	
    	// get the supported set of values  
    	TUint32 suppformats = hist->SupportedHistograms();

    	// the array should not be empty 
        if (suppformats == 0)
        	{
        	result = EFail;
        	}
        // Preapare DSA Based Histogram	
        TUint histhandle = 0;
        TPoint orgPos(5,5);
        TSize orgSize(90,50);
        TRgb orgColor(100,100,100);
        
    	TRAP(error, histhandle = hist->PrepareDSAHistogramL(CCamera::CCameraHistogram::ELuminance, orgPos, orgSize, orgColor));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("DSA Histogram Created"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("DSA Histogram Created- %d"), error);
    		return EFail;
    		}
    		
    	// set new value
    	TPoint curPos;
        TSize curSize;
        TRgb curColor;
    	TRAP(error, hist->GetDSAHistogramPropertiesL(histhandle, curPos, curSize, curColor));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Get Histogram Properties"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Get Histogram Properties- %d"), error);
    		return EFail;
    		}
    		
    	if(curPos != orgPos || curSize != orgSize || curColor != orgColor)
    		{
    		INFO_PRINTF1(_L("Properties dont match"));
    		return EFail;
    		}
    		
    	// new values for properties
    	orgPos = TPoint(10,10);
        orgSize = TSize(80,40);
        orgColor = TRgb(90,90,90);
       	
       	// Lets try update
       	TRAP(error, hist->UpdateDSAHistogramPropertiesL(histhandle, orgPos, orgSize, orgColor));
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Update Histogram Proporties Successfull"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Update Histogram Proporties- %d"), error);
    		return EFail;
    		}	
    		
    	// Get and verify with new values
    	TRAP(error, hist->GetDSAHistogramPropertiesL(histhandle, curPos, curSize, curColor));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Get Histogram Properties Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Get Histogram Properties- %d"), error);
    		return EFail;
    		}
    		
    	if(curPos != orgPos || curSize != orgSize || curColor != orgColor)
    		{
    		INFO_PRINTF1(_L("Properties dont match"));
    		return EFail;
    		}
    	
    	    			
		TRAP(error, hist->DestroyHistogramL(histhandle));
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Destroy Histogram Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Destroy Histogram - %d"), error);
    		return EFail;
    		}
    	CleanupStack::PopAndDestroy(hist);
    	}
	CleanupStack::PopAndDestroy(camera);
	
	__MM_HEAP_MARKEND;
   
	return result;
	}

// Test step MM-ECM-ADV-U-022-HP 
TVerdict RECamHistTest::DoTestStep_22L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraHistogram* hist = NULL;
		
	// create a Hist object using NewL
	__MM_HEAP_MARK;
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, hist  = CCamera::CCameraHistogram::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraHistogram object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraHistogram object was not created using NewL")); 
    		return EFail;
    		}	
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	
	if (hist!= NULL)
    	{
    	CleanupStack::PushL(hist);
    	
    	// get the supported set of values  
    	TUint32 suppformats = hist->SupportedHistograms();

    	// the array should not be empty 
        if (suppformats == 0)
        	{
        	result = EFail;
        	}
        // Preapare Non-DSA Based Histogram	
        TUint histhandle = 0;
        TPoint orgPos(5,5);
        TSize orgSize(90,50);
        TRgb orgColor(100,100,100);
        
    	TRAP(error, histhandle = hist->PrepareHistogramL(CCamera::CCameraHistogram::ELuminance));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Non-DSA Histogram Created Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Non-DSA Histogram Created- %d"), error);
    		return EFail;
    		}
    		
	
		hist->StartHistogramL(histhandle);
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Start Histogram Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Start Histogram - %d"), error);
    		return EFail;
    		}
    	
    	// Get a list of active histograms : This has not been implemented and so the count will be 0
    	RArray<TUint> activeHistograms;
        TRAP(error, hist->GetActiveHistogramsL(activeHistograms));
        if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Stop Histogram Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Stop Histogram - %d"), error);
    		return EFail;
    		}
    			
		TRAP(error, hist->StopHistogramL(histhandle));
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Stop Histogram Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Stop Histogram - %d"), error);
    		return EFail;
    		}    			
    		
		TRAP(error, hist->DestroyHistogramL(histhandle));
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Destroy Histogram Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Destroy Histogram - %d"), error);
    		return EFail;
    		}
    	CleanupStack::PopAndDestroy(hist);
    	}
	CleanupStack::PopAndDestroy(camera);
	
	__MM_HEAP_MARKEND;
   
	return result;
	}
	
//
// RECamHistogramNotificationTest
//
RECamHistogramNotificationTest* RECamHistogramNotificationTest::NewL(TBool aAllocTest)
	{
	RECamHistogramNotificationTest* self = new (ELeave) RECamHistogramNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamHistogramNotificationTest::ConstructL()
	{
	iOriginalSet.Reset();
	iResultSet.Reset();

	}
RECamHistogramNotificationTest::RECamHistogramNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-023-HP");
	}

TVerdict RECamHistogramNotificationTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraHistogram* hist = NULL;
		
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		
		TRAP(error, hist  = CCamera::CCameraHistogram::NewL(*camera));	
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraHistogram object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraHistogram object was not created using NewL")); 
    		result = EFail;
    		}	
    		
	    if (hist!= NULL)
	    	{
	    	CleanupStack::PushL(hist);
	    	
	    	// get the supported set of values  
	    	TUint32 suppformats = hist->SupportedHistograms();

	    	// the array should not be empty 
	        if (suppformats == 0)
	        	{
	        	result = EFail;
	        	}
	        TUint histhandle = 0;
        	TRAP(error, histhandle = hist->PrepareHistogramL(CCamera::CCameraHistogram::ELuminance));
        	
        	// set new value
	    	iInputEventUid.iUid = KUidECamEventHistogramUidValue;
	        	    	
	    	hist->StartHistogramL(histhandle);
	    	if(error==KErrNone)
	    		{
	    		INFO_PRINTF1(_L("Start Histogram Successful"));
	    		}
	    	else
	    		{
	    		INFO_PRINTF2(_L("Start Histogram - %d"), error);
	    		result = EFail;
	    		}
	    	
	        CheckNotification(iInputEventUid, result);
    		
    		// Ignore the Data - check the function
    		TRAP(error, hist->HistogramDataL());
    		if(error==KErrNone)
    			{
    			INFO_PRINTF1(_L("Histogram DataL Successful"));
    			}
	    	else
	    		{
	    		INFO_PRINTF2(_L("Histogram DataL - %d"), error);
	    		result = EFail;
	    		}
	    		
	    	// Overload of HistogramDataL
	    	TRAP(error, hist->HistogramDataL(histhandle));
    		if(error==KErrNone)
    			{
    			INFO_PRINTF1(_L("Histogram DataL Successful"));
    			}
	    	else
	    		{
	    		INFO_PRINTF2(_L("Histogram DataL - %d"), error);
	    		result = EFail;
	    		}
    			
    		TRAP(error, hist->StopHistogramL(histhandle));
    		
    		if(error==KErrNone)
    			{
    			INFO_PRINTF1(_L("Stop Histogram Successful"));
    			}
	    	else
	    		{
				INFO_PRINTF2(_L("Stop Histogram - %d"), error);
	    		result = EFail;
	    		}
	    			
    		TRAP(error, hist->DestroyHistogramL(histhandle));
			if(error==KErrNone)
    			{
    			INFO_PRINTF1(_L("Destroy Histogram Successful"));
    			}
	    	else
	    		{
	    		INFO_PRINTF2(_L("Destroy Histogram - %d"), error);
	    		result = EFail;
	    		}
	    	    		
    		CleanupStack::PopAndDestroy(hist);
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

/*------------------------------------------------------
 	Overlay : RECamOvrlayTest
 *------------------------------------------------------*/

RECamOvrlayTest* RECamOvrlayTest::NewL(const TDesC& aTestStep)
	{
	RECamOvrlayTest* self = new (ELeave) RECamOvrlayTest(aTestStep);
	return self;	
	}
	
RECamOvrlayTest::RECamOvrlayTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamOvrlayTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-030-HP")))
		{
		return DoTestStep_30L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-031-HP")))
		{
		return DoTestStep_31L();
		}
	return EFail;	
	}
	
TVerdict RECamOvrlayTest::DoTestStep_30L()
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

		TRAP(err, verdict = DoOverlayTestStepL());

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

TVerdict RECamOvrlayTest::DoOverlayTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraOverlay* ovrlay = NULL;
	
	// using observer 2 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		ovrlay = 	static_cast<CCamera::CCameraOverlay*> (camera->CustomInterface(KECamOverlayUid));
	    if (ovrlay!= NULL)
	    	{
	    	CleanupStack::PushL(ovrlay);
	  		INFO_PRINTF1(_L("CCameraOverlay object was created")); 
	    	CleanupStack::PopAndDestroy(ovrlay);
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
	
	// create overlay object using NewL
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, ovrlay  = CCamera::CCameraOverlay::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(ovrlay);
	  		INFO_PRINTF1(_L("CCameraOverlay object was created using NewL")); 
	  		
	  		CleanupStack::PopAndDestroy(ovrlay);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraOverlay object was not created using NewL")); 
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

TVerdict RECamOvrlayTest::DoTestStep_31L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	TInt error = KErrNone;
	CCamera::CCameraOverlay* ovrlay = NULL;
		
	// create a Overlay object using NewL
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, ovrlay  = CCamera::CCameraOverlay::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	INFO_PRINTF1(_L("CCameraOverlay object was created using NewL")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("CCameraOverlay object was not created using NewL - %d"), error); 
    		result = EFail;
    		}
    	CleanupStack::PushL(ovrlay);	
    	
    	
    	CCamera::CCameraOverlay::TOverlayParameters parameters;
    	parameters.iCurrentModes = CCamera::CCameraOverlay::EModeStillImage;
		parameters.iCurrentTypes = CCamera::CCameraOverlay::EPerPixel;
		parameters.iAlphaValue= 0;	
		parameters.iPosition = TPoint(2,2);		
		parameters.iZOrder = 1;	
 	 	
 	 	TInt err = RFbsSession::Connect();
    	if (err != KErrNone)
    		{
    		INFO_PRINTF2(_L("RFbsSession::Connect() failed, err = %d"), err);
			return EInconclusive;
    		}   
 	 	
 	 	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    	CleanupStack::PushL(bitmap);	
    	    	
    	TUint ovrhandle =0;	 
    	
    	TRAP(error, ovrhandle = ovrlay->CreateOverlayL(parameters, bitmap));
 	 	
 	 	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Create overlay was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Create overlay - Error - %d"), error); 
    		result = EFail;
    		}
   
 		TRAP(error, ovrlay->SetOverlayBitmapL(ovrhandle, bitmap));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set Overlay Bitmap was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set Overlay Bitmap - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	CFbsBitmap* modBitmap = new (ELeave) CFbsBitmap;
    	User::LeaveIfError(modBitmap->Create(TSize(1000,1000),EGray256)); 
    	CleanupStack::PushL(modBitmap);		
    	TRAP(error, ovrlay->SetModifiableOverlayBitmapL(ovrhandle, modBitmap));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set Modifiable Overlay Bitmap was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set Modifiable Overlay Bitmap - Error - %d"), error); 
    		result = EFail;
    		}
   
       	CFbsBitmap* getBitmap = new (ELeave) CFbsBitmap;
    	User::LeaveIfError(getBitmap->Create(TSize(1000,1000),EGray256)); 
    	CleanupStack::PushL(getBitmap);
    	TRAP(error, ovrlay->GetOverlayBitmapL(ovrhandle, getBitmap));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay Bitmap was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay Bitmap - Error - %d"), error); 
 	  		result = EFail;
    		}

    	TRAP(error, ovrlay->GetOverlayParametersL(ovrhandle, parameters));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay Parameters was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay Parameters - Error - %d"), error); 
    		result = EFail;
    		}
   	  		 	
 		TRAP(error, ovrlay->SetOverlayParametersL(ovrhandle, parameters));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set Overlay Parameters  was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set Overlay Parameters  - Error - %d"), error); 
    		result = EFail;
    		}
 		 
 		CCamera::CCameraOverlay::TOverlaySupportInfo SuppInfo;
 		TRAP(error, ovrlay->GetOverlaySupport(SuppInfo));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay support was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay support - Error - %d"), error); 
    		result = EFail;
    		}
 		
 		RArray<TUint> overlayHandles;
    	TRAP(error, ovrlay->GetAllOverlaysInZOrderL(overlayHandles));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get All Overlays in Z order was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get All Overlays in Z order - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	TRAP(error, ovrlay->GetAllOverlaysInZOrderL(CCamera::CCameraOverlay::EModeStillImage,1,overlayHandles));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get 'Mode specific' All Overlays in Z order was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get 'Mode specific' All Overlays in Z order - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	
 		TRAP(error, ovrlay->SetAllOverlaysInZOrderL(overlayHandles));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set All Overlay in Z order was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set All Overlay in Z order- Error - %d"), error); 
    		result = EFail;
    		}
    		
    	TRAP(error, ovrlay->SetAllOverlaysInZOrderL(CCamera::CCameraOverlay::EModeStillImage,1,overlayHandles));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set 'Mode specific' All Overlay in Z order was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set 'Mode specific' All Overlay in Z order- Error - %d"), error); 
    		result = EFail;
    		}
    		
    	overlayHandles.Close();
    	
    	TRAP(error, ovrlay->ReleaseOverlay(ovrhandle));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Release Overlay was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Release Overlay - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	CleanupStack::PopAndDestroy(4, ovrlay); //bitmap, ovrlay	
    		
    	TRAP(error, ovrlay  = CCamera::CCameraOverlay::NewL(*camera));		
    	CleanupStack::PushL(ovrlay);	
        bitmap = new (ELeave) CFbsBitmap;
    	CleanupStack::PushL(bitmap);	
    	ovrhandle =0;	   	
	   	
	   	TRAP(error, ovrhandle = ovrlay->CreateOverlayL(parameters, bitmap));
    	TSize originalSize = bitmap->SizeInPixels();
    	
    	INFO_PRINTF1(_L("Test size change through CFbsbitmap")); 
    	TSize modifiedSize(640, 480);
    	CFbsBitmap* modifiedBitmap = new (ELeave) CFbsBitmap;    	
    	User::LeaveIfError(modifiedBitmap->Create(modifiedSize,EColor4K)); 
    	CleanupStack::PushL(modifiedBitmap);   	
    	originalSize = modifiedBitmap->SizeInPixels();
    	TRAP(error, ovrlay->SetModifiableOverlayBitmapL(ovrhandle,modifiedBitmap));   
     
    	CFbsBitmap* retrievedBitmap = new (ELeave) CFbsBitmap;
    	User::LeaveIfError(retrievedBitmap->Create(TSize(1000,1000),EGray256)); 
    	CleanupStack::PushL(retrievedBitmap);  
    	TRAP(error, ovrlay->GetOverlayBitmapL(ovrhandle, retrievedBitmap));
    	
    	TSize retrievedSize = retrievedBitmap->SizeInPixels();
    	
    	if(originalSize != retrievedSize)
    		{
    		INFO_PRINTF2(_L("Modifying overlay size - Error - %d"), error);
    		result = EFail;
    		}	
   	 	
 	 	CleanupStack::PopAndDestroy(4, ovrlay);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}

	RFbsSession::Disconnect();
	
	CleanupStack::PopAndDestroy(camera);
	__MM_HEAP_MARKEND;
	
	//
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, ovrlay  = CCamera::CCameraOverlay::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	INFO_PRINTF1(_L("CCameraOverlay object was created using NewL")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("CCameraOverlay object was not created using NewL - %d"), error); 
    		result = EFail;
    		}
    	CleanupStack::PushL(ovrlay);	
    	
    	
    	CCamera::CCameraOverlay::TOverlayParameters parameters;
    	parameters.iCurrentModes = CCamera::CCameraOverlay::EModeStillImage;
		parameters.iCurrentTypes = CCamera::CCameraOverlay::EPerPixel;
		parameters.iAlphaValue= 0;	
		TSize size2(20,20);				
		parameters.iPosition = TPoint(2,2);		
		parameters.iZOrder = 1;	
				
    	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    	CleanupStack::PushL(bitmap);
    
    	TUint ovrhandle =0;
   
    	TRAP(error, ovrhandle = ovrlay->CreateOverlayL(parameters, bitmap));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Create overlay was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Create overlay - Error - %d"), error); 
    		result = EFail;
    		}
   
 	   	TRAP(error, ovrlay->GetOverlayParametersL(ovrhandle, parameters));
    	if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay Parameters was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay Parameters - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	if(parameters.iCurrentModes & CCamera::CCameraOverlay::EModeStillImageContinuous)
    		{
    		INFO_PRINTF1(_L("GetOverlayParametersL  was successful"));
    		}
    	else
    		{
    		INFO_PRINTF1(_L("Get Overlay Parameters  unexpected result")); 
    		result = EFail;	
    		}
   	  		 	
 		TRAP(error, ovrlay->SetOverlayParametersL(ovrhandle, parameters));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set Overlay Parameters  was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set Overlay Parameters  - Error - %d"), error); 
    		result = EFail;
    		}
 		
 		CCamera::CCameraOverlay::TOverlaySupportInfo SuppInfo;
 		TRAP(error, ovrlay->GetOverlaySupport(SuppInfo));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay support was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay support - Error - %d"), error); 
    		result = EFail;
    		}
    		
    	if(SuppInfo.iSupportedModes & CCamera::CCameraOverlay::EModeStillImageContinuous)
    		{
    		INFO_PRINTF1(_L("GetOverlaySupport values were successful"));
    		}
    	else
    		{
    		INFO_PRINTF1(_L("GetOverlaySupport values unexpected result")); 
    		result = EFail;	
    		}
    		
    	//conditional coverage for SetOverlay Parameter
    	parameters.iCurrentModes = CCamera::CCameraOverlay::EModeViewfinder;
    	TRAP(error, ovrlay->SetOverlayParametersL(ovrhandle, parameters));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Set Overlay Parameters  was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Set Overlay Parameters  - Error - %d"), error); 
    		result = EFail;
    		}
    	
    	parameters.iCurrentModes = CCamera::CCameraOverlay::EModeNone;
    	TRAP(error, ovrlay->GetOverlayParametersL(ovrhandle, parameters));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Get Overlay Parameters  was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Get Overlay Parameters  - Error - %d"), error); 
    		result = EFail;
    		}	
    		
    	if(parameters.iCurrentModes & CCamera::CCameraOverlay::EModeClientViewfinder)
    		{
    		INFO_PRINTF1(_L("GetOverlayParametersL  was successful"));
    		}
    	else
    		{
    		INFO_PRINTF1(_L("Get Overlay Parameters  unexpected result")); 
    		result = EFail;	
    		}
    		
      	TRAP(error, ovrlay->ReleaseOverlay(ovrhandle));
 		if (error == KErrNone)
	    	{
	    	INFO_PRINTF1(_L("Release Overlay was successful")); 
	       	}
	    else
    		{
    		INFO_PRINTF2(_L("Release Overlay - Error - %d"), error); 
    		result = EFail;
    		}
    	
    	CleanupStack::PopAndDestroy(2, ovrlay); //bitmap, ovrlay
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	CleanupStack::PopAndDestroy(camera);
	__MM_HEAP_MARKEND;
	//
   
	return result;
	}

/*------------------------------------------------------
 	Snapshot : RECamSnapTest
 *------------------------------------------------------*/

RECamSnapTest* RECamSnapTest::NewL(const TDesC& aTestStep)
	{
	RECamSnapTest* self = new (ELeave) RECamSnapTest(aTestStep);
	return self;	
	}
	
RECamSnapTest::RECamSnapTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamSnapTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-040-HP")))
		{
		return DoTestStep_40L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-041-HP")))
		{
		return DoTestStep_41L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-042-HP")))
		{
		return DoTestStep_42L();
		}	
	return EFail;
	}

TVerdict RECamSnapTest::DoTestStep_40L()
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

		TRAP(err, verdict = DoSnapTestStepL());

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
	
TVerdict RECamSnapTest::DoSnapTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraSnapshot* snap = NULL;
	
	// using observer 2 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		snap = 	static_cast<CCamera::CCameraSnapshot*> (camera->CustomInterface(KECamSnapshotUid));
	    if (snap!= NULL)
	    	{
	    	CleanupStack::PushL(snap);
	  		INFO_PRINTF1(_L("CCameraSnaphot object was created")); 
	    	CleanupStack::PopAndDestroy(snap);
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
	
	// create a Snapshot object using NewL
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, snap  = CCamera::CCameraSnapshot::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(snap);
	  		INFO_PRINTF1(_L("CCameraSnapshot object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(snap);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraSnapshot object was not created using NewL")); 
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

TVerdict RECamSnapTest::DoTestStep_41L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraSnapshot* snap = NULL;
		
	// create a snapshot object using NewL
	__MM_HEAP_MARK;
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, snap  = CCamera::CCameraSnapshot::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraSnapshot object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraSnapshot object was not created using NewL")); 
    		return EFail;
    		}	
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	
	if (snap!= NULL)
    	{
    	CleanupStack::PushL(snap);
    	
    	// get the supported formats  
    	TUint32 suppformats = snap->SupportedFormats();
		
		CCamera::TFormat format = CCamera::EFormatMonochrome;
		TSize orgSize(90,50);
        TRgb orgColor(100,100,100);
        TBool AspectRatio = ETrue;
        TPoint orgPos(2,2);
        
    	TRAP(error, snap->PrepareSnapshotL(format, orgPos, orgSize, orgColor,AspectRatio));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Snapshot Creation Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Snapshot Creation Failed - %d"), error);
    		result = EFail;
    		}
    		
    	TBool isActive = EFalse;
    	TRAP(error, isActive = snap->IsSnapshotActive());
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Is Snapshot Active Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Is Snapshot Active Failed - %d"), error);
    		result = EFail;
    		}
    		
    	if(isActive)
    		{
    		INFO_PRINTF1(_L("Snapshot must not be active"));
    		result = EFail;
    		}
    		
    	// Start Snapshot    	
       	TRAP(error, snap->StartSnapshot());
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Start Snapshot Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Start Snapshot Failed - %d"), error);
    		return EFail;
    		}	
    	
    	TRAP(error, isActive = snap->IsSnapshotActive());
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Is Snapshot Active Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Is Snapshot Active Failed - %d"), error);
    		result = EFail;
    		}	
    	
    	if(!isActive)
    		{
    		INFO_PRINTF1(_L("Snapshot must not be active"));
    		result = EFail;
    		}
    	
    	TRAP(error, snap->StopSnapshot());
    	if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Stop Snapshot Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Stop Snapshot Failed - %d"), error);
    		return EFail;
    		}	
    		
    	// Test new snapshot methods. They shall leave with KErrNotSupported
    	INFO_PRINTF1(_L("Check New Snapshot methods leave with KErrNotSupported"));
    	CCamera::CCameraSnapshot::TSnapshotParameters snapshotParameters;
    	TRAP(error, snap->PrepareSnapshotL(snapshotParameters));
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("PrepareSnapshotL unexpected result!"));
    		return EFail;
    		}
	
		TRAP(error, snap->GetSnapshotParametersL(snapshotParameters));
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("GetSnapshotParametersL unexpected result!"));
    		return EFail;
    		}
	
		TRAP(error, snap->SetSnapshotParametersL(snapshotParameters));
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("SetSnapshotParametersL unexpected result!"));
    		return EFail;
    		}
    	
    	CCamera::CCameraSnapshot::TSnapshotState snapshotState = CCamera::CCameraSnapshot::ESnapshotInactive;	
		TRAP(error, snap->GetSnapshotStatusL(snapshotState));
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("GetSnapshotStatusL unexpected result!"));
    		return EFail;
    		}
    		
    	CCamera::CCameraSnapshot::TSnapshotVideoFrames snapshotVideoFrames = CCamera::CCameraSnapshot::ESnapshotVideoFirstFrame;	
		TRAP(error, snap->SelectSnapshotVideoFramesL(snapshotVideoFrames));
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("SelectSnapshotVideoFramesL unexpected result!"));
    		return EFail;
    		}
		
		TRAP(error, snap->EnableSnapshotL());
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("EnableSnapshotL unexpected result!"));
    		return EFail;
    		}
    		
    	TRAP(error, snap->DisableSnapshotL());
    	if(error != KErrNotSupported)
    		{
    		INFO_PRINTF1(_L("DisableSnapshotL unexpected result!"));
    		return EFail;
    		}
	
		    			
    	CleanupStack::PopAndDestroy(snap);
    	}
	CleanupStack::PopAndDestroy(camera);
	
	__MM_HEAP_MARKEND;
   
	return result;
	}

TVerdict RECamSnapTest::DoTestStep_42L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraSnapshot* snap = NULL;
		
	// create a Hist object using NewL
	__MM_HEAP_MARK;
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, snap  = CCamera::CCameraSnapshot::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraSnapshot object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraSnapshot object was not created using NewL")); 
    		return EFail;
    		}	
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	
	if (snap!= NULL)
    	{
    	CleanupStack::PushL(snap);
    	
    	// get the supported formats  
    	TUint32 suppformats = snap->SupportedFormats();
		
		CCamera::TFormat format = CCamera::EFormatMonochrome;
		TSize orgSize(90,50);
        TRgb orgColor(100,100,100);
        TBool AspectRatio = ETrue;
        TPoint orgPos(2,2);
        
        // Test the other overload of PrepareSnapshotL
    	TRAP(error, snap->PrepareSnapshotL(format, orgSize, AspectRatio));
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Snapshot Creation Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Snapshot Creation Failed - %d"), error);
    		result = EFail;
    		}
    	
    	// Set Background color Snapshot    		
    	TRAP(error, snap->SetBgColorL(orgColor));
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Set Background Color Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Set Background Color Failed - %d"), error);
    		result = EFail;
    		}
    	
    	// Set Position
    	TRAP(error, snap->SetPositionL(orgPos));
       	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Set Snapshot Position Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Set Snapshot Position Failed - %d"), error);
    		result = EFail;
    		}
    			
    	// Start Snapshot    	
       	TRAP(error, snap->StartSnapshot());
		if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Start Snapshot Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Start Snapshot Failed - %d"), error);
    		return EFail;
    		}	
    	
    	// Is shapshot active	
    	TBool isActive = EFalse;
    	TRAP(error, isActive = snap->IsSnapshotActive());
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("Is Snapshot Active Successful"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("Is Snapshot Active Failed - %d"), error);
    		result = EFail;
    		}	
    	
    	if(!isActive)
    		{
    		INFO_PRINTF1(_L("Snapshot must not be active"));
    		result = EFail;
    		}
    	
    	// Stop Snapshot
    	TRAP(error, snap->StopSnapshot());
    	if(error==KErrNone)
			{
			INFO_PRINTF1(_L("Stop Snapshot Successful"));
			}
    	else
    		{
    		INFO_PRINTF2(_L("Stop Snapshot Failed - %d"), error);
    		return EFail;
    		}	
    	    			
    	CleanupStack::PopAndDestroy(snap);
    	}
	CleanupStack::PopAndDestroy(camera);
	
	__MM_HEAP_MARKEND;
   
	return result;
	}
//
// RECamSnapNotificationTest
//
RECamSnapNotificationTest* RECamSnapNotificationTest::NewL(TBool aAllocTest)
	{
	RECamSnapNotificationTest* self = new (ELeave) RECamSnapNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamSnapNotificationTest::ConstructL()
	{
	iOriginalSet.Reset();
	iResultSet.Reset();

	}
RECamSnapNotificationTest::RECamSnapNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-043-HP");
	}

TVerdict RECamSnapNotificationTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraSnapshot* snap = NULL;
		
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		
		TRAP(error, snap  = CCamera::CCameraSnapshot::NewL(*camera));	
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraSnapshot object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraSnapshot object was not created using NewL")); 
    		result = EFail;
    		}	
    		
	    if (snap!= NULL)
			{
			CleanupStack::PushL(snap);

			// get the supported set of values  
			TUint32 suppformats = snap->SupportedFormats();

			CCamera::TFormat format = CCamera::EFormatMonochrome;
			TSize orgSize(90,50);
			TRgb orgColor(100,100,100);
			TPoint orgPos(2,2);
			TBool AspectRatio = ETrue;

			TRAP(error, snap->PrepareSnapshotL(format, orgPos, orgSize, orgColor,AspectRatio));

			if(error==KErrNone)
				{
				INFO_PRINTF1(_L("Snapshot Creation Successful"));
				}
			else
				{
				INFO_PRINTF2(_L("Snapshot Creation Failed - %d"), error);
				result = EFail;
				}
			
			// set new value
	    	iInputEventUid.iUid = KUidECamEventSnapshotUidValue;
	    		
			// Start Snapshot    	
			TRAP(error, snap->StartSnapshot());
			if(error==KErrNone)
				{
				INFO_PRINTF1(_L("Start Snapshot Successful"));
				}
			else
				{
				INFO_PRINTF2(_L("Start Snapshot Failed - %d"), error);
				return EFail;
				}	
			
			CheckNotification(iInputEventUid, result);
			
			RArray<TInt> FrameIndexOrder;
			
			// Ignore the data
			TRAP(error, snap->SnapshotDataL(FrameIndexOrder));
			if(error==KErrNone)
				{
				INFO_PRINTF1(_L("Snapshot DataL Successful"));
				}
			else
				{
				INFO_PRINTF2(_L("Snapshot DataL Failed - %d"), error);
				return EFail;
				}	
			FrameIndexOrder.Close();
			
			TBool isActive = EFalse;
			
			TRAP(error, isActive = snap->IsSnapshotActive());
			if(error==KErrNone)
				{
				INFO_PRINTF1(_L("Is Snapshot Active Successful"));
				}
			else
				{
				INFO_PRINTF2(_L("Is Snapshot Active Failed - %d"), error);
				return EFail;
				}
					
			if(!isActive)
				{
				INFO_PRINTF1(_L("Snapshot must not be active"));
				result = EFail;
				}

			TRAP(error, snap->StopSnapshot());
			if(error==KErrNone)
				{
				INFO_PRINTF1(_L("Stop Snapshot Successful"));
				}
			else
				{
				INFO_PRINTF2(_L("Stop Snapshot Failed - %d"), error);
				return EFail;
				}	
			    			
			CleanupStack::PopAndDestroy(snap);
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

