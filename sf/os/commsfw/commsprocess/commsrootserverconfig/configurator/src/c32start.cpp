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
// Implementation of the root server startup confguration process
// 
//

#include "c32start.h"
#include <f32file.h>
#include "c32cmi.h"
#include <rserror.h>
#include <cfextras.h>
	#include <ssm/ssmdomaindefs.h>
#include <rsshared.h>				// for TRSAddMBufAllocInfoContainer
	#include "c32startserv.h"
	#include <c32startshared.h>

#include <es_mbman.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_RootSrvc32start, "RootSrvc32start.");
#endif


/**
@file
Implements the reference Comms Configurator

@publishedPartner
@released
*/

/** Local Flogger instance for debug logging. We can't use the tidier __FLOG_DECLARATION_VARIABLE macro because
the need for a statement terminator causes a warning for WINSCW UREL
*/
#if defined(__FLOG_ACTIVE)
RFileLogger __logger__;
#endif

static void Fault(TConfiguratorServerPanic aFault)
    /** Panic the configurator server
     @param aFault Fault code as defined in cs_std.h
     @internalComponent
     */
    {
    User::Panic(KRsConfiguratorServerName, aFault);
    }

/** Name of application.
*/
_LIT(KC32Start, "C32Start");
_LIT8(KAbbreviatedC32Start, "C32:");

/** Configuration file wildcard
*/
_LIT(KC32WildCard, "*.cmi");
/** Configuration process global settings
*/
_LIT(KC32StartIniFile, "c32start.ini");
/** Global section in configuration process settings
*/
_LIT8(KC32StartIniGlobalSection, "Global");

/** Optional global setting for the size of the MBuf pool, in bytes 
*/ 	 
_LIT8(KC32StartMBufPoolSize, "MBufPoolSize");
	
/** Optional (but highly recommended) setting to specify mbuf size allocation information; mbuf size, initial allocation, growth rate, auto growth.
*/
_LIT8(KC32StartMBufSizeAllocInfo, "MBufSizeAllocInfo");
/** Optional global override for the TestMode, in which the failure to reload a shutdown or
crashed critical module is logged, rather than causing the phone to reboot.
*/
_LIT8(KC32StartTestMode, "TestMode");

/** PANICs which can occur in this module.
*/
enum TC32StartPanics
     {
     /** There was a problem when wanting to load a module.
     */
     EPanicLoad,
     /** When starting, the system property that must be created
     by the configurator was already created.
     */
     EPanicPubSub,
     /** Unable to get value of the Rootserver Death Property.
     */
     EDeathWatch,
     /** Out of memory.
     */
     ENoMemory,
     /** SystemCritical module died and any attempt made at resurrection
	 failed. Category text will include start of module's name.
     */
     ECriticalModuleDeath
     };

/** Source of PANICs from this application.
*/
_LIT(KPanicSource, "C32Start");

CBindingRef::CBindingRef(RRootServ& aRootServer, CCommsProcessStarter& aStartConfigProcess,
							   const TRSBindingInfo& aBinding)
: CActive(EPriorityStandard),
  iRootServer(aRootServer),
  iStartProcess(aStartConfigProcess),
  iState(ENotBound),
  iBindInfo(aBinding)
/**
C'tor
*/
	{
	CActiveScheduler::Add(this);
	}

TInt CBindingRef::BindIfReady()
/** Bind a submodule if it is ready to be bound.
*/
	{
		
	if(iState==EBound || iState==EBindFailed)
		{
		// We don't attempt to reapply bindings once they've failed; the usual case is
		// that they've timed-out, so they may complete later and by re-requesting them
		// we'd risk generating unwanted multiple bindings.
		return KErrRSAlreadyBound;
		}

	CModuleRef* pModule = NULL;
	CModuleRef* qModule = NULL;
	TInt res = KErrNotReady; // assume the worst
	// While enumerating the CMI files it may be that a binding refers to a module not yet described; once
	// we start loading modules then all bindings will be fully defined. So if a module can't be found by
	// name we return ERSSubModuleUnknown to flag the configuration error
	qModule = iStartProcess.GetModule(FromModule().Module());
	if(!qModule)
		{
		__FLOG_1(_L8("BindIfReady(): first module %S (involved in binding) is unknown"), &FromModule().Module());
		
		return KErrRSSubModuleUnknown;
		}
	if(qModule->State()==ELoaded)
		{
		pModule = iStartProcess.GetModule(ToModule().Module());
		if(!pModule)
			{
			__FLOG_1(_L8("BindIfReady(): second module %S (involved in binding) is unknown"), &ToModule().Module());
		
			return KErrRSSubModuleUnknown;
			}
		if(pModule->State() == ELoaded && (iState !=EBinding || iState == ENotBound ))
			{
			// track bindings of either end
			pModule->IncrementBindings();
			qModule->IncrementBindings();
			iState = EBinding;
			SetPriority(EPriorityHigh);
			iBindInfo.iParams.iState1 = KErrNone;
			iBindInfo.iParams.iState2 = KErrNone;
			__FLOG_2(_L8("BindIfReady() - binding %S to %S"), &qModule->Name(), &pModule->Name());
			iRootServer.Bind(iStatus, iBindInfo);
			SetActive();
			res = KErrNone;
			}
		else if(pModule->State() == ELoadFailed)
			{
			// second named module in binding failed to load, SetState(EBindInComplete)
			SetState(EBindInComplete);
			__FLOG_2(_L8("BindIfReady(): second module %S (involved in binding) failed to load, first module %S loaded, SetState(EBindInComplete)"), &pModule->Name(), &qModule->Name());
			RDebug::Printf("WARNING: second module %S (involved in binding) failed to load, first module %S loaded, SetState(EBindInComplete)", &pModule->Name(), &qModule->Name());
			__DEBUGGER();
			}
		}
	else if(qModule->State() == ELoadFailed)
		{
		// first named module in binding failed to load, SetState(EBindInComplete)
		SetState(EBindInComplete);
		__FLOG_1(_L8("BindIfReady(): first named module %S in binding failed to load, SetState(EBindInComplete) "), &qModule->Name());
		RDebug::Printf("WARNING: first named module %S in binding failed to load, SetState(EBindInComplete)", &qModule->Name());
		__DEBUGGER();
		}

	if(iState == EBindInComplete)
		{
		// inComplete Binding, check if any of the modules is being OnDemand loaded (iRMessage != NULL)
		// if the module is tried for 'Load' complete message with KErrRSEitherModuleInBindingFailedToLoad
		if(qModule->State() != ENotLoaded)
			{
			qModule->MaybeCompleteMessage(KErrRSEitherModuleInBindingFailedToLoad);
			}
		pModule = iStartProcess.GetModule(ToModule().Module());
		if(pModule)
			{
			if(pModule->State() != ENotLoaded)
				{
				pModule->MaybeCompleteMessage(KErrRSEitherModuleInBindingFailedToLoad);
				}
			}
		}
	return res;
	}


void CBindingRef::RunL()
/** This function is invoked on completion of a bind
and is used to trigger further bindings by calling NotifyBindCompletion
in the controller object which will deal with correct behaviour.
*/
	{
	if(iStatus.Int()==KErrNone)
		{
		iState = EBound;
		__FLOG(_L8("CBindingRef completed with KErrNone."));
		UpdateCompletedBinding(iStatus.Int());
		}
	else
		{
		iState = EBindFailed;
		if(iStatus.Int()==KErrRSAlreadyBound)
			{
			__FLOG(_L8("CBindingRef completed with KErrRSAlreadyBound. This is usually OK if configurator was SoftBooted, e.g. during testing."));
			UpdateCompletedBinding(iStatus.Int());
			}
		else
			{
			__FLOG_1(_L8("CBindingRef completed with %d"), iStatus.Int());
			UpdateCompletedBinding(iStatus.Int());
			}
		}
	iStartProcess.NotifyBindCompletion(*this);
	}

