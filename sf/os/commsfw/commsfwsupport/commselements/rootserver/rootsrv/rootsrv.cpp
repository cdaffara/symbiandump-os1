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

/**
 @file
 @internalComponent
*/

#include "rsstd.h"
#include "bindmgr.h"
#include <cflog.h>
#include <e32uid.h>
#include <e32debug.h>



#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemRootServerrt, "ElemRootServerrt");
#endif

//TODO:
//REMOVE CFLOGGER AS THIS WILL PANIC FOR MULTIMEDIA!!


__CFLOG_STMT(_LIT8(KLogSubSysRS, "RootServer");) // subsystem name

static void Fault(TRootServerPanic aFault)
    /** Panic the server
     @param aFault Fault code as defined in cs_std.h
     @internalComponent
     */
    {
    __CFLOG_1(KLogSubSysRS, KLogCode, _L8("Fault %d."), aFault);
    User::Panic(RThread().Name(), aFault);
    }


EXPORT_C CRootServer::CRootServer(TInt aPriority, const TPolicy& aPolicy)
	: CPolicyServer(aPriority,aPolicy,ESharableSessions)
	, iModules(_FOFF(CCommsProviderModule, iLink))
	{}

EXPORT_C CRootServer::~CRootServer()
	{
	__CFLOG_0(KLogSubSysRS, KLogCode, _L8("CRootServer::~CRootServer()" ));
	__ASSERT_DEBUG(iModules.IsEmpty(), User::Panic(KSpecAssert_ElemRootServerrt, 1)); //ASSERTS it the queue is not empty
	delete iBindMgr;
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	delete iPerfStore;
#endif
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
	}

TInt CRootServer::Shutdown()
	/** Shutdown RootServer
	 Modules should have already been unloaded so all we do is stop
	 active scheduler and delete self.
	 @internalComponent
	 @post The object is deleted
	 */
	{
	if(CpmCount()!=0)
		{
		__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServer::Shutdown() return KErrRSModulesStillRunning"));
		return KErrRSModulesStillRunning;
		}

	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServer::Shutdown() - Stopping active scheduler"));
	CActiveScheduler::Stop();
	return KErrNone;
	}

EXPORT_C void CRootServer::ReceivedMessage(const TCFModuleName& /*aModule*/, const TCFMessage& /*aNewMsg*/)
	{
	__CFLOG(KLogSubSysRS, KLogCode, _L8("CRootServer::ReceivedMessage()"));
    }

EXPORT_C void CRootServer::LazyLoadL()
    {
    //default do nothing
    }

EXPORT_C TBool CRootServer::IsCallerConfigurator(const CRootServerSession* /*aSession*/, const RMessage2& /*aMessage*/)
	{
	return EFalse;  // not RootServer Configurator by default
	}

EXPORT_C void CRootServer::DisconnectSession(const CRootServerSession* /*aSession*/)
	{
	//default do nothing
	}

RHeap* CRootServer::FindHeap(const TCFModuleNameF& aName)
    /**
     Find a heap pointer by name.
     Search the cpms for a mathching name and return
     the heap pointer if found, else NULL.
     @param aName The name whose heap pointer is requested.
     @return The heap pointer for the module named or NULL
	 @internalComponent
     */
	{
	CCommsProviderModule* pModule;
    TCFModuleNameF name1;
    TCFModuleNameF name2;
    name1 = aName;
    name1.UpperCase();
	__CFLOG_VAR( (KLogSubSysRS, KLogCode, _L8("CRootServer::FindHeap - Looking for %S"), &name1) );

    // First check whether the name already exists
 	TDblQueIter<CCommsProviderModule> cpmIter(iModules);
 	cpmIter.SetToFirst();
    while((pModule=cpmIter++)!=NULL)
        {
        name2 = pModule->GetName();
        name2.UpperCase();
		__CFLOG_VAR( (KLogSubSysRS, KLogCode, _L8("CRootServer::FindHeap - Iterating: %S"), &name2) );
        if(name2 == name1)
            {
            return pModule->GetHeapPtr();
            }
        }
    return NULL;
	}


CCommsProviderModule* CRootServer::FindCpm(const TCFModuleNameF& aName)
    /** Find a comms provider module by name.
     Search the TDblQue list for a cpm with the matching name
     if found return a pointer, otherwise return NULL.
     @param aName The name of the cpm to be found.
     @return The cpm found or NULL
	 @internalComponent
     */
    {
    CCommsProviderModule* pModule;

    // First check whether the name already exists
    TDblQueIter<CCommsProviderModule> cpmIter(iModules);
    cpmIter.SetToFirst();
    while((pModule=cpmIter++)!=NULL)
        {
        if(aName.CompareF(pModule->GetName())==0)
        	{
        	return pModule;
        	}
        }
    return NULL;
    }

