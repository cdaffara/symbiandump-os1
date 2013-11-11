// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file PsdStates.cpp
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "psdstatesTraces.h"
#endif

#include <comms-infras/cagentsmbase.h>
#include <etelpckt.h>
#include <connectprog.h>  // for circuit-switched progress enums
#include <netconerror.h>
#include "psdlogger.h"

#include "psdstates.h"
#include "psdagt.h"
#include "psdprog.h"

#include <logengevents.h>

CPsdOutInit::CPsdOutInit(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CPsdInitBase(aObserver,aPsdSM,aDb)
/**
Constructor
*/
	{}

CPsdOutInit::~CPsdOutInit()
/**
Destructor
*/
	{}

CAgentStateBase* CPsdOutInit::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	if (iStatus==KErrNone && aContinue)	// which it will be if there has been no error so far
		return new (ELeave) CPsdOutCheckConfig(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

CPsdOutCheckConfig::CPsdOutCheckConfig(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CPsdCheckConfig(aObserver,aPsdSM,aDb)
/**
Constructor
*/
	{}

CPsdOutCheckConfig::~CPsdOutCheckConfig()
/**
Destructor
*/
	{}

CAgentStateBase* CPsdOutCheckConfig::NextStateL(TBool aContinue)
/**
move the state machine to the next state function creates the next state in the state machine.

@see CAgentStateBase* CPsdOutInit::NextStateL(TBool aContinue)
*/
	{
	if (iStatus==KErrNone && aContinue)	// which it will be if there has been no error so far
		return new (ELeave) CPsdNetworkCheck(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

#ifndef INCOMING_NOT_SUPORTED

CPsdInInit::CPsdInInit(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CPsdInitBase(aObserver,aPsdSM,aDb)
/**
Constructor
*/
	{}
	
CPsdInInit::~CPsdInInit()
/**
Destructor
*/
	{}

void CPsdInInit::DoStartStateL()
/**
Internal function that initialises the state machine for an incoming connection
overriding the virtual function in the base class

Check first of all whether we are allowed to do inbound
*/
	{
	TBool allowed = iDbPsd->IsIncomingAllowedL();
	if (!allowed)
		User::Leave(KErrAccessDenied);
	CPsdInitBase::DoStartStateL();
	}	

CAgentStateBase* CPsdInInit::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@see CAgentStateBase* CPsdOutInit::NextStateL(TBool aContinue)
*/	
	{
	if (iStatus==KErrNone && aContinue)	// which it will be if there has been no error so far
		return new (ELeave) CPsdInCheckConfig(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

CPsdInCheckConfig::CPsdInCheckConfig(MAgentObserver* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CPsdCheckConfig(aObserver,aPsdSM,aDb)
/**
Constructor
*/
	{}
	
CPsdInCheckConfig::~CPsdInCheckConfig()
/**
Destructor
*/
	{}

CAgentStateBase* CPsdInCheckConfig::NextStateL(TBool aContinue)
/**
@see CAgentStateBase* CPsdOutInit::NextStateL(TBool aContinue)
*/
	{
	if (iStatus==KErrNone && aContinue)	// which it will be if there has been no error so far
		return new (ELeave) CPsdWaitForIncoming(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}
#endif // #ifndef INCOMING_NOT_SUPORTED

CPsdInitBase::CPsdInitBase(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdInitBase::~CPsdInitBase()
/**
Desctructor
*/
	{
	Cancel();
	}

void CPsdInitBase::StartState() 
/**
Function called by the genconn state machine framework to start the state
*/
	{
	TRAPD(ret,DoStartStateL());
	if (ret!=KErrNone)
		JumpToRunl(ret);
	}

void CPsdInitBase::DoStartStateL()
/**
Internal function that performs all the initialisation of the common resources
held by the state machine for use by subsequent states
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDINITBASE_DOSTARTSTATEL_1, "Packet Data:\tInitialising");
	
	iSMObserver->PreventConnectionRetries();
	//
	//	PSD AGX does not allow any more connection retries. It assumes that GenConn has
	//  already done the packet network availability check, so we would only get to this stage
	//	should there be packet network (or if it was unknown).

	RTelServer& etel = iSM->TelServer();
	RPhone& phone = iSM->Phone();
	RPacketService& packetNetwork = iSM->PacketNetwork();

	User::LeaveIfError(etel.Connect());
	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	iSM->BaseEnv().Db()->GetTsyNameL(tsyName);
	User::LeaveIfError(etel.LoadPhoneModule(tsyName));
	iSM->SetTsyLoaded(ETrue);

	User::LeaveIfError(etel.SetExtendedErrorGranularity(RTelServer::EErrorExtended));

	RTelServer::TPhoneInfo info;
	TInt r=tsyName.Locate('.');
	if (r!=KErrNotFound)
		tsyName.SetLength(r);
	GetPhoneInfoL(tsyName,info);	// If this leaves, TSY should be unloaded
							// when RTelServer handle is closed.
	User::LeaveIfError(phone.Open(etel,info.iName));
	User::LeaveIfError(packetNetwork.Open(phone));

	//
	RPhone::TStatus status;
	User::LeaveIfError(phone.GetStatus(status));
	if (status.iModemDetected==RPhone::EDetectedNotPresent || status.iModemDetected==RPhone::EDetectedUnknown)
		{
		phone.Initialise(iStatus);
		SetActive();
		}
	else	// status.iModemDetected==RPhone::EDetectedPresent
		JumpToRunl(KErrNone);
	}


void CPsdInitBase::GetPhoneInfoL(const TDesC& aLoadedTsyName, RTelServer::TPhoneInfo& aInfo)
/**
Function obtains the info on the phone object implemented in the TSY

Assumes aloadedTsyName has no ".TSY" appendage
Finds the phone information for the TSY just loaded. 
Assumes just one phone in that TSY - or that every phone in it is equally useful.

@param name of the loaded Tsy.
@param information about the phone.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	TInt count;
	User::LeaveIfError(iSM->TelServer().EnumeratePhones(count));
	if (count<=0)
		User::Leave(KErrNotFound);
	TBool found=EFalse;
	for (TInt i=0; i<count; i++)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		User::LeaveIfError(iSM->TelServer().GetTsyName(i,currentTsyName));
		
		TInt r=currentTsyName.Locate('.');
		if (r!=KErrNotFound)
			currentTsyName.SetLength(r);
		if (currentTsyName.CompareF(aLoadedTsyName)==KErrNone)
			{
			User::LeaveIfError(iSM->TelServer().GetPhoneInfo(i,aInfo));
			found=ETrue;
			break;
			}	
		}

	if (!found)
		User::Leave(KErrNotFound);
	}


void CPsdInitBase::RunL()
/**
Complete state for the initialisation state for both incomming and outgoing connections.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDINITBASE_RUNL_1, "Packet Data:\tInitialised");
	if (iStatus!=KErrNone)
		iSMObserver->ConnectionComplete(EPsdStartingConfiguration,iStatus.Int()); // correct progress?
	else
		iSM->BaseEnv().CompleteState(KErrNone);	// this tells the SM to move onto the next state
	}

void CPsdInitBase::DoCancel()
/**
Cancell the asynchronous request of initialisation state for both incomming and outgoing connections.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	
	iSM->Phone().InitialiseCancel();
	}

CPsdCheckConfig::CPsdCheckConfig(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{}

CPsdCheckConfig::~CPsdCheckConfig()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdCheckConfig::StartState()
/**
Start checking Configuration states.
*/
	{
	JumpToRunl(KErrNone);
	}

void CPsdCheckConfig::DoInitL()
	{
	RPacketService& packetNetwork = iSM->PacketNetwork();

	const CPsdContextConfig& config = iSM->Config();
	if(!config.IsConfigSupportedL(packetNetwork,iSM->Direction()) )
		{
		User::Leave(KErrNotSupported);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCHECKCONFIG_DOINITL_1,"Packet Data:\tChecked Config");
	}

void CPsdCheckConfig::RunL()
/**
Complete state for the config checking state for both incomming and outgoing connections.
*/
	{
	TRAPD(ret,DoInitL());
	iStatus=ret;
	if (iStatus!=KErrNone)
		iSMObserver->ConnectionComplete(EPsdStartingConfiguration,ret); // correct progress?
	else
		iSM->BaseEnv().CompleteState(KErrNone);	// this tells the SM to move onto the next state
	}

void CPsdCheckConfig::DoCancel()
/**
Need do nothing since StartState() already triggered the request status.
*/
	{}

CPsdNetworkCheck::CPsdNetworkCheck(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdNetworkCheck::~CPsdNetworkCheck()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdNetworkCheck::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDNETWORKCHECK_STARTSTATE_1,"Packet Data:\tChecking network availability");

	iSubState = EGettingInitialStatus;
	iSM->PacketNetwork().GetNtwkRegStatus(iStatus,iRegStatus);
	SetActive();
	}

void CPsdNetworkCheck::DoNetworkCheck()
/**
Checking for Network
*/

	{
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	if (iRegStatus==RPacketService::ERegistrationDenied || 
		iRegStatus==RPacketService::ENotRegisteredNotSearching || 
		iRegStatus==RPacketService::ENotRegisteredAndNotAvailable)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDNETWORKCHECK_DONETWORKCHECK_1, "Packet Data:\tNo network");
		JumpToRunl(KErrNetConNoGPRSNetwork);
		return;
		}
	if (iRegStatus==RPacketService::ERegisteredOnHomeNetwork || 
		iRegStatus==RPacketService::ERegisteredRoaming ||
		iRegStatus==RPacketService::ENotRegisteredButAvailable ||
		iRegStatus==RPacketService::EUnknown)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDNETWORKCHECK_DONETWORKCHECK_2,"Packet Data:\tReg status %d. Proceeding",iRegStatus);

		iSM->BaseEnv().CompleteState(KErrNone);
		return;
		}

	__ASSERT_ALWAYS(iRegStatus==RPacketService::ENotRegisteredSearching,PanicAgt(EPsdInvalidNtwkRegistration));

	iSM->PacketNetwork().NotifyChangeOfNtwkRegStatus(iStatus,iRegStatus);
	SetActive();	
	}

void CPsdNetworkCheck::RunL()
/**
iRegStatus should have been updated, if aError is KErrNone

Complete Connection for the Network Check state.
*/
	{
	if(iSubState == EGettingInitialStatus)
		{
		if(iStatus==KErrNotSupported)
		//tsy could not provide the information
			{
			iRegStatus=RPacketService::EUnknown;
			iStatus=KErrNone;
			}
		iSubState = EWaitingForStatusChange;
		}
	if (iStatus==KErrNone)
		DoNetworkCheck();
	else
		iSMObserver->ConnectionComplete(EPsdStartingConfiguration,iStatus.Int());
	}

void CPsdNetworkCheck::DoCancel()
/**
Cancell Asynchronous request of network check state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	iSM->PacketNetwork().CancelAsyncRequest(EPacketNotifyChangeOfNtwkRegStatus);
	}

CAgentStateBase* CPsdNetworkCheck::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@see CAgentStateBase* CPsdOutInit::NextStateL(TBool aContinue).
*/
	{
	if (iStatus==KErrNone && aContinue)
		return new (ELeave) CPsdCreateContext(iSMObserver,iSM,iDbPsd);
	else 
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

#ifndef INCOMING_NOT_SUPORTED

CPsdWaitForIncoming::CPsdWaitForIncoming(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdWaitForIncoming::~CPsdWaitForIncoming()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdWaitForIncoming::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDWAITFORINCOMING_STARTSTATE_1,"Packet Data:\tWaiting for incoming Packet request");

	iSM->PacketNetwork().NotifyContextActivationRequested(iStatus,iPdpTypeRequested,iAddressRequested);
	SetActive();
	}

void CPsdWaitForIncoming::RunL()
/**
Rejects any incoming request for an IP address that isn't in our Incoming context config

Complete Connection for wait for incomming state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	if (iStatus==KErrNone)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDWAITFORINCOMING_RUNL_1, "Packet Data:\tReceived network context activation request");
		
		if(iSM->Config().QueryIfIncommingConnectionAcceptable(iPdpTypeRequested,iAddressRequested) )
			{
			iSMObserver->UpdateProgress(EPsdAnsweringIncoming,KErrNone);
			TInt ret = iSMObserver->IncomingConnectionReceived();	// to switch NIFMAN to connecting
										// if this returns an error, it means NIFMAN is already
										// connected (this can only happen in the "window" 
										// during callback
			if (ret==KErrNone)
				iSM->BaseEnv().CompleteState(KErrNone);
			else
				{
				iSM->PacketNetwork().RejectActivationRequest();
				StartState();
				}
			}
		else	// wrong address
			{
			iSM->PacketNetwork().RejectActivationRequest();	// how long will this take? Should it be
													// asynchronous?
			StartState();
			}
		}
	else
		iSMObserver->ConnectionComplete(EPsdAnsweringIncoming,iStatus.Int());
	}

void CPsdWaitForIncoming::DoCancel()
/**
Cancell Asynchronous request of wait for incomming state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	iSM->PacketNetwork().CancelAsyncRequest(EPacketNotifyContextActivationRequested);
	}

CAgentStateBase* CPsdWaitForIncoming::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

Assumes cancelling a notification will result in KErrCancel being passed back in iStatus

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	if (iStatus==KErrNone && aContinue)
		return new (ELeave) CPsdCreateContext(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}
#endif // #ifndef INCOMING_NOT_SUPORTED

CPsdCreateContext::CPsdCreateContext(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdCreateContext::~CPsdCreateContext()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdCreateContext::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCREATECONTEXT_STARTSTATE_1, "Packet Data:\tCreating context");

	RPacketService& packetNetwork = iSM->PacketNetwork();
	RPacketContext& context = iSM->Context();

	TName name;
	TInt ret = context.OpenNewContext(packetNetwork,name);
	if (ret==KErrNone)
		{
		iSMObserver->UpdateProgress(EPsdStartingConfiguration,KErrNone);
		context.SetConfig(iStatus,iSM->Config().PackedConfig());
		SetActive();
		}
	else
		JumpToRunl(ret);
	}

void CPsdCreateContext::RunL()
/**
Don't want to do any processing after SetConfig, just move to next state.

Complete Connection for Create Context state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCREATECONTEXT_RUNL_1, "Packet Data:\tCompleted with error %d",iStatus.Int());

	if (iStatus==KErrNone)
		{
		// Have added new configure qos state that follows this one
		// therefore don't report config completion until that state has finished
		iSM->BaseEnv().CompleteState(KErrNone);
		}
	else
		{
		// There is now another configuration state so we are still in starting configuration now
		iSMObserver->ConnectionComplete(EPsdStartingConfiguration,iStatus.Int());
		}
	}

void CPsdCreateContext::DoCancel()
/**
Cancell Asynchronous request of create context state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	iSM->Context().CancelAsyncRequest(EPacketContextSetConfig);
	}

CAgentStateBase* CPsdCreateContext::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

iStatus will be KErrNone if there has been no error so far. If the state has been cancelled,
iStatus should be KErrCancel. (can we rely on this?)

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/	
	{
	if (iStatus==KErrNone && aContinue)	
		// New state
		return new (ELeave) CPsdCreateQoS(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

CPsdCreateQoS::CPsdCreateQoS(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdCreateQoS::~CPsdCreateQoS()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdCreateQoS::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCREATEQOS_STARTSTATE_1, "Packet Data:\tCreating QoS");

	RPacketContext& context = iSM->Context();
	RPacketQoS& qoS = iSM->QoS();

	TName name;
	TInt ret = qoS.OpenNewQoS(context,name);
	if (ret==KErrNone)
		{
		qoS.SetProfileParameters(iStatus,iSM->QoSConfig().PackedQoSConfig());
		SetActive();
		}
	else if (ret == KErrNotSupported)//the TSY doesn't support QoS -> flag it
		{
			iSM->SetQoSSupported(EFalse); 
		    JumpToRunl(KErrNone);
		}
	else
		JumpToRunl(ret);
	}

void CPsdCreateQoS::RunL()
/**
Don't want to do any processing after SetProfileParameters, just move to next state.

Complete Connection for Create QoS state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCREATEQOS_RUNL_1,"Packet Data:\tCompleted with error %d",iStatus.Int());

	if (iStatus==KErrNone)
		{
		iSMObserver->UpdateProgress(EPsdFinishedConfiguration,KErrNone);
		iSM->BaseEnv().CompleteState(KErrNone);
		}
	else
		iSMObserver->ConnectionComplete(EPsdFinishedConfiguration,iStatus.Int());
	}

void CPsdCreateQoS::DoCancel()
/**
Cancell Asynchronous request of create QoS state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	if(iSM->IsQoSSupported())
    	iSM->QoS().CancelAsyncRequest(EPacketQoSSetProfileParams);
	}

CAgentStateBase* CPsdCreateQoS::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

iStatus will be KErrNone if there has been no error so far. If the state has been cancelled,
iStatus should be KErrCancel. (can we rely on this?)

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	if (iStatus==KErrNone && aContinue)	
		return new (ELeave) CPsdActivateContext(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

CPsdActivateContext::CPsdActivateContext(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdActivateContext::~CPsdActivateContext()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdActivateContext::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDACTIVATECONTEXT_STARTSTATE_1, "Packet Data:\tActivating context");
	

	iSMObserver->UpdateProgress(EPsdStartingActivation,KErrNone);
	// Check if the context is already active since in the reconnect case it may be
	RPacketContext::TContextStatus contextStatus;
	iSM->Context().GetStatus(contextStatus);

	iSubState = EActivatingContext;
	if( !((contextStatus==RPacketContext::EStatusActive)
		||(contextStatus==RPacketContext::EStatusSuspended)
		||(contextStatus==RPacketContext::EStatusActivating)) )
		{
		// Context not active so activate it
		iSM->Context().Activate(iStatus);
		SetActive();
		}
	else
		{
		// Context already active so move to the RunL
		JumpToRunl(KErrNone);
		}
	}

void CPsdActivateContext::RunL()
/**
Complete Connection for Activate Context state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDACTIVATECONTEXT_RUNL_1, "Packet Data:\tCompleted with error %d",iStatus.Int());

	if( (iSubState==EActivatingContext&&(iStatus==KErrNone || iStatus==KErrNotSupported) )
		||(iSubState==ELoaningCommPort&&iStatus==KErrNone) ) 
		{
		if(iSubState==EActivatingContext)
			{
			iSM->Context().LoanCommPort(iStatus,iCommport);
			SetActive();
			iSubState=ELoaningCommPort;
			}
		else
			{
			// Substate == ELoaningCommPort
			TRAPD(ret,iSM->BaseEnv().Db()->SetCommPortL(iCommport));
			if (ret!=KErrNone)
				{
				iStatus=ret;
				iSMObserver->ConnectionComplete(EPsdFinishedActivation,iStatus.Int());
				}
			else
				{
				iSMObserver->UpdateProgress(EPsdFinishedActivation,KErrNone);// may want to do this when it really
																// activates
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDACTIVATECONTEXT_RUNL_2,"Packet Data:\tHanding control to PPP");

				iSMObserver->ServiceStarted();	
				iSM->BaseEnv().CompleteState(KErrNone);
				}
			}
		}
	else
		iSMObserver->ConnectionComplete(EPsdFinishedActivation,iStatus.Int());
	}

void CPsdActivateContext::DoCancel()
/**
Cancell Asynchronous request of activate context state. 
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	iSM->Context().CancelAsyncRequest(EPacketContextActivate);
	}

CAgentStateBase* CPsdActivateContext::NextStateL(TBool aContinue)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	if (iStatus==KErrNone && aContinue)
		return CPsdOpen::NewL(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdRecoverCommPort(iSMObserver,iSM,iDbPsd);
	}

CPsdOpen* CPsdOpen::NewL(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
/**
Constructing an object of class CPsdOpen,pushing it to the clean up stack and popping it out.
*/
	{
	CPsdOpen* state = new (ELeave) CPsdOpen(aObserver,aPsdSM,aDb);
	CleanupStack::PushL(state);
	state->ConstructL();
	CleanupStack::Pop();
	return state;
	}

CPsdOpen::CPsdOpen(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

void CPsdOpen::ConstructL()
/**
2nd Phase Construction
*/
	{}

CPsdOpen::~CPsdOpen()
/**
Destructor
*/
	{
	delete iContextChangeMonitor;
	Cancel();
	}

void CPsdOpen::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM && iSMObserver,User::Invariant());

	
	iSM->Context().GetStatus(iContextStatus);

	if (iContextStatus==RPacketContext::EStatusActive)
		{
		LogActive();
		TRAPD(err,WatchForConfigChangesL());
		if(err != KErrNone)
			{
			iSMObserver->ConnectionComplete(KConnectionOpen,err);	
			return;
			}
		}

	//Either, we've successfully handled the active state or we have to 
	//wait for TSY to tell us it has become active. This is because calling Activate()
	//doesn't necessarily mean it's activated in the network - have to wait for PPP to do its
	//stuff.
	RequestStatusChange();
 
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_STARTSTATE_1, "Packet Data:\tCalling ConnectionComplete");


	iSMObserver->Notification(EAgentToNifEventTypeModifyInitialTimer,NULL);
	iSMObserver->UpdateProgress(KConnectionOpen,KErrNone); // I think NIFMAN just swallows this after
												// a ConnectComplete(Open)
	iSMObserver->ConnectionComplete(KConnectionOpen,KErrNone);	
	}

void CPsdOpen::LogActive()
/**
This internal function starts the logging and also starts watching for the connection quality
dropping below minimum acceptable values

This is called every time state goes to Active. Only want to log the first time, as
ensuing times it could be a Suspended->Active transition.
*/
	{
	if (!iEventLoggerStarted)
		{
		TBuf<KCommsDbSvrMaxFieldLength> remoteParty;
		remoteParty.Zero();
		TRAPD(err, iDbPsd->GetRemotePartyL(remoteParty));
		// logg err
		if(err != KErrNone)
			{
#ifdef __FLOG_ACTIVE
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_LOGACTIVE_1, "CPsdOpen:\t Error in getting remote party %d.",err);
#endif
			}
		iEventLoggerStarted = ETrue;
		if (!iSM->BaseEnv().IsReconnect())
			{
			if (iSM->Direction()==ECommDbConnectionDirectionOutgoing)
				iSM->Logger()->LogDataAddEvent(R_LOG_CON_CONNECTED, remoteParty,R_LOG_DIR_OUT,KNullDesC,KLogPacketDataEventTypeUid);
			else
				iSM->Logger()->LogDataAddEvent(R_LOG_CON_CONNECTED, remoteParty,R_LOG_DIR_IN,KNullDesC,KLogPacketDataEventTypeUid);
			iSM->DataLogger()->Start();
			}
		else
			{
			//the logevent is already existing, we just have reconnected then update the status of the connection
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_CONNECTED, KLogPacketDataEventTypeUid);
			// no need to start again the datalogger since it has not been stopped previously
			}
		}
	}

void CPsdOpen::WatchForConfigChangesL()
/**
Watch for Configuration changes in the open state
*/
	{
	// Should only be called once the context is active
	if(!iContextChangeMonitor)
		{
		//start watching for changes in the context status
		iContextChangeMonitor = CPsdContextChangeMonitor::NewL(iSM,this);
		}
	}

void CPsdOpen::RequestStatusChange()
/**
Request for the change of status in the open state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	iSM->Context().NotifyStatusChange(iStatus,iContextStatus);
	SetActive();
	}

void CPsdOpen::RunL()
/**
No need for this state ever to call CompleteState(), because we are expecting the state 
machine to be told to Disconnect from above, which means this state will have NextStateL()
called on it.

Update progress for open state.
*/
	{
	__ASSERT_DEBUG(iSM && iSMObserver,User::Invariant());

	if (iStatus==KErrNone)
		{
		switch (iContextStatus)
			{
		case RPacketContext::EStatusSuspended:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_1, "PacketData:\tStatus change to suspended");
			
			iSMObserver->Notification(EAgentToNifEventTypeDisableTimers,NULL);	
			iSMObserver->UpdateProgress(EPsdSuspended,KErrNone);
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_SUSPENDED, KLogPacketDataEventTypeUid);
			RequestStatusChange();
			}
			break;
		case RPacketContext::EStatusActive:
			{
			// if reconnect happened then the logger is allready started and we just do an update
			if (!iEventLoggerStarted)
				{
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_2, "PacketData:\tStatus change to log active");
				WatchForConfigChangesL();	//those 2 functions should have been called in start state but
				LogActive();				//RPacketContext was not EStatusActive at that time
				}
			else	// the LogCallStart() has completed 
				{
				iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_CONNECTED, KLogPacketDataEventTypeUid);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_3, "PacketData:\tStatus change to active");
				
				}
			iSMObserver->Notification(EAgentToNifEventTypeEnableTimers,NULL);	
			iSMObserver->UpdateProgress(KConnectionOpen,KErrNone);
			RequestStatusChange();
			}
			break;
		case RPacketContext::EStatusDeactivating:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_4, "PacketData:\tStatus change to deactivating");
			iSMObserver->UpdateProgress(EPsdStartingDeactivation,KErrNone);
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTING, KLogPacketDataEventTypeUid);
			RequestStatusChange();			
			}
			break;
		case RPacketContext::EStatusInactive:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_5, "PacketData:\tStatus change to inactive");
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogPacketDataEventTypeUid);
			}
			break;
		case RPacketContext::EStatusDeleted:
			{ 
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_6, "PacketData:\tStatus change to deleted");
			iSMObserver->UpdateProgress(EPsdFinishedDeactivation,KErrNone);
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogPacketDataEventTypeUid);
			}
			break;
		case RPacketContext::EStatusActivating:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDOPEN_RUNL_7, "PacketData:\tStatus change to activating");
			iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_CONNECTING, KLogPacketDataEventTypeUid);
			RequestStatusChange();
			}
			break;
		default:
			PanicAgt(EPsdUnknownContextState);
			break;
			};	
		}
		// no way to signal upwards that there's been an error. 
		// Have to rely on the agent's client to close down, or PPP to detect it's gone down
	}