CBindingRef::~CBindingRef()
/** Destructor of CBindingRef. Removes self from list of CBindingRef objects.
*/
	{
	Deque();
	}

void CBindingRef::DoCancel()
	{
	TRSSubModuleAddress subMod1(iBindInfo.iParams.iAddress1);
	TRSSubModuleAddress subMod2(iBindInfo.iParams.iAddress2);
	iRootServer.CancelBind(subMod1, subMod2);
	}

const TCFSubModuleAddress& CBindingRef::FromModule() const
/**
@return Reference to a Submodule address.
*/
	{
	return iBindInfo.iParams.iAddress1;
	}

const TCFSubModuleAddress& CBindingRef::ToModule() const
/**
@return Reference to a Submodule address.
*/
	{
	return iBindInfo.iParams.iAddress2;
	}

void CBindingRef::UpdateCompletedBinding(TInt aReturnValue)
	{
	CModuleRef* pModule = iStartProcess.GetModule(FromModule().Module());
	if(pModule)
		{
		pModule->UpdateCompletedBinding(aReturnValue);
		}
	pModule = iStartProcess.GetModule(ToModule().Module());
	if(pModule)
		{
		pModule->UpdateCompletedBinding(aReturnValue);
		}
	}

CModuleRef* CModuleRef::NewLC(RRootServ& aRootServer,
							  CCommsProcessStarter& aCommsProcess,
							  C32CmiData& aModuleCMIData)
/** Create instance of CModuleRef.
@param aRootServer The rootserver API.
@param aCommsProcess The controller object.
@param aModuleCMIData The object containing the ini data for the module.
@return A pointer to the new instance.
*/
	{
	CModuleRef* pModule=new(ELeave) CModuleRef(aRootServer, aCommsProcess);
	CleanupStack::PushL(pModule);
	pModule->ConstructL(aModuleCMIData);
	return pModule;
	}

CModuleRef::CModuleRef(RRootServ& aRootServer,CCommsProcessStarter& aCommsProcess)
	:	CActive(EPriorityStandard),
		iRootServer(aRootServer),
		iCommsProcess(aCommsProcess)
/** C'tor
@param aRootServer The rootserver API.
@param aCommsProcess The controller object.
*/
	{}

void CModuleRef::ConstructL(C32CmiData& aModuleCMIData)
/** Second phase construction, sets data members.
@param aModuleCMIData The object containing the ini data for the module
@leave KErrNoMemory
*/
	{
	if(aModuleCMIData.IsServer())
		{
		iType = EServerModule;
		}
	else
		{
		iType = EProviderModule;
		}
	iState = ENotLoaded;
	iScaledStartupState = aModuleCMIData.ScaledStartupState();
	iIniData = aModuleCMIData.IniDataL();
	iSystemCritical = aModuleCMIData.SystemCritical();
	iSystemCriticalAfterInit = aModuleCMIData.SystemCriticalAfterInit();
	iParams.iParams.iName.Copy(aModuleCMIData.Name());
	iParams.iParams.iFilename.Copy(aModuleCMIData.FileName());
	iParams.iParams.iPriority = aModuleCMIData.Priority();
	iParams.iParams.iStackSize = aModuleCMIData.StackSize();
	iParams.iParams.iHeapType = aModuleCMIData.HeapOption();
    iParams.iParams.iMinHeapSize = aModuleCMIData.MinHeapSize();
    iParams.iParams.iMaxHeapSize = aModuleCMIData.MaxHeapSize();
	iParams.iParams.iThreadFunctionOrdinal = aModuleCMIData.ThreadFuncOrdinal();
	iParams.iParams.iShareHeapWith.Copy(aModuleCMIData.SharedHeapName());
	iParams.iParams.iIsSticky = aModuleCMIData.IsSticky();
	iParams.iParams.iIsServer = aModuleCMIData.IsServer();

	// DEF040831 - Experience has demonstrated that test code frequently triggers
	// the demise of a server, deliberately or otherwise.
	//
	// CMI files are only suppressed by test code. In such a state we don't
	// panic about module deaths even when they're labelled as SystemCritical
	// since test code also manipulates these. On a real device only the
	// configurator (eg this current process) should ever be manipulating the
	// RootServer state
	//
	// DEF068506 - Determine whether to turn off this option (specified by the .CMI files) based on 'TestMode'
	// defined in the C32Start.ini. This is generally done by tests temporarily replacing the original .ini file at runtime.
	iParams.iParams.iControlFlags = aModuleCMIData.ControlFlags();
	if (iCommsProcess.InTestMode())
		{
		__FLOG(_L8("CModuleRef::ConstructL - TestMode set."));
		iParams.iParams.iControlFlags &= ~TRSStartModuleParamContainer::KCF_ThreadAsSystem;
		}

	iConfigParams.iGroupName.Copy(aModuleCMIData.GroupName());
	iConfigParams.iOnDemand = aModuleCMIData.IsOnDemand();


	CActiveScheduler::Add(this);
	}

CModuleRef::~CModuleRef()
/** D'tor. Dequeues the instance.
*/
	{
	delete iIniData;
	Deque();
	}

void CModuleRef::UpdateCompletedBinding(TInt aReturnValue)
	{
	IncrementCompletedBindings();
	MaybeCompleteMessage(aReturnValue);
	}

void CModuleRef::Load()
/** Attempts to load the module with the Rootserver.
*/
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicSource, EPanicLoad));
	__ASSERT_ALWAYS(iState==ENotLoaded || iState==ELoadFailed, User::Panic(KPanicSource, EPanicLoad));

	__FLOG_1(_L8("CModuleRef::LoadingCPM - %S"), &iParams.iParams.iName);

	iState = ELoading;

	SetPriority(EPriorityHigh);

	iRootServer.LoadCpm(iStatus, iParams, *iIniData);
	SetActive();
	}

TBool CModuleRef::Resurrect()
/** Called when the Root Server publishes the death of this module. In an extended-feature configurator this
may attempt to reload and rebind the module. This reference implementation simply returns that resurrection failed.
@return EFalse if the module could not be reloaded.
*/
	{
	return EFalse;
	}


TBool CModuleRef::operator==(const TCFModuleNameF& aModuleName) const
	{
   	return Name() == aModuleName;
 	}

