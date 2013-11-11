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

#include "lbstestlocmonitorapi.h"

#include <e32base.h>
#include <e32debug.h> 
#include <e32std.h>
#include <LbsClassTypes.h>

_LIT(KLocMonServerName,"testlocmonitorserver");

// The third UID of the server EXE
const TUid KLocMonServerUid3 = { 0x102869E9 };

/**
Client of the Location Monitor server.
 */

EXPORT_C RLbsLocMonitorSession::RLbsLocMonitorSession()
	{

	}

TInt RLbsLocMonitorSession::StartServer()
	{
	TRequestStatus started;
	started = KRequestPending;

#ifdef _DEBUG
	RDebug::Print(_L("EPos: LocMonitor API: Starting <Test> Location Monitor Server..."));
#endif
	
	//const TUid KLocMonServerUid2 = { 0x1000009B };
	const TUidType serverUid(KNullUid, KNullUid, KLocMonServerUid3);

	// Simultaneous launching of two such processes should be detected 
	// when the second one attempts to create the server object, 
	// failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt ret = server.Create(KLocMonServerName, KNullDesC, serverUid);
	
	if (ret != KErrNone)
		{
#ifdef _DEBUG
		RDebug::Print(_L("EPos: LocMonitor API:  server start failed %d"), ret);
#endif
		return ret;
		}

	TRequestStatus died;
	server.Rendezvous(died);

	if (died != KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		User::WaitForRequest(died);             // eat signal
		server.Kill(0);                         // abort startup
		}
	else
		{
		server.Resume();
		User::WaitForRequest(died);             // wait for start or death
		}

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	ret = (server.ExitType() == EExitPanic) ? KErrGeneral : died.Int();
	server.Close();
	return ret;

	}

EXPORT_C TVersion RLbsLocMonitorSession::Version()
	{
	return TVersion(1,0,0);
	}

EXPORT_C TInt RLbsLocMonitorSession::Connect()
	{
	const TInt KNumAttempts = 200;
	TInt retry = KNumAttempts;

	_LIT(KLocMonServerName,"testLocMonitorServer");
	//	

	TInt testcase, propRead;
	propRead = iTestSessionKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey,testcase);
	// Get the value of the location server test case to alter the behaviour 
	// of this test location monitor 
	
	if (propRead==KErrNone)
		{
		switch(testcase)
			{
			case EClearDBLocMonitorOpenSessionFail:
				// The test case where the creation of a session with the location monitor fails
				return KErrServerTerminated;
				break;
			default:
				break;
			}
		}

	
	TRAPD(ret, CreateTimerL());
	
	if (KErrNone==ret)
		{
		for (;;) // FOREVER
			{
			ret = CreateSession(KLocMonServerName, TVersion(0,0,0),12);

			if ( (ret != KErrNotFound) && (ret != KErrServerTerminated))
				{
				return ret;
				}

			if (--retry == 0)
				{
				return ret;
				}

			ret = StartServer();

			if ( (ret != KErrAlreadyExists) && (ret != KErrNone) )
				{
				return ret;
				}
			}
		}
	else
		{
		return ret;
		}

	}

EXPORT_C void RLbsLocMonitorSession::Close()
	{
	
	StopTimer();
	if (iDelayedWipeOutDatabaseUpdate!=NULL)
		{
		delete iDelayedWipeOutDatabaseUpdate;
		}
	
	if (iWipeOutDatabaseReqStatus!=NULL)
		{
		delete iWipeOutDatabaseReqStatus;
		}
	
	RSessionBase::Close();
	}

// Closes the database and wipes out the DB file.
// What are the effects for outstanding requests 
// from other client sessions?
EXPORT_C TInt RLbsLocMonitorSession::WipeOutDatabase(TRequestStatus& aStatus) const
{
	const_cast<RLbsLocMonitorSession*>(this)->SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	
	TTimeIntervalMicroSeconds32 delay, interval;
	delay    = 2000000;
	interval = 100000000;
	
	// ------------------------------ In case of timeout tests ------------------------------------
	TInt testcase, propRead;
	propRead = iTestSessionKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey,testcase);
	// Get the value of the location server test case to alter the behaviour 
	// of this test location monitor 
	if (propRead==KErrNone)
		{
		switch(testcase)
			{
			case EClearDBReqTimedOut:
				// The test case where the request times out
				delay = 1000000000;
				break;
			default:
				break;
			}
		}
	// -----------------------------------------------------------------------------------------------
	
	iDelayedWipeOutDatabaseUpdate->Start(delay, interval, TCallBack(TimerCallback, const_cast<RLbsLocMonitorSession*>(this)));	
	return KErrNone;
}


