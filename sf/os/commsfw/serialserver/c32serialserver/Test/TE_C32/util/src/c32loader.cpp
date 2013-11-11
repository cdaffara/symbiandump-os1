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

#include "c32loader.h"

__FLOG_STMT(_LIT8(KC32LogSubsys, "C32SerComms");)
__FLOG_STMT(_LIT8(KC32LogComponent, "C32loader");)

/** When unloading C32 forced we try first in a graceful manner, if it hasn't
unloaded after this timeout it is asked to shutdown immediately.
This will happen if C32 has "hanging" sessions.
 */
const TInt KGracefulTimeout = 90 * 1000000;

#if defined (__EPOC32__)
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

EXPORT_C TInt C32Loader::LoadC32(MHarnessLogAdaptor* aHarnessLogger)
/** Connect to the Comms Rootserver and request load of the Socket Server CPM.
Returns when it is loaded. 
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KC32LogSubsys, KC32LogComponent);
	__FLOG(_L("C32Loader::loadC32."));
	
	TInt result;
#if defined (__EPOC32__)
	//Load the drivers first..No use starting C32 without them (only on harware)
	result = User::LoadPhysicalDevice(PDD_NAME);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_1(_L("Could not load PDD! Leaving with error %d."), result);
//		return result;
		}

	result=User::LoadLogicalDevice(LDD_NAME);
	if (result!=KErrNone && result!=KErrAlreadyExists)
		{
		__FLOG_1(_L("Could not load LDD! Leaving with error %d."), result);	
//		return result;
		}
#endif
	// Warm-boot the Comms Process, but avoid loading the Phonebook Synchronizer
	aHarnessLogger->Log(_L("Starting C32Start warm-boot"));
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	result=WarmBootC32(KPhbkSyncCMI);
	aHarnessLogger->Log(_L("C32Start warm-boot result=%d"), result);
	
	__FLOG_1(_L("C32Loader::loadC32. Done %d."), result);
	__FLOG_CLOSE;
	return result;
	}

// Helper fn: build a list of instances of CPMs running C32.DLL
void C32Loader::BuildC32CPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList)
	{
	TRSIter iter;
	TCFModuleName modName;
	TRSModuleInfo modInfo;
	aInfo.ResetAndDestroy();
	aDispList.SetLength(0);
	while(aRootServer.EnumerateModules(iter, modName) == KErrNone)
		{
		if(aRootServer.GetModuleInfo(modName, modInfo) == KErrNone)
			{
			_LIT(KC32DLL, "*C32.DLL");
			if(modInfo.iParams.iDll.MatchF(KC32DLL) >= 0)
				{
				TC32SvrUnloadInfo* unloadInfo = new(ELeave) TC32SvrUnloadInfo;
				unloadInfo->iName.Copy(modInfo.iParams.iName);
				unloadInfo->iStatus=modInfo.iParams.iState;
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
	}


static TBool TestC32ExeStarted()
	{
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
void C32Loader::DoUnloadC32L(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger, RFileLogger& __logger__)
#else
void C32Loader::DoUnloadC32L(TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
#endif
	{
	int leakValue;
	RProperty leakProperty;
	TBool checkForLeaks; //if enabled
		
	if (!TestC32ExeStarted())	//No need to restart C32 if C32(c32exe) is not started;
		{
		return;
		}
	// Start the Comms Process
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err=StartC32WithCMISuppressions(KPhbkSyncCMI);
		
	//The root server should have been started so we can attack to the leak Property now
	checkForLeaks = (leakProperty.Attach(KUidCommsProcess, KUidCommsModuleLeakCounter) == KErrNone);
	CleanupClosePushL(leakProperty);
	if (checkForLeaks)
		{
		checkForLeaks = (leakProperty.Get(leakValue) == KErrNone);	
		}
		
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
	BuildC32CPMListL(rootserver, unloadArray, modList);

	aHarnessLogger->Log(_L8("%d modules to shutdown: %S"), unloadArray.Count(), &modList);

	// Start by asking all of the C32 threads to unload when there are no sessions
	TInt numLoaded = unloadArray.Count();
	TInt mod;
	for(mod = numLoaded - 1; mod >= 0; --mod)
		{
		TC32SvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("C32Loader::UnloadCPM(%S, EGraceful)"), &info->iName);
		//rootserver.UnloadCpm(info->iStatus, info->iName, EGraceful);
		rootserver.UnloadCpm(info->iStatus, info->iName, EImmediate);
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
	BuildC32CPMListL(rootserver, pollUnloadArray, modList);
	for(poll = maxPolls; poll != 0 && pollUnloadArray.Count() > 0; --poll)
		{
		User::After(KPollPeriod);
		// See what's left - an earlier version of this code relied upon the unload completions for this, but unload
		// may complete prematurely with failure if the unbind times out. It can legitimately do this if waiting for
		// a session to timeout
		BuildC32CPMListL(rootserver, pollUnloadArray, modList);
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
		TC32SvrUnloadInfo* info = unloadArray[mod];
		__FLOG_1(_L8("C32Loader::CancelUnloadCPM(%S)"), &info->iName);
		rootserver.CancelUnloadCpm(info->iName);
		User::WaitForRequest(info->iStatus);
		}

	// See what's left
	BuildC32CPMListL(rootserver, unloadArray, modList);

	err = KErrNone; // Will use this to catch first error below.
	if(unloadArray.Count() > 0 && aType == EImmediate)
		{
		__FLOG(_L8("Entering EImmediate loop."));
		// No more waiting; we order immediate unloads
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TC32SvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("Module: %S. Status %d."), &info->iName, info->iStatus.Int());
			if(mod == 0)
				{
				aHarnessLogger->Log(_L8("Timed-out waiting for shutdowns"));
				}
			aHarnessLogger->Log(_L8("Re-trying %S with EImmediate shutdown"), &info->iName);
			__FLOG_1(_L8("C32Loader::UnloadCPM(%S, EImmediate)"), &info->iName);
			rootserver.UnloadCpm(info->iStatus, info->iName, EImmediate);
			}
			
		// Wait for them all to return.
		for(mod = 0; mod < unloadArray.Count(); mod++)
			{
			TC32SvrUnloadInfo* info = unloadArray[mod];
			__FLOG_2(_L8("Module: %S. Status %d."), &info->iName, info->iStatus.Int());
			aHarnessLogger->Log(_L8("Waiting for %S to unload."), &info->iName);
			__FLOG_1(_L8("C32Loader::WaitForUnloadCPM(%S, EImmediate)"), &info->iName);
			User::WaitForRequest(info->iStatus);
			}
		}

	__FLOG(_L8("Get first failure errorcode."));
	// The first module that failed to unload becomes the error code for all
	for(mod = unloadArray.Count() - 1; mod >= 0; --mod)
		{
		TC32SvrUnloadInfo* info = unloadArray[mod];
		__FLOG_2(_L8("Module: %S. Status %d."), &info->iName, info->iStatus.Int());
		if(info->iStatus != KErrNone && info->iStatus != KErrRSModuleNotLoaded)
			{
			aHarnessLogger->Log(_L8("%S completed with %d"), &info->iName, info->iStatus.Int());
			if(err==KErrNone)
				{
    			err=info->iStatus.Int();
				}
			}
		}
	aHarnessLogger->Log(_L("Shutdown completed"));
	CleanupStack::PopAndDestroy(2);	// unloadArray, rootserver
	
	#if defined (__EPOC32__)	
	TInt retDD = User::FreePhysicalDevice(PDD_NAME);
	if (retDD != KErrNone)
		{
		aHarnessLogger->Log(_L8("Failed unloading Physical Device Driver return code %d "), retDD);	
		}
	
	retDD = User::FreeLogicalDevice(LDD_NAME);
	if (retDD != KErrNone)
		{
		aHarnessLogger->Log(_L8("Failed unloading Logical Device Driver return code %d "), retDD);	
		}
	#endif
	// LEave with last seen error, if any
	if(err!=KErrNone)
		{
//		User::Leave(err);
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


EXPORT_C TInt C32Loader::UnloadC32(MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	return UnloadC32(EGraceful, aHarnessLogger);
    }

EXPORT_C TInt C32Loader::UnloadC32(CommsFW::TCFShutdownType aType, MHarnessLogAdaptor* aHarnessLogger)
/**
Connects to the Comms Rootserver and requests unload of the Socket Server CPM.
Returns when it is unloaded.
*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KC32LogSubsys, KC32LogComponent);
	__FLOG_1(_L("C32Loader::UnloadC32, CommsFW::TCFShutdownType==%d."), aType);

#ifdef __FLOG_ACTIVE
	TRAPD(err, DoUnloadC32L(aType, aHarnessLogger, __logger__));
#else
	TRAPD(err, DoUnloadC32L(aType, aHarnessLogger));
#endif

	__FLOG_1(_L("C32Loader::UnloadC32: returning %d."), err);
	__FLOG_CLOSE;
	return err;
	}

