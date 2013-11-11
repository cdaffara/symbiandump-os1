// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "openwfcjob.h"
#include "openwfcjobmanager.h"

TOpenWfcJob::TOpenWfcJob(COpenWfcJobManger& aJobManager):
iJobManager(aJobManager),
iJobId(EInvalidJobId), 
iCallingThread(KNullThreadId),
iCompleteRequest(NULL)
	{
	
	}

void TOpenWfcJob::Set(EOpenWfcJobId aJob,
		              TThreadId aThreadId,
		              TRequestStatus* aRequest)
	{
	iJobId = aJob; 
	iCallingThread = aThreadId;
	iCompleteRequest =aRequest;
    JQLOG(("** TOpenWfcJob::Sett() : iCompleteRequest(0x%x) iJobId(%d)", aRequest, aJob));
	}


void TOpenWfcJob::Reset()
	{
	iJobId = EInvalidJobId; 
	iCallingThread = KNullThreadId;
	iCompleteRequest =NULL;
	}

TOpenWfcJob::~TOpenWfcJob()
	{
	}


void TOpenWfcJob::Run()
	{
	switch (iJobId)
		{
		case EComposeJobId:
            JQLOG(("** LAUNCH * TOpenWfcJob::Run EComposeJobId"));
			iJobManager.DoComposeJob(*this);
			break;
		case EPauseCompositionJobId:
            JQLOG(("** LAUNCH * TOpenWfcJob::Run EPauseCompositionJobId"));
			iJobManager.DoPauseCompositionJob(*this);
			break;
		case EResumeCompositionJobId:
            JQLOG(("** LAUNCH * TOpenWfcJob::Run EResumeCompositionJobId"));
			iJobManager.DoResumeCompositionJob(*this);
			break;
		default:
		    JQLOG(("** ERROR * TOpenWfcJob::Run jobId(%d)", iJobId));
			break;
		}
	}

void TOpenWfcJob::CompleteRequest(TInt aResult)
    {
    if (iCompleteRequest)
        {
        RThread thread;
        
        if (thread.Open(iCallingThread) == KErrNone)
            {
            JQLOG(("** TOpenWfcJob::CompleteRequest(): CompleteReq(0x%x))", iCompleteRequest));
            thread.RequestComplete(iCompleteRequest, aResult);
            thread.Close();
            }
        }
    }
