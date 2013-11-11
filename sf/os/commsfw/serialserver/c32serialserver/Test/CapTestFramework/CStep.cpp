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
//



//Test Step header
#include "PARAM_MESS_NAME_CStep.h"


//TO BE SAFE
IMPORT_C TInt StartDialogThread();

CPARAM_MESS_NAMEStep::CPARAM_MESS_NAMEStep()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	//DEF iTestStepName = _L("CPARAM_MESS_NAMEStep");

	//The server name and IPC number is obtained and all messages are checked Sync
	SR_ServerName		= _L("PARAM_SVR_NAME");
	SR_MESSAGE_TYPE		=	2;
	SR_MESSAGE_ID		= PARAM_MESS_VALUE;
	SR_MESSAGE_MASK		= PARAM_MESS_CAPMASK;

	//The iServer_Panic is a unique name from Server,but always truncated to KMaxExitCategoryName
	
	iServer_Panic		=	_L("PARAM_SVR_T16_PANIC");

	TCapability cap[] = {ECapabilityPARAM_MESS_NAMECAP, ECapability_Limit};
	
	TSecurityInfo info;
	info.Set(RProcess());
	TInt capMask = 0;
	
	// Dummy Port 2 is deliberately inaccessible and will fail every caps check.
	if( SR_MESSAGE_ID == KCommOpenWithDummy2 )
		{
		iExpect_Rejection = ETrue;
		}
	else
		{
		iExpect_Rejection = EFalse;
		
		for ( TInt i = 0; cap[i] != ECapability_Limit; i ++ ) 
			{
			if ( !( info.iCaps.HasCapability( cap[i] ) ) )
				{
				iExpect_Rejection = ETrue;
				}
			else
				{
				capMask |= 1<<i;		
				}
			}
		}
	
	iStepCap			= PARAM_MESS_CAPMASK;

	//Get a unique thread name
	ChildThread_SR.Format(_L("ChildThread_%S_%d"),&SR_ServerName,SR_MESSAGE_ID);

	}

/*
Exec_SendReceive():
	This Fn is called by the Child Thread
1.	Create a session with the server
2.	Test an SendReceive call
3.	Informs the main thread about the status of the call using
	a.	iSessionCreated, if the a connection is established
	b.	iResult_Server, holds the return value for connection 
	c.	iResult_SR, the return value of SendReceive	call
*/
TInt CPARAM_MESS_NAMEStep::Exec_SendReceive()
	{
	iResult_Server = CreateSession(SR_ServerName,Version(),2);
	
	if (iResult_Server!=KErrNone)
		{

		iResult_Server=StartServer();
		if (iResult_Server!=KErrNone)
			return(iResult_Server);

		iResult_Server = CreateSession(SR_ServerName,TVersion(),2);
		}
	if(iResult_Server == 0)
		{
		iSessionCreated = ETrue;
		
		if(SR_MESSAGE_ID >= 0)
			{
			switch(SR_MESSAGE_ID)
				{
			case ECommDbgMarkHeap:	
			case ECommDbgCheckHeap:
			case ECommDbgMarkEnd:
			case ECommDbgFailNext:
				iResult_SR = TestDebugHeap(SR_MESSAGE_ID);
				break;
			case KCommOpenWithEcuart0:
			case KCommOpenWithEcuart1:
			case KCommOpenWithEcuart2:
			case KCommOpenWithDummy0:
			case KCommOpenWithDummy1:
			case KCommOpenWithDummy2:
				iResult_SR = TestDeferCaps(SR_MESSAGE_ID);
				break;
			default:		
				iResult_SR	=	SendReceive(SR_MESSAGE_ID,TIpcArgs(0,0,0,0));
				break;
				}
			}
		}

	return iResult_Server;
	}

