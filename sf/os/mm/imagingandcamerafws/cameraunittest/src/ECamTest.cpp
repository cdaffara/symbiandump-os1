
// ECamTest.cpp

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

#include "ECamTest.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecamuids.hrh>

#include <ecom/ecomresolverparams.h>

#include <mm/mmpluginutils.h>

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
	iTestStepName = _L("MM-ECM-API-U-011");
	}
	
TVerdict RECamTest::DoTestStepL()
	{
	TVerdict result = EPass;
	CCamera* camera = NULL;
	MCameraObserver* observer = NULL; // should not be used, so pass silly value. TODO make this an MCameraObserver, or arrange would not KernExec3 on real plugin
	MCameraObserver2* observer2 = NULL; // TODO ditto
	TInt error = KErrNone;

	INFO_PRINTF1(_L("Check Opening CCam gives KErrNotSupported"));
	
	INFO_PRINTF1(_L("Checking basic CCamera::NewL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewL(*observer, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::NewL()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking basic CCamera::NewDuplicateL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewDuplicateL(*observer, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::NewDuplicateL()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewDuplicateL()"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking additional \"ChunkAPI\" factory functions"));
	INFO_PRINTF1(_L("Checking CCamera::NewL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewL(*observer2, 0, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::NewL()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL()"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking CCamera::NewDuplicateL()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewDuplicateL(*observer2, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::NewDuplicateL()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewDuplicateL()"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking CCamera::New2L()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::New2L(*observer2, 0, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::New2L()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L()"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	
	INFO_PRINTF1(_L("Checking CCamera::NewDuplicate2L()"));
	__MM_HEAP_MARK;
	TRAP(error, camera = CCamera::NewDuplicate2L(*observer2, 0));
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("Unexpected KErrNone return from CCamera::NewDuplicate2L()"));
		delete camera;
		result = EFail;
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewDuplicate2L()"), error);
		result = EFail;		
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
	iTestStepName = _L("MM-ECM-API-U-012");
	}
	
TVerdict RECamInnerTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check can actually open ECam plugin"));
	TVerdict result = EPass;


	INFO_PRINTF1(_L("Open stub plugin directly"));
		{
		__MM_HEAP_MARK;
		// TODO - would be nice to actually call CCameraPlugin::NewL, but this is not exported, so this is extract
		TUid implementationUid = {KUidOnboardCameraStubPlugin};
		TUid destructorKey = {0};
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
	
	INFO_PRINTF1(_L("Open stub plugin via interface"));
		{
		__MM_HEAP_MARK;
		// TODO - would be nice to actually call CCameraPlugin::NewL, but this is not exported, so this is extract
		TUid interfaceUid = {KUidOnboardCameraPlugin};
		TUid destructorKey = {0};
		CCamera* camera = NULL;
		TRAPD(error,
			camera = static_cast<CCamera*>
				(MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, KECamPluginMatchString, KRomOnlyResolverUid)));
				
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