TInt CRootServer::GetModuleInfo(const TCFModuleNameF& aName, TRSModuleInfo& aInfo)
    /** Return current held information of a module.
     Search the cpm list for the name.
     If found retrieve the appropriate information into aInfo
     @internalComponent
     @param aName The name of the module.
     @param aInfo class to hold the information.
     @return Error code.
     */
    {
    CCommsProviderModule* pModule;
    if((pModule = FindCpm(aName)) != NULL)
		{
  		TInt numSubs = 0;
		TInt result = iBindMgr->NumSubModules(aName, numSubs);
		// If the module has not yet rendezvoused then the bind mgr will not know of it
		if(result == KErrNone ||
				(result == KErrRSModuleUnknown && pModule->GetState() == EStarting))
			{
			aInfo.iParams.iName = pModule->GetName();
			aInfo.iParams.iDll = pModule->GetDll();
			aInfo.iParams.iState = pModule->GetState();
			aInfo.iParams.iNumSubModules = numSubs;
			return KErrNone;
			}
		else
			{
			__CFLOG_2(KLogSubSysRS, KLogCode, _L8("CRootServer::GetModuleInfo - error %d getting %S submodules"), result, &aName);
			return KErrRSModuleUnknown;
			}
		}
	__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServer::GetModuleInfo - %S not found"), &aName);
	return KErrRSModuleUnknown;
    }

CCommsProviderModule* CRootServer::CreateCpmL(const TRSStartModuleParams& aParams, HBufC8* aIniData)
    /** Create Comms Provider Module and put it in list.
     Refuses to create module if one with the same name
     already exists in list of cpms.
     @internalComponent
     @param aParams The new module info.
     @param aIniData Module specific ini data.
     @return Pointer for new module.
     */
    {
    if(FindCpm(static_cast<const TCFModuleNameF&>(aParams.iParams.iName))!=NULL)
    	{
	 	__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServer::CreateCpmL - %S already exists"), &aParams.iParams.iName);
		User::Leave(KErrRSModuleAlreadyExist);
    	}

    CCommsProviderModule* pModule = (*iCommsProviderModuleCtorL)(this, aParams, aIniData);
    iModules.AddLast(*pModule);
    return pModule;
    }

EXPORT_C void CRootServer::SuddenDeath(TInt /*aReason*/)
    /** Sudden Death has happened, notify all listeners.
     Called by the modules upon death. Publish
     property so that all interested party's can
     take action.
     @internalComponent
     @param aReason The reason of the Sudden Death.
     */
    {
	TInt result = iDeathProperty.Set(KUidSystemCategory, iDeathKey.iUid, ++iDeathCount);
	if( result != KErrNone)
		{
		__CFLOG_VAR((KLogSubSysRS, KLogCode, _L8("CRootServer::SuddenDeath() - error %d setting death property"), result));
		User::Panic(RThread().Name(), KRSPubSubFault);
		}
    }


TInt CRootServer::Forward(const TCFModuleNameF& aModule, const RMessage2& aMessage)
	{
	TRAPD( res, iBindMgr->SendL( aModule, aMessage ) );
	return res;
	}


CBindManager* CRootServer::BindManager() const
	{
	return iBindMgr;
	}

EXPORT_C TInt CRootServer::CpmCount()
    /** Count number of comms provider modules.
     @internalComponent
     @return Number of comms provider modules.
     */
    {
    TInt count;
    TDblQueIter<CCommsProviderModule> cpmIter(iModules);
    cpmIter.SetToFirst();
    for(count=0;(cpmIter++)!=NULL;count++){};

    return count;
    }

TInt CRootServer::EnumerateModules(TInt aStartAt, TCFModuleNameF& aName)
    /** Return a module name from the running modules.
     Fil aName with name of module at aStartAt position in list.  This function is called
     successively to request names of all modules running
     @internalComponent
     @param aStartAt Give us the name at this position.
     @param aName And put it in here.
     @return Error code.
     */
    {
    CCommsProviderModule* pModule;

    if(aStartAt >= CpmCount())
    	return KErrEof;

    TDblQueIter<CCommsProviderModule> cpmIter(iModules);
    cpmIter.SetToFirst();
   	TInt i=0;
    // get to the correct position
    do
    	{
    	pModule = cpmIter++;
    	if(pModule==NULL)
    		break;
    	i++;
    	} while(i<=aStartAt);

    if(pModule)
    	{
  		//Copy TCFModuleNameF from module
   		aName = pModule->GetName();
   		}
   	else
   		{
   		return KErrNone;
   		}
   	return KErrNone;
    }

TInt CRootServer::EnumerateSubModules(const TCFModuleNameF& aModuleName,
					TInt aStartAt, TCFSubModuleNameF& aSubModuleName)
    /** Enumerate all known modules.
     Stuff as many comms provider module names in the lst as there is room for,
     until there are no more modules or the list is full.
     @internalComponent
     @param aModuleName Name of module whose sub-module names are being enumerated.
     @param aStartAt The name required, based on last request - first 0, second 1...
     @param aSubModuleName The name returned
     @return Error code or number of entries returned.
     */
    {
	return iBindMgr->EnumerateSubModules(aModuleName, aStartAt, aSubModuleName);
    }

