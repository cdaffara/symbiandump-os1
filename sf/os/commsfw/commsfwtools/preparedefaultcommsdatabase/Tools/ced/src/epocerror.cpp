// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalComponent
*/

#include "epocerror.h"


TBool
EpocError::Evaluate(TInt err)
/**
Check the return value for an error and set the global error values
on error

@param err Error code
@return ETrue if successful or EFalse
*/
	{
	TInt valid = ETrue;
	
	if (err == KErrNone)
		{
		static TPtrC x = _L("");
		iLastError = KErrNone;
		LastErrorMessage.Set(x);
		}
	else
		{
		static TPtrC x[] = {
			_L("Unable to find the specified object or missing required field values"),
			_L("General (unspecified) error"),
			_L("The operation was cancelled"),
			_L("Not enough memory.  Close some applications and try again"),
			_L("The operation requested is not supported"),
			_L("Bad argument supplied"),
			_L("Total loss of precision"),
			_L("Bad object"),
			_L("Overflow"),
			_L("Underflow"),
			_L("Object already exists"),
			_L("Unable to find the specified folder"),
			_L("Closed"),
			_L("The specified object is currently in use by another program"),
			_L("Server has closed"),
			_L("Server is busy"),
			_L("Completion error"),
			_L("Not ready"),
			_L("Unknown error"),
			_L("Corrupt"),
			_L("Access denied"),
			_L("Locked"),
			_L("Failed to write"),
			_L("Wrong disk present"),
			_L("Unexpected end of file"),
			_L("Disk full"),
			_L("Bad device driver"),
			_L("Bad name"),
			_L("Comms line failed"),
			_L("Comms frame error"),
			_L("Comms overrun error"),
			_L("Comms parity error"),
			_L("Timed out"),
			_L("Failed to connect"),
			_L("Failed to disconnect"),
			_L("Disconnected"),
			_L("Bad library entry point"),
			_L("Bad descriptor"),
			_L("Interrupted"),
			_L("Too big"),
			_L("Divide by zero"),
			_L("Batteries too low"),
			_L("Folder full"),
			_L("Hardware is not available")
		};
		
		// loop through the general error codes
		TInt i = -1;
		TInt j = 0;
		for (i=-1, j=0;i > -45;i--, j++)
			{
			if (err == i)
				{
				iLastError = err;
				LastErrorMessage.Set(x[j]);
				valid = EFalse;
				break;
				}
			}
		
		// error not found
		if (valid)
			{
			iLastError = err;
			LastErrorMessage.Set(_L("Unspecified error occurred"));
			valid = EFalse;
			}
		}
	
	return valid;
	}