void CPsdOpen::DoCancel()
/**
The SM shouldn't call Cancel() on us because CancelConnect can never be called on it.
But we must Cancel ourselves when leaving the state.

Cancell Asynchronous request of open state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	// the only pending request is RequestStatusChange, do not cancel the logging request which is in the queue.
		iSM->Context().CancelAsyncRequest(EPacketContextNotifyStatusChange);
	}	

CAgentStateBase* CPsdOpen::NextStateL(TBool /*aContinue*/)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	Cancel();
	return new (ELeave) CPsdRecoverCommPort(iSMObserver,iSM,iDbPsd);
	}

void CPsdOpen::ReportContextBelowAcceptableQuality()
/**
Callback function from CPsdContextChangeMonitor object
when called launches a dialog to inform the user that the
connection has dropped below acceptable quality
*/
	{
	// The connection has gone below the acceptable level of quality
	// so put up a dialog asking the user what they want to do
	iSM->BaseEnv().DlgPrc()->QoSWarning(*this);
	}

void CPsdOpen::MDPOQoSWarningComplete(TInt aError, TBool aResponse)
/**
Callback from the QoS warning dialog

@param TInt aError indication if an error occured in the dialog
@param TBool flag indicating the response from the dialog. ETrue means disconnect
*/
	{
	// Callback from the QoS warning dialog
	// aResponse == True means disconnect
	if((aResponse)&&(aError == KErrNone))
		{
		// Disconnect
		// Request Nifman to disconnect
		iSMObserver->Notification(EAgentToNifEventTypeDisableConnection,NULL);
		}
	}