void RLbsLocMonitorSession::StopTimer()
{
if (iDelayedWipeOutDatabaseUpdate!=NULL)
{
if (iDelayedWipeOutDatabaseUpdate->IsActive())
	{
	iDelayedWipeOutDatabaseUpdate->Cancel();
	}
}
}

void RLbsLocMonitorSession::SetRequestStatus(TRequestStatus* aStatus)
	{
	iWipeOutDatabaseReqStatus = aStatus;
	}

TInt RLbsLocMonitorSession::TimerCallback(TAny* aAny)
   {
   reinterpret_cast<RLbsLocMonitorSession*>(aAny)->UpdateWipeOutDatabase();
   return 0; //Returning 0 stops the active object [no longer scheduled to run]
   }

void RLbsLocMonitorSession::UpdateWipeOutDatabase()
	{
	User::RequestComplete(iWipeOutDatabaseReqStatus, KErrNone);
	StopTimer();
	}

void RLbsLocMonitorSession::UpdateWipeOutDatabaseCancel()
	{
	User::RequestComplete(iWipeOutDatabaseReqStatus, KErrCancel);
	StopTimer();
	}

EXPORT_C TInt RLbsLocMonitorSession::CancelDatabaseWipeOut()const
	{
	if (iDelayedWipeOutDatabaseUpdate->IsActive())
		{
		iDelayedWipeOutDatabaseUpdate->Cancel();
		}
	const_cast<RLbsLocMonitorSession*>(this)->UpdateWipeOutDatabaseCancel();
	
	return KErrNone;
	}

void RLbsLocMonitorSession::CreateTimerL()
	{
	// Construct the Periodic Timer Active Object
	// Used instead of CTimer for its convenient callback capability
	iDelayedWipeOutDatabaseUpdate = CPeriodic::NewL(CActive::EPriorityStandard);
	}


/**
This class is used to create a sub-session with the location monitor
server for the purpose of obtaining the current position based on area
information.
Before using the class, a primary connection must have already been
established with the server.
*/

EXPORT_C RLbsLocMonitorAreaPositioner::RLbsLocMonitorAreaPositioner() 
		
	{
	iTestLKPAreaKey.Attach(KLocSrvTestSuite, KLbsLocMonitorTestLKPAreaKey, EOwnerThread);
	iTestLKPosKey.Attach(KLocSrvTestSuite, KLbsLocMonitorTestLKPosKey, EOwnerThread);
	}

void RLbsLocMonitorAreaPositioner::CreateLKPATimerL()
	{
	iDelayedLKPAreaUpdate = CPeriodic::NewL(CActive::EPriorityStandard);
	}


EXPORT_C TInt RLbsLocMonitorAreaPositioner::OpenL(RLbsLocMonitorSession& aLocMonSession)
	{	

	TInt testcase, propRead;
	propRead = iTestLKPAreaKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestLKPAreaKey,testcase);
	
	if (propRead==KErrNone)
		{
		switch(testcase)
			{
			case EGetLKPAreaLocMonitorOpenSubsessionFail:
				return KErrCouldNotConnect;
				break;
			default:
				break;
			}
		}
	
	
	propRead = iTestLKPosKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestLKPosKey,testcase);
	if (propRead==KErrNone)
		{
		switch(testcase)
			{
			case EGetLKPosLocMonitorOpenSubsessionFail:
				return KErrCouldNotConnect;
			default:
				break;
			}
		}
	
	// Create the timer active object to delay the response to the 
	// location server
	TRAPD(ret1, CreateLKPATimerL());
	if (ret1!=KErrNone)
		{
		return ret1;
		}
	
	
	// Create the timer active object to delay the response to the 
	// location server
	TRAPD(ret2, CreateLKPosTimerL());

	if (ret2==KErrNone)
		{
			ret2 = CreateSubSession(aLocMonSession, EOpenPositioner, TIpcArgs());
		}
	return ret2;
	
	}


