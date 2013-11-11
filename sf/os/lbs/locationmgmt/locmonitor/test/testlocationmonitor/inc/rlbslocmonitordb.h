/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LBSLOCMONITORDB_H
#define LBSLOCMONITORDB_H


//-------------------------------------------------------------------------------
#include <e32base.h>
#include <LbsPositionInfo.h> 
#include <LbsCommon.h>
#include <lbsareainfo.h>
#include <e32property.h>
#include "lbslocmonitorareainfodefs.h"
#include "../../../../locmonitor/test/te_locmonitorsuite/inc/te_locmonitorsuitedefs.h"

class CTestCommandListener;
//-------------------------------------------------------------------------------


class RLbsLocMonitorDb
	{

public: // Real interface methods
		
	IMPORT_C RLbsLocMonitorDb();
	IMPORT_C ~RLbsLocMonitorDb();
	IMPORT_C void OpenL();
	IMPORT_C void Close();
	IMPORT_C TInt SavePosition(const TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TBool aUserPosition, TRequestStatus& aStatus);
	IMPORT_C TInt GetPosition(TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TPositionAreaExtendedInfo& aMatchingAreaInfo,TRequestStatus& aStatus);
	IMPORT_C TInt CancelSavePosition();
	IMPORT_C TInt CancelGetPosition();
	IMPORT_C TInt GetLastStoredPosition(TPosition& aPosition, TRequestStatus& aStatus);
	IMPORT_C TInt CancelGetLastStoredPosition();
	IMPORT_C TInt ClearDatabase();
	
public: // For Loc Monitor UNIT test only

    static TInt StopWaiting(TAny*);
    void ShortDelayNextRequest();
    void LongDelayNextRequest();
    void FailNextRequest();
    void KillProcess();
	
private:// For Loc Monitor UNIT test only

    void DoStopWaiting();

private:// For Loc Monitor UNIT test only

	TUint iCallCount; // for testcase that call DB methods multiple times

	// Required for testcases that need to delay the completion of an
	// asynchronous request.
	CTestCommandListener* iTestCommandListener;
	TBool iShortDelayNextRequest; // the test request this via P&S
	TBool iLongDelayNextRequest;
	TBool iFailNextRequest;
	CPeriodic* iTimer; 

	// Client request data that needs to be remembered when	the request
	// is put on hold (when iDelayNextRequest is true).
	TPosition* iClientPosition;
	TLbsLocMonitorAreaInfoGci* iClientAreaInfo;
	TPositionAreaExtendedInfo* iClientMatchAreaInfo;
	TRequestStatus* iClientStatus;
		
	};


// This AO just waits for a delay to be published
// by the test in a P&S Property. If that happens,
// this object will inform the owning RLbsLocMonitorDb.
class CTestCommandListener : public CActive
{
public:

	static CTestCommandListener* NewL(RLbsLocMonitorDb& aDbInterface);
	~CTestCommandListener();

private:

	CTestCommandListener(RLbsLocMonitorDb& aDbInterface);
	void ConstructL();
	
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:

	RProperty iDelayProperty;
	RLbsLocMonitorDb& iDbInterface;
};


#endif //LBLOCATIONMONITORDATABASE_H
