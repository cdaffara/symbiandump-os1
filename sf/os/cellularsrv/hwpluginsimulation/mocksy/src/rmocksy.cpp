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
//

#include <e32base.h>
#include "rmocksy.h"

/**
Conect to the server
*/
EXPORT_C TInt RMockSY::Connect()
	{
	return CreateSession(ServerName(),TVersion(1,0,0));
	}

/**
Notify when mockSY is done handling queued call and completion, either because
it handled all posted messages or because there was an error

@param aStatus
*/
EXPORT_C void RMockSY::NotifyTerminated(TRequestStatus& aStatus)
	{
	SendReceive(KNotifyTerminated,aStatus);
	}

/**
Get the next line of the rotating log

@param aLine Empty if there is no more line
*/
EXPORT_C void RMockSY::GetNextLogLine(TDes& aLine)
	{
	TInt err=SendReceive(KGetNextLogLine,TIpcArgs(&aLine));
	ASSERT(err == KErrNone); // just fail if we run out of mem there is not a lot to do in this case
	}


/**
Get MockSY status

@param aPendingEvents number of waiting/pending events
@param aError true if there was an error
*/
EXPORT_C void RMockSY::GetStatus(TBool& aHasWaitingExpect, TBool& aHasPendingComplete, TBool& aHasError)
	{
	TPckg<TBool> hasWaitingExpectPckg(aHasWaitingExpect);
	TPckg<TBool> hasPendingComplete(aHasPendingComplete);
	TPckg<TBool> hasErrorPckg(aHasError);
	TInt err=SendReceive(KGetStatus,TIpcArgs(&hasWaitingExpectPckg,&hasPendingComplete,&hasErrorPckg));
	ASSERT(err == KErrNone); // just fail if we run out of mem there is not a lot to do in this case
	}

/**
Set the next command SY is expected to receive.
param
@param aCmdId
@param aErrorCode
@param aLeave
@param aData
*/
EXPORT_C void RMockSY::DoExpect(TInt aCmdId, TInt aErrorCode, TBool aLeave, const TDesC8& aData)
	{
	TInt err=SendReceive(KExpect,TIpcArgs(aCmdId,aErrorCode,aLeave,&aData));
	ASSERT(err == KErrNone); // just fail if we run out of mem there is not a lot to do in this case
	}

/**
Queue completion to send after the next expected message 

@param aCmdId
@param aErrorCode
@param aDelay
@param aData
*/
EXPORT_C void RMockSY::DoComplete(TInt aCmdId, TInt aErrorCode, TInt aDelay, const TDesC8& aData)
	{
	TInt err=SendReceive(KComplete,TIpcArgs(aCmdId,aErrorCode,aDelay,&aData));
	ASSERT(err == KErrNone); // just fail if we run out of mem there is not a lot to do in this case
	}

/**
Pause MockSY from sending completion till a resume is send. 
Use this when you want to make sure that the Mock will not complete anything until you had time to 
tell the mock what to expect.

*/
EXPORT_C TInt RMockSY::PauseCompletion()
    {
    return Send(KPause);
    }

/**
Resume MockSY completion sending. Use this after you finished telling the mock
what to expect, in order to start completing again.

*/
EXPORT_C TInt RMockSY::ResumeCompletion()
    {
    return Send(KResume);
    }