EXPORT_C void RLbsLocMonitorAreaPositioner::Close()
	{
	
	// Last Known position area request
	iTestLKPAreaKey.Close();	
	
	CloseSubSession(EClosePositioner);
	
	if (iDelayedLKPAreaUpdate!=NULL)
		{
		delete iDelayedLKPAreaUpdate;
		}
	iLKPAReqStatus = NULL;

	// Last Known Position request 
	iTestLKPosKey.Close();
	
	CloseSubSession(EClosePositioner);
	
	if (iDelayedLKPosUpdate!=NULL)
		{
		delete iDelayedLKPosUpdate;
		}

	iLKPosReqStatus = NULL;
	
	
	}

EXPORT_C TInt RLbsLocMonitorAreaPositioner::SetLastKnownPosition(const TPositionInfoBase& /*aPosInfo*/) const
	{
	return KErrNone;
	}

void RLbsLocMonitorAreaPositioner::SetLKPARequestStatus(TRequestStatus* aStatus)
	{
	iLKPAReqStatus = aStatus;
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
		TPositionAreaInfoBase& aAreaInfo,
		const TPosAreaReqParams& aParameters,
		TRequestStatus& aStatus)const
		{

		const_cast<RLbsLocMonitorAreaPositioner*>(this)->SetLKPARequestStatus(&aStatus);
		aStatus = KRequestPending;
		
		// Return a dummy area info
		switch(aParameters.iPositionAreaType)
			{
			case EPositionAreaInfoClass:
				{
				TPositionAreaInfo& areainfo = static_cast<TPositionAreaInfo &>(aAreaInfo);
				areainfo.SetArea(TPositionAreaInfo::EAreaCity);
				break;	
				}
			case (EPositionAreaExtendedInfoClass+EPositionAreaInfoClass):
				{
				TPositionAreaExtendedInfo& areaextendedinfo = static_cast<TPositionAreaExtendedInfo &>(aAreaInfo);
				areaextendedinfo.SetArea(TPositionAreaInfo::EAreaCity);
				areaextendedinfo.SetCellIdMatch(ETrue);
				areaextendedinfo.SetLocationAreaCodeMatch(ETrue);
				areaextendedinfo.SetMobileCountryCodeMatch(ETrue);
				areaextendedinfo.SetMobileNetworkCodeMatch(ETrue);
				break;	
				}
			}
		
		// Return a dummy position
		TPositionInfo& posinfo =  static_cast<TPositionInfo &>(aPosInfo);
		TPosition testpos;
 		testpos.SetCoordinate(DUMMY_LAST_KNOWN_POS_LATITUDE1, DUMMY_LAST_KNOWN_POS_LONGITUDE1, DUMMY_LAST_KNOWN_POS_ALTITUDE1);
		posinfo.SetPosition(testpos);
		
		// The values used in the timer for delaying the response from the test location monitor
		// to the location server
		TTimeIntervalMicroSeconds32 delay, interval;
		delay 	= 2000000; //2 sec
		interval = 100000000;
		// as CPeriodic class is used, a higher value of interval ensures we do not receive 
		// multiple callbacks [we would like to complete the timer after the first callback].
		
		// Modify the behaviour of the Location Monitor based on the testcase using the value of the
		// property
		TInt testcase, propRead;
		propRead = iTestLKPAreaKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestLKPAreaKey,testcase);
		if (propRead==KErrNone)
			{
			switch(testcase)
				{
				case EGetLKPosAreaNotFound:
					{
					TRequestStatus* pStat = &aStatus;
					User::RequestComplete(pStat, KErrNotFound);
					return;	
					}
				case EGetLKPAreaTimedOut:
					delay = 1000000000;;
					break;
				default:
					break;
				}
			}

		iDelayedLKPAreaUpdate->Start(delay, interval, TCallBack(LKPATimerCallback, const_cast<RLbsLocMonitorAreaPositioner*>(this)));
		
		}


void RLbsLocMonitorAreaPositioner::StopLKPATimer()
	{
	if (iDelayedLKPAreaUpdate!=NULL)
		{
		if (iDelayedLKPAreaUpdate->IsActive())
			{
			iDelayedLKPAreaUpdate->Cancel();
			}
		}
	}

TInt RLbsLocMonitorAreaPositioner::LKPATimerCallback(TAny* aAny)
	{
	reinterpret_cast<RLbsLocMonitorAreaPositioner*>(aAny)->UpdateLKPArea();
	return 0;
	}

