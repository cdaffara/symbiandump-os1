
// ecamreservetest.cpp

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

#include "ecamreservetest.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecamuids.hrh>

#include <ecom/ecomresolverparams.h>

#include <mm/mmpluginutils.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamconst.h>
#endif

//max. no. of camera slots =8
static const TInt KECamUnitTestMaxCameraIndex = 8;

//
// RECamReserveTest
//

RECamReserveTest* RECamReserveTest::NewL(TBool aAllocTest)
	{
	RECamReserveTest* self = new (ELeave) RECamReserveTest(aAllocTest);
	return self;	
	}
	
RECamReserveTest::RECamReserveTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-API-U-013");
	}
	
TVerdict RECamReserveTest::DoTestStepL()
	{
	TVerdict result = EPass;
	
	INFO_PRINTF1(_L("Checking Reserve Subscription"));
	__MM_HEAP_MARK;
	
	for(TInt index=0; index<(KECamUnitTestMaxCameraIndex+1); index++)
		{
		MReserveObserver* reserveObserver=NULL;
		CCameraStatusWatch* cameraStatusWatch = NULL;

		// create and install the active scheduler we need
		INFO_PRINTF1(_L("Create and install active Scheduler"));
		CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
		CleanupStack::PushL(activeScheduler);
		CActiveScheduler::Install(activeScheduler);

		INFO_PRINTF1(_L("SubscribeReserveInfoL and hence create CCameraStatusWatch object"));
		TRAPD(err, TReservedInfo::SubscribeReserveInfoL(*reserveObserver, index, cameraStatusWatch));
		if(index == KECamUnitTestMaxCameraIndex)
			{
			if(err != KErrArgument)
				{
				INFO_PRINTF2(_L("Checking Reserve Subscription: Error %d is unexpected!"), err);
				result = EFail;
				}
			}
		else
			{
			if(err != KErrNone)
				{
				INFO_PRINTF2(_L("Checking Reserve Subscription: Error %d is unexpected!"), err);
				result = EFail;
				}	
			}
	
		INFO_PRINTF1(_L("destroy the CCameraStatusWatch object"));
		delete cameraStatusWatch;

		INFO_PRINTF1(_L("destroy the activeScheduler"));
		CleanupStack::PopAndDestroy(activeScheduler);
		}
		
	__MM_HEAP_MARKEND;
	
	return result;
	}
