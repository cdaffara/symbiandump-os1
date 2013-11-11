
// ECamInfoTest.cpp

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

#include "ECamInfoTest.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include <ecamuids.hrh>
#include <ecom/ecomresolverparams.h>

#include <mm/mmpluginutils.h>

//
// RECamInfoTest
//

RECamInfoTest* RECamInfoTest::NewL(TBool aAllocTest)
	{
	RECamInfoTest* self = new (ELeave) RECamInfoTest(aAllocTest);
	return self;	
	}
	
RECamInfoTest::RECamInfoTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-API-U-001");
	}
	
TVerdict RECamInfoTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check CCamera::CamerasAvailable"));
	__MM_HEAP_MARK;
	TInt numCameras = CCamera::CamerasAvailable();
	__MM_HEAP_MARKEND;
	if (numCameras!=0)
		{
		INFO_PRINTF2(_L("Error - %d returned"), numCameras);
		return EFail;
		}
	return EPass;
	}
	
//
// RECamInnerInfoTest
//

RECamInnerInfoTest* RECamInnerInfoTest::NewL(TBool aAllocTest)
	{
	RECamInnerInfoTest* self = new (ELeave) RECamInnerInfoTest(aAllocTest);
	return self;	
	}
	
RECamInnerInfoTest::RECamInnerInfoTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-API-U-002");
	}
	
TVerdict RECamInnerInfoTest::DoTestStepL()
	{
	TVerdict verdict = EPass;
	INFO_PRINTF1(_L("Check CCameraInfoPlugin"));
	__MM_HEAP_MARK;

	TUid interfaceUid = {KUidOnboardCameraInfoPlugin};
	TUid destructorKey = {0};

	CCameraInfoPlugin* info = NULL;

	TRAPD(error, info = static_cast<CCameraInfoPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, KECamPluginMatchString, KRomOnlyResolverUid)));

	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Could not open InfoPlugin - error %d"), error);
		return EFail;
		}

	TInt numCameras = info->CamerasAvailable();
	delete info;
	REComSession::DestroyedImplementation(destructorKey);
	REComSession::FinalClose();
	__MM_HEAP_MARKEND;
	if (numCameras!=0)
		{
		INFO_PRINTF2(_L("Error - %d returned"), numCameras);
		verdict = EFail;
		}
	return verdict;
	}