void CPsdOpen::ReportError(TInt aError)
/**
Report error in open state.

@param aError, error code for error occurs during open state.
*/
	{
	JumpToRunl(aError);
	}


CPsdRecoverCommPort::CPsdRecoverCommPort(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdRecoverCommPort::~CPsdRecoverCommPort()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdRecoverCommPort::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM && iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDRECOVERCOMMPORT_STARTSTATE_1, "Packet Data:\tRecovering Comm Port to ETel");

	iSMObserver->UpdateProgress(EPsdStartingDeactivation,KErrNone);
	iSM->Context().RecoverCommPort(iStatus);
	SetActive();
	}

void CPsdRecoverCommPort::RunL()
/**
Complete state for Recover comm port state.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());
	// what can we do with errors at this stage in the game?
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDRECOVERCOMMPORT_RUNL_1, "Packet Data:\tRecovered Comm Port");

	iSM->BaseEnv().CompleteState(KErrNone);
	}

void CPsdRecoverCommPort::DoCancel()
/**
Cancell Asynchronous request of comm port state.
*/
	{
	iSM->Context().CancelAsyncRequest(EPacketContextRecoverCommPort);
	}

CAgentStateBase* CPsdRecoverCommPort::NextStateL(TBool /*aContinue*/)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

Ignoring aContinue because the end is in sight now.
If the SM has been asked to reconnect by PPP, we need to go back and 
activate the context again.

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	if (iSM->BaseEnv().IsReconnect())
		{
		return new (ELeave) CPsdActivateContext(iSMObserver,iSM,iDbPsd);
		}
	else
		{
		iSM->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogPacketDataEventTypeUid);
		// request to update and close  log
		iSM->DataLogger()->StopL();	
		return new (ELeave) CPsdCloseLog(iSMObserver,iSM,iDbPsd);
		}
	}

