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
#include <ecom/ecomresolverparams.h>
#include "ecamdirectviewfindertest.h"
#include "ECamUnitTestPluginUids.hrh"
#include "ECamUnitTestPlugin.h"
#include <ecam/ecamcommonuids.hrh>

/*--------------------------------------------------------
 	DirectViewFinder : RECamDirectViewFinderTest
 *--------------------------------------------------------*/
RECamDirectViewFinderTest* RECamDirectViewFinderTest::NewL(const TDesC& aTestStep)
	{
	RECamDirectViewFinderTest* self = new (ELeave) RECamDirectViewFinderTest(aTestStep);
	return self;	
	}
	
RECamDirectViewFinderTest::RECamDirectViewFinderTest(const TDesC& aTestStep)
	{
	iTestStepName.Copy(aTestStep);
	}
	
TVerdict RECamDirectViewFinderTest::DoTestStepL()
	{
	if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-044-HP")))
		{
		return DoTestStep_44L();
		}
	else if(!iTestStepName.Compare(_L("MM-ECM-ADV-U-045-HP")))
		{
		return DoTestStep_45L();
		}
	return EInconclusive;	
	}
	
TVerdict RECamDirectViewFinderTest::DoTestStep_44L()
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

		TRAP(err, verdict = DoTestStep_Alloc_44L());

		TAny* testAlloc = NULL;
		testAlloc = User::Alloc(1);
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
	
TVerdict RECamDirectViewFinderTest::DoTestStep_Alloc_44L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	MCameraObserver* observer = NULL; 
	TInt error = KErrNone;
	CCamera::CCameraDirectViewFinder* directviewfinder = NULL;
	
	// using observer 	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver"));

	TRAP(error, camera = CCamera::NewL(*observer, 0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::NewL()"));
		directviewfinder = static_cast<CCamera::CCameraDirectViewFinder*> (camera->CustomInterface(KECamDirectViewFinderUid));
	    if (directviewfinder!= NULL)
	    	{
	    	CleanupStack::PushL(directviewfinder);
	    	INFO_PRINTF1(_L("CCameraDirectViewFinder object was created")); 
	    	CleanupStack::PopAndDestroy(directviewfinder);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraDirectViewFinder object wasn't created"));
    		result = EFail;    		
    		CleanupStack::PopAndDestroy(camera);
    		User::Leave (KErrNoMemory);
    		}
    	CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;
		User::Leave (KErrNoMemory);
		}
	__MM_HEAP_MARKEND;
	
	// using observer 2 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		directviewfinder = 	static_cast<CCamera::CCameraDirectViewFinder*> (camera->CustomInterface(KECamDirectViewFinderUid));
	    if (directviewfinder!= NULL)
	    	{
	    	CleanupStack::PushL(directviewfinder);
	  		INFO_PRINTF1(_L("CCameraDirectViewFinder object was created")); 
	    	CleanupStack::PopAndDestroy(directviewfinder);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraDirectViewFinder object wasn't created"));
    		result = EFail;
    		CleanupStack::PopAndDestroy(camera);
    		User::Leave (KErrNoMemory);    		
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;
		User::Leave (KErrNoMemory);
		}
	__MM_HEAP_MARKEND;
	
	// create a directviewfinder object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, directviewfinder  = CCamera::CCameraDirectViewFinder::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(directviewfinder);
	  		INFO_PRINTF1(_L("CCameraDirectViewFinder object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(directviewfinder);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraDirectViewFinder object was not created using NewL")); 
    		result = EFail;
    		CleanupStack::PopAndDestroy(camera);
    		User::Leave (KErrNoMemory);
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;
		User::Leave (KErrNoMemory);
		}
	__MM_HEAP_MARKEND;
   
	return result;	
	}
TVerdict RECamDirectViewFinderTest::DoTestStep_45L()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraDirectViewFinder* directviewfinder = NULL;
		
	// create a directviewfinder object using New2L
	__MM_HEAP_MARK;
	MCameraObserver2* observer2 = NULL; 
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, directviewfinder  = CCamera::CCameraDirectViewFinder::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	  		INFO_PRINTF1(_L("CCameraDirectViewFinder object was created using NewL")); 
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraDirectViewFinder object was not created using NewL")); 
    		return EFail;
    		}	
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	
	if (directviewfinder!= NULL)
    	{
    	CleanupStack::PushL(directviewfinder);
    	
    	// get the default viewfinder state
    	CCamera::CCameraDirectViewFinder::TViewFinderState vfstate;
    	vfstate = directviewfinder->ViewFinderState();
    	
    	// the vfstate should be EViewFinderInActive 
        if (vfstate != CCamera::CCameraDirectViewFinder::EViewFinderInActive)
        	{
        	INFO_PRINTF1(_L("Direct Viewfinder state incorrectly set")); 
        	return EFail;
        	}
        	
        // Assumption is that meanwhile direct viewfinder has been started. This would be done by calling CCamera::StartViewFinderDirectL(). 
        // This function starts the direct viewfinder and hence changes the state to active. 
        // Also performs initial boundary checking that 'viewfinder remains inactive before its called'. So, the vfstate changes to active.
        // The next step is to pause the direct viewfinder
                
    	TRAP(error, directviewfinder->PauseViewFinderDirectL());
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("View finder paused successfully"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("direct viewfinder pause - %d"), error);
    		return EFail;
    		}
    	
    	vfstate = directviewfinder->ViewFinderState();
    	
    	// the vfstate should be changed to EViewFinderPause 
    	if (vfstate != CCamera::CCameraDirectViewFinder::EViewFinderPause)
        	{
        	INFO_PRINTF1(_L("Direct Viewfinder state incorrectly set after pausing")); 
        	return EFail;
        	}
    		
    	// The next step is to resume the direct viewfinder
    		
    	TRAP(error, directviewfinder->ResumeViewFinderDirectL());
    	
    	if(error==KErrNone)
    		{
    		INFO_PRINTF1(_L("View finder resumed successfully"));
    		}
    	else
    		{
    		INFO_PRINTF2(_L("direct viewfinder resume - %d"), error);
    		return EFail;
    		}
    		
    	vfstate = directviewfinder->ViewFinderState();
    	
    	// the vfstate should be changed to EViewFinderActive 
    	if (vfstate != CCamera::CCameraDirectViewFinder::EViewFinderActive)
        	{
        	INFO_PRINTF1(_L("Direct Viewfinder state incorrectly set after resuming")); 
        	return EFail;
        	}
    		
    	CleanupStack::PopAndDestroy(directviewfinder);
    	}
	CleanupStack::PopAndDestroy(camera);
	
	__MM_HEAP_MARKEND;
   
	return result;
	}
