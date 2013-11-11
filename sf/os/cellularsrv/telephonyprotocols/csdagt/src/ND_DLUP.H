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
* Header for NetDial dial up state machine.
* 
*
*/



/**
 @file Nd_dlup.h
 @internalComponent
*/

#if !defined(__ND_DLUP_H__)
#define __ND_DLUP_H__

#include "Nd_Bases.h"

// Forward declarations
class CTelServerProcessor;
class CEventLogger;
class CNetDialScript;

class CDlUpAgXSM : public CNetdialSM
/**
Dial Up state machine

@internalComponent
*/
	{
public:
	static CDlUpAgXSM* NewL(MAgentNotify& aControllerObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CDlUpAgXSM();
	//CNetdialSM Inheritence
	virtual void SetUpScriptL();
	// Derived from CAgentSMBase
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual TInt IncomingConnectionReceived();
private:
	CDlUpAgXSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual void ConstructL();
	};

#endif