//
//
// CPsdCloseLog
//
// This class ensures that the asynchronous "final data transfer logging" completes 
// before the asynchronous Log Call End happens.

CPsdCloseLog::CPsdCloseLog(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdCloseLog::~CPsdCloseLog()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdCloseLog::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCLOSELOG_STARTSTATE_1, "Packet Data:\tClosing Log");
	// We need to make sure that logging is finished before closing everything.
	// forward iStatus to the logger, which will be responsible to complete iStatus when it is finished.
			iStatus = KRequestPending;
			SetActive();
	iSM->Logger()->LogDataNotifyLastEventUpdate(&iStatus);
		}

void CPsdCloseLog::RunL()
/**
Complete state for Close Log state.
*/
	{
		iSM->BaseEnv().CompleteState(KErrNone);	
	}

void CPsdCloseLog::DoCancel()
/**
Cancell Asynchronous request of close log state.
*/
	{
	iSM->Logger()->Cancel();	//cancel because we will delete everything
	}

CAgentStateBase* CPsdCloseLog::NextStateL(TBool /*aContinue*/)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

Ignoring aContinue because the end is in sight now.
If the context is still active, SM moves to Deactivation state
otherwise proceed to Closure state

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	RPacketContext::TContextStatus contextstatus;
	iSM->Context().GetStatus(contextstatus);
	if (contextstatus==RPacketContext::EStatusActive || contextstatus==RPacketContext::EStatusSuspended)
		return new (ELeave) CPsdDeactivation(iSMObserver,iSM,iDbPsd);
	else
		return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

