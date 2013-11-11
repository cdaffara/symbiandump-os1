
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

#include "ECamAdvTest.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecamuids.hrh>

#include <ecom/ecomresolverparams.h>
#include "ECamUnitTestPluginUids.hrh"

const TInt KEmptyKeyValue = 0;

//
// RECamTest
//
RECamTest* RECamTest::NewL(TBool aAllocTest)
	{
	RECamTest* self = new (ELeave) RECamTest(aAllocTest);
	return self;	
	}
	
RECamTest::RECamTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-003-HP");
	}
	
TVerdict RECamTest::DoTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	MCameraObserver* observer = NULL; 
	MCameraObserver2* observer2 = NULL; 
	TInt error = KErrNone;

	INFO_PRINTF1(_L("Check Opening CCam gives KErrNone"));
	
	INFO_PRINTF1(_L("Checking basic CCamera::NewL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		if (camera != NULL)
			{
			delete camera;
			}
		result = EFail;
		}
		
	if (camera != NULL)	
		{
		delete camera;
		}
	
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking basic CCamera::NewDuplicateL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewDuplicateL(*observer, 0));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewDuplicateL()"), error);
		if (camera != NULL)
			{
			delete camera;
			}
		result = EFail;
		}
		
	if (camera != NULL)	
		{
		delete camera;
		}
	
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking additional \"ChunkAPI\" factory functions"));
	INFO_PRINTF1(_L("Checking CCamera::New2L()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::New2L(*observer2, 0, 0));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L()"), error);
		if (camera != NULL)
			{
			delete camera;
			}
		result = EFail;
		}
	
	if (camera != NULL)	
		{
		delete camera;
		}

	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking CCamera::NewDuplicate2L()"));
	
	__MM_HEAP_MARK;
	
	TRAP(error, camera = CCamera::NewDuplicate2L(*observer2, 0));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewDuplicate2L()"), error);
		if (camera != NULL)
			{
			delete camera;
			}
		result = EFail;
		}
		
	if (camera != NULL)	
		{
		delete camera;
		}

	__MM_HEAP_MARKEND;
	
	return result;
	}
	
//
// RECamInnerTest
//

RECamInnerTest* RECamInnerTest::NewL(TBool aAllocTest)
	{
	RECamInnerTest* self = new (ELeave) RECamInnerTest(aAllocTest);
	return self;	
	}
	
RECamInnerTest::RECamInnerTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-004-LP");
	}
	
TVerdict RECamInnerTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check can actually open ECam plugin"));
	TVerdict result = EPass;

	INFO_PRINTF1(_L("Open stub plugin directly"));
		{
		__MM_HEAP_MARK;
		TUid implementationUid = {KUidCameraUnitTestPlugin};
		TUid destructorKey = {KEmptyKeyValue};
		CCamera* camera = NULL;
		
		TRAPD(error,
			camera = static_cast<CCamera*>
				(REComSession::CreateImplementationL(implementationUid, destructorKey)));
				
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("CreateImplementationL() failed - %d"), error);
			result = EFail;
			}
		else
			{
			delete camera;
			REComSession::DestroyedImplementation(destructorKey);
			REComSession::FinalClose();
			}
		__MM_HEAP_MARKEND;
		}

	return result;
	}
