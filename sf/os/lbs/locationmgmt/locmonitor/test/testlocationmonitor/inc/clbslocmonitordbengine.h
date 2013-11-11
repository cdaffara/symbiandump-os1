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


#ifndef LBS_LOCMONITORDBENGINE_H
#define LBS_LOCMONITORDBENGINE_H


//-------------------------------------------------------------------------------
#include <e32base.h>
#include <sqldb.h> 
#include <lbspositioninfo.h> 
#include <e32capability.h>
#include "rlbslocmonitordb.h"
#include "clbslocmonitordbtimer.h"

//-------------------------------------------------------------------------------
/** 
*/
class CLbsLocMonitorDbEngine : public CActive, MLocMonitorDBCallback
	{

public:		
	static CLbsLocMonitorDbEngine* NewL();
	virtual ~CLbsLocMonitorDbEngine();
	TInt SavePosition(TUint aMcc, TUint aMnc, TUint aLac, 
			TUint aCid, const TPosition& aPosition, TBool aUserPosition, TRequestStatus& aStatus);
	TInt GetPosition(TUint aMcc, TUint aMnc, TUint aLac, 
			TUint aCid, TPosition& aPosition, TPositionAreaExtendedInfo& aMatchingAreaInfo, TRequestStatus& aStatus);
	TInt GetPosition(TPosition& aPosition, TRequestStatus& aStatus);
	TInt ClearDatabase();
	
   // from CLbsLocMonitorDbTimer
    void FlushTimerCallback();


private:
	CLbsLocMonitorDbEngine();
	void ConstructL();
	void InitDbL();
	TBool Select(TPosition& aPosition, TPtrC aQuery, 
			TInt aMcc = KErrNotFound, TInt aMnc = KErrNotFound, 
			TInt aLac = KErrNotFound, TInt aCid = KErrNotFound);
	void CheckFlush();
	void Flush(TBool aShutdown);
	TPositionAreaExtendedInfo CacheMatchLevel(TInt aMcc, TInt aMnc, TInt aLac, TInt aCid);
	TInt Insert(TBool aShutdown);
	TInt DbSize();
	virtual void RunL();
	virtual void DoCancel();
	
private:
	RSqlDatabase iDatabase;
	TTimeIntervalMicroSeconds32 iFlushInterval;
	TInt iCount;
	RSqlStatement iSqlSaveStatement;
	TRequestStatus* iClientStatus;
	CLbsLocMonitorDbTimer* iDbTimer;
	
	TBool iSaveLastPos;
	TBool iDBInitialised;
	TInt iLastMcc;
	TInt iLastMnc;
	TInt iLastLac;
	TInt iLastCid;
	TPosition iLastPosition;
	TPosition iLastKnownPosition;
	TTime iLastTime;
	TBool iIsLastValid;
	};
	



#endif //LBS_LOCATIONMONITORDATABASE_H