CPsdDeactivation::CPsdDeactivation(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdDeactivation::~CPsdDeactivation()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdDeactivation::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDDEACTIVATION_STARTSTATE_1, "Packet Data:\tDeactivating Context");

	iSM->Context().Deactivate(iStatus);
	SetActive();
	}

void CPsdDeactivation::RunL()
/**
Complete state for deactivation state.
*/
	{
	__ASSERT_DEBUG(iSM && iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDDEACTIVATION_RUNL_1,"Packet Data:\tDeactivated Context");
		
	iSMObserver->UpdateProgress(EPsdFinishedDeactivation,KErrNone);
	iSM->BaseEnv().CompleteState(KErrNone);

	}

void CPsdDeactivation::DoCancel()
/**
Cancell Asynchronous request of deactivation state.
*/
	{
	iSM->Context().CancelAsyncRequest(EPacketContextDeactivate);
	}	

CAgentStateBase* CPsdDeactivation::NextStateL(TBool /*aContinue*/)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	return new (ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}

//
//
// CPsdClosure
//

CPsdClosure::CPsdClosure(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb)
: CAgentStateBase(*aObserver),iSM(aPsdSM),iDbPsd(aDb)
/**
Constructor
*/
	{
	__ASSERT_DEBUG(aObserver&&aPsdSM&&aDb, User::Invariant());
	}

