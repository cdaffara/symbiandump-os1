// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Netdial Agent Factory and Agent implementations
// 
//

/**
 @file Nd_Agent.cpp 
*/

#include "Nd_Agent.h"

/**
First ordinal export

@internalComponent
*/
extern "C" EXPORT_C CNifAgentFactory* NewAgentFactoryL()
	{	
	
	return new(ELeave) CNetdialAgentFactory;
	}

//                                   
//  Netdial Agent Factory functions  
//                                   

void CNetdialAgentFactory::InstallL()
/**
Performs any Agent initialisation
Treat as ConstructL()
*/
	{}

CNifAgentBase* CNetdialAgentFactory::NewAgentL(const TDesC& /*aName*/)
/**
Creates a new Netdial Agent

@param aName the name of the Agent to create
*/
	{
	
	return CNetDialAgent::NewL();
	}

TInt CNetdialAgentFactory::Info(TNifAgentInfo& aInfo, TInt /*aIndex*/) const
/**
Retrieves information about this Agent
*/
	{

	aInfo.iName = KNetdialAgentName;
	aInfo.iName.AppendFormat(_L("-AgentFactory[0x%08x]"), this);
	aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);

	return KErrNone;
	}



//                                
//  Netdial Agent Implementation  
//                                


CNetDialAgent::~CNetDialAgent()
/**
Destructor
*/
	{}

CNetDialAgent* CNetDialAgent::NewL()
/**
Static NewL function
*/
	{
	CNetDialAgent* self = new (ELeave) CNetDialAgent();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CNetDialAgent::ConstructL()
/**
2nd Phase Constructor
*/
	{

	// Call base class constructor to create database and dialog processor
	CStateMachineAgentBase::ConstructL();
	}

CAgentSMBase* CNetDialAgent::CreateAgentSML(MAgentNotify& aObserver,CDialogProcessor* aDlgPrc, CCommsDbAccess& aDb, TCommDbConnectionDirection aDir)
/**
Creates CSD agent extension.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to dialog processor.
@param aDb a referecen to CommDB accessor.
@param aDir is direction of the connection.
@exception Leaves if IsDefaultTelNumL() or NewL() leaves, or not enough memory is available.
@exception Panics if connection direction is unknown.
@return a new CDlUpAgXSM, CDirCtAgXSM or CDlInAgXSM object.
*/
	{

	if (aDir==ECommDbConnectionDirectionOutgoing)
		{
		TBool dialUp=EFalse;

		CCommsDbNetDialAccess* db=CCommsDbNetDialAccess::NewL(&aDb);
		CleanupStack::PushL(db);
		db->IsDefaultTelNumL(dialUp);
		CleanupStack::PopAndDestroy(db);

		if (dialUp)
			{
			return CDlUpAgXSM::NewL(aObserver,aDlgPrc,aDb);
			}
		else
			{
			return CDirCtAgXSM::NewL(aObserver,aDlgPrc,aDb);
			}
		}
	else if (aDir==ECommDbConnectionDirectionIncoming)
		{
		return CDlInAgXSM::NewL(aObserver,aDlgPrc,aDb);
		}
	else
		{
		NetDialPanic(EUnknownCallDirection);
		return NULL;
		}
	}

CNetDialAgent::CNetDialAgent()
/**
Default Constructor
*/
	{}

void CNetDialAgent::Info(TNifAgentInfo& aInfo) const
/**
Information about this Agent

@param aInfo on return contains information about the agent
*/
	{
	
	aInfo.iName = KNetdialAgentName;
	aInfo.iName.AppendFormat(_L("-Agent[0x%08x]"), this);
	aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

void CNetDialAgent::MDPOLoginComplete(TInt /*aError*/)
	{
	}

void CNetDialAgent::MDPOReadPctComplete(TInt /*aError*/)
	{
	}

void CNetDialAgent::MDPODestroyPctComplete(TInt /*aError*/)
	{
	}

void CNetDialAgent::MDPOQoSWarningComplete(TInt /*aError*/, TBool /*aResponse*/)
	{
	}

