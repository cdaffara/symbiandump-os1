// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "logcheck.h"

CSmsLogChecker* CSmsLogChecker::NewL(RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority)
	{
	CSmsLogChecker* self = new (ELeave) CSmsLogChecker(aFs, aTest, aPriority);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop(self);

	return self;
	}

void CSmsLogChecker::ConstructL()
	{
	iClient = CLogClient::NewL(iFs);
	iView = CLogViewEvent::NewL(*iClient, Priority());
	iFilter = CLogFilter::NewL();

	iFilter->SetEventType(KLogShortMessageEventTypeUid);
	iClient->GetString(iDirection, R_LOG_DIR_IN);
	iFilter->SetDirection(iDirection);
	}

CSmsLogChecker::CSmsLogChecker(RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority)
	:CActive(aPriority), iFs(aFs), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}

CSmsLogChecker::~CSmsLogChecker()
	{
	Cancel();
	iOriginalIds.Close();
	iNewIds.Close();

	delete iView;
	delete iFilter;
	delete iClient;
	}

void CSmsLogChecker::CountOriginalIdsL(TRequestStatus& aStatus)
/**
 *  Finds all incoming SMS Log Events,
 *  and appends each LogId to iOriginalIds.
 *  
 *  This function must be called before any new log events are added
 *  or SMS messages received.
 */
	{
//	__UHEAP_MARK;
	iOriginalIds.Reset();

	iNewIds.Reset();
	iMessageLogIds = NULL;

	StartTaskL(ECountOriginal, iOriginalIds, aStatus);
//	__UHEAP_MARKEND;
	}

void CSmsLogChecker::CompareNewIdsL(const RArray<TLogId>& aMessageLogIds, TRequestStatus& aStatus)
/**
 *  Compares array aMessageLogIds with the "new" incoming SMS Log Events.
 *  
 *  The "new" incoming SMS Log Events are determined by taking the difference between
 *  the current Log Events in the Log Engine and array iOriginalIds.
 *  
 *  Pre: CountOriginalIdsL() has already been called.
 */
	{
	iMessageLogIds = &aMessageLogIds;
	iNewIds.Reset();
	StartTaskL(ECountNew, iNewIds, aStatus);
	}

void CSmsLogChecker::Complete(TInt aError)
	{
	//INFO_PRINTF6(_L("CSmsLogChecker Complete Task %d Error %d Orig %d New %d Msg %d"), iTask, aError, iOriginalIds.Count(), iNewIds.Count(), iMessageLogIds ? iMessageLogIds->Count() : 0))
	iTest->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,_L("CSmsLogChecker Complete Task %d Error %d Orig %d New %d Msg %d\n"), iTask, aError, iOriginalIds.Count(), iNewIds.Count(), iMessageLogIds ? iMessageLogIds->Count() : 0);

	if (iReport)
		{
		User::RequestComplete(iReport, aError);
		}

	iTask = ENone;
	iState = EIdle;
	iMessageLogIds = NULL;
	}

void CSmsLogChecker::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	TBool found = EFalse;

	switch (iState)
		{
		case EFilter:

			found = iView->FirstL(iStatus);

			if (found)
				{
				iState = EFirst;
				SetActive();
				}
			else
				{
				CompleteTaskL();
				}

			break;

		case EFirst:
		case ENext:

			User::LeaveIfError(iCurrentArray->Append(iView->Event().Id()));
			found = iView->NextL(iStatus);

			if (found)
				{
				iState = ENext;
				SetActive();
				}
			else
				{
				CompleteTaskL();
				}

			break;

		default:
			Complete(KErrGeneral); //should not get here
			break;
		}
	}

void CSmsLogChecker::CompleteTaskL()
	{
	//INFO_PRINTF2(_L("CSmsLogChecker CompleteTask Task %d"), iTask);
	iTest->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,_L("CSmsLogChecker CompleteTask Task %d\n"), iTask );

//    __UHEAP_MARK;
	switch (iTask)
		{
		case ECountOriginal:

			Complete(KErrNone);
			break;

		case ECountNew:
			{
			TInt count = iNewIds.Count();

			while (count--)
				{
				if (iOriginalIds.Find(iNewIds[count]) != KErrNotFound)
					iNewIds.Remove(count);
				}

			count = iNewIds.Count();
			TInt err = KErrNone;

			while (count-- && !err)
				{
				if (iMessageLogIds->Find(iNewIds[count]) == KErrNotFound)
					err = KErrOverflow;
				}

			count = iMessageLogIds->Count();

			while (count-- && !err)
				{
				if (iNewIds.Find((*iMessageLogIds)[count]) == KErrNotFound)
					err = KErrUnderflow;
				}

			Complete(err);
			break;
			}

		default:
			Complete(KErrGeneral); //should not get here
			break;
		}
//	__UHEAP_MARKEND;
	}

TInt CSmsLogChecker::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void CSmsLogChecker::DoCancel()
	{
	switch (iState)
		{
		case EFilter:
		case EFirst:
		case ENext:
			iView->Cancel();
			break;

		default:
			break; //should not get here!!
		}

	Complete(KErrCancel);
	}

void CSmsLogChecker::StartTaskL(TTask aTask, RArray<TLogId>& aIds, TRequestStatus& aStatus)
	{
	//INFO_PRINTF2(_L("CSmsLogChecker StartTask Task %d"), aTask);
	iTest->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,_L("CSmsLogChecker StartTask Task %d\n"),aTask );

	iReport = &aStatus;
	aStatus = KRequestPending;
//	__UHEAP_MARK;
	iTask = aTask;
	iCurrentArray = &aIds;
	iState = EFilter;
//	 __UHEAP_MARKEND;


	TBool found = iView->SetFilterL(*iFilter, iStatus);


	if (found)
		{
		SetActive();
		}
	else
		{
		CompleteTaskL();
		}
	}
