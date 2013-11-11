// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//Custom command to validate RTC and publish the result using Publish and Subscribe mechanism

/**
 @file
 @internalComponent
 @released
*/

#include "cmdvalidatertc.h"
#include "ssmdebug.h"
#include "ssmuiproviderdll.h"
#include <e32property.h>

CCustomCmdValidateRTC* CCustomCmdValidateRTC::NewL()
	{
	CCustomCmdValidateRTC* self = new (ELeave) CCustomCmdValidateRTC();
	return self;
	}

CCustomCmdValidateRTC::CCustomCmdValidateRTC()
                        :CActive(EPriorityNormal)
    {
    CActiveScheduler::Add(this);
    }

CCustomCmdValidateRTC::~CCustomCmdValidateRTC()
	{
    //Cancel the request if there is any
    ExecuteCancel();
	}

TInt CCustomCmdValidateRTC::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	iPckgValidateRtc = EFalse;
	
	//Connect RSsmRtcAdaptation
	return iSsmRtcAdaptation.Connect();
	}

void CCustomCmdValidateRTC::Close()
	{
	//Cancel the request if there is any
	ExecuteCancel();
	
	//Close RSsmRtcAdaptation
	iSsmRtcAdaptation.Close();
	}

void CCustomCmdValidateRTC::Release()
	{
	delete this;
	}

void CCustomCmdValidateRTC::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Set the user request to pending
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	//Request RTC adaptation to validate RTC
	iSsmRtcAdaptation.ValidateRtc(iPckgValidateRtc, iStatus);
	SetActive();
	}

void CCustomCmdValidateRTC::RunL()
    {
    TInt result = iStatus.Int();
    if (KErrNone == result)
        {
        //Set the PS value for RTC validation
        result = RProperty::Set(CSsmUiSpecific::ValidateRTCPropertyCategory(), 
                                   CSsmUiSpecific::ValidateRTCPropertyKey(), iPckgValidateRtc());
        if(KErrNone != result)
            {
            DEBUGPRINT2A("Setting PS key value for Validating RTC custom cmd failed with error: %d", result);
            }
        }
    CompleteClientRequest(result);
    }

void CCustomCmdValidateRTC::DoCancel()
    {
    //Cancel the request if there is any
    iSsmRtcAdaptation.Cancel();
    }

TInt CCustomCmdValidateRTC::RunError(TInt aError)
    {
    DEBUGPRINT2A("CCustomCmdValidateRTC::RunError received error: %d", aError);
    CompleteClientRequest(aError);

    // We've returned the error via the status so return KErrNone here.
    return KErrNone;
    }

void CCustomCmdValidateRTC::ExecuteCancel()
	{
	Cancel();
	CompleteClientRequest(KErrCancel);
	}

void CCustomCmdValidateRTC::CompleteClientRequest(TInt aError)
    {
    if (iExecuteRequest)
        {
        //Complete the user request with the given completion code
        User::RequestComplete(iExecuteRequest, aError);

        //Reset iExecuteRequest
        iExecuteRequest = NULL; 
        }
    }
