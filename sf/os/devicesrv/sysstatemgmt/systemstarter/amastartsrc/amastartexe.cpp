// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// amastarterexe.cpp
// 
//


#include <e32std.h>
#include "amastartexe_patch.h"
#include "amastart.h"

LOCAL_C void AmaStarterL()
	{
	//The application should have capabilities at least what AMAStarter has (PowerMgmt, ReadDeviceData, ProtServ)
	//to prevent a process being able to get AMAStarter to perform operations it doesn't have the capabilities for. 
	//The application should also have SID as expected to ensure that AMAStarter.exe cannot be launched by a malicious process 
	if(!User::CreatorHasCapability(ECapabilityPowerMgmt, ECapabilityProtServ) ||
		!User::CreatorHasCapability(ECapabilityReadDeviceData)||
		(User::CreatorSecureId() != KSysStartSID))
		{
		User::Leave(KErrPermissionDenied); // make sure we are being called by a trusted app for startup purposes
		}	
	
	CAmaStart* const amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);

	//default DSC
	TUid id = TUid::Uid(KDefaultSymbianDsc);
	const TInt commandSize=User::CommandLineLength();
	if (commandSize >0)
		{
		RBuf command;
		command.CreateL(commandSize);
		CleanupClosePushL(command);

		//get the command line argument
		User::CommandLine(command);

		TLex lex(command);
		TUint dscId=0;
		
		//Parses the string to extract a TUint32, using hexadecimal
		User::LeaveIfError(lex.Val(dscId, EHex));
		id=TUid::Uid(dscId);
	
		CleanupStack::PopAndDestroy(&command);
		}
		
	amaStart->StartL(id);
	CleanupStack::PopAndDestroy(amaStart);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();
	
	TInt error=KErrNoMemory;

	if (theTrapCleanup)
		{
		TRAP(error, AmaStarterL());		
		delete theTrapCleanup;
		}
	
	__UHEAP_MARKEND; // check no memory leak

	return(error);
	}
