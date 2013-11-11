// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__SMSULOG_H__)
#define __SMSULOG_H__

#include <e32base.h>
#include <logwrap.h>
#include <e32std.h>
#include "smsuact.h"
#include <logsmspdudata.h>

class RFs;
class CSmsMessage;


/**
 *  Logs SMS-related events.
 *  
 *  The class uses the log engine functionality provided by the Log Engine (CLogWrapper
 *  etc.).
 *  
 *  @publishedAll
 *  @released 
 */
class CSmsEventLogger : public CSmsuActiveBase
	{
public:
	IMPORT_C static CSmsEventLogger* NewL(RFs& aFs,TInt aPriority=CActive::EPriorityStandard);
	IMPORT_C ~CSmsEventLogger();

	IMPORT_C void AddEvent(TRequestStatus& aStatus,const CSmsMessage& aSmsMessage,const TLogSmsPduData& aData,TInt* aStatusId=NULL);
	IMPORT_C void GetEvent(TRequestStatus& aStatus,TLogId aId);
	IMPORT_C void ChangeEvent(TRequestStatus& aStatus,const CSmsMessage& aSmsMessage,const TLogSmsPduData& aData,TInt* aStatusId=NULL);
    IMPORT_C void ChangeEvent(TRequestStatus& aStatus,const CSmsMessage& aSmsMessage, const TTime* aTime, const TLogSmsPduData& aData, TInt* aStatusId=NULL);	
	IMPORT_C void DeleteEvent(TRequestStatus& aStatus);

	inline TBool ClientAvailable() const;
	inline TInt GetString(TDes& aString,TInt aId) const;
	inline CLogEvent& Event();
	inline const CLogEvent& Event() const;
	inline const TLogSmsPduData& SmsPDUData() const;

protected:

	void DoCancel();

private:
	CSmsEventLogger(TInt aPriority);
	void ConstructL(RFs& aFs);
	void DoRunL();
	void DoAddEventL(const CSmsMessage& aSmsMessage,TInt* aStatusId);
    void SetDataL(const CSmsMessage& aSmsMessage,TInt* aStatusId, const TTime* aDischargeTime);
	void DoComplete(TInt& aStatus);
	inline void GetStringL(TDes& aString,TInt aId) const;

private:
	enum TSmsEventLoggerState
		{
		ESmsEventLoggerIdle,
		ESmsEventLoggerGettingEvent,
		ESmsEventLoggerAddingEvent,
		ESmsEventLoggerChangingEvent,
		ESmsEventLoggerDeletingEvent
		};
private:
	TSmsEventLoggerState iState;

	CLogWrapper* iLogWrapper;
	CLogEvent* iLogEvent;

	TLogSmsPduData iSmsPDUData;
	};

#include "smsulog.inl"

#endif
