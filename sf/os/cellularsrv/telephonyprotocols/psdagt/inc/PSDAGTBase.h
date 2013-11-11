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
* Header for PSD Agent Factory and Agent.
* 
*
*/



/**
 @file PSDAGTBase.h
 @internalComponent
*/

#ifndef __PSDAGTBase_H__
#define __PSDAGTBase_H__

#include <comms-infras/nifagt.h>
#include <comms-infras/cstatemachineagentbase.h>
#include "MipCdma.h"

const TInt KMajorVersionNumber=8; ///< PSD Agent Major Version Number
const TInt KMinorVersionNumber=0; ///< PSD Agent Minor Version Number
const TInt KBuildVersionNumber=1; ///< PSD Agent Build Version Number

_LIT(KPSDAgentName,"psd"); ///< KPSDAgentName holds the name of PSD Agent

class CPSDAgentFactory : public CNifAgentFactory
/**
A Factory for creating a PSD Agent

@internalComponent
*/
	{
protected:
	void InstallL();
	CNifAgentBase *NewAgentL(const TDesC& aName);
	TInt Info(TNifAgentInfo& aInfo, TInt aIndex) const;
	};


class CPSDAgent : public CStateMachineAgentBase
/**
A PSD Agent

@internalComponent
*/
	{
public:
	static CPSDAgent* NewL();
	virtual ~CPSDAgent();

protected:
	void ConstructL();
	CPSDAgent();

public:
	// pure virtuals from CStateMachineAgent
	void Info(TNifAgentInfo& aInfo) const;
	CAgentSMBase* CreateAgentSML(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDb, TCommDbConnectionDirection aDir);

	// Overridden functions
	void ConnectionComplete(TInt aError);
	void Reconnect();
	void DisconnectComplete();

	virtual void MDPOLoginComplete(TInt aError);
	virtual void MDPOReadPctComplete(TInt aError);
	virtual void MDPODestroyPctComplete(TInt aError);
	virtual void MDPOQoSWarningComplete(TInt aError, TBool aResponse);

	void AgentEvent(TNetworkAdaptorEventType aEventType, TUint aEvent, const TDesC8& aEventData, TAny* aSource=0);

private:

	};

#endif