void RLbsLocMonitorAreaPositioner::UpdateLKPArea()
	{
	User::RequestComplete(iLKPAReqStatus, KErrNone);
	StopLKPATimer();
	}

void RLbsLocMonitorAreaPositioner::UpdateLKPAreaCancel()
	{
	User::RequestComplete(iLKPAReqStatus, KErrCancel);
	StopLKPATimer();
	}

EXPORT_C TInt RLbsLocMonitorAreaPositioner::CancelGetLastKnownPositionArea() const
	{
	const_cast<RLbsLocMonitorAreaPositioner*>(this)->UpdateLKPAreaCancel();
	return KErrNone;
	}



void RLbsLocMonitorAreaPositioner::SetLKPosRequestStatus(TRequestStatus* aStatus)
	{
	iLKPosReqStatus = aStatus;
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::GetLastKnownPosition(TPositionInfoBase& aPosInfo,
		TRequestStatus& aStatus) const
		{

		const_cast<RLbsLocMonitorAreaPositioner*>(this)->SetLKPosRequestStatus(&aStatus);
		aStatus = KRequestPending;
		
		TPositionInfo& posinfo =  reinterpret_cast<TPositionInfo &>(aPosInfo);
		TReal64 testlatitude,testlongitude, testaltitude;
		testlatitude = 51.88; testlongitude = 0.45; testaltitude = 0.0;
		TPosition testpos;
 		testpos.SetCoordinate(DUMMY_LAST_KNOWN_POS_LATITUDE1, DUMMY_LAST_KNOWN_POS_LONGITUDE1, DUMMY_LAST_KNOWN_POS_ALTITUDE1);
		posinfo.SetPosition(testpos);

		// as CPeriodic class is used, a higher value of interval ensures we do not receive 
		// multiple callbacks [we would like to complete the timer after the first callback].
		TTimeIntervalMicroSeconds32 delay, interval;
		delay    = 2000000; //2 sec
		interval = 100000000;
		
		// Modify the behaviour of the Location Monitor based on the testcase using the value of the
		// property
		TInt testcase, propRead;
		propRead = iTestLKPosKey.Get(KLocSrvTestSuite, KLbsLocMonitorTestLKPosKey,testcase);
		if (propRead==KErrNone)
			{
			switch(testcase)
				{
				case EGetLKPosNotFound:
					{
					TRequestStatus* pStat = &aStatus;
					User::RequestComplete(pStat, KErrNotFound);
					return;
					}

				case EGetLKPosTimedOut:
					delay = 1000000000;;
					break;
				default:
					break;
				}
			}
		iDelayedLKPosUpdate->Start(delay, interval, TCallBack(LKPosTimerCallback, const_cast<RLbsLocMonitorAreaPositioner*>(this)));
		}


EXPORT_C TInt RLbsLocMonitorAreaPositioner::CancelGetLastKnownPosition() const
{
	const_cast<RLbsLocMonitorAreaPositioner*>(this)->UpdateLKPosCancel();
	return KErrNone;
}



void RLbsLocMonitorAreaPositioner::CreateLKPosTimerL()
	{

	iDelayedLKPosUpdate = CPeriodic::NewL(CActive::EPriorityStandard);

	}

void RLbsLocMonitorAreaPositioner::StopLKPosTimer()
	{
	if (iDelayedLKPosUpdate!=NULL)
		{
		if (iDelayedLKPosUpdate->IsActive())
			{
			iDelayedLKPosUpdate->Cancel();
			}
		}
	}

TInt RLbsLocMonitorAreaPositioner::LKPosTimerCallback(TAny* aAny)
	{
	reinterpret_cast<RLbsLocMonitorAreaPositioner*>(aAny)->UpdateLKPos();
	return 0;
	}

void RLbsLocMonitorAreaPositioner::UpdateLKPos()
	{
	User::RequestComplete(iLKPosReqStatus, KErrNone);
	StopLKPosTimer();
	}

void RLbsLocMonitorAreaPositioner::UpdateLKPosCancel()
	{
	User::RequestComplete(iLKPosReqStatus, KErrCancel);
	StopLKPosTimer();
	}

EXPORT_C void RLbsLocMonitorAreaPositioner::ConstructL()
	{
	
	}

		
EXPORT_C void RLbsLocMonitorAreaPositioner::Destruct()
	{
	
	}

