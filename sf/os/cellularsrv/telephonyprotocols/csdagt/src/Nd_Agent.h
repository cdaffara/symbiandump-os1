/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* An Agent that owns a NetDial State Machine
* Header for NetDial Agent Factory and Agent.
* 
*
*/



/**
 @file ND_AGENT.H
 @internalComponent
*/

#if !defined(__ND_AGENT_H__)
#define __ND_AGENT_H__

#include <comms-infras/cstatemachineagentbase.h>
#include "Nd_Bases.h"
#include "ND_DIRCT.H"
#include "ND_DLIN.H"
#include "ND_DLUP.H"
#include "ND_DBACC.H"

const TInt KMajorVersionNumber=8;
const TInt KMinorVersionNumber=0;
const TInt KBuildVersionNumber=1;
_LIT(KNetdialAgentName,"csd");

class CNetdialAgentFactory : public CNifAgentFactory
/**
A Factory for creating a NetDial Agent
*/
	{
protected:
	void InstallL();
	CNifAgentBase *NewAgentL(const TDesC& aName);
	TInt Info(TNifAgentInfo& aInfo, TInt aIndex) const;
	};

class CNetDialAgent : public CStateMachineAgentBase
/**
A NetDial Agent
*/
	{
public:
	static CNetDialAgent* NewL();
	virtual ~CNetDialAgent();

protected:
	void ConstructL();
	CNetDialAgent();

public:
	// pure virtuals from CStateMachineAgent
	void Info(TNifAgentInfo& aInfo) const;
	CAgentSMBase* CreateAgentSML(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDb, TCommDbConnectionDirection aDir);

	virtual void MDPOLoginComplete(TInt aError);
	virtual void MDPOReadPctComplete(TInt aError);
	virtual void MDPODestroyPctComplete(TInt aError);
	virtual void MDPOQoSWarningComplete(TInt aError, TBool aResponse);

	};

#endif
