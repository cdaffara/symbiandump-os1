/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@internalTechnology
@prototype
*/

#ifndef LBSTESTMONITORAPI_H_
#define LBSTESTMONITORAPI_H_

// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

#include <lbspositioninfo.h>
#include <LbsAreaInfo.h>
#include <lbs.h>
#include <lbsipc.h>
#include "lbstestlocmonitormessageenums.h"
#include "lbstestlocmonitorapidefs.h"


/*
Parameters sent from client to server in a GetLastKnownPositionArea request
*/
class TPosAreaReqParams
	{
public:
	TInt iPositionAreaType;
	};


/**
Client-side session with the Location Monitor server.
 */
class RLbsLocMonitorSession : public RSessionBase
	{
public:
	IMPORT_C RLbsLocMonitorSession();

	IMPORT_C TInt Connect();
	
	IMPORT_C void Close();

	// Closes the database and wipes out the DB file.
   	IMPORT_C TInt WipeOutDatabase(TRequestStatus& aStatus) const;
   	
   	IMPORT_C TInt CancelDatabaseWipeOut() const;

	IMPORT_C TVersion Version();

	// Test methods [not part of the actual API]
   	static TInt TimerCallback(TAny* aAny);
	void UpdateWipeOutDatabase();
	void SetRequestStatus(TRequestStatus* aStatus);
	void StopTimer();
	void UpdateWipeOutDatabaseCancel();

protected:
	void ConstructL();
	void Destruct();  
	
	// From RSecureSessionBase
	//TInt RLbsLocMonitorSession::StartServer(TServerStartParams& aParams);
	static TInt StartServer();		// TODO ??

private:
	void CreateTimerL();	

private:
	RProperty iTestSessionKey;
	CPeriodic* iDelayedWipeOutDatabaseUpdate;
	TRequestStatus* iWipeOutDatabaseReqStatus;
	
	};

/**
This class is used by clients of the Location Monitor server to
create a sub-session with the server.

Before using the class, a primary connection must have already been
established with the server with RLbsLocMonitorSession::Connect().

@see RLocMonitorSession
@publishedAll
@released
*/
class RLbsLocMonitorAreaPositioner : public RSubSessionBase
	{
public:
	IMPORT_C RLbsLocMonitorAreaPositioner();

	IMPORT_C TInt OpenL(RLbsLocMonitorSession& aLocMonSession);
	IMPORT_C void Close();

  	// synchronous call
   	// Called by the Location Server to send the last Position obtained from
   	// any of its PSYs to the Location Monitor server.
   	IMPORT_C TInt SetLastKnownPosition(const TPositionInfoBase& aPosInfo)const;
   	
   	IMPORT_C void GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
				      				 TPositionAreaInfoBase& aAreaInfo,
				      				 const TPosAreaReqParams& aParameters,
                                	 TRequestStatus& aStatus) const;
   	IMPORT_C TInt CancelGetLastKnownPositionArea() const;

   	/* Retrieve the position last stored in the database */
    IMPORT_C void GetLastKnownPosition(TPositionInfoBase& aPosInfo,
                                	 TRequestStatus& aStatus) const;
    
    /* Cancel a previous call to GetLastKnownPosition*/
    IMPORT_C TInt CancelGetLastKnownPosition() const;    
   	
    
   	static TInt LKPATimerCallback(TAny* aAny);
	void UpdateLKPArea();
	void SetLKPARequestStatus(TRequestStatus* aStatus);
	void StopLKPATimer();
	void UpdateLKPAreaCancel();
	
   	static TInt LKPosTimerCallback(TAny* aAny);
	void UpdateLKPos();
	void SetLKPosRequestStatus(TRequestStatus* aStatus);
	void StopLKPosTimer();
	void UpdateLKPosCancel();
	
protected:
	IMPORT_C void ConstructL();		
	IMPORT_C void Destruct();			
	
private:
	void CreateLKPATimerL();
	void CreateLKPosTimerL();
	void StoreLKPARequestStatus(TRequestStatus& aStatus);
	void StoreLKPosRequestStatus(TRequestStatus& aStatus);

private:
	/** A pointer to a container that holds pointer descriptors, needed to point
	to the clients request data during asynchronous requests */
	// ************************** TODO **********************
	// CLocMonSubSessionPtrHolder* iPtrHolder;
	CPeriodic* iDelayedLKPAreaUpdate;
	CPeriodic* iDelayedLKPosUpdate;

private:
	RProperty 		iTestLKPAreaKey;
	TRequestStatus* iLKPAReqStatus;
	
	RProperty 		iTestLKPosKey;
	TRequestStatus* iLKPosReqStatus;
	};

#endif /*LBSTESTLOCMONITORAPI_H_*/
