// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides a reference example of a Custom Command dll within the ssma architecture.
// 
//


#include "tcustom_cmdimpl.h"
#include "ssmdebug.h"

/**
Constructor
*/	
RefCustomCommand::RefCustomCommand()
	{
	//initilises the count to zero.
	iCount=0;
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
	DEBUGVERBOSE1A("Initialising custom command");
	TInt err = KErrNone;
	// A count to maintain the number of calls made to this Initialize() method.
	iCount++;
	
	if (aCmdEnv)
		{	
		// Creates 3 sessions with the File Server. When this method gets called for the first time
		// it returns 'KErrNoMemory' which makes the BIC's Initilize to fail. BIC tries to Initilize
		// this DLL again and again until the number of retries equals to number of attempts made.
		// On second attempt, it returns 'KErrNone'.
		err = iFs1.Connect();
		err = iFs2.Connect();
		err = iFs3.Connect();
					
		if(iCount<2)
			err = KErrNoMemory;	// On first call, forces an error here.
		else
			err = KErrNone;		// On second call, returns successfully.
		
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
	DEBUGVERBOSE1A("Executing custom command");
	TInt err = KErrNone;
	
	// do main execution here
	TInt i=0;
	TInt len = aParams.Length();
	DEBUGPRINT2A("Length %d", len);
	for (i=0;i<len;i++)
		{
		DEBUGPRINT3A("Params %d : %d", i, aParams[i]);
		}
	
	if (len && aParams[0] == '0')
		{
		DEBUGVERBOSE1A("Completing with error...");
		TRequestStatus* status = &aRequest;
		User::RequestComplete(status, KErrBadHandle);
		}
	else
		{
		DEBUGVERBOSE1A("Completing with success...");
		TRequestStatus* status = &aRequest;
		User::RequestComplete(status, err);
		}
	}

	
/**
Provide any cleanup in the custom command
*/	
void RefCustomCommand::Close()
	{
	DEBUGVERBOSE1A("Closing custom command");
	
	// closes all the opened resource handles.
	iFs1.Close();
	iFs2.Close();
	iFs3.Close();
	}


/**
Release resources associated with the custom command
*/	
void RefCustomCommand::Release()
	{
	DEBUGVERBOSE1A("Releasing custom command");
	delete this;
	}


/**
Cancels any outstanding Execute() operation
*/	
void RefCustomCommand::ExecuteCancel()
	{
	DEBUGVERBOSE1A("Cancelling custom command");
	
	// cancel the outstanding request
	
	}