void CModuleRef::RunL()
/** Calls NotifyLoadCompletion on CCommsProcessStarter.
*/
	{
	const TInt result=iStatus.Int();
	if(result==KErrNone)
		{
		if (iState == ELoading)
			{
			iState = ELoaded;
			if(!iHasBinding)
				{
				__FLOG_1(_L8("Comms Provider Module (%S) loaded successfully, has no bindings - Complete msg"), &iParams.iParams.iName);
				MaybeCompleteMessage(result);
				}
			iCommsProcess.NotifyLoadCompletion(*this);
			}
		else if(iState == EUnLoading)
			{
			iState = ENotLoaded;
			UnBindCpm();
			__FLOG_2(_L8("Comms ProviderModule (%S) unloaded successfully state (%d) - Complete msg"), &iParams.iParams.iName, iState);
			MaybeCompleteMessage(result);
			}
		}

	else
		{
		if(iState == ELoading)
			{
			__FLOG_2(_L8("Comms Provider Module load operation failed with (%d) - state (%d) - Complete msg"), result, iState);
			}
			else if(iState == EUnLoading)
				{
				__FLOG_2(_L8("Comms Provider Module unload operation failed with (%d) - state (%d) - Complete msg"), result, iState);
				}
		else
			{
			__FLOG_2(_L8("Comms Provider Module operation failed with (%d) - state (%d) - Complete msg"), result, iState);
			}
		MaybeCompleteMessage(result);
		if(result==KErrRSModuleAlreadyExist)
			{
			__FLOG_1(_L8("Comms Provider Module %S load failed with KErrRSModuleAlreadyExist. This is usually OK if configurator was SoftBooted, e.g. during testing."), &iParams.iParams.iName);
			iState = ELoaded;
			iCommsProcess.NotifyLoadCompletion(*this);
			}
		else
			{
			if (iState == ELoading)
				{
				__FLOG_2(_L8("Comms Provider Module %S NOT loaded. Error: %d"), &iParams.iParams.iName, iStatus.Int());
				iState = ELoadFailed;
				iCommsProcess.NotifyLoadCompletion(*this);
				}
			else if (iState == EUnLoading)
				{
				__FLOG_2(_L8("Comms Provider Module %S NOT Unloaded. Error: %d"), &iParams.iParams.iName, iStatus.Int());
				iState = ELoaded;
				}
			}
		}
	}

void CModuleRef::DoCancel()
	{
	iRootServer.CancelLoadCpm(iParams.iParams.iName);
	}

TInt CModuleRef::CompareScaledStartupState(const CModuleRef& aModule1, const CModuleRef& aModule2)
	{
	// Rely upon 2s-complement maths & the comparative closeness of values to avoid explicit comparisons
	return (TInt32) aModule1.ScaledStartupState() - (TInt32) aModule2.ScaledStartupState();
	}

void CModuleRef::MaybeCompleteMessage(TInt aReturnValue)
	{
	// only complete msg for module which has RMessage set on OnDemand Loading/Unloading NOT on
	// OnDemand property of module
	if (!iRMessage.IsNull())
		{
		if(iNumBindings == iCompletedBindings && iRMessage.Function() == CSLoadCpm)
			{
			iRMessage.Complete(aReturnValue);
			}
		else if (iRMessage.Function() == CSUnloadCpm)
			{
			iRMessage.Complete(aReturnValue);
			}
		}
	}

void CModuleRef::UnBindCpm()
	/**Sets the iState of CBindinRef to ENotBound once UnLoadCpm is successful
	*/
	{
	RPointerArray<CBindingRef>& bindings = iCommsProcess.GetBindingList();
	for(TInt i=0; i<bindings.Count(); i++)
		{
		if(Name() == bindings[i]->FromModule().Module())
			{
			bindings[i]->SetState(ENotBound);
			}
		}
	}

void CModuleRef::UnLoadCpm(const TCFModuleNameF& aModuleName,TRSUnLoadType aType)
	{
	iState = EUnLoading;

	SetPriority(EPriorityHigh);

	iRootServer.UnloadCpm(iStatus, aModuleName, aType);
	SetActive();
	}

void CModuleRef::CancelLoadCpm(const TCFModuleNameF& aModuleName)
	{
	iRootServer.CancelLoadCpm(aModuleName);
	}

void CModuleRef::CancelUnLoadCpm(const TCFModuleNameF& aModuleName)
	{
	iRootServer.CancelUnloadCpm(aModuleName);
	}

CDeathWatcher::CDeathWatcher(CCommsProcessStarter& aStartConfigProcess)
: CActive(EPriorityStandard),
  iStartProcess(aStartConfigProcess)
	{
	CActiveScheduler::Add(this);
	}

CDeathWatcher::~CDeathWatcher()
	{
	Cancel();
	iDeathProperty.Close();
	}

void CDeathWatcher::DoCancel()
	{
	iDeathProperty.Cancel();
	}

void CDeathWatcher::RunL()
	{
	TInt deathReason = iStatus.Int();
	iDeathProperty.Subscribe(iStatus);
	SetActive();
	TInt deathCount;
	TInt result = iDeathProperty.Get(deathCount);
	if(result != KErrNone)
		{
		__FLOG_1(_L8("Error %d upon Get of death property"), result);
		User::Panic(KC32Start, EDeathWatch);
		}
	iStartProcess.NotifySuddenDeath(deathCount, deathReason);
	}

CDeathWatcher* CDeathWatcher::NewL(CCommsProcessStarter& aStartConfigProcess)
	{
	CDeathWatcher* self = new(ELeave) CDeathWatcher(aStartConfigProcess);
	self->Construct();
	return self;
	}

void CDeathWatcher::Construct()
	{
	TInt result = iDeathProperty.Attach(KUidSystemCategory,KUidC32RootModuleDeathKey.iUid);
	if(result != KErrNone)
		{
		__FLOG_1(_L8("Error %d upon attach to death property"), result);
		User::Panic(KC32Start, EDeathWatch);
		}
	iDeathProperty.Subscribe(iStatus);
	SetActive();
	}


CStartupStateObserver::CStartupStateObserver(CCommsProcessStarter& aStarter)
: CDmDomain(KDmHierarchyIdStartup, KSM2GenMiddlewareDomain3),
  iStarter(aStarter)
	{
	}


