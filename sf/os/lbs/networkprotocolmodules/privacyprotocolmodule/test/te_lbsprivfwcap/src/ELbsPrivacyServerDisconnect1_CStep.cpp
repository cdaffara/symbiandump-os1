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
#include "ELbsPrivacyServerDisconnect1_CStep.h"


//TO BE SAFE
IMPORT_C TInt StartDialogThread();

CELbsPrivacyServerDisconnect1Step::CELbsPrivacyServerDisconnect1Step()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	//DEF iTestStepName = _L("CELbsPrivacyServerDisconnect1Step");

	//The server name and IPC number is obtained and all messages are checked Sync
	SR_ServerName		= _L("LBSPrivacyServer");
	SR_MESSAGE_TYPE		=	2;
	SR_MESSAGE_ID		= 2;
	SR_MESSAGE_MASK		= 139280;

	//The iServer_Panic is a unique name from Server,but always truncated to KMaxExitCategoryName
	
	iServer_Panic		=	_L("EPos privacy");

	TCapability caplist1[] = {ECapabilityLocation,ECapabilityReadDeviceData, ECapability_Limit};
	TCapability caplist2[] = {ECapabilityCommDD, ECapability_Limit};
	
	TSecurityInfo info;
	info.Set(RProcess());
	TBool result = EFalse;
	
	// capability policing rules are all caps from list1 OR all caps from list2 
	for (TInt i = 0; caplist1[i] != ECapability_Limit; i++) 
	{
			if (!(info.iCaps.HasCapability(caplist1[i])))
		{
			result=ETrue;
		}
	}
	if (result) // capabilities does NOT match with list1
	{
		result = EFalse;
		for (TInt i = 0; caplist2[i] != ECapability_Limit; i++) 
		{
				if (!(info.iCaps.HasCapability(caplist2[i])))
			{
				result=ETrue;
			}
		}
	}
	
	
	
	iExpect_Rejection = result;
	
	iStepCap			= 139280;

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
TInt CELbsPrivacyServerDisconnect1Step::Exec_SendReceive()
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
			iResult_SR	=	SendReceive(SR_MESSAGE_ID,TIpcArgs(0,0,0,0));
		}

	return iResult_Server;
	}
