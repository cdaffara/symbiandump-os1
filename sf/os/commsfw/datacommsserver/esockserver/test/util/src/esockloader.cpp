// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32property.h>
#include <c32root.h>
#include <cfshared.h>
#include <comms-infras/commsdebugutility.h>

#include "esockloader.h"

__FLOG_STMT(_LIT8(KESockLogSubsys, "esock");)
__FLOG_STMT(_LIT8(KESockLogComponent, "esockloader");)

/** When unloading ESock forced we try first in a graceful manner, if it hasn't
unloaded after this timeout it is asked to shutdown immediately.
This will happen if ESock has "hanging" sessions.
 */
const TInt KGracefulTimeout = 90 * 1000000;

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

EXPORT_C TInt ESockLoader::LoadESock(MHarnessLogAdaptor* aHarnessLogger)
/** Connect to the Comms Rootserver and request load of the Socket Server CPM.
Returns when it is loaded. 
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KESockLogSubsys, KESockLogComponent);
	__FLOG(_L("ESockLoader::LoadESock"));
	
	TPtrC pddName = PDD_NAME;
	TPtrC lddName = LDD_NAME;

	// Load the communications drivers first - no use starting C32 without them.
	TInt result = User::LoadPhysicalDevice(pddName);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_2(_L("ESockLoader::LoadConfig - could not load Serial Physical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &pddName, result);
		aHarnessLogger->Log(_L("Could not load Serial Physical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &pddName, result);
		}

	result=User::LoadLogicalDevice(lddName);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_2(_L("ESockLoader::LoadConfig - could not load Serial Logical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &lddName, result);	
		aHarnessLogger->Log(_L("Could not load Serial Logical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &lddName, result);
		}

	// Boot the Comms Process, but avoid loading the Phonebook Synchronizer.
	#ifdef __FLOG_ACTIVE
		TBool warmBoot = TestC32ExeStarted(__logger__);
	#else
		TBool warmBoot = TestC32ExeStarted();
	#endif
	if(warmBoot)
		{
		aHarnessLogger->Log(_L("Starting C32Start warm-boot"));
		}
	else
		{
		aHarnessLogger->Log(_L("Starting C32Start cold-boot"));
		}
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	result=WarmBootC32(KPhbkSyncCMI);
	
	if(result == KErrNone)
		{
		aHarnessLogger->Log(warmBoot ? _L("C32Start warm-boot succeeded") : _L("C32Start cold-boot succeeded"));
		__FLOG(_L("ESockLoader::LoadESock - succeeded"));
		}
	else
		{
		aHarnessLogger->Log(warmBoot ? _L("C32Start warm-boot failed with error %d") : _L("C32Start cold-boot failed with error %d"), result);
		__FLOG_1(_L("ESockLoader::LoadESock - failed with error %d"), result);
		}
	__FLOG_CLOSE;

	return result;
	}

// Helper fn: build a list of instances of CPMs running ESOCKSVR.DLL
void ESockLoader::BuildESockCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList)
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPENC(KESockLogSubsys, KESockLogComponent);
	__FLOG(_L8("ESockLoader::BuildConfigCPMListL"));

	TRSIter iter;
	TCFModuleName modName;
	TRSModuleInfo modInfo;
	aInfo.ResetAndDestroy();
	aDispList.SetLength(0);
	while(aRootServer.EnumerateModules(iter, modName) == KErrNone)
		{
		if(aRootServer.GetModuleInfo(modName, modInfo) == KErrNone)
			{
			_LIT(KESockSvrDLL, "*ESOCKSVR.DLL");
			if(modInfo.iParams.iDll.MatchF(KESockSvrDLL) >= 0)
				{
				TESockSvrUnloadInfo* unloadInfo = new(ELeave) TESockSvrUnloadInfo;
				unloadInfo->iName.Copy(modInfo.iParams.iName);
				unloadInfo->iState=modInfo.iParams.iState;
				unloadInfo->iStatus=KErrNone;
				if(aDispList.Length() != 0)
					{
					aDispList.Append(_L8(", "));
					}
				aDispList.Append(unloadInfo->iName);
				TInt err = aInfo.Append(unloadInfo);
				if(err != KErrNone)
					{
					delete unloadInfo;
					User::Leave(err);
					}
				}
			}
		}
	
	__FLOG_CLOSE_CLEANUP;
	}

#ifdef __FLOG_ACTIVE
	TBool ESockLoader::TestC32ExeStarted(RFileLogger& __logger__)
#else
	TBool ESockLoader::TestC32ExeStarted()
#endif
	{
	__FLOG(_L8("ESockLoader::TestC32ExeStarted"));

	_LIT(KC32StartName,"*");
	TBool ret = EFalse;
	TFullName fn;
	TFindProcess fp(KC32StartName);
	while(!ret && fp.Next(fn) == KErrNone)
		{
		RProcess proc;
		TInt result = proc.Open(fn);
		if(result == KErrNone)
			{
			TUidType type = proc.Type();
			if(type[2] == KUidCommsProcess && proc.ExitType() == EExitPending) 
				{
				ret = ETrue;
				}
			proc.Close();	
			}
		}

	return ret;
	}

#ifdef __FLOG_ACTIVE
	TBool ESockLoader::KillC32Exe(RFileLogger& __logger__)
#else
	TBool ESockLoader::KillC32Exe()
#endif
	{
	__FLOG(_L8("ESockLoader::KillC32Exe"));

	_LIT(KC32StartName,"*");
	TBool ret = EFalse;
	TFullName fn;
	TFindProcess fp(KC32StartName);
	while(!ret && fp.Next(fn) == KErrNone)
		{
		RProcess proc;
		TInt result = proc.Open(fn);
		if(result == KErrNone)
			{
			TUidType type = proc.Type();
			if(type[2] == KUidCommsProcess) 
				{
				proc.Kill(KErrNone);
				}
			proc.Close();	
			}
		}

	return ret;
	}
	
#ifdef __FLOG_ACTIVE
	void ESockLoader::DoUnloadESockL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__)
#else
	void ESockLoader::DoUnloadESockL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
#endif
	{
	__FLOG_1(_L8("ESockLoader::DoUnLoadConfigL - CommsFW::TCFShutdownType==%d"), aType);

	// No need to restart esock (c32exe.exe) if esock (c32exe.exe) is not started...
#ifdef __FLOG_ACTIVE
	if (!TestC32ExeStarted(__logger__))
#else
	if (!TestC32ExeStarted())
#endif
		{
		aHarnessLogger->Log(_L8("C32 is not running so it does not need to be shutdown"));
		__FLOG(_L8("ESockLoader::DoUnloadESockL - C32 is not running so it does not need to be shutdown"));

		return;
		}		
	
	// Start the Comms Process
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err=StartC32WithCMISuppressions(KPhbkSyncCMI);

	int leakValue;
	RProperty leakProperty;
	TBool checkForLeaks; //if enabled
	
	//The root server should have been started so we can attack to the leak Property now
	checkForLeaks = (leakProperty.Attach(KUidCommsProcess, KUidCommsModuleLeakCounter) == KErrNone);
	CleanupClosePushL(leakProperty);
	if (checkForLeaks)
		{
		checkForLeaks = (leakProperty.Get(leakValue) == KErrNone);	
		}

	// With the Staged Start-up Architecture it is likely that ESOCK modules continue to load after the ECoreComponentsStarted
	// state which releases StartC32() and RSocketServ::Connect(). So here we wait until the RootServer is fully configured before
	// starting the shutdown, which avoids various races (modules may not be bound yet, or even loaded at all by the Configurator
	// and hence invisible to this unloading code)
	RProperty configurationProperty;
	configurationProperty.Attach(KUidSystemCategory, KUidC32StartPropertyKey.iUid);	// needs the KEY
	TInt propertyValue = EInitialising;	// set to safe state
	TInt propertyResult = configurationProperty.Get(propertyValue);
	TRequestStatus propertyStatus;
	while(propertyValue < EConfigurationComplete)
		{
		configurationProperty.Subscribe(propertyStatus);
		
		if(configurationProperty.Get(propertyValue) == KErrNone && propertyValue == EConfigurationComplete)
			{
			configurationProperty.Cancel();
			}
		User::WaitForRequest(propertyStatus);
		}

	RRootServ rootserver;
	if(err==KErrNone)
		{
		err=rootserver.Connect();
		}
	User::LeaveIfError(err);
	CleanupClosePushL(rootserver);
	
	// Find all instances of CPMs running ESOCKSVR.DLL. We treat them all equally rather than trying anything
	// clever such as telling the Main Thread to shutdown last, etc - ESOCK does whatever is best

	RUnloadInfoArray unloadArray(16);
	CleanupClosePushL(unloadArray);
	TBuf8<256> modList;
	BuildESockCPMListL(rootserver, unloadArray, modList);

	aHarnessLogger->Log(_L8("%d modules to shutdown: %S"), unloadArray.Count(), &modList);

	// Start by asking all of the ESOCK threads to unload when there are no sessions
	TInt numLoaded = unloadArray.Count();
	TInt mod;
	for(mod = numLoaded - 1; mod >= 0; --mod)
		{
		TESockSvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("ESockLoader::DoUnLoadConfigL - UnloadCPM(%S, EGraceful)"), &info->iName);
		rootserver.UnloadCpm(info->iStatus, info->iName, EGraceful);
		}

	// Start polling to see when they all complete unloading - crude but easy. 
	const TInt KPollPeriod = 2000 * 1000;
	TUint maxPolls = 0xFFFFFFFF;	// a saintly degree of patience
	if(aType==EImmediate)
		{
		// Limited patience for the unload; 
		maxPolls = KGracefulTimeout / KPollPeriod;
		}
	TUint poll;
	RUnloadInfoArray pollUnloadArray(16);
	BuildESockCPMListL(rootserver, pollUnloadArray, modList);
	for(poll = maxPolls; poll != 0 && pollUnloadArray.Count() > 0; --poll)
		{
		User::After(KPollPeriod);
		// See what's left - an earlier version of this code relied upon the unload completions for this, but unload
		// may complete prematurely with failure if the unbind times out. It can legitimately do this if waiting for
		// a session to timeout
		BuildESockCPMListL(rootserver, pollUnloadArray, modList);
		if(pollUnloadArray.Count() > 0)
			{
			aHarnessLogger->Log(_L8("[%d]:%d remaining: %S"), poll, pollUnloadArray.Count(), &modList);
			}
		}
	pollUnloadArray.ResetAndDestroy();

	if(poll == 0 && unloadArray.Count() > 0)
	    {
	    __FLOG(_L8("Reached maxpolls, gave up."));
	    }
	
	// Cancel any remaining unloads and eat the events
	for(mod = unloadArray.Count() - 1; mod >= 0; --mod)
		{
		TESockSvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("ESockLoader::DoUnLoadConfigL - CancelUnloadCPM(%S)"), &info->iName);
		rootserver.CancelUnloadCpm(info->iName);
		User::WaitForRequest(info->iStatus);
		}

	// See what's left
	BuildESockCPMListL(rootserver, unloadArray, modList);

	err = KErrNone; // Will use this to catch first error below.
	if(unloadArray.Count() > 0 && aType == EImmediate)
		{
		__FLOG(_L8("Entering EImmediate loop."));
		// No more waiting; we order immediate unloads
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TESockSvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("ESockLoader::DoUnLoadConfigL - Module: %S, State %d"), &info->iName, info->iState);
			if(mod == 0)
				{
				aHarnessLogger->Log(_L8("Timed-out waiting for shutdowns"));
				err = KErrTimedOut;
				}
			aHarnessLogger->Log(_L8("Re-trying %S with EImmediate shutdown"), &info->iName);
			__FLOG_1(_L8("ESockLoader::DoUnLoadConfigL - UnloadCPM(%S, EImmediate)"), &info->iName);
			rootserver.UnloadCpm(info->iStatus, info->iName, EImmediate);
			}
			
		// Wait for them all to return.
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TESockSvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("ESockLoader::DoUnLoadConfigL - Module: %S, State %d"), &info->iName, info->iState);
			aHarnessLogger->Log(_L8("Waiting for %S to unload."), &info->iName);
			__FLOG_1(_L8("ESockLoader::DoUnLoadConfigL - WaitForUnloadCPM(%S, EImmediate)"), &info->iName);
			User::WaitForRequest(info->iStatus);
			}
		}

	// Display the status of any remaining modules.  The first module that
	// failed to unload becomes the error code for all.
	for(mod = unloadArray.Count() - 1; mod >= 0; --mod)
		{
		TESockSvrUnloadInfo* info = unloadArray[mod];
		if(info->iStatus != KErrNone && info->iStatus != KErrRSModuleNotLoaded)
			{
			__FLOG_3(_L8("ESockLoader::DoUnLoadConfigL - Unresponsive Module: %S cannot be gracefully or immediatly unloaded due to error %d and is stuck in state %d"), &info->iName, info->iStatus.Int(), info->iState);
			aHarnessLogger->Log(_L8("Unresponsive Module: %S cannot be unloaded due to error %d"), &info->iName, info->iStatus.Int());
			if(err==KErrNone)
				{
    			err=info->iStatus.Int();
				}
			}
		else
			{
			__FLOG_2(_L8("ESockLoader::DoUnLoadConfigL - Unresponsive Module: %S could not be gracefully unloaded and had to be immediatly unloaded while it was stuck in state %d"), &info->iName, info->iState);
			aHarnessLogger->Log(_L8("Unresponsive Module: %S could not be gracefully unloaded and had to be immediatly unloaded while it was stuck in state %d"), &info->iName, info->iState);
			}
		}
	if(err==KErrNone)
		{
		aHarnessLogger->Log(_L("Shutdown completed"));
		}
	CleanupStack::PopAndDestroy(2);	// unloadArray, rootserver
	
	// Leave with last seen error, if any
	if(err!=KErrNone)
		{
		User::Leave(err);
		}
		
	if (checkForLeaks)
		{
		TInt leakVal2;
		if (leakProperty.Get(leakVal2) == KErrNone)
			{
			if (leakVal2 > leakValue)
				{
				User::Leave(KModuleLeaked);
				}
			}
		}
	CleanupStack::Pop(&leakProperty);
	leakProperty.Close();
	}


EXPORT_C TInt ESockLoader::UnloadESock(MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KESockLogSubsys, KESockLogComponent);
	__FLOG(_L8("ESockLoader::UnloadESock"));
	__FLOG_CLOSE;

	TInt err = UnloadESock(EGraceful, aHarnessLogger);
	
	return err;
    }

EXPORT_C TInt ESockLoader::UnloadESock(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KESockLogSubsys, KESockLogComponent);
	__FLOG_1(_L8("ESockLoader::UnloadESock, CommsFW::TCFShutdownType==%d."), aType);

#ifdef __FLOG_ACTIVE
	TRAPD(err, DoUnloadESockL(aType, aHarnessLogger, __logger__));
#else
	TRAPD(err, DoUnloadESockL(aType, aHarnessLogger));
#endif

	if(err == KErrNone)
		{
		__FLOG(_L("ESockLoader::UnLoadConfig - succeeded."));
		}
	else
		{
		__FLOG_1(_L("ESockLoader::UnLoadConfig - failed with error %d.  The C32 process is in an unstable state and will be killed."), err);
		aHarnessLogger->Log(_L("Shutdown failed with error %d.  The C32 process is in an unstable state and will be killed."), err);
		
		#ifdef __FLOG_ACTIVE
			KillC32Exe(__logger__);
		#else
			KillC32Exe();
		#endif
		}
		
	__FLOG_CLOSE;
	
	return err;
	}