CStartupStateObserver* CStartupStateObserver::NewL(CCommsProcessStarter& aStarter)
	{
	CStartupStateObserver* self = new(ELeave) CStartupStateObserver(aStarter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CStartupStateObserver::~CStartupStateObserver()
	{
	Cancel();
	}

void CStartupStateObserver::RunL()
	{
	iStarter.NotifyStartupStateChange();
	}

#ifdef SYMBIAN_ZERO_COPY_NETWORKING
CCommsProcessStarter::CCommsProcessStarter(RRootServ& aRootServer, RProperty& aProperty)
:
  iRootServer(aRootServer),
  iConfigurationProperty(aProperty)
#else
CCommsProcessStarter::CCommsProcessStarter(RRootServ& aRootServer, RProperty& aProperty)
:
  iRootServer(aRootServer),
  iConfigurationProperty(aProperty),
  iInitMBufPoolSize(KMBufDefaultHeapSize),
  iMaxMBufPoolSize(KMBufDefaultHeapSize)
#endif // SYMBIAN_ZERO_COPY_NETWORKING
/** C'tor. derived from CBase, hence member zero initialized
@param aRootServer The rootserver API.
@param aProperty The property through which to publish level information.
*/
    {
    }

CCommsProcessStarter* CCommsProcessStarter::NewLC(RRootServ& aRootServer, RProperty& aProperty)
	{
	CCommsProcessStarter* self = new(ELeave) CCommsProcessStarter(aRootServer, aProperty);
	CleanupStack::PushL(self);
	TRAPD(err, self->iStartupStateObserver = CStartupStateObserver::NewL(*self));
	if(err != KErrNone)
		{
		// The likely cause of this is that the system has no domains defined & in fact is running a legacy starter.
		__FLOG_1(_L8("CCommsProcessStarter - domain observer construction failed (err %d); going immediately to fully-booted state"), err);
		}
	return self;
	}

CCommsProcessStarter::~CCommsProcessStarter()
/** D'tor.
*/
    {
	iModules.ResetAndDestroy();
	iBindings.ResetAndDestroy();
	delete iDeathWatcher;
	delete iStartupStateObserver;
	iRootServer.Close();
    delete iConfiguratorServer;
    }

TBool CCommsProcessStarter::ConfigurationComplete() const
	{
	return iUnloadedModuleIdx >= iModules.Count();
	}

void CCommsProcessStarter::NotifySuddenDeath(TInt aDeathCount, TInt aDeathStatus)
/** Runs when the Rootserver publish the deathcount.
*/
	{
	if(aDeathCount > iDeathCount)
		{
		iDeathCount = aDeathCount;
		if(aDeathStatus != KErrServerTerminated)
			{
			__FLOG_1(_L8("CCommsProcessStarter::NotifySuddenDeath - DeathCount: %d"), iDeathCount);
			// At least one module just died - enumerate the current modules to determine
			// which have died, and for each of these pass control to its death handler,
			// which may attempt to reload it with some suitable algorithm to prevent
			// endless looping (etc). If the death handler doesn't resurrect it and it
			// was marked as SystemCritical then we have to reboot the RootServer. This
			// is done to clear up all process-owned artifacts (eg old thread names,
			// semaphores) which could be blocking a restart.
			TRSModuleInfo modInfo;
			for(int i = iModules.Count() - 1; i >= 0; --i)
				{
				CModuleRef* mod = iModules[i];
				if((mod->State() != ENotLoaded) && (iRootServer.GetModuleInfo(mod->Name(), modInfo) != KErrNone ||
					modInfo.iParams.iState == EZombie ||
					modInfo.iParams.iState == EDead))
				{
				if(!mod->Resurrect() && !InTestMode() && (mod->SystemCritical() || (mod->SystemCriticalAfterInit() && ConfigurationComplete())))
					{
						// if SystemCriticalAfterInit then will not reboot as problem could be due to corrupt system files
						// in c drive, hence best let the phone to continue and allow a master reset.

						// Here we do the RS reboot in the most brutal way possible, by rebooting
						// the whole OS.
						// In principle all we need is to kill and respawn the Root Server, since
						// that will usually release all conflicting resources. However it's possible that
						// other processes are sharing these too, in which case we'll end up having to do
						// a full reboot anyway. A worthwhile improvement could be falling all the way
						// out of InitConfiguratorL() with some "reboot" code to trigger us to
						// republish the progress to the base state, relaunch the RS, and restart the
						// whole load and configuration process, and only opt for a device reboot if
						// this second chance fails.
						// However this will add significant testing load (risk of endless loops) so
						// it is excluded from the scope of this reference implementation.
						_LIT16(KSystemCriticalMsg1, "%S: A thread specified as Critical, i.e. in its .cmi file, has exited and cannot be restarted.");
						_LIT8 (KSystemCriticalMsg2, "\t\"%s\" thread at fault. The system will now reboot!");
						_LIT8 (KSystemCriticalMsg3, "\t\"%s=1\", that would switch off this behaviour for testing, was not specified in file:");
						_LIT16(KSystemCriticalMsg4, "\t\t\"\\private\\101F7988\\%S\"");

						RDebug::Print (KSystemCriticalMsg1, &KC32Start);
						RDebug::Printf(reinterpret_cast<const char*>(KSystemCriticalMsg2().Ptr()), mod->Name().Ptr());
						RDebug::Printf(reinterpret_cast<const char*>(KSystemCriticalMsg3().Ptr()), KC32StartTestMode().Ptr());
						RDebug::Print (KSystemCriticalMsg4, &KC32StartIniFile);

						User::SetCritical(User::ESystemPermanent);
						TBuf8<KMaxExitCategoryName> panicCategory(KAbbreviatedC32Start);
						panicCategory.Append(mod->Name().Left(KMaxExitCategoryName - KAbbreviatedC32Start().Length()));
						TBuf<KMaxExitCategoryName> unicodePanicCategory;
						unicodePanicCategory.Copy(panicCategory);
						User::Panic(unicodePanicCategory, ECriticalModuleDeath);
						}
					}
				}

			}
// surely this is legacy of pre-pubsub?
//		else
//			{
//			__FLOG_1(_L8("CCommsProcessStarter::NotifySuddenDeath - Root Server has died. Error: %d."), iStatus.Int());
//			Shutdown();
//			}
		}
	else
		{
		__FLOG(_L8("CCommsProcessStarter::NotifySuddenDeath - Signalled to shutdown."));	//@todo - check whether the double pub of status really done by RS shutting down?
		Shutdown();
		}
	}

CModuleRef* CCommsProcessStarter::GetModule(const TCFModuleNameF& aModuleName) const
/**
@param aModule Name of a module to search for.
@return Pointer to an instance with matching name or NULL.
*/
	{
	for(TInt i=0; i<iModules.Count(); i++)
		{
		if(*iModules[i] == aModuleName)
			{
			return iModules[i];
			}
		}
	return NULL;
	}


void CCommsProcessStarter::NotifyLoadCompletion(CModuleRef& /*aModule*/)
/** Called by each CModuleRef when they have finished loading, and calls
bindIfReady() on them.
*/
	{
	iModulesLoading--;
	iModulesLoaded++;


	if(!iModulesLoading)	// if all at this level are done then can process the next level
		{
		__FLOG(_L8("no more pending at current level. Trying binds."));
		iBindCount = 0;
		TInt result;
		for(TInt i=0; i<iBindings.Count(); i++)
			{
			 result = iBindings[i]->BindIfReady();
			if(result == KErrNone)
				{
				iBindCount++;
				}
			else if(result == KErrRSSubModuleUnknown)
				{
				// This is a definite configuration error - the binding refers to a module
				// that wasn't described in any CMI file. We delete the binding and proceed -
				// although such a configuration error is panic worthy this is too likely to
				// make the phone useless
				__FLOG_STMT(
					{
					TCFFullModuleName __FullModName1;
					iBindings[i]->ToModule().Printable(__FullModName1);
					TCFFullModuleName __FullModName2;
					iBindings[i]->FromModule().Printable(__FullModName2);
					__FLOG_VA( (_L8("CONFIGURATION ERROR: binding %S - %S failed with ERSSubModuleUnknown"), &__FullModName1, &__FullModName2) );
					}
					);
				__DEBUGGER();
				iBindings.Remove(i);
				--i;	// balance the loop
				}
			}
		if(iBindCount==0)
			{
		    ContinueLoading();
			}
		}
	}

void CCommsProcessStarter::NotifyBindCompletion(CBindingRef& /*aBinding*/)
/** If this was the last binding, ascends to next level.
*/
	{
	iBindCount--;

	if(iBindCount==0)
		{
		ContinueLoading();
		}
	}



void CCommsProcessStarter::NotifyStartupStateChange()
	{
	__ASSERT_DEBUG(iStartupStateObserver, User::Panic(KSpecAssert_RootSrvc32start, 1));

	iNeedToAckLastState = ETrue;
	// Upper bound for the next tranche of scaled start-up states
	iScaledStartupState = ((iStartupStateObserver->GetState() + 1) * (TUint) KC32StartupStateScalingFactor) - 1;
	ContinueLoading();
	}

void CCommsProcessStarter::PrintSummary()
/** In debug mode this prints information of loaded modules and bindings.
*/
    {
#ifdef __FLOG_ACTIVE
	// Enumerate all modules
	TCFModuleNameF name;
	TRSIter modulePos;

	// NB: ABove we set eReset to true, the rootserver will set it to false
	// for next run, so we never ned to worry about it being a wrong value.
	// the loop just run to end of entries as if by magic.
	while(iRootServer.EnumerateModules(modulePos, name) == KErrNone)
		{
		TRSModuleInfo info;
		// Get more information
		TInt result = iRootServer.GetModuleInfo(name, info);
		if(result==KErrNone)
			{
			__FLOG_STMT(TBuf8<KMaxFileName> fnam;)
			__FLOG_STMT(fnam.Copy(info.iParams.iDll);)
			__FLOG_3(_L8("Module: \"%S\" from \"%S\". State: %d."),
				&name, &fnam, info.iParams.iState);

			// Enumerating submodules for this module
			TRSIter subModulePos;
			TCFSubModuleName subname;
			while(KErrNone==iRootServer.EnumerateSubModules(name, subModulePos, subname))
				{
				__FLOG_2(_L8("  Submodule: \"%S:%S\""), &name, &subname);
				// Enumerate bindings for this module
				TRSBindingInfo binding;
				TRSIter bindPos;
				TCFSubModuleAddress addr(name,subname);

				while(KErrNone==iRootServer.EnumerateBindings(addr, bindPos, binding))
					{
					if(subname == binding.iParams.iAddress1.SubModule())
					    {
						__FLOG_5(_L8("    Binding: \"%S:%S\", status(%d,%d), typ=%d."),
						    &binding.iParams.iAddress2.Module(),
							&binding.iParams.iAddress2.SubModule(),
							binding.iParams.iState1,
							binding.iParams.iState2,
							binding.iParams.iType);
					    }
					else
					    {
						__FLOG_5(_L8("    Binding: \"%S:%S\", status(%d,%d), typ=%d."),
						    &binding.iParams.iAddress1.Module(),
							&binding.iParams.iAddress1.SubModule(),
							binding.iParams.iState1,
							binding.iParams.iState2,
							binding.iParams.iType);
					    }
					}
				}
			}
		}
	__FLOG(_L8("*** End of configuration summary ***"));
#endif
    }

void CCommsProcessStarter::Shutdown()
	{
	__FLOG(_L8("Shutting down"));
	CActiveScheduler::Stop();
 	}


/** Start loading all modules with a scaled startup state below the
current level and which haven't yet been tried.
*/
void CCommsProcessStarter::ContinueLoading()
	{
	const TInt modulesCount = iModules.Count();
	__FLOG_6(_L8("ContinueLoading: Scaled start-up state 0x%x (%u), %d loads pending, %d binds pending, load pos %d of %d"), iScaledStartupState, iScaledStartupState, iModulesLoading, iBindCount, iUnloadedModuleIdx, modulesCount);

	if(iModulesLoading > 0 || iBindCount > 0)
		{
		return;	// as these complete they'll attempt to continue loading
		}

	if (iNeedToAckLastState)
		{
		iNeedToAckLastState = EFalse;
		iStartupStateObserver->RequestTransitionNotification();
		iStartupStateObserver->AcknowledgeLastState(KErrNone);
		if (iScaledStartupState >= KC32HighStartSequenceScaleBase)
			{
			//The SuDHSC has transitioned past our startup state
			//so we are not interested in state change information anymore
			delete iStartupStateObserver;
			iStartupStateObserver = NULL;
			__FLOG(_L8("iStartupStateObserver observation ended"));
			}
		}

	CModuleRef* mod = NULL;
	TUint32 firstModScaledState = 0;
	TBool allModsOnDemand = ETrue;	// all mods encountered in this loop have been skipped as OnDemand
	
	if(iUnloadedModuleIdx < modulesCount)
		{
		mod = iModules[iUnloadedModuleIdx];
		}
	while(iUnloadedModuleIdx < modulesCount && 
		((mod = iModules[iUnloadedModuleIdx])->ScaledStartupState() == firstModScaledState || allModsOnDemand))
		{
			// Not all modules load now, as there are modules with OnDemand=1
			// if ((module is NOT loaded) AND (EITHER (OnDemand module being loaded (RMessage set)) OR (boot-time module)))
			// msg will be set for module only if OnDemand loading is requested
			if(mod->State()==ENotLoaded && ((mod->IsOnDemand() && !mod->Message().IsNull()) || !mod->IsOnDemand()))
				{
				mod->Load();
				if(allModsOnDemand)
					{
					allModsOnDemand = EFalse;	// this one wasn't!
					firstModScaledState = mod->ScaledStartupState();	// can only simultaneously command the load of others with the exact same sequence
					}
				++iModulesLoading;		// this must be subtracted as each module load completes
				}
			++iUnloadedModuleIdx;
			}


	if(iUnloadedModuleIdx == modulesCount && allModsOnDemand)
		{
		// All currently loadable modules have loaded and bound, however unless no
		// OnDemand modules are present we keep listening for startup state changes 
		// as it's possible that an OnDemand load request will occur that still 
		// needs correct startup sequencing. However we're can declare ourself
		// "booted" and dump the summary
		
		if(iPublicationLevel < EPublishedComplete)
			{
			// set the sequence to max and publish it
			iPublicationLevel = EPublishedComplete;
			iConfigurationProperty.Set(EConfigurationComplete);

			}
#if defined _DEBUG || defined __FLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
		if(!iSummaryAlreadyPrinted)
			{
			iSummaryAlreadyPrinted = ETrue;
			
			_LIT8(KAllModulesStarted, "ESockSvr startup modules loaded! (%d of %d)");
			
			#ifdef _DEBUG
				RDebug::Printf(reinterpret_cast<const char*>(TPtrC8(KAllModulesStarted).Ptr()), iModulesLoaded, modulesCount);
			#endif
				
			// Log the modules startup message to UTrace if available.
			#ifdef SYMBIAN_TRACE_ENABLE
			    enum
			        {
			        KPrimaryFilter = 194, // server den
			        KMaxLogTextLength = 250
			        };

			    class TLogIgnoreOverflow8 : public TDes8Overflow
			        {
			        public:
			            void Overflow(TDes8& /*aDes*/) { }
			        };

				// Format the log text into a buffer for UTrace.
			    TBuf8<KMaxLogTextLength> buf;
		    	TLogIgnoreOverflow8 overflowHandler;
			    buf.AppendFormat(KAllModulesStarted, &overflowHandler, iModulesLoaded, modulesCount);

				UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
			#endif
			
			__FLOG_2(KAllModulesStarted, iModulesLoaded, modulesCount);
			
    	    PrintSummary();
			}
#endif
		}
	else if(iPublicationLevel < EPublishedCore && firstModScaledState >= KC32CoreComponentsStartedScaled)
		{
		iPublicationLevel = EPublishedCore;
		iConfigurationProperty.Set(ECoreComponentsStarted);

		}
	}


void CCommsProcessStarter::LoadOnDemandModule(CModuleRef& aModule)
	/** This is called when there is request for on demand load of a module.
	Compares the Scaled Startup State of OnDemand module with scaled state of module in the list (iModules) which
	is being currently loaded, if current scaled startup state has already passed wrt the scaled startup state of
	OnDemand requested module, this sets the iUnloadedModuleIdx to index (in iModules) of MIN scaled startup
	state of current and OnDemand requested module
	*/
	{
	__FLOG_1(_L8("CCommsProcessStarter::LoadOnDemandModule(%S)"), &aModule.Name());

	const TUint32 modScaledState = aModule.ScaledStartupState();//Scaled start up state of requested module
	if(iUnloadedModuleIdx!=0)
		{
		const TUint32 currentModScaledState = iModules[iUnloadedModuleIdx-1]->ScaledStartupState();//Scaled start up state of last module loaded.

		if(currentModScaledState >= modScaledState)
			{
			TInt minIndex;
			for(minIndex=0;minIndex<iUnloadedModuleIdx;minIndex++)
				{
				// since iModules are added in order of scaled startup state, finding index this way
				// will not skip any to-be-loaded modules
				if(iModules[minIndex] == &aModule)
					{
					break;
					}
				}
			iUnloadedModuleIdx = minIndex;	// set the index
			}
		}

	ContinueLoading();

	}

// private MBufSizeAllocInfo defaults
// - 128 mbuf size is mandatory since their exists some (poorly writen) legacy code that assumes the existance of mbufs with this size
static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_InitialAllocation = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;

void CCommsProcessStarter::ReadConfigAndStartAllL(const TDesC& aCMISuppressions)
/** Initiate the loading of comms server modules and
comms provider modules.  We publish a property with
the state of configuration we have reached to allow waiting
StartC32() to complete when we indicate we are finished by
publishing CONFIGURED to our property.
@param aCMISuppressions List of .cmi files to ignore should they exist.
*/
	{
	__FLOG(_L8("CBindingRef::ReadConfigAndStartAllL"));

	// start the configurator server
	TRAPD(err, iConfiguratorServer = CRsConfiguratorServer::NewL(*this));
	__FLOG_1(_L8(" Create ConfiguratorServer returned %d."), err);
	__ASSERT_ALWAYS(err == KErrNone || err == KErrAlreadyExists , Fault(EPanicConfigServer));

	TAutoClose<RFs> fs;
	TInt result;
	if(KErrNone!=(result=fs.iObj.Connect()))
		{
		__FLOG(_L8("Unable to connect to the File Server"));
		User::Leave(result);
		}
	fs.PushL();

	TBuf<KMaxPath> privatePath;
	User::LeaveIfError(fs.iObj.PrivatePath(privatePath));

	__FLOG_STMT(TBuf8<KMaxPath> fpath;)
	__FLOG_STMT(fpath.Copy(privatePath);)
	__FLOG_1(_L8("Using .CMI search path: %S"), &fpath);

	// Find the (optional) c32start.ini configuration file across all drive
	TFindFile ff(fs.iObj);
	CDir* iniFileList;
	TParse fullentry;
	result = ff.FindWildByDir(KC32StartIniFile, privatePath, iniFileList);
	CleanupStack::PushL(iniFileList);
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
	TInt initTotalPoolSize = 0;
	TInt overAllCeiling = 0;
#endif
	RArray<TRSAddMBufAllocInfoContainer> mBufAllocSizeInfo;
	if (KErrNone == result)
		{
		// Read global configuration options
		fullentry.Set((*iniFileList)[0].iName, &ff.File(), NULL);
		C32ParseIniFile* globalConfigFile = C32ParseIniFile::NewL(fullentry.FullName(), fs);

		TPtrC8 mBufCfg;
		// read mbuf pool size

		globalConfigFile->FindVar(KC32StartIniGlobalSection, KC32StartMBufPoolSize, mBufCfg);
#ifdef  SYMBIAN_ZERO_COPY_NETWORKING		
      	SplitInt(mBufCfg, initTotalPoolSize);
        SplitInt(mBufCfg, overAllCeiling);
#else
 		SplitInt(mBufCfg, iInitMBufPoolSize);
  		SplitInt(mBufCfg, iMaxMBufPoolSize);
#endif  // SYMBIAN_ZERO_COPY_NETWORKING      
		// read mbuf size alloc info entries
		TRSAddMBufAllocInfoContainer allocInfo;
		for (TInt i = 1 ;; i++)	// the first missing key enumeration (starting from 1) aborts the loop
			{
			if (globalConfigFile->FindVar(KC32StartIniGlobalSection, KC32StartMBufSizeAllocInfo, mBufCfg, i) == EFalse)
				break;
			if (SplitUint(mBufCfg, allocInfo.iMBufSize)  || SplitUint(mBufCfg, allocInfo.iInitialAllocation) ||
				SplitUint(mBufCfg, allocInfo.iMinGrowth) || SplitUint(mBufCfg, allocInfo.iGrowthThreshold))
				{
				__FLOG_1(_L("CCommsProcessStarter::ReadConfigAndStartAllL() Error! - failed to parse MBufSizeAllocInfo=%d"), i);
				continue;
				}
#ifdef  SYMBIAN_ZERO_COPY_NETWORKING				
			allocInfo.iPoolCeiling = 0;
			(void)SplitUint(mBufCfg, allocInfo.iPoolCeiling); // If it is not set we will set the value based on the overall pool ceiling
#endif // SYMBIAN_ZERO_COPY_NETWORKING			
			mBufAllocSizeInfo.AppendL(allocInfo);
			}
		
		
		
		// read test mode flag
		globalConfigFile->FindVar(KC32StartIniGlobalSection, KC32StartTestMode, iTestMode);

		delete globalConfigFile;
		}
#ifdef  SYMBIAN_ZERO_COPY_NETWORKING			
	if(overAllCeiling > 0)
		{
		TBool needPoolCeilingAssign = EFalse;
		// Check we have the pool ceiling value for every entry
		// If pool ceiling is missing in some entry we will align them based on the overall pool size
		for(TInt i = 0; i < mBufAllocSizeInfo.Count(); ++i)
			{
			if(mBufAllocSizeInfo[i].iPoolCeiling == 0)	
				{
				__FLOG_0(_L("CCommsProcessStarter::ReadConfigAndStartAllL() Error! - One of the pool ceiling value is wrong. Initializing to default"));
				needPoolCeilingAssign = ETrue;
				break;					
				}
			}
		if(needPoolCeilingAssign)
			{
			TInt poolSize = overAllCeiling / mBufAllocSizeInfo.Count();
			for(TInt i = 0; i < mBufAllocSizeInfo.Count(); ++i)
				{
				mBufAllocSizeInfo[i].iPoolCeiling = poolSize/mBufAllocSizeInfo[i].iMBufSize;
				}
			}
		}
#endif // SYMBIAN_ZERO_COPY_NETWORKING		
	
	
	// if no valid mbuf size alloc info entry, then assign a default value
	if (mBufAllocSizeInfo.Count() == 0)
		{
		__FLOG(_L("CCommsProcessStarter::ReadConfigAndStartAllL() Warning! - no valid MBufSizeAllocInfoX entry found, assigning default value instead"));
		TRSAddMBufAllocInfoContainer allocInfo;
		allocInfo.iMBufSize = KMBuf_MBufSize;
		allocInfo.iInitialAllocation = KMBuf_InitialAllocation;
		allocInfo.iMinGrowth = KMBuf_MinGrowth;
		allocInfo.iGrowthThreshold = KMBuf_GrowthThreshold;
#ifdef  SYMBIAN_ZERO_COPY_NETWORKING		
		allocInfo.iPoolCeiling = overAllCeiling/KMBuf_MBufSize;
#endif // SYMBIAN_ZERO_COPY_NETWORKING		
		mBufAllocSizeInfo.AppendL(allocInfo);
		}

	CleanupStack::PopAndDestroy(iniFileList);
	__FLOG(iTestMode? _L("In test mode: termination of system-critical CPMs tolerated") : _L("In normal mode") );

	// Generate a list of config files (*.CMI in the *\system\data directory, ie searches all drives
	// available drives, ending with Z:)
	CommsFW::COwnEntryList* cmiList = new(ELeave) CommsFW::COwnEntryList(6);
	CleanupStack::PushL(cmiList);

#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
	cmiList->UniqueWildScanAcrossDrivesL(privatePath, KC32WildCard);		// masking of CMI files on basis of module names Y->A:Z
#else
	cmiList->WildScanAcrossDrivesL(privatePath, KC32WildCard);		// masking of CMI files on basis of module names Y->A:Z
#endif

	if(cmiList->Count()==0)
		{
		__FLOG(_L8("Unable find any CMI files"));
		}

	// ************************************* checks to ensure we don;t get race conditions
	TCFModuleNameF name;
	TRSIter modulePos;
	TInt totalModules = 0;
	while((result = iRootServer.EnumerateModules(modulePos, name)) == KErrNone)
		{
		++totalModules;
		}
	__ASSERT_DEBUG(result == KErrEof, User::Panic(KSpecAssert_RootSrvc32start, 2));
	if(totalModules == cmiList->Count())
		{
		// we found the same number of servers running in rootserver as
		// we expected to load, so rootserver has already been configured
		__FLOG(_L8("root server found to be configured already"));
		// we must set the property, just to put us into the right state
		iConfigurationProperty.Set(EConfigurationComplete);
		CleanupStack::PopAndDestroy(cmiList);
		fs.Pop();
		return;	// because rootserver is already configured
		}
	// ********************************** end of starting check

	// before starting load get the death watcher ready to catch any collateral damage
	iDeathWatcher = CDeathWatcher::NewL(*this);
#ifndef SYMBIAN_ZERO_COPY_NETWORKING
  	// Configure the MBuf pool size
  	__FLOG_2(_L8("Setting MBufPoolSize(Init:%d Max:%d)"), iInitMBufPoolSize, iMaxMBufPoolSize);
  	result = iRootServer.SetMBufPoolSize(iInitMBufPoolSize, iMaxMBufPoolSize);
  	if(result==KErrInUse)
  		{
  		__FLOG(_L8("SetMBufPoolSize failed with KErrInUse. This is usually OK if configurator was SoftBooted, e.g. during testing."));
  		}
  	else if(result != KErrNone)
  		{
  		__FLOG_1(_L8("SetMBufPoolSize failed with err %d"), result);
  		}
#endif // SYMBIAN_ZERO_COPY_NETWORKING

	// configure the mbuf size allocation info - via a message between the configurator to rootserver
    for (int i = 0; i < mBufAllocSizeInfo.Count(); i++)
		{
		__FLOG_4(_L8("Setting MBufSizeAllocInfo; mbufSize=%d initialAlloc=%d minGrowth=%d growthTrigger=%d"),
			mBufAllocSizeInfo[i].iMBufSize, mBufAllocSizeInfo[i].iInitialAllocation, mBufAllocSizeInfo[i].iMinGrowth,
			mBufAllocSizeInfo[i].iGrowthThreshold);
		result = iRootServer.AddMBufAllocInfo(mBufAllocSizeInfo[i]);
		if (result != KErrNone)
			{
			__FLOG_1(_L8("AddMBufAllocInfo failed with err %d"), result);
			}
		}

	// set the mbuf size allocation commit flag - via a message between the configurator to rootserver
//	result = iRootServer.SetMBufSizeAllocCommit();
//	if (result != KErrNone)
//		{
//		__FLOG_1(_L8("SetMBufSizeAllocCommit failed with err %d"), result);
//		}

	__FLOG_STMT(TBuf<KMaxFileName> nam;)
	C32CmiData* pIniData;
	CModuleRef* pModule;
	TRSBindingInfo binding;
	TLinearOrder<CModuleRef> scaledStartupStateSort(CModuleRef::CompareScaledStartupState);

	// We have a list of filenames, process them...
	for (TInt i=0; i<cmiList->Count(); i++)
		{
		const TDesC& cmiEntry = (*cmiList)[i].iName;
		fullentry.Set(cmiEntry, &ff.File(), NULL);

		// Test whether it's a suppressed file
		const TPtrC ptrc = fullentry.NameAndExt();
		TInt pos = aCMISuppressions.FindF(ptrc);
		if(pos >= 0)
			{
			__FLOG_1(_L("Skipping .CMI file %S due to explicit suppression."), &cmiEntry);
			continue;
			}
		__FLOG_STMT(nam.Copy(fullentry.FullName());)
		__FLOG_1(_L("Processing .CMI file %S."), &nam);

			TRAP(result,
			 pIniData = C32CmiData::NewL(fullentry.FullName(), fs);
			 CleanupStack::PushL(pIniData);
			 pModule=CModuleRef::NewLC(iRootServer, *this, *pIniData);
			 if(KErrNone!=(result=iModules.InsertInOrderAllowRepeats(pModule, scaledStartupStateSort)))
				 {
				 __FLOG_2(_L("Unable to add %S configuration to task list, error %d."), &nam, result);
				 User::Leave(result);
				 }
			__FLOG_1(_L8("...ModuleName=\"%S\""), &pModule->Name());
			 CleanupStack::Pop(pModule);

			 while(pIniData->NextBindingL(binding))
				 {
				 CBindingRef* pBinding = new(ELeave) CBindingRef(iRootServer, *this, binding);
				 if(KErrNone!=(result=iBindings.Append(pBinding)))
					 {
					 __FLOG_1(_L8("Unable to add binding to task list, error %d."), result);
					 delete pBinding;
					 User::Leave(result);
					 }
				 }
			 CleanupStack::PopAndDestroy(pIniData);
 			); //)TRAP	//lint !e429	// custody taken of pBinding

		__FLOG_STMT(
			if(KErrNone!=result)
				{
				__FLOG_2(_L("Unable to use .CMI file %S, error %d."), &nam, result);
				__DEBUGGER();
				}
			);
		}
#ifdef _DEBUG
	// Duplicate module configurations can arise from incomplete edits, backup copies, or having a .CMI present on
	// multiple drive letters. Although these are reported as "load failed with KErrRSModuleAlreadyExist" this is
	// too subtle to help most users understand why a module loads with the wrong configuration, etc. Hence this
	// UDEB-only sanity check & diagnostic slap.
	for(TInt i = iModules.Count() - 1; i >= 1; --i)	// modules are sorted by start order so no optimised search available
		{
		const CModuleRef& mod1(*iModules[i]);
		for(TInt j = i - 1; j >= 0; --j)
			{
			if(mod1.Name() == iModules[j]->Name())
				{
				__FLOG_1(_L8("Duplicate \"%S\" module found. Only the first will successfully load (if any do). To find the files containing the duplicates search this log file for \"ModuleName=\""), &mod1.Name() );
				// You hit this breakpoint because two or more .CMI files described a module with the same name.
				// Configurations with duplicate modules can arise from incomplete edits, backup copies, or having a .CMI present on
				// multiple drive letters. Enable the "C32Start" category in "c:\logs\commsdbg.ini" and re-run, then study the output
				// in "c:\logs\log.txt" (path can be overridden through the .ini file). If you can't find commsdbg.ini look for advice
				// on enabling logging through the "Comms Debug Utility"
				__DEBUGGER();
				continue;	// gratuitous, just to try to ensure the execution point in the debugger is close to the above breakpoint instruction
				}
			}
		}
#endif
	CleanupStack::PopAndDestroy(cmiList);
	fs.Pop();

	iModulesLoading = 0;
	iUnloadedModuleIdx = 0;
	iPublicationLevel = EUnpublished;
	if(iStartupStateObserver)
		{
		// Prime the change notification pump
		NotifyStartupStateChange();
		}
	else
		{
		// No start-up observation possible, so fake the system fully-booted state
		iScaledStartupState = 0x7FFFFFFF;
		}

#ifdef __FLOG_ACTIVE
	// Log summary of CMI files in their startup order including the simple states they tie to.
	_LIT( KCMISummaryLogMsg, "CCommsProcessStarter::ReadConfigAndStartAllL: CMI Load Order" );
	__FLOG_0( KCMISummaryLogMsg );
	RDebug::Print( KCMISummaryLogMsg );

	// CMI names are ASCII so we have to use 8-bit.
	_LIT8( KCMISummaryLogEntry, "\t%S\t[SystemCritical(%d), SystemCriticalAfterInit(%d), ScaledStartupState(%08x), GroupName(%S), IsOnDemand(%d)]" );
	for( TInt i = 0; i < iModules.Count(); i ++ )
		{
		const CModuleRef& mod = *iModules[i];

		// Generate the log entry using Format - RDebug::Print does not seem to cope with ASCII strings properly.
		TBuf8<sizeof( KCMISummaryLogEntry ) + KMaxFileName + 8> entry;
		entry.Format( KCMISummaryLogEntry, &mod.Name(), mod.SystemCritical(), mod.SystemCriticalAfterInit(), mod.ScaledStartupState()
		, &mod.GroupName(), mod.IsOnDemand()
		);

		__FLOG_0( entry );
		RDebug::RawPrint( entry );
		}
#endif

	RDebug::Printf("\n");

	ContinueLoading();
	}

// split/extract the first integer from the descriptor, the descriptor is incremented afterwards
TBool CCommsProcessStarter::SplitInt(TPtrC8 &aDes, TInt& aValue) const
	{
	// locate the start of number
	TLex8 lex(aDes);
	while (!lex.Eos() &&!lex.Peek().IsDigit())
		lex.Inc();

	// extract the number & increment descriptor
	TInt result = lex.Val(aValue);
	aDes.Set(lex.Remainder());

	return result;
	}

// split/extract the first integer from the descriptor, the descriptor is incremented afterwards
TBool CCommsProcessStarter::SplitUint(TPtrC8 &aDes, TUint& aValue) const
	{
	// locate the start of number
	TLex8 lex(aDes);
	while (!lex.Eos() &&!lex.Peek().IsDigit())
		lex.Inc();

	// extract the number & increment descriptor
	TInt result = lex.Val(aValue);
	aDes.Set(lex.Remainder());

	return result;
	}

static TInt InitConfiguratorL(CCommsProcessStarter*& pCCommsStartProcess, RRootServ& aRootServer, RProperty& aProperty, const TDesC& aCMISuppressions)
/** Start the Rootserver and initiates the process of loading modules and bindings.
*/
    {
	// There are 3 normal possibilities:
	//	(1) C32EXE process is not running - we must launch it, wait for rendezvous, then connect
	//	(2) C32EXE process is running but has yet to start the RS server - since we can't reliably distinguish
	//		this from #1 we launch another and wait for it to exit when it can't start the server. At this
	//		point we can try connecting to it.
	//	(3) RS is running and connects us.
	// After this we're connected. We may not be the only instance of C32START doing this so we ask the RS
	// if we are "the configurator" (thanks to its protected server it's a true singleton, so we can trust
	// its answer to be authoratative). If we aren't then we exit; anyone who launched us can now watch
	// the KUidC32StartPropertyKey to monitor the "the configurator's" progress.

    TBool alreadyRunning = aRootServer.Connect() == KErrNone;

    //If we couldnt connect, we need to launch c32exe.exe
    if(!alreadyRunning)
		{
		__FLOG(_L8("As expected, Root Server has to be started by us"));
		RProcess rootServerExe;
		TInt result = rootServerExe.Create(KRootServerExe, KNullDesC);
		if (result == KErrNone)
			{
			__FLOG(_L8("Starting RS/c32exe.exe."));
			TRequestStatus status;
			rootServerExe.SetPriority(KRootServerDefaultPriority);
			rootServerExe.Rendezvous(status);
			rootServerExe.Resume();
			__FLOG(_L8("wait for RS to complete init."));

			User::WaitForRequest(status);
			rootServerExe.Close();
			}
		else
			{
			__FLOG_1(_L8("Launching RS had error %d"), result);
			}

		__FLOG(_L8("RS has completed init and is ready for connection."));

	    result = aRootServer.Connect();
	    if(result!=KErrNone)
			{
			__FLOG(_L8("Unable to connect to Root Server!"));
			User::Leave(result);
			}
		else
			{
			__FLOG(_L8("Successfully connected to Root Server."));
			}
		}
	else
		{
		}

	// Now we have RS connection, ask it whether we're "the configurator". If not then we bow
	// out immediately; anyone who launched us can now be watching the starting publications
	// from "The Configurator"
	if(!aRootServer.IsCallerConfigurationAuthority())
		{
		__FLOG(_L8("RootServer reports existing configurator; terminating to let it do the real work"));
		User::Leave(KErrAlreadyExists);
		}

	pCCommsStartProcess = CCommsProcessStarter::NewLC(aRootServer, aProperty);
	__FLOG_STMT(
		if(alreadyRunning)
			{
			__FLOG(_L8("RootServer already running! Current config:"));			
			pCCommsStartProcess->PrintSummary();
			}
		);

    pCCommsStartProcess->ReadConfigAndStartAllL(aCMISuppressions);
	CleanupStack::Pop(pCCommsStartProcess);
	return KErrNone;
    }

TInt E32Main()
/** Life starts here!
*/
    {

	// Renaming is very cosmetic so we ignore the result
    (void)RThread::RenameMe(KC32Start);
	(void)RProcess::RenameMe(KC32Start);

    TInt result;
	RProperty configurationProperty;
	// publish initial property state to indicate we have started configuration

	TSecurityPolicy readPolicy(ECapability_None);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	result = configurationProperty.Define(KUidSystemCategory, KUidC32StartPropertyKey.iUid, RProperty::EInt, readPolicy, writePolicy);
	__ASSERT_ALWAYS(result == KErrNone || result == KErrAlreadyExists, User::Panic(KPanicSource, EPanicPubSub));
	result = configurationProperty.Attach(KUidSystemCategory, KUidC32StartPropertyKey.iUid);
	__ASSERT_ALWAYS(result == KErrNone, User::Panic(KPanicSource, EPanicPubSub));

	__UHEAP_MARK;

	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG(_L8("| - - - - - - - - - - C32Start New - - - - - - - - - - - - - |"));

	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if(!cleanupStack)
		{
		__FLOG(_L8("Out Of Memory initializing"));
		return KErrNoMemory;
		}
	CActiveScheduler* pA = new CActiveScheduler;
	__ASSERT_ALWAYS(pA != NULL, User::Panic(KPanicSource, ENoMemory));
	CActiveScheduler::Install(pA);

	// Fetch the list of CMI suppressions
    TInt suppressionBufLength;
	suppressionBufLength = User::CommandLineLength();
	HBufC* suppressionBuf = HBufC::NewMax(suppressionBufLength);
	__ASSERT_ALWAYS(suppressionBuf != NULL, User::Panic(KPanicSource, ENoMemory));
	TPtr suppressions = suppressionBuf->Des();
	User::CommandLine(suppressions);

	RRootServ rootServerSession;
	CCommsProcessStarter* pCCommsStartProcess = NULL;

	// we indicate we are running and busy configuring so caller can start watching the progress property
	RProcess::Rendezvous(KErrNone);	// we indicate we are done as property is important to caller!

    TRAP(result, result = InitConfiguratorL(pCCommsStartProcess, rootServerSession, configurationProperty, suppressions));

	delete suppressionBuf;
	if(result==KErrNone)
		{
		CActiveScheduler::Start();
		}
	else if(result != KErrAlreadyExists)
		{
		__FLOG_1(_L8("InitConfiguratorL error %d"), result);
		User::Panic(KPanicSource, result);	// as specified in the design we panic here not in CSCP
		}

	delete pCCommsStartProcess;
	CActiveScheduler::Install(NULL);
	delete pA;
	delete cleanupStack;
	configurationProperty.Close();	

	__FLOG(_L8("| - - - - - - - - C32Start Finished - - - - - - - - - - |"));
	__FLOG_CLOSE;

	__UHEAP_MARKEND;

	return KErrNone;
	}
