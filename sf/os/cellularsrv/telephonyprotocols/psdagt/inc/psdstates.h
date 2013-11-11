/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Header for PSD States
* 
*
*/



/**
 @file PSDStates.h
 @internalComponent
*/


#ifndef __PSDSTATES_H__
#define __PSDSTATES_H__

#include <comms-infras/dialogprocessor.h>
#include <etelpckt.h>
#include "psdagt.h"
#include "PSDAGTBase.h"

class MPsdEnv;
class MPsdCommDbAccess;
class CPsdInitBase : public CAgentStateBase
/**
Base class for classes performing the initialisation state for both
incomming and outgoing connections

@internalComponent
*/
	{
public:
	CPsdInitBase(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdInitBase();
	virtual void StartState();
protected:
	virtual void DoStartStateL();
	void GetPhoneInfoL(const TDesC& aLoadedTsyName, RTelServer::TPhoneInfo& aInfo);
	virtual void RunL();
	virtual void DoCancel();
protected:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};

class CPsdOutInit : public CPsdInitBase
/**
Class implementing the initialisation state for outgoing connections

@internalComponent
*/
	{
public:
	CPsdOutInit(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdOutInit();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	};


#ifndef INCOMING_NOT_SUPORTED

class CPsdInInit : public CPsdInitBase
/**
Class implementing the initialisation state for incomming connections

@internalComponent
*/
	{
public:
	CPsdInInit(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdInInit();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	virtual void DoStartStateL();
	};
#endif // #ifndef INCOMING_NOT_SUPORTED


class CPsdCheckConfig : public CAgentStateBase
/**
Base class for classes performing the config checking state for both
incomming and outgoing connections

@internalComponent
*/
	{
public:
	CPsdCheckConfig(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdCheckConfig();
	virtual void StartState();
protected:
	virtual void DoInitL();
	virtual void RunL();
	virtual void DoCancel();
protected:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};

class CPsdOutCheckConfig : public CPsdCheckConfig
/**
Class implementing the config checking state for outgoing connections

@internalComponent
*/
	{
public:
	CPsdOutCheckConfig(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdOutCheckConfig();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	};


#ifndef INCOMING_NOT_SUPORTED

class CPsdInCheckConfig : public CPsdCheckConfig
/**
Class implementing the config checking state for incomming connections

@internalComponent
*/
	{
public:
	CPsdInCheckConfig(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdInCheckConfig();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	};
#endif // #ifndef INCOMING_NOT_SUPORTED


class CPsdNetworkCheck : public CAgentStateBase
/**
Class implementing the network check state. Responsible for ensuring that 
the network is in a suitable state before advancing the state machine

@internalComponent
*/
	{
public:
	CPsdNetworkCheck(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdNetworkCheck();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	void DoNetworkCheck();
private:
	RPacketService::TRegistrationStatus iRegStatus;
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	enum
	{EGettingInitialStatus,EWaitingForStatusChange} iSubState;
	};


#ifndef INCOMING_NOT_SUPORTED

class CPsdWaitForIncoming : public CAgentStateBase
/**
Class implementing the wait for incomming state. Used in incomming connections
it is responsible for listening out for an incomming connection, validating the 
connection and advancing the state machine.

@internalComponent
*/
	{
public:
	CPsdWaitForIncoming(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdWaitForIncoming();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	RPacketContext::TProtocolType iPdpTypeRequested;
	RPacketContext::TProtocolAddress iAddressRequested;
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};
#endif // #ifndef INCOMING_NOT_SUPORTED


class CPsdCreateContext : public CAgentStateBase
/**
Class implementing the create context state. Responsible for creating the 
context and configuring it with the correct parameters

@internalComponent
*/
	{
public:
	CPsdCreateContext(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdCreateContext();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};

class CPsdCreateQoS : public CAgentStateBase
/**
Class implementing the create QoS state. Responsible for creating the 
QoS and configuring it with the correct parameters

@internalComponent
*/
	{
public:
	CPsdCreateQoS(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdCreateQoS();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};

class CPsdActivateContext : public CAgentStateBase
/**
Class implementing the activate context state. Responsible for activating
the previously configured context

@internalComponent
*/
	{
public:
	CPsdActivateContext(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdActivateContext();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	enum
	{EActivatingContext,ELoaningCommPort} iSubState;
	RCall::TCommPort iCommport;
	};

class CDataLogger;

class CPsdOpen : public CAgentStateBase, public MPsdContextChangeObserver, public MDialogProcessorObserver
/**
Class implementing the open state. Responsible for maintaining the connection
reporting progress and logging

@internalComponent
*/
	{
public:
	static CPsdOpen* NewL(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdOpen();
	// From CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	// from MPsdContextChangeObserver
	virtual void ReportContextBelowAcceptableQuality();
	virtual void ReportError(TInt aError);
	 // from MDialogProcessorObserver
	virtual void MDPOQoSWarningComplete(TInt aError, TBool aResponse);
private:
	void RequestStatusChange();
	CPsdOpen(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	void ConstructL();
	void LogActive();
	void WatchForConfigChangesL();
protected:
	void RunL();
	void DoCancel();
private:
	RPacketContext::TContextStatus iContextStatus;
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	TBool iEventLoggerStarted;
	CPsdContextChangeMonitor* iContextChangeMonitor;
	};



class CPsdRecoverCommPort : public CAgentStateBase
/**
Class implementing the recover comm port state. First state during
disconnection it is responsible for recovering the previously loaned
comm port

@internalComponent
*/
	{
public:
	CPsdRecoverCommPort(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdRecoverCommPort();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};


class CPsdCloseLog : public CAgentStateBase
/**
Class implementing the close log state. Responsible for the discontinuation
of logging

@internalComponent
*/
	{
public:
	CPsdCloseLog(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdCloseLog();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	enum
	{EStart,ELoggingFinalValues,EEndingLog} iSubState;
	};


class CPsdDeactivation : public CAgentStateBase
/**
Class implementing the deactivation state. Responsible for deactivating the
context

@internalComponent
*/
	{
public:
	CPsdDeactivation(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdDeactivation();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	};


class CPsdClosure : public CAgentStateBase
/**
Class implementing the closure state. Responsible for cleaning up all the
resources used for the connection

@internalComponent
*/
	{
public:
	CPsdClosure(MAgentStateMachineEnv* aObserver,MPsdEnv* aPsdSM, MPsdCommDbAccess* aDb);
	virtual ~CPsdClosure();
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
protected:
	void RunL();
	void DoCancel();
private:
	MPsdEnv* iSM;
	MPsdCommDbAccess* iDbPsd;
	enum
	{EClosureStart,EDeletingContext,EClosureEnd} iSubState;
	};

#endif