CPsdClosure::~CPsdClosure()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdClosure::StartState()
/**
Function called by the genconn state machine framework to start the state
*/
	{
	iSubState = EClosureStart;
	JumpToRunl(KErrNone);
	}

void CPsdClosure::RunL()
/**
Complete Disconnection for closure state.
*/
	{
	__ASSERT_DEBUG(iSM && iSMObserver,User::Invariant());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDCLOSURE_RUNL_1,"Packet Data:\tClosing down");

	RTelServer& etel = iSM->TelServer();
	RPhone& phone = iSM->Phone();
	RPacketService& packetNetwork = iSM->PacketNetwork();
	RPacketContext& context = iSM->Context();
	RPacketQoS& qoS = iSM->QoS();

	if(iSubState == EClosureStart)
		{
		if (qoS.SubSessionHandle()!=0)
			{
			qoS.Close();
			}

		if (context.SubSessionHandle()!=0)
			{
			TInt aError=KErrNone;
			if (context.GetLastErrorCause(aError)==KErrNone)
				iSM->ErrorCause()=aError;
			iSubState = EDeletingContext;
			context.Delete(iStatus);
			SetActive();
			return; // break out and wait for delete to complete
			}
		else
			{
			// Advance to end sub state
			iSubState = EClosureEnd;
			}
		}
	if(iSubState == EDeletingContext)
		{
		// context delete completed so now close it down
		context.Close();
		// Advance to end sub state
		iSubState = EClosureEnd;
		}
	// Intentional fallthrough
	if(iSubState == EClosureEnd)
		{
		packetNetwork.Close();
		phone.Close();
		if(iSM->TsyLoaded())
			{
			TBuf<KCommsDbSvrMaxFieldLength> tsyName;
			iSM->BaseEnv().Db()->GetTsyNameL(tsyName);
			User::LeaveIfError(etel.UnloadPhoneModule(tsyName));
			iSM->SetTsyLoaded(EFalse);
			}
		etel.Close();	// Phone module unloaded automatically
		iSMObserver->DisconnectComplete();
		}
	}

void CPsdClosure::DoCancel()
/**
Cancell Asynchronous request of closure state.
*/
	{}

CAgentStateBase* CPsdClosure::NextStateL(TBool /*aContinue*/)
/**
Called by the genconn state machine frame work to move the state machine to the next state
function creates the next state in the state machine

@param TBool aContinue flag indicating if the state machine wishes to continue the connection or close it down
@return CAgentStateBase* pointer to the next state in the state machine the calling function takes ownership of the returned object
*/
	{
	return new(ELeave) CPsdClosure(iSMObserver,iSM,iDbPsd);
	}
