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
* Header for NetDial dial in state machine.
* 
*
*/



/**
 @file Nd_dlin.h
 @internalComponent
*/

#ifndef __ND_DLIN_H__
#define __ND_DLIN_H__

#include "Nd_Bases.h"

// Forward declarations

class CCommsDbAccess;
class CTelServerProcessor;
class CEventLogger;
class CNetDialScript;

class CDlInAgXSM : public CNetdialSM
/**
Dial In state machine.

@internalComponent
*/
	{
public:
	static CDlInAgXSM* NewL(MAgentNotify& aControllerObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CDlInAgXSM();
	// CNetdialSM Inheritance Polymorphism
	virtual TBool UseScript() const;
	virtual void SetUpScriptL();
	virtual TBool IsReconnect() const;
	// CAgentSMBase Inheritance
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual TInt IncomingConnectionReceived();
private:
	CDlInAgXSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual void ConstructL();
	};

#endif
