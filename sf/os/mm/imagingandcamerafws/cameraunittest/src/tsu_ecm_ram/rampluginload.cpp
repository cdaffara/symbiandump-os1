// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "rampluginload.h"
#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecamuids.hrh>

#include <ecom/ecomresolverparams.h>

#ifndef __WINSCW__
const TInt KMaxProcessNameLength = 50; // Max process name length.
_LIT(KProcessFileManCopyFile, "copyfile.exe");
_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames

_LIT(KOriginalDllOnZ, "Z:\\testplugins\\ecamramplugin.dll");
_LIT(KMovedDllOnC, "C:\\Sys\\Bin\\ecamramplugin.dll");
_LIT(KOriginalRscFileOnZ, "Z:\\testplugins\\ecamramplugin.rsc");
_LIT(KMovedRscFileOnC, "C:\\Resource\\Plugins\\ecamramplugin.rsc");

const TInt KWaitThreeSeconds = 3000000;
#endif


void LaunchProcessL(const TDesC& aProcessName, const TDesC& aCmdLine)
	{
	TRequestStatus stat;    
	RProcess p;
	User::LeaveIfError(p.Create(aProcessName, aCmdLine));

	// Asynchronous logon: completes when process terminates with process 
	// exit code
	p.Logon(stat);
	p.Resume();
	User::WaitForRequest(stat);

	TExitType exitType = p.ExitType();
	TInt exitReason = p.ExitReason();

	p.Close();
	User::LeaveIfError(exitReason);
	}

TInt VersionLinearOrderFunction(const CImplementationInformation& pluginA, 
                                const CImplementationInformation& pluginB)
	{
	if (pluginA.Version() == pluginB.Version())
		{
		return 0;
		}

	return (pluginA.Version() < pluginB.Version())? 1: -1;	
	}

//
// RRamPluginLoadTest
//

RRamPluginLoadTest* RRamPluginLoadTest::NewL(TBool aAllocTest)
	{
	RRamPluginLoadTest* self = new (ELeave) RRamPluginLoadTest(aAllocTest);
	return self;	
	}
	
RRamPluginLoadTest::RRamPluginLoadTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-RAM-U-001");
	}
	
TVerdict RRamPluginLoadTest::DoTestStepL()
	{
	TVerdict result = EPass;
#ifndef __WINSCW__
	TBuf<KMaxFileName*2> dllNames(KOriginalDllOnZ);
	TBuf<KMaxFileName*2> rscNames(KOriginalRscFileOnZ);

	dllNames.Append(KSeparator);
	dllNames.Append(KMovedDllOnC);
	rscNames.Append(KSeparator);
	rscNames.Append(KMovedRscFileOnC);
	TBufC<KMaxProcessNameLength> copyProcess(KProcessFileManCopyFile);

	// launch copyfile.exe to copy plugin from z: to c:
	TRAPD(ret, LaunchProcessL(copyProcess, dllNames));
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("Succesfully copied dll"));
		}
	else
		{
		INFO_PRINTF2(_L("Copying dll failed with %d"), ret);
		result = EFail;
		return result;
		}
	TRAP(ret, LaunchProcessL(copyProcess, rscNames));
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("Succesfully copied rsc file"));
		}
	else
		{
		INFO_PRINTF2(_L("Copying rsc file failed with %d"), ret);
		result = EFail;
		return result;
		}

	User::After(KWaitThreeSeconds); // Wait for ECom to re-scan drives so it can pick up new plugin now on c:
#endif
	_LIT(KRamPluginDisplayName, "Ram ECam Plugin");
	TBufC<15> expectedName(KRamPluginDisplayName);

	TBool pluginFound = EFalse;
	TInt error = KErrNone;

	INFO_PRINTF1(_L("Listing available plugins using REComSession::ListImplementationsL()..."));
	TUid interfaceUid = {KUidOnboardCameraPlugin};
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KECamPluginMatchString);
	RImplInfoPtrArray pluginArray;

	TRAP(error, REComSession::ListImplementationsL(interfaceUid, resolverParams, pluginArray));	
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("ListImplementationsL() failed - %d"), error);
		result = EFail;
		return result;
		}
	else
		{
		pluginArray.Sort(VersionLinearOrderFunction);
		for (TInt i = 0; i < pluginArray.Count(); ++i)
			{
			TBuf<2> drive;
			drive = pluginArray[i]->Drive().Name();
			INFO_PRINTF3(_L("%S located on drive %S"), &(pluginArray[i]->DisplayName()), &(drive));
			if((pluginArray[i]->DisplayName() == expectedName) && (pluginArray[i]->Drive() == 2))
				{
				pluginFound = ETrue;
				}
			}
		pluginArray.Close();

		if(!pluginFound)
			{
			INFO_PRINTF1(_L("REComSession::ListImplementationsL() could not find plugin in RAM"));
			}
		}

	INFO_PRINTF1(_L("Checking if CCamera::New2L() can successfully load plugin residing in RAM"));
	MCameraObserver2* observer2 = NULL;
	TRAP(error, CCamera* camera = CCamera::New2L(*observer2, 0, 0));
#ifdef _DEBUG
	if(error == KErrNone)
		{
		INFO_PRINTF1(_L("UDEB mode: Expected behaviour - successfully loaded plugin"));
		}
	else
		{
		INFO_PRINTF2(_L("UDEB mode: Unexpected behaviour - could not load plugin, error = %d"), error);
		result = EFail;
		}
#else
	if (error == KErrNone)
		{
		INFO_PRINTF1(_L("UREL mode: Unexpected behaviour - successfully loaded plugin"));
		result = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("UREL mode: Expected behaviour - could not load plugin, error = %d"), error);
		}
#endif

	return result;
	}
