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
 @internalComponent 
*/

#ifndef LOGCHECK_H
#define LOGCHECK_H

#include <logwrap.h>
#include <logview.h>
#include <logcli.h>
#include <e32test.h>

#include "smsstackbaseteststeps.h"

class CSmsLogChecker : public CActive
	{
	public:
		static CSmsLogChecker* NewL(RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority);
		~CSmsLogChecker();

		void CountOriginalIdsL(TRequestStatus& aStatus);
		void CompareNewIdsL(const RArray<TLogId>& aMessageLogIds, TRequestStatus& aStatus);

	private:
		enum TTask
			{
			ENone,
			ECountOriginal,
			ECountNew
			} iTask;

		enum TState
			{
			EIdle,
			EFilter,
			EFirst,
			ENext
			} iState;

		CSmsLogChecker(RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority);
		void ConstructL();
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();

		void Complete(TInt aStatus);
		void StartTaskL(TTask aTask, RArray<TLogId>& aIds, TRequestStatus& aStatus);
		void CompleteTaskL();

	private:

		RFs& iFs;
		CSmsBaseTestStep* iTest;

		const RArray<TLogId>* iMessageLogIds;
		RArray<TLogId> iOriginalIds;
		RArray<TLogId> iNewIds;
		RArray<TLogId>* iCurrentArray;
		TRequestStatus* iReport;

		CLogClient* iClient;
		CLogViewEvent* iView;
		CLogFilter* iFilter;

		TBuf<32> iDirection;
	};

#endif
