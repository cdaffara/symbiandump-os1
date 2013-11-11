
// ECamInfoAdvTest.cpp

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

#include <ecamuids.hrh>
#include "ECamInfoAdvTest.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include <ecom/ecomresolverparams.h>
#include "ECamUnitTestPluginUids.hrh"

const TInt KInvalidNumberOfCameras = -1;
const TInt KEmptyKeyValue = 0;
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
	iTestStepName = _L("MM-ECM-ADV-U-001-HP");
	}
	
TVerdict RECamInfoTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check CCamera::CamerasAvailable"));
	INFO_PRINTF1(_L("using static function call"));
	__MM_HEAP_MARK;
	TInt numCameras = CCamera::CamerasAvailable();
	__MM_HEAP_MARKEND;
	// the verdict
	if (numCameras <= 0)
		{
		INFO_PRINTF2(_L("Error %d returned"), numCameras);
		return EFail;
		}
	INFO_PRINTF2(_L("Available cameras  %d returned"), numCameras);
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
	iTestStepName = _L("MM-ECM-ADV-U-002-LP");
	}
	
TVerdict RECamInnerInfoTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check CCameraInfoPlugin"));
	INFO_PRINTF1(_L("Use CCamera::CamerasAvailable"));
	TInt numCameras = KInvalidNumberOfCameras;
	TVerdict result = EPass;

	INFO_PRINTF1(_L("Open set plugin directly"));
		
	__MM_HEAP_MARK;
	TUid implementationUid = {KUidCameraUnitTestInfo};
	TUid destructorKey = {KEmptyKeyValue};
	CCameraInfoPlugin* info = NULL;
	TRAPD(error, info = static_cast<CCameraInfoPlugin*>
						(REComSession::CreateImplementationL(implementationUid, destructorKey)));
			
	if (error != KErrNone)
		{
		INFO_PRINTF3(_L("CreateImplementationL InfoPlugin %x failed %d"), KUidCameraUnitTestInfo, error);
		result = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("CreateImplementationL InfoPlugin %x is OK"), destructorKey.iUid);

		numCameras = info->CamerasAvailable();
		INFO_PRINTF2(_L("Available cameras  %d returned"), numCameras);
		delete info;
		REComSession::DestroyedImplementation(destructorKey);
		REComSession::FinalClose();
		}
	
	// the verdict	
	if (numCameras <= 0)
		{
		INFO_PRINTF2(_L("Error - camera %d returned"), numCameras);
		result = EFail;
		}	
	__MM_HEAP_MARKEND;

	return result;
	}
