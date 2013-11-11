// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "configloader.h"

__FLOG_STMT(_LIT8(KC32LogSubsys, "RootServer");)
__FLOG_STMT(_LIT8(KC32LogComponent, "Configloader");)

/** When unloading C32 forced we try first in a graceful manner, if it hasn't
unloaded after this timeout it is asked to shutdown immediately.
This will happen if C32 has "hanging" sessions.
 */
const TInt KGracefulTimeout = 90 * 1000000;

#if defined (__EPOC32__)
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

EXPORT_C TInt ConfigLoader::LoadConfig(MHarnessLogAdaptor* aHarnessLogger)
/** Connect to the Comms Rootserver and request load of the Socket Server CPM.
Returns when it is loaded. 
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KC32LogSubsys, KC32LogComponent);
	__FLOG(_L("ConfigLoader::LoadConfig"));
	
	TInt result;
	
#if defined (__EPOC32__)
	TPtrC pddName = PDD_NAME;
	TPtrC lddName = LDD_NAME;

	// Load the communications drivers first - no use starting C32 without them.
	result = User::LoadPhysicalDevice(pddName);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_2(_L("ConfigLoader::LoadConfig - could not load Serial Physical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &pddName, result);
		aHarnessLogger->Log(_L("Could not load Serial Physical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &pddName, result);
		}

	result=User::LoadLogicalDevice(lddName);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_2(_L("ConfigLoader::LoadConfig - could not load Serial Logical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &lddName, result);	
		aHarnessLogger->Log(_L("Could not load Serial Logical Device Driver \"%S\" due to error %d so the device's COMM ports will not be available"), &lddName, result);
		}
#endif

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
	
	RProcess C32Start;
	result = C32Start.Create(KCommC32StartExe,KNullDesC);

	// Kill the existing configurator process.
	TRequestStatus status;
	C32Start.Logon(status);
	C32Start.Kill(KErrNone);
	User::WaitForRequest(status);
	
	// Create a timer in case some other entity holds an open handle on the
	// configurator which prevents the kernel from destroying it.  We timeout
	// after one second.
	TAutoClose<RTimer> timer;
	if(timer.iObj.CreateLocal() == KErrNone)
		{
		// Request destruction notification so we know when it is safe to start
		// the process again.
		TRequestStatus destructionStatus;
		C32Start.NotifyDestruction(destructionStatus);
		C32Start.Close();
	
		enum{ KProcessDestructionTimeout = 1000000 };
  					
		TRequestStatus timerStatus;
		timer.iObj.After(timerStatus, KProcessDestructionTimeout);
		
		// Wait for the process to be destroyed or for the timeout.
		User::WaitForRequest(destructionStatus, timerStatus);
		if(timerStatus.Int() == KRequestPending)
			{
			timer.iObj.Cancel();
			User::WaitForRequest(timerStatus);
			}
		else
			{
			User::CancelMiscNotifier(destructionStatus);
			User::WaitForRequest(destructionStatus);
			
			__FLOG_1(_L("ConfigLoader::LoadConfig - existing configurator process has still not been destroyed after %f.0s"), KProcessDestructionTimeout / 1000000.0);
			}
		}
	else
		{
		C32Start.Close();
		}

	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	result=WarmBootC32(KPhbkSyncCMI);
	
	if(result == KErrNone)
		{
		aHarnessLogger->Log(warmBoot ? _L("C32Start warm-boot succeeded") : _L("C32Start cold-boot succeeded"));
		__FLOG(_L("ConfigLoader::LoadConfig - succeeded"));
		}
	else
		{
		aHarnessLogger->Log(warmBoot ? _L("C32Start warm-boot failed with error %d") : _L("C32Start cold-boot failed with error %d"), result);
		__FLOG_1(_L("CConfigLoader::LoadConfig - failed with error %d"), result);
		}
	__FLOG_CLOSE;

	return result;
	}

// Helper fn: build a list of instances of CPMs running C32.DLL
void ConfigLoader::BuildConfigCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList)
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPENC(KC32LogSubsys, KC32LogComponent);
	__FLOG(_L8("ConfigLoader::BuildConfigCPMListL"));

	TRSIter iter;
	TCFModuleName modName;
	TRSModuleInfo modInfo;
	aInfo.ResetAndDestroy();
	aDispList.SetLength(0);
	while(aRootServer.EnumerateModules(iter, modName) == KErrNone)
		{
		if(aRootServer.GetModuleInfo(modName, modInfo) == KErrNone)
			{
			_LIT(KC32DLL, "dummycpm.DLL");
			if(modInfo.iParams.iDll.MatchF(KC32DLL) >= 0)
				{
				TConfigSvrUnloadInfo* unloadInfo = new(ELeave) TConfigSvrUnloadInfo;
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
	TBool ConfigLoader::TestC32ExeStarted(RFileLogger& __logger__)
#else
	TBool ConfigLoader::TestC32ExeStarted()
#endif
	{
	__FLOG(_L8("ConfigLoader::TestC32ExeStarted"));

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
	TBool ConfigLoader::KillC32Exe(RFileLogger& __logger__)
#else
	TBool ConfigLoader::KillC32Exe()
#endif
	{
	__FLOG(_L8("ConfigLoader::KillC32Exe"));

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
	void ConfigLoader::DoUnLoadConfigL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__)
#else
	void ConfigLoader::DoUnLoadConfigL(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
#endif
	{
	__FLOG_1(_L8("ConfigLoader::DoUnLoadConfigL - CommsFW::TCFShutdownType==%d"), aType);

	// No need to restart C32 if C32(c32exe) is not started...
#ifdef __FLOG_ACTIVE
	if (!TestC32ExeStarted(__logger__))
#else
	if (!TestC32ExeStarted())
#endif
		{
		aHarnessLogger->Log(_L8("C32 is not running so it does not need to be shutdown"));
		__FLOG(_L8("ConfigLoader::DoUnLoadConfigL - C32 is not running so it does not need to be shutdown"));

		return;
		}

	// Start the Comms Process
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err=StartC32WithCMISuppressions(KPhbkSyncCMI);

	// With the Staged Start-up Architecture it is likely that C32 modules continue to load after the ECoreComponentsStarted
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
	
	// Find all instances of CPMs running C32.DLL. We treat them all equally rather than trying anything
	// clever such as telling the Main Thread to shutdown last, etc - C32 does whatever is best

	RUnloadInfoArray unloadArray(16);
	CleanupClosePushL(unloadArray);
	TBuf8<256> modList;
	BuildConfigCPMListL(rootserver, unloadArray, modList);

	aHarnessLogger->Log(_L8("%d modules to shutdown: %S"), unloadArray.Count(), &modList);

	// Start by asking all of the C32 threads to unload when there are no sessions
	TInt numLoaded = unloadArray.Count();
	TInt mod;
	for(mod = numLoaded - 1; mod >= 0; --mod)
		{
		TConfigSvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("ConfigLoader::DoUnLoadConfigL - UnloadCPM(%S, EGraceful)"), &info->iName);
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
	BuildConfigCPMListL(rootserver, pollUnloadArray, modList);
	for(poll = maxPolls; poll != 0 && pollUnloadArray.Count() > 0; --poll)
		{
		User::After(KPollPeriod);
		// See what's left - an earlier version of this code relied upon the unload completions for this, but unload
		// may complete prematurely with failure if the unbind times out. It can legitimately do this if waiting for
		// a session to timeout
		BuildConfigCPMListL(rootserver, pollUnloadArray, modList);
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
		TConfigSvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("ConfigLoader::DoUnLoadConfigL - CancelUnloadCPM(%S)"), &info->iName);
		rootserver.CancelUnloadCpm(info->iName);
		User::WaitForRequest(info->iStatus);
		}

	// See what's left
	BuildConfigCPMListL(rootserver, unloadArray, modList);

	err = KErrNone; // Will use this to catch first error below.
	if(unloadArray.Count() > 0 && aType == EImmediate)
		{
		__FLOG(_L8("Entering EImmediate loop."));
		// No more waiting; we order immediate unloads
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TConfigSvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("ConfigLoader::DoUnLoadConfigL - Module: %S, State %d"), &info->iName, info->iState);
			if(mod == 0)
				{
				aHarnessLogger->Log(_L8("Timed-out waiting for shutdown"));
				err = KErrTimedOut;
				}
			aHarnessLogger->Log(_L8("Re-trying %S with EImmediate shutdown"), &info->iName);
			__FLOG_1(_L8("ConfigLoader::DoUnLoadConfigL - UnloadCPM(%S, EImmediate)"), &info->iName);
			rootserver.UnloadCpm(info->iStatus, info->iName, EImmediate);
			}
			
		// Wait for them all to return.
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TConfigSvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("ConfigLoader::DoUnLoadConfigL - Module: %S, State %d"), &info->iName, info->iState);
			aHarnessLogger->Log(_L8("Waiting for %S to unload"), &info->iName);
			__FLOG_1(_L8("ConfigLoader::DoUnLoadConfigL - WaitForUnloadCPM(%S, EImmediate)"), &info->iName);
			User::WaitForRequest(info->iStatus);
			}
		}

	// Display the status of any remaining modules.  The first module that
	// failed to unload becomes the error code for all.
	for(mod = unloadArray.Count() - 1; mod >= 0; --mod)
		{
		TConfigSvrUnloadInfo* info = unloadArray[mod];
		if(info->iStatus != KErrNone && info->iStatus != KErrRSModuleNotLoaded)
			{
			__FLOG_3(_L8("ConfigLoader::DoUnLoadConfigL - Unresponsive Module: %S cannot be gracefully or immediatly unloaded due to error %d and is stuck in state %d"), &info->iName, info->iStatus.Int(), info->iState);
			aHarnessLogger->Log(_L8("Unresponsive Module: %S cannot be unloaded due to error %d and is stuck in state %d"), &info->iName, info->iStatus.Int(), info->iState);
			if(err==KErrNone)
				{
    			err=info->iStatus.Int();
				}
			}
		else
			{
			__FLOG_2(_L8("ConfigLoader::DoUnLoadConfigL - Unresponsive Module: %S could not be gracefully unloaded and had to be immediatly unloaded while it was stuck in state %d"), &info->iName, info->iState);
			aHarnessLogger->Log(_L8("Unresponsive Module: %S could not be gracefully unloaded and had to be immediatly unloaded while it was stuck in state %d"), &info->iName, info->iState);
			}
		}
	if(err==KErrNone)
		{
		aHarnessLogger->Log(_L("Shutdown completed"));
		}
	CleanupStack::PopAndDestroy(2);	// unloadArray, rootserver
	
	#if defined (__EPOC32__)	
	TPtrC pddName = PDD_NAME;
	TPtrC lddName = LDD_NAME;

	TInt retDD = User::FreePhysicalDevice(pddName);
	if (retDD != KErrNone)
		{
		aHarnessLogger->Log(_L8("Failed unloading Physical Device Driver \"%S\" due to error %d"), &pddName, retDD);	
		}
	
	retDD = User::FreeLogicalDevice(lddName);
	if (retDD != KErrNone)
		{
		aHarnessLogger->Log(_L8("Failed unloading Logical Device Driver \"%S\" due to error %d"), &lddName, retDD);	
		}
	#endif
	
	// Leave with last seen error, if any
	if(err!=KErrNone)
		{
//		User::Leave(err);
		}
	}


EXPORT_C TInt ConfigLoader::UnLoadConfig(MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KC32LogSubsys, KC32LogComponent);
	__FLOG(_L8("ConfigLoader::UnLoadConfig"));
	__FLOG_CLOSE;

	TInt err = UnLoadConfig(EGraceful, aHarnessLogger);
	
	return err;
    }

EXPORT_C TInt ConfigLoader::UnLoadConfig(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KC32LogSubsys, KC32LogComponent);
	__FLOG_1(_L8("ConfigLoader::UnLoadConfig - CommsFW::TCFShutdownType==%d."), aType);

#ifdef __FLOG_ACTIVE
	TRAPD(err, DoUnLoadConfigL(aType, aHarnessLogger, __logger__));
#else
	TRAPD(err, DoUnLoadConfigL(aType, aHarnessLogger));
#endif

	if(err == KErrNone)
		{
		__FLOG(_L("ConfigLoader::UnLoadConfig - succeeded."));
		}
	else
		{
		__FLOG_1(_L("ConfigLoader::UnLoadConfig - failed with error %d.  The C32 process is in an unstable state and will be killed."), err);
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