EXPORT_C void CRootServer::ConstructL(const TRootServerStartupInfo& aRootServerStartupInfo)
    /** Second level construction of rootserver.
     @internalComponent
     */
    {
	__ASSERT_DEBUG(iCommsProviderModuleCtorL, User::Panic(KSpecAssert_ElemRootServerrt, 2)); // iCommsProviderModuleCtorL must have been set in NewL of the derived class
	iDeathKey=aRootServerStartupInfo.iDeathKey;
	iLeakKey=aRootServerStartupInfo.iLeakKey;
	iProcessKey=aRootServerStartupInfo.iProcessKey;
	StartL(aRootServerStartupInfo.iServerName);
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	TRAP_IGNORE(iPerfStore = CPerfMetricStore::NewL(CPerfMetricStore::KPriority, CPerfMetricStore::KPeriod));	// failure is an option; it's only logging
#endif

   	TSecurityPolicy readPolicy(ECapability_None);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
    (void)iDeathProperty.Define(KUidSystemCategory, iDeathKey.iUid, RProperty::EInt, readPolicy, writePolicy, iDeathCount);

	// can't cflog before this point!
	#ifdef __CFLOG_ACTIVE
		__CFLOG_CREATEL;

		// Check to make sure the logger is available.
		if( !__cflog__ )
			{
			RDebug::Print( _L( "CRootServer::ConstructL - the log interface could not be created.  This normally means that the logging version of c32rootsrv.dll has been mixed with a stub version of cflog.dll.  See CommsDebugUtility How-To Document FAQ section for details on enabling logging in a release build." ) );

			User::Panic( aRootServerStartupInfo.iServerName, KRSCFlogIfCreationFailed );
			}

		__CFLOG_OPEN;

		__CFLOG( KLogSubSysRS, KLogCode, _L8( "CRootServer - Initializing" ) );
	#endif

	iBindMgr = CBindManager::NewL(*this);
    }

EXPORT_C CSession2* CRootServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
    /** Create new session object.
     @internalComponent
     */
    {
    __CFLOG(KLogSubSysRS, KLogCode,_L8("CRootServer - NewSession"));

    TVersion v(TVersion(KRProcRootMajorVersionNumber,KRProcRootMinorVersionNumber,KRProcRootBuildVersionNumber));
    TBool r = User::QueryVersionSupported(v, aVersion);
    if (r==EFalse)
        User::Leave(KErrNotSupported);
    CRootServerSession* p = new(ELeave) CRootServerSession(this);
    return p;
    }

EXPORT_C TInt RunRootServer(const TRootServerStartupInfo& aRootServerStartupInfo)
    /** Run the Root Server - shouldn't return
	 Setup all the RProcess and RThread parameters, create
     the scheduler and server, use rendezvous to signal the
     thread, and finally start the active scheduler.
     */
    {
	__UHEAP_MARK;

    TFindServer findCS(aRootServerStartupInfo.iServerName);
    TFullName name;
    if (findCS.Next(name) == KErrNone)
        {
        return KErrAlreadyExists;
        }

     // Doesn't really matter renaming is purely cosmetic and
     // for the benefit of the log so we ignore the result
	(void)RThread::RenameMe(aRootServerStartupInfo.iServerName);

    RThread().SetPriority(EPriorityAbsoluteHigh);


	CTrapCleanup* pCleanup = CTrapCleanup::New();
    if(!pCleanup)
		{
        Fault(KRSNoMemory);
		}

    TInt result = KErrNone;

	CActiveScheduler* scheduler = new CActiveScheduler;
    __ASSERT_ALWAYS(scheduler!=NULL, Fault(KRSSchedulerError));
    CActiveScheduler::Install(scheduler);

	CRootServer* rootServer = NULL;
    TRAP(result, rootServer = (*aRootServerStartupInfo.iRootServerCtor)(aRootServerStartupInfo));
    __ASSERT_ALWAYS(result==KErrNone, Fault(KRSNoMemory));

#ifdef _DEBUG
		__CFLOG(KLogSubSysRS, KLogCode, _L8("RunRootServer - Defining Leak detection Proprty"));
		RProperty::Define(aRootServerStartupInfo.iLeakKey.iUid, RProperty::EInt, TSecurityPolicy(TSecurityPolicy::EAlwaysPass),
						TSecurityPolicy(ECapabilityNetworkControl));
#endif

    RProcess::Rendezvous(KErrNone);
    __CFLOG(KLogSubSysRS, KLogCode, _L8("RunRootServer - Entering Active Scheduler."));

    //
    // start the Active Scheduler
    //
    CActiveScheduler::Start();
    __CFLOG(KLogSubSysRS, KLogCode, _L8("RunRootServer - Ended Active Scheduler. Exiting..."));

	CActiveScheduler::Install(NULL);
	delete rootServer;
	delete scheduler;

	delete pCleanup;

	__UHEAP_MARKEND;
    return(KErrNone);
    }


