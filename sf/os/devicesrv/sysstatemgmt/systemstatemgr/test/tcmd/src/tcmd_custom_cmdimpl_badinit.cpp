// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tcustom_cmdimpl_badinit.cpp
// Provides a reference example of a Custom Command dll within the ssma architecture.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include <f32file.h>
#include <ssm/ssmcustomcommand.h>

#include "tcustom_cmdimpl.h"

/**
Constructor
*/	
RefCustomCommand::RefCustomCommand()
	{
	}

	
/**
Destructor
*/	
RefCustomCommand::~RefCustomCommand()
	{
	}
	

/**
Provides initialisation of the dll
@param aRfs 	An initialised File System object
@return	One of the system-wide error codes.
*/	
TInt RefCustomCommand::Initialize(CSsmCustomCommandEnv* aCmdEnv)
	{
	RDebug::Printf("RefCustomCommand::Test Initialising custom command");
	TInt err = KErrNotFound;
	
	if (aCmdEnv)
		{
		const RFs& rfs = aCmdEnv->Rfs();
		
		// do initialisation here
		TFullName name = rfs.FullName();
		RDebug::Print(_L("Test Rfs: [%S]"), &name);
		
		// we force an error here
		err = KErrBadHandle;
		}

	return err;
	}

	
/**
Provides the main execution of the custom command
@param aParams 	Parameters to the custom command
@param aRequest	A status object to be completed
*/	
void RefCustomCommand::Execute(const TDesC8& aParams, TRequestStatus& aRequest)
	{
	aRequest = KRequestPending;
	RDebug::Printf("RefCustomCommand::Test Executing custom command");
	TInt err = KErrNone;
	
	// do main execution here
	TInt i=0;
	TInt len = aParams.Length();
	 RDebug::Print(_L("Params %d : %d"), i, aParams[i]);
	for (i=0;i<len;i++)
		{
		RDebug::Print(_L("Test Params %d : %d"), i, aParams[i]);
		}
	
	if (len && aParams[0] == '0')
		{
		RDebug::Printf("Test Completing with error...");
		TRequestStatus* status = &aRequest;
		User::RequestComplete(status, KErrBadHandle);
		}
	else
		{
		RDebug::Printf("Test Completing with success...");
		TRequestStatus* status = &aRequest;
		User::RequestComplete(status, err);
		}
	}

	
/**
Provide any cleanup in the custom command
*/	
void RefCustomCommand::Close()
	{
	RDebug::Printf("RefCustomCommand::Test Closing custom command");
	
	// close any handles, free temporary resources here
	// ...
		
	}


/**
Release resources associated with the custom command
*/	
void RefCustomCommand::Release()
	{
	RDebug::Printf("RefCustomCommand::Test Releasing custom command");
	delete this;
	}


/**
Cancels any outstanding Execute() operation
*/	
void RefCustomCommand::ExecuteCancel()
	{
	RDebug::Printf("RefCustomCommand::Test Cancelling custom command");
	
	// cancel the outstanding request
	
	}

