// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ctlbsclientperiodictester.h"
#include "ctlbsclientlog.h"

// CONSTANTS
const TInt KSmallestUpdateIntervalToLog = 4000000; //2000000
const TInt KMilliSecondTolerance = 2500000; //One second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPeriodicTester::CPeriodicTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CT_LbsClientPeriodicTester::CT_LbsClientPeriodicTester() : CActive(CActive::EPriorityHigh), iError(EFalse), iErrorCode(KErrNone)
    {
    }

// -----------------------------------------------------------------------------
// CPeriodicTester::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::ConstructL(
	TTimeIntervalMicroSeconds aInterval, 
	const TDesC& aServiceName, 
	CT_LbsClientLog* aLog,
	TUid aUid,
	TInt aNumberOfRuns)
    {
    iLog = aLog;
	iName = aServiceName;
	iNumberOfRuns = aNumberOfRuns;
	iInterval = aInterval;
	iUid = aUid;
	iNumberOfUpdates = 0;
    iPrematureChangeOfParameters = EFalse;
    iCanceledRequest = EFalse;
    iCancelRequest = EFalse;
    iWriteToLog = ETrue; //EFalse
	iRecentTime = TTime(0);
	iPositionInfo = TPositionInfo();

	_LIT(KConnectErr, "Error when connecing to EPos server,  %d");
	TInt err = iPosServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);

	_LIT(KOpenErr, "Error when opening positioner,  %d");
	err = iPositioner.Open(iPosServer, aUid);
	AssertTrueL(err == KErrNone, KOpenErr, err);

	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(aInterval);
	
	_LIT(KUpdateErr, "Error when setting update interval,  %d");
	err = iPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KUpdateErr, err);

	iPositioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, aServiceName);
	
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CPeriodicTester::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CT_LbsClientPeriodicTester* CT_LbsClientPeriodicTester::NewL(
	TTimeIntervalMicroSeconds aInterval,
	const TDesC& aServiceName,
	CT_LbsClientLog* aLog,
	TUid aUid,
	TInt aNumberOfRuns)
    {
    CT_LbsClientPeriodicTester* self = new( ELeave ) CT_LbsClientPeriodicTester;
    
    CleanupStack::PushL(self);
    self->ConstructL(aInterval, aServiceName, aLog, aUid, aNumberOfRuns);
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CT_LbsClientPeriodicTester::~CT_LbsClientPeriodicTester()
    {
    iPositioner.Close();
	iPosServer.Close();
	
	iLog = NULL;
	}

// -----------------------------------------------------------------------------
// CPeriodicTester::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::RunL()
    {
	iNumberOfUpdates++;
    //User::Leave(-10); //used for testing the test code
	TBuf<175> buf;
	_LIT(KStatus,"Status invalid in RunL, status = %d");
	buf.Format(KStatus, iStatus.Int());
	AssertTrueL((iStatus == KErrNone || iStatus == KPositionQualityLoss || 
                 iStatus == KErrCancel), buf, iStatus.Int());
	if(iNumberOfUpdates <= iNumberOfRuns)
		{
		_LIT(KTime, "%F%H:%F%T:%F%S:%F%*C3");
		TBuf<100> time;
		TTime now;
		now.UniversalTime();
		TInt difference;
		TInt smallestIntervalToLog = KSmallestUpdateIntervalToLog;
		if(iNumberOfUpdates == 1)
			{
            difference = I64LOW(now.MicroSecondsFrom(iRecentTime).Int64());
			}
		else
			{
			difference = I64LOW(now.MicroSecondsFrom(iRecentTime).Int64());
			}
		
        _LIT(KInterval, "The interval is %d, and the difference is %d");
        TBuf<150> buf; 
        buf.Format(KInterval,iInterval.Int64(), difference); 
        iLog->Put (buf);
		if((iInterval.Int64() > smallestIntervalToLog) && (I64LOW(iRecentTime.Int64()) != 0))
			{
			_LIT(KTimeErr, "ERROR: Time difference greater than tolerated interval, time difference = %d");
			AssertTrueL(difference < (iInterval.Int64() +  KMilliSecondTolerance), KTimeErr, I64LOW(now.MicroSecondsFrom(iRecentTime).Int64()));
            
            if(difference < 0)
				{
                #ifdef __WINS__
                _LIT(KEarlyErr, "ERROR: Negative time difference on WINS greater than 0.4s");
                AssertTrueL((KToleratedEarliness > -(difference)), KEarlyErr, KErrTotalLossOfPrecision);
                #else
                _LIT(KEarlyErr, "ERROR: Time difference negative on target");
				iLog->Put(KEarlyErr);
				User::LeaveIfError(KErrTotalLossOfPrecision);
                #endif

				}
			}

		if(iWriteToLog && ((iNumberOfUpdates % KTracePeriod) == 0))
            {
		    now.UniversalTime();
		    now.FormatL(time, KTime);
    		_LIT(KLog, "Client got update no. %d at %S,  Time-diff = %d, Status = ");
    		TBuf<175> buf;
    		buf.Format(KLog, iNumberOfUpdates, &time, difference);
    		buf.AppendNum(iStatus.Int());
    		buf.Append(iName);
    		_LIT(KNrOfRuns, "Nr of runs = ");
            buf.Append(KNrOfRuns);
            buf.AppendNum(iNumberOfRuns);
            iLog->Put(buf);
            }
		
		if(iPrematureChangeOfParameters)
			{
			iInterval = TTimeIntervalMicroSeconds(2000000);
			User::After(I64LOW(iInterval.Int64()));
	
			TPositionUpdateOptions posOption;
			posOption.SetUpdateInterval(iInterval);
	
			TInt err = iPositioner.SetUpdateOptions(posOption);
			_LIT(KChange, "ERROR: Got error code when setting update options prematurely in ChangeTestParameters");
			AssertTrueL(err == KErrNone, KChange, err);
			iPrematureChangeOfParameters = EFalse;
			}
		
		RequestNotification();
            
        if(iCancelRequest)
			{
			User::After(3000000);
			iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
			iCancelRequest = EFalse;
            iCanceledRequest = ETrue;
			}
     
        if(iCanceledRequest)
            {
            iCanceledRequest = EFalse;
            _LIT(KCancelErr, "ERROR: Status was not KErrCancel after CancelRequest");
			AssertTrueL(iStatus.Int() == KErrCancel, KCancelErr, KErrCancel);
            }
        }
	else 
		{
		CActiveScheduler::Stop();
		}
		
	}


// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::RunError
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CT_LbsClientPeriodicTester::RunError(TInt aError)
    {
    iError = ETrue;
    iErrorCode = aError;
    CActiveScheduler::Stop();
    _LIT(KErrorInRunL, "Error code in RunError is, %d");
    TBuf<100> buf;
    buf.Format(KErrorInRunL, aError);
    iLog -> Put(buf);
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::IsCompletedOK
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CT_LbsClientPeriodicTester::IsCompletedOK()
    {
    return !iError;
    }

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::GetError
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CT_LbsClientPeriodicTester::GetError()
    {   
    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::ChangeTestParametersL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::ChangeTestParametersL(TTimeIntervalMicroSeconds aInterval, TInt aNumberOfRuns)
	{
	iNumberOfRuns = aNumberOfRuns;
	iNumberOfUpdates = 0;
	iInterval = aInterval;
	iRecentTime = TTime(0);
    iError = EFalse;
    iErrorCode = KErrNone;
	
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(aInterval);
	
	TInt err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KChange, "ERROR: When setting update options in ChangeTestParameters, err: %d");
	AssertTrueL(err == KErrNone, KChange, err);
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::ChangeRequestorInformation
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::ChangeRequestorInformation(const TDesC& aServiceName)
	{
	iPositioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, aServiceName);
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::ClosePositioner
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::ClosePositioner()
	{
	iPositioner.Close();
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::OpenPositionerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::OpenPositionerL(const TDesC& aServiceName)
	{
	_LIT(KOpenErr, "Error when opening positioner,  %d");
	TInt err = iPositioner.Open(iPosServer, iUid);
	AssertTrueL(err == KErrNone, KOpenErr, err);
	
	iPositioner.SetRequestor(CRequestor::ERequestorService,
						 CRequestor::EFormatApplication, aServiceName);
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::SetMaxAgeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::SetMaxAgeL(TTimeIntervalMicroSeconds aInterval)
	{
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(iInterval);
	posOption.SetMaxUpdateAge(aInterval);

	TInt err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KChange, "ERROR: When setting max age in SetMaxAge, err: %d");
	AssertTrueL(err == KErrNone, KChange, err);	
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::SetPartialUpdateL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::SetPartialUpdateL(TBool aUpdateOption)
	{
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(iInterval);
	posOption.SetAcceptPartialUpdates(aUpdateOption);

	TInt err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KChange, "ERROR: When setting partial update in SetPartialUpdate, err: %d");
	AssertTrueL(err == KErrNone, KChange, err);	
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::SetTimeOutL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::SetTimeOutL(TTimeIntervalMicroSeconds aInterval)
	{
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(iInterval);
	posOption.SetUpdateTimeOut(aInterval);

	TInt err = iPositioner.SetUpdateOptions(posOption);
	_LIT(KChange, "ERROR: When setting timeout in SetTimeOut");
	AssertTrueL(err == KErrNone, KChange, err);	
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::SetChangeParametersPrematurely
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::SetChangeParametersPrematurely(TBool aCondition)
	{
	iPrematureChangeOfParameters = aCondition;
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::CancelRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::CancelRequest()
	{
	iCancelRequest= ETrue;
    iCanceledRequest = EFalse;
	}


// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::RequestNotification
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::RequestNotification()
	{
    TTime now;
    now.UniversalTime();
    iRecentTime = now;
	iPositioner.NotifyPositionUpdate(iPositionInfo, iStatus);
	SetActive();
	}

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::RequestNotification
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::GetPosition(TPositionInfo& aPosInfo)
    {
    aPosInfo = iPositionInfo;
    }

// -----------------------------------------------------------------------------
// CT_LbsClientPeriodicTester::DoCancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CT_LbsClientPeriodicTester::DoCancel()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPeriodicTester::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPeriodicTester::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
  {
  if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		iLog->Put(buf);
        User::Leave(aErrorCode);
		}
  }

//  End of File
