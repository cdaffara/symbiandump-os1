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
// Defines the reference Comms Configurator
//
//

/**
 @file
 @internalTechnology
*/

#if !defined(__C32START_H__)
#define __C32START_H__
#include <c32root.h>
#include "c32cmi.h"
#include "c32startshared.h"
#include <e32property.h>
#include <domainmember.h>
#include <comms-infras/commsdebugutility.h>
/** Subsystem name, used for logging.
*/
__FLOG_STMT(_LIT8(KSubsys,"C32Start");)

/** Component name, used for logging.
*/
__FLOG_STMT(_LIT8(KComponent,"Events");)

/** Name of the application.
*/
_LIT(KC32StartName, "C32Start");

/** Name of the Rootserver executable.
*/
_LIT(KRootServerExe, "c32exe.exe");

/** Default priority of the Rootserver executeable and
thus the priority Comms Process.
*/
const TProcessPriority KRootServerDefaultPriority=EPriorityHigh;

class CCommsProcessStarter;
class CBindingRef;

/** Type of module.
*/
enum TModuleType {
     /** A module which can only be at the top end of a hierarchical binding.
     */
     EServerModule,
     /** Standard Comms Provider Module.
     */
     EProviderModule
     };

/** State of module.
*/
enum TModuleState {
     /** The module is not currently loaded.
     */
     ENotLoaded,
     /** The module is currently being loaded by the Rootserver.
     */
     ELoading,
     /** The module has been loaded and is running.
     */
     ELoaded,
     /** The module could not be loaded by the Rootserver.
     */
     ELoadFailed
     /** The module is being currently Unloaded by the root server
     */
     ,EUnLoading
     };

/** State of a binding
*/
enum TBindingState {
     /** The binding is not currently active.
     */
     ENotBound,
     /** The binding is being set up by the Rootserver.
     */
     EBinding,
     /** The binding has been created and is active.
     */
     EBound,
     /** The Rootserver was unable to set up the binding.
     */
     EBindFailed
     /** Either 'end' module of the binding has failed to load, result being binding won't start
     */
     ,EBindInComplete
     };

class CModuleRef : public CActive
/** An instance of this object represents one module to be-, being- or which is
loaded by the RootServer. The public inheritance of CActive
*/
	{
public:
	static CModuleRef* NewLC(RRootServ& aRootServer, CCommsProcessStarter& aCommsProcess, C32CmiData& aCModuleCMIData);
	~CModuleRef();
	const inline TCFGroupNameF& GroupName() const;
	TModuleState State() const		{ return iState; }
	const TCFModuleNameF& Name() const		{ return static_cast<const TCFModuleNameF&>(iParams.iParams.iName); }
	TUint32 ScaledStartupState() const	{ return iScaledStartupState; }
	TBool SystemCritical() const	{ return iSystemCritical; }
	TBool SystemCriticalAfterInit() const {return iSystemCriticalAfterInit;}

	void Load();
	TBool Resurrect();
	TBool operator==(const TCFModuleNameF& aModuleName) const;
	static TInt CompareScaledStartupState(const CModuleRef& aModule1, const CModuleRef& aModule2);

	inline HBufC8* GetIniData() const;
	inline TBool IsOnDemand() const;
	inline void SetRMessage(const RMessage2& aMessage);
	const RMessage2& Message() const { return iRMessage; }
	void MaybeCompleteMessage(TInt aReturnValue);
	void UnLoadCpm(const TCFModuleNameF& aModuleName,TRSUnLoadType aType);
	void CancelUnLoadCpm(const TCFModuleNameF& aModuleName);
	void CancelLoadCpm(const TCFModuleNameF& aModuleName);
	void UnBindCpm();
	inline void IncrementBindings() { ++iNumBindings; }
	inline void IncrementCompletedBindings() { ++iCompletedBindings; }
	void UpdateCompletedBinding(TInt aReturnvalue);
	inline void SetHasBindings() { iHasBinding = ETrue; }

protected:
	void DoCancel();
	void RunL();

private:
	CModuleRef(RRootServ& aRootServer, CCommsProcessStarter& aCommsProcess);
	void ConstructL(C32CmiData& aCModuleCMIData);

private:
	RPointerArray<CBindingRef> iBindings;
	TUint32 iScaledStartupState;
	TUint iNumBindings;
	RRootServ& iRootServer;
	CCommsProcessStarter& iCommsProcess;
	TModuleState iState;
	TModuleType iType;
	HBufC8* iIniData;
	TRSStartModuleParams iParams;
	TBool iSystemCritical;
	TBool iSystemCriticalAfterInit;
	TConfiguratorModuleParams iConfigParams;
	RMessage2 iRMessage;
	TUint iCompletedBindings; 	// when iNumBindings == iCompletedBindings, Message is completed
	TBool iHasBinding;
	};

