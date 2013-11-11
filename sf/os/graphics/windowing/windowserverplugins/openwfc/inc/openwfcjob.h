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

#ifndef OPENWFCJOB_H_
#define OPENWFCJOB_H_

#include <e32std.h>
#include <WF/wfc.h>

#define KNullThreadId 0

class COpenWfcJobManger;

struct TOpenWfcJob
	{
	enum EOpenWfcJobId
		{
			EInvalidJobId = 0,
			EComposeJobId,
			EPauseCompositionJobId,
			EResumeCompositionJobId,
		};
	
	TOpenWfcJob(COpenWfcJobManger& aJobManager);
	

	~TOpenWfcJob();
	
	void Set(EOpenWfcJobId aJob, 
			 TThreadId aThreadId = KNullThreadId, 
			 TRequestStatus* aRequest = NULL);
	
	void Reset();
	
	EOpenWfcJobId JobId();
	
	void CompleteRequest(TInt aResult);
	
	void Run();
	
	TDblQueLink iDlink;
	COpenWfcJobManger& iJobManager;
	EOpenWfcJobId iJobId;
	TThreadId iCallingThread;
	TRequestStatus* iCompleteRequest;
	};

inline TOpenWfcJob::EOpenWfcJobId TOpenWfcJob::JobId()
	{
	return iJobId;
	}

#endif /* OPENWFCJOB_H_ */
