/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#include <e32def.h>

#include "DataDealer.h"
#include "BreathSaver.h"

CBreathSaver* CBreathSaver::NewL()
    {
    CBreathSaver* self = new (ELeave) CBreathSaver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CBreathSaver::~CBreathSaver()
    {
    // Free DataDealer
	if (iDataDealer)
		{
		delete iDataDealer;
		iDataDealer = 0;
		}
    // Close handle to File server session
    iRFsession.Close();
    }

/**
 * Start function for data dealer
 * @param aKey			Data dealer key
 * @return				TInt. Error code.
 */
TInt CBreathSaver::StartDealer(const TUint aKey)
	{
	TInt error = KErrNone;
    // return error if DataDealer is already started
    if (iDealerStarted != EFalse)
        {
        error = KErrLocked;
        }
    else
    	{
    	// iDataDealer is deleted in End-function or in destructor and
    	// Start cannot be called again before End is called first
        TRAP (error, iDataDealer = CDataDealer::NewL(ETrue, aKey));
        // NewL failed
        if (error == KErrNone)
            {
            // DataDealer is started
        	iDealerStarted = ETrue;
        	}        
    	}
    
	return error;
	}

/**
 * Start function for file saving. This function replaces the file if it already exists
 * @param aFileName		File name
 * @return				TInt. Error code.
 */
TInt CBreathSaver::StartFile(const TDesC &aFileName)
    {
    TInt error = KErrNone;
    // Check that start is not called when somebody else have already called start
    // but not end
    if (iFileStarted != EFalse)
        {
        error = KErrLocked;
        }
    else
    	{
    	error = iFile.Replace(iRFsession,aFileName,EFileWrite|EFileStream);
        // Check that file replase was successful
        if (error == KErrNone)
            {
            // All must be fine so lets start file
            iFileStarted = ETrue;
            }
    	}
    return error;
    }

/**
 * Appends Buffer into a started file.
 * @param aBuffer		Buffer to be appened.
 * @return				TInt. Error code.
 */
TInt CBreathSaver::Append(TDes8 &aBuffer)
    {
	// If nothing is started then nothing should be appended
    if (iFileStarted == EFalse && iDealerStarted == EFalse)
        {
        return KErrNotFound;
        }

    TInt error = KErrNone;

    // Check if file is started
    if (iFileStarted)
        {
        // Add more to existing file
        error = iFile.Write(aBuffer);
		if (error != KErrNone)
			{
			return error;
			}
        }
    // Check if DataDealer is started
    if (iDealerStarted)
		{
		// ProduceDataL waits until consumer has devoured all the previously
		// produced data
		TRAP (error, iDataDealer->ProduceDataL(aBuffer));
		if (error != KErrNone)
			{
			return error;
			}
		}
    // more types can be added here
    return error;
    }

/**
 * Ends function for file saving.
 * @return				TInt. Error code.
 */
TInt CBreathSaver::End()
    {
    TInt error = KErrNone;
	// If nothing is started then nothing should be ended
    if (iFileStarted == EFalse && iDealerStarted == EFalse)
        {
        error = KErrNotFound;
        }
    else
    	{
        // Check if file is started
        if (iFileStarted)
            {
            // Flush and close file
            error = iFile.Flush();
            iFile.Close();
            // Now new file can be started
    		iFileStarted = EFalse;
            }
        // Check if DataDealer is started
    	if (iDealerStarted)
    		{
            // free DataDealer, new dealer is born(captured?) in start function
    		delete iDataDealer;
    		iDataDealer = 0;
            // Now new data dealer can be started
    		iDealerStarted = EFalse;
    		}
    	}
    return error;
    }

CBreathSaver::CBreathSaver()
    {
    iFileStarted = EFalse;
	iDealerStarted = EFalse;
	iDataDealer = 0;
    }

void CBreathSaver::ConstructL()
    {
    // Open file server session
    // Session is closed in destructor
    User::LeaveIfError(iRFsession.Connect());
    }