class CBindingRef : public CActive
/** An instance of this object represents one binding to be-, being- or which is
bound by the RootServer.
*/
	{
public:
	CBindingRef(RRootServ& aRootServer, CCommsProcessStarter& aStartConfigProcess,
				const TRSBindingInfo& aBinding);
	~CBindingRef();
	inline void SetState(TBindingState aState);
	void UpdateCompletedBinding(TInt aReturnValue);
	const TCFSubModuleAddress& FromModule() const;
	const TCFSubModuleAddress& ToModule() const;
	TInt BindIfReady();
protected:
	void DoCancel();
	void RunL();

private:
	CBindingRef();
	void ConstructL(RRootServ& aRootServer, CCommsProcessStarter& aStartConfigProcess,
					TRSBindingInfo& aBinding);
private:
	RRootServ& iRootServer;
	CCommsProcessStarter& iStartProcess;
	TBindingState iState;
	TRSBindingInfo iBindInfo;
	};

class CDeathWatcher : public CActive
/** Observes the death publication and notifies the CommsProcessStarter
*/
	{
public:
	static CDeathWatcher* NewL(CCommsProcessStarter& aStartConfigProcess);
	~CDeathWatcher();
protected:
	void DoCancel();
	void RunL();
private:
	CDeathWatcher(CCommsProcessStarter& aStartConfigProcess);
	void Construct();
private:
	CCommsProcessStarter& iStartProcess;
	RProperty iDeathProperty;
	};

class CStartupStateObserver : public CDmDomain
/** Observes the Generic Start-up Architecture state transitions, if the GSA is available. The
need to cope with its absence prevents the more obvious approach of simply deriving
CCommsProcessStarter from CDmDomain.
*/
	{
public:
	static CStartupStateObserver* NewL(CCommsProcessStarter& aStarter);
	~CStartupStateObserver();
protected:
	void RunL();
private:
	CStartupStateObserver(CCommsProcessStarter& aStarter);
private:
	CCommsProcessStarter& iStarter;
	};

	class CRsConfiguratorServer;
class CCommsProcessStarter : public CBase
/** The main class orchestrating loading and binding of the Comms Provider
Modules as specified by the .CMI files.
*/
	{
public:
	enum TModuleState { ELoadingModule, EListeningforDeath };

	static CCommsProcessStarter* NewLC(RRootServ& aRootServer, RProperty& aProperty);
	~CCommsProcessStarter();
	void ReadConfigAndStartAllL(const TDesC& aCMISuppressions);
	CModuleRef* GetModule(const TCFModuleNameF& aModule ) const;
	void NotifyLoadCompletion(CModuleRef& aModule);
	void NotifyBindCompletion(CBindingRef& aBinding);
	void NotifySuddenDeath(TInt aDeathCount, TInt aDeathStatus);
	void NotifyStartupStateChange();
	TBool ConfigurationComplete() const;
	TBool InTestMode() const
		{
		return iTestMode;
		}
    void PrintSummary();
	void LoadOnDemandModule(CModuleRef& aModule);
    RPointerArray<CModuleRef>& GetModulesList()
		{
		return iModules;
		}
	RPointerArray<CBindingRef>& GetBindingList()
		{
		return iBindings;
		}
protected:
	TInt UnloadedModuleIdx() const
		{
		return iUnloadedModuleIdx;
		}
private:
	CCommsProcessStarter(RRootServ& aRootServer, RProperty& aProperty);
	TBool SplitInt(TPtrC8 &aDes, TInt& aValue) const;
    TBool SplitUint(TPtrC8 &aDes, TUint& aValue) const;
	void ContinueLoading();
    void Shutdown();

private:
	TInt iUnloadedModuleIdx;
	TInt iModulesLoading;
	TInt iModulesLoaded;
	RRootServ& iRootServer;
	RPointerArray<CModuleRef> iModules;
	RPointerArray<CBindingRef> iBindings;
	CDeathWatcher* iDeathWatcher;
	TInt iDeathCount;
	RProperty& iConfigurationProperty;
	enum TPublicationLevel { EUnpublished = 0, EPublishedCore, EPublishedComplete };
	TPublicationLevel iPublicationLevel;
	// Because of monitoring startup state changes we can come through twice in the "final" state,
	// but only want one summary dump (avoid deluging the user with apparent ambiguity)
	__FLOG_STMT(TBool iSummaryAlreadyPrinted; )
	TUint iBindCount;
#ifndef SYMBIAN_ZERO_COPY_NETWORKING
 	TInt iInitMBufPoolSize;
  	TInt iMaxMBufPoolSize;
#endif
	TBool iTestMode;
	CStartupStateObserver* iStartupStateObserver;
	TBool iNeedToAckLastState;
	TUint32 iScaledStartupState;
	TBool iFinishedloading;
	CRsConfiguratorServer* iConfiguratorServer;
	};
#include "c32start.inl"
#endif
