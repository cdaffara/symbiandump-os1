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
* Declares direct connection state machine class.
* 
*
*/



/**
 @file Nd_dirct.h
 @internalComponent
*/

#ifndef __ND_DIRCT_H__
#define __ND_DIRCT_H__

#include <comms-infras/cagentsmbase.h>
#include "Nd_Bases.h"

// Forward declarations

class CCommsDbNetDialAccess;
class CNetDialScript;


class CDirCtAgXSM : public CNetdialSM
/**
Direct Connection state machine.

@internalComponent
*/
	{
public:
	static CDirCtAgXSM* NewL(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CDirCtAgXSM();
	//CNetdialSM Inheritance
	virtual inline CEventLogger* Logger();
	virtual void SetUpScriptL();

	// Derived from CAgentSMBase 
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual TInt IncomingConnectionReceived();
private:
	CDirCtAgXSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual void ConstructL();
	};

inline CEventLogger* CDirCtAgXSM::Logger()
/**
Default implementation for direct state logger.
*/
	{
	NetDialPanic(ELoggerNotDefined);
	return NULL;
	}

#endif
