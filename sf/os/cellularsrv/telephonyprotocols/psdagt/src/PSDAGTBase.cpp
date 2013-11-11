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
// PSD Agent Factory and Agent implementations
// 
//

/**
 @file PSDAGTBase.cpp
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PSDAGTBaseTraces.h"
#endif

#include "PSDAGTBase.h"
#include "psdagt.h"

/**
First ordinal export

@internalComponent
@return an object of class CPSDAgentFactory.
*/
extern "C" EXPORT_C CNifAgentFactory* NewAgentFactoryL()
	{	
	
	return new(ELeave) CPSDAgentFactory;
	}

void CPSDAgentFactory::InstallL()
/**
PSD Agent Factory functions
Perform any Agent initialisation

Treat as ConstructL()
*/
	{}

CNifAgentBase* CPSDAgentFactory::NewAgentL(const TDesC& /*aName*/)
/**
Create a new PSD Agent

@param aName the name of the Agent to create
@return a newly created object of class CPSDAgent
*/
	{
	
	return CPSDAgent::NewL();
	}

TInt CPSDAgentFactory::Info(TNifAgentInfo& aInfo, TInt /*aIndex*/) const
/**
Retrieve information about this Agent

@param aInfo, information about agent. 
@return KErrNone if information retrieved successfully.
*/
	{

	aInfo.iName = KPSDAgentName;
	aInfo.iName.AppendFormat(_L("-AgentFactory[0x%08x]"), this);
	aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);

	return KErrNone;
	}


// PSD Agent Implementation  
                               
CPSDAgent::~CPSDAgent()
/**
Destructor
*/
	{

	}

CPSDAgent* CPSDAgent::NewL()
/**
Static NewL function creating an object of CPSDAgent, pushing it to the clean up stack and popping it out.

@return a pointer to class CPSDAgent.
*/
	{
	CPSDAgent* self = new (ELeave) CPSDAgent();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CPSDAgent::ConstructL()
/**
2nd Phase Construction
*/
	{

	// Call base class constructor to create database and dialog processor
	CStateMachineAgentBase::ConstructL();
	}

CAgentSMBase* CPSDAgent::CreateAgentSML(MAgentNotify& aObserver,CDialogProcessor* aDlgPrc, CCommsDbAccess& aDb, TCommDbConnectionDirection aDirection)
/**
Creates PSD agent extension.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to dialog processor.
@param aDb a referecen to CommDB accessor.
@param aDir is direction of the connection.
@return a new CPsdOutSM or CPsdInSM object.
@exception Leaves if NewL() leaves, or not enough memory is available.
@exception Panics if connection direction is unknown.
*/
	{
	if (aDirection==ECommDbConnectionDirectionOutgoing)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDAGENT_CREATEAGENTSML_1,"GPRS:\tCreating a new PSD state machine - Outbound");
		return CPsdOutSM::NewL(aObserver,aDlgPrc,aDb);
		}
	else 
		{
#ifndef INCOMING_NOT_SUPORTED
		__ASSERT_ALWAYS(aDirection==ECommDbConnectionDirectionIncoming, PanicAgx(EPsdBadDirection));
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPSDAGENT_CREATEAGENTSML_2,"GPRS:\tCreating a new PSD state machine - Inbound");
		return CPsdInSM::NewL(aObserver,aDlgPrc,aDb);
#else
		User::Leave(KErrNotSupported);
		return NULL;
#endif
		}
	}

CPSDAgent::CPSDAgent()
/**
Default Constructor
*/
	{}

void CPSDAgent::Info(TNifAgentInfo& aInfo) const
/**
Information about this Agent
 
@param aInfo on return contains information about the agent
*/
	{
	
	aInfo.iName = KPSDAgentName;
	aInfo.iName.AppendFormat(_L("-Agent[0x%08x]"), this);
	aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

void CPSDAgent::ConnectionComplete(TInt aError)
/**
Establish the connection with the agent.
Override previous definition to add mobile IP reconnection functionality

@param aError, error code if connection is not completed.
*/
	{
	CStateMachineAgentBase::ConnectionComplete(aError);
	}

void CPSDAgent::Reconnect()
/**
Reconnecting to the agent if link is down.
Override previous definition to add mobile IP reconnection functionality
*/
	{

	}

void CPSDAgent::DisconnectComplete()
/**
It is called when Shutdown of packet switched connection using the values in the CommDB is complete.
*/
	{
	CStateMachineAgentBase::DisconnectComplete();
	}

void CPSDAgent::MDPOLoginComplete(TInt /*aError*/)
/**
Log in complete.

@param aError, a error code for completition.
*/
	{
	}

void CPSDAgent::MDPOReadPctComplete(TInt /*aError*/)
/**
Packet read complete.

@param aError, a error code for completition.
*/
	{
	}

void CPSDAgent::MDPODestroyPctComplete(TInt /*aError*/)
/**
Packet destruction complete.

@param aError, a error code for completition.
*/
	{
	}

void CPSDAgent::MDPOQoSWarningComplete(TInt /*aError*/, TBool /*aResponse*/)
/**
Callback from the QoS warning dialog

@param aError indication if an error occured in the dialog
@param flag indicating the response from the dialog. ETrue means disconnect
*/
	{
	}

void CPSDAgent::AgentEvent(TNetworkAdaptorEventType aEventType, TUint aEvent, const TDesC8& aEventData, TAny* aSource)
	{
	iNotify->AgentEvent( aEventType, aEvent, aEventData, aSource);
	}
