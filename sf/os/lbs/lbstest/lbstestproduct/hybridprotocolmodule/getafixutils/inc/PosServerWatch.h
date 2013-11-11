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
//
// PosServerWatch.h
//
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__
#define __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbsipc.h>

class RPositioner;


class MPosServerObserver 
	{
	public:
		virtual void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
		virtual void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
	};
	
	
class CPosServerWatcher : public CActive
	{
public:
	IMPORT_C static CPosServerWatcher* NewLC(RPositioner& aPos, MPosServerObserver& aObserver);
	IMPORT_C ~CPosServerWatcher();
	
	IMPORT_C void IssueGetLastKnownPosition();
	IMPORT_C void IssueNotifyPositionUpdate();
	
	IMPORT_C void CancelRequest();
	IMPORT_C void CompleteRequest();
	
private:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
	CPosServerWatcher(RPositioner& aPos, MPosServerObserver& aObserver);
	void ConstructL();
	
private:	
	MPosServerObserver& iObserver;
	
	RPositioner&   iPos;
	TPositionInfo  iData;
	TPositionIpcId iRequest;
	};


#endif // __LBS_GETAFIX_TEST_POS_SERVER_WATCH_H__

