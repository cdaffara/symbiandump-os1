// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "epos_cpospsylog.h"
#include <f32file.h>
#include <bautils.h>

// New path for the log files
// Constant Definition

_LIT(KLogsDir, ":\\logs\\");
_LIT(KPsyTesterDir, ":\\logs\\psytester\\");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYLog::CPosPSYLog()
    {
    }

// EPOC default constructor can leave.
void CPosPSYLog::ConstructL()
    {
    User::LeaveIfError(iFileSession.Connect());
 
	TFileName logDir;
    GetLogsDirL(logDir);

    // Create the c:\logs directory
    iFileSession.MkDir(logDir);

    GetPsyTesterLogsDirL(logDir);
    // Create a directory under c:\logs
	iFileSession.MkDir(logDir);
    }

// Two-phased constructor.
CPosPSYLog* CPosPSYLog::NewL()
    {
    CPosPSYLog* self = new (ELeave) CPosPSYLog;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYLog::~CPosPSYLog()
    {
    iFileSession.Close();
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//        
void CPosPSYLog::AppendResultL(
    const CPosPSYResult& aPSYResult)
    {
    _LIT(KInfoMessage, "Info: ");
    _LIT(KWarningMessage, "Warning: ");
    _LIT(KErrorMessage, "Error: ");

    if (aPSYResult.ResultType() == EInfoMessage)
        {
        iInfo++;
        AppendLineL(KInfoMessage);
        }
    else if (aPSYResult.ResultType() == EWarningMessage)
        {
        iWarning++;
        AppendLineL(KWarningMessage);
        }
    else if (aPSYResult.ResultType() == EErrorMessage)
        {
        iError++;
        AppendLineL(KErrorMessage);
        }
    else // EDisplayMessage
        {
        AppendLineL(KNewLine);    
        }

    AppendLineL(*aPSYResult.Message());
    AppendLineL(KNewLine);
    }

// ---------------------------------------------------------
// CPosPSYLog::CreateLogL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//   
void CPosPSYLog::CreateLogL(
    const TPosPSYInfo& aInfo, const TPosPSYTestParameters& aTestParams)
    {
   
    iFileName.Zero();       
    _LIT(KLogExtension,".txt");
    TLex psyName(aInfo.PSYBaseInfo().Name());

	// Add the directory path to the file name
    TFileName testerDir;
    GetPsyTesterLogsDirL(testerDir);
	iFileName.Append(testerDir);
	
	while (!psyName.Eos())
		{
		if (psyName.Peek().IsDigit() || psyName.Peek().IsAlpha())
			{
			iFileName.Append(psyName.Get());
			}
		else
			{
			psyName.Inc();
			}
		}
	iFileName.Append('_');
	
	TLex uid (aInfo.PSYBaseInfo().Uid().Name());
	while (!uid.Eos())
		{
		if (uid.Peek() != '[' && uid.Peek() != ']')
			{
			iFileName.Append(uid.Get());
			}
		else
			{
			uid.Inc();
			}
		}
    iFileName.Append(KLogExtension);
    
    TInt error = iCurrentLogFile.Create(iFileSession, iFileName, EFileWrite | EFileShareExclusive);    
    if (error == KErrAlreadyExists) // File does exist
        {
        error = iCurrentLogFile.Open(iFileSession, iFileName, EFileWrite | EFileShareExclusive);
        if(error == KErrNone)
        {
        	iCurrentLogFile.Seek(ESeekEnd, iCurrentPosition);	
        }
        }
    else if (error == KErrNone)
        {
		MakeTestHeaderL(aInfo);
		// Write the PSY Tester parameters to the
		// log file
		WriteTestParametersL(aTestParams);
        iCurrentPosition = 0;
        }
    else
        {
        User::LeaveIfError(error);
        }
    }

// ---------------------------------------------------------
// CPosPSYLog::WriteTestParametersL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//   
 void CPosPSYLog::WriteTestParametersL(
    const TPosPSYTestParameters& aTestParams)
	{
 	AppendLineL(KDelim);
 	_LIT(KParamHeader, "PSY Tester Parameters.");
 	AppendLineL(KParamHeader);
 	AppendLineL(KNewLine);
 	AppendLineL(KDelim);
 	
 	//+ Retrieve the parameters
 	
 	//+ Number of memory requests
 	_LIT(KNumMemReqs, "Number of Location Requests for memory tests: ");
 	AppendLineL(KNumMemReqs);
 	AppendLineL(aTestParams.NumberOfMemoryRequests());
 	AppendLineL(KNewLine);	
 	
 	//+ Number of location requests
 	_LIT(KNumLocReqs, "Number of Location Requests: ");
 	AppendLineL(KNumLocReqs);
 	AppendLineL(aTestParams.NumberOfLocationRequests());
 	AppendLineL(KNewLine);
 	
 	//+ Number of partial update requests
 	_LIT(KNumPartUpdateReqs, "Number of Partial Update Requests: ");
 	AppendLineL(KNumPartUpdateReqs);
 	AppendLineL(aTestParams.NumberOfPartialUpdateRequests());
 	AppendLineL(KNewLine);	
 	
 	//+ Number of stress test instances
 	_LIT(KNumStressTestInsts, "Number of Stress Test Instances: ");
 	AppendLineL(KNumStressTestInsts);
 	AppendLineL(aTestParams.NumberOfStressTestInstances());
 	AppendLineL(KNewLine);	
 	
 	//+ Number of multiple session test instances
 	_LIT(KNumMultipleSessionTestInsts, "Number of Multiple Session Test Instances: ");
 	AppendLineL(KNumMultipleSessionTestInsts);
 	AppendLineL(aTestParams.NumberOfMultipleSessionTestInstances());
 	AppendLineL(KNewLine);	
 	
 	//+ Number of multiple session cancel test instances
 	_LIT(KNumMultipleSessionCancelTestInsts, "Number of Multiple Session Cancel Test Instances: ");
 	AppendLineL(KNumMultipleSessionCancelTestInsts);
 	AppendLineL(aTestParams.NumberOfMultipleSessionCancelTestInstances());
 	AppendLineL(KNewLine);	
 	
 	//+ Heap size percentage level
 	_LIT(KHeapSizePercentLevel, "Heap Size Percentage Level: ");
 	AppendLineL(KHeapSizePercentLevel);
 	AppendLineL(aTestParams.HeapSizePercentageLevel());
 	AppendLineL(KNewLine);	
 	
 
 	//+ Time to fix threshold
 	_LIT(KTimeToFixThreshold, "Time To Fix Threshold: ");
 	AppendLineL(KTimeToFixThreshold);
 	AppendLineL(aTestParams.TimeToFixThreshold());
 	AppendLineL(KNewLine);	
 	
 	//+ Horizontal accuracy threshold
 	_LIT(KHorizontalAcc, "Horizontal Accuracy Threshold: ");
 	AppendLineL(KHorizontalAcc);
 	AppendLineL(aTestParams.HorizontalAccuracyThreshold());
 	AppendLineL(KNewLine);	
 	
 	//+ Vertical accuracy threshold
 	_LIT(KVerticalAcc, "Vertical Accuracy Threshold: ");
 	AppendLineL(KVerticalAcc);
 	AppendLineL(aTestParams.VerticalAccuracyThreshold());
 	AppendLineL(KNewLine);	
 	
 	//+ Maxage max test time
 	_LIT(KMaxageMaxTime, "Maxage Max Test Time: ");
 	AppendLineL(KMaxageMaxTime);
 	AppendLineL(aTestParams.MaxAgeMaxTestTime());
 	AppendLineL(KNewLine);	
 	
 	//+ Max horizontal accuracy
 	_LIT(KMaxHorizAcc, "Max Horizontal Accuracy: ");
 	AppendLineL(KMaxHorizAcc);
 	AppendLineL(aTestParams.MaxHorizontalAccuracy());
 	AppendLineL(KNewLine);	
 	
 	//+ Max vertical accuracy
 	_LIT(KMaxVerticalAcc, "Max Vertical Accuracy: ");
 	AppendLineL(KMaxVerticalAcc);
 	AppendLineL(aTestParams.MaxVerticalAccuracy());
 	AppendLineL(KNewLine);	
 	
 	//+ Reasonable horizontal accuracy
 	_LIT(KReasonableHorizAcc, "Reasonable Horizontal Accuracy: ");
 	AppendLineL(KReasonableHorizAcc);
 	AppendLineL(aTestParams.ResonableHorizontalAccuracy());
 	AppendLineL(KNewLine);	
 	
 	//+ Reasonable vertical accuracy
 	_LIT(KReasonableVerticalAcc, "Reasonable Vertical Accuracy: ");
 	AppendLineL(KReasonableVerticalAcc);
 	AppendLineL(aTestParams.ResonableVertictalAccuracy());
 	AppendLineL(KNewLine);	
 	
 	//+ Max time to first fix
 	_LIT(KMaxTimeToFirstFix, "Max Time To First Fix: ");
 	AppendLineL(KMaxTimeToFirstFix);
 	AppendLineL(aTestParams.MaxTimeToFirstFix());
 	AppendLineL(KNewLine);	
}
 
 
// ---------------------------------------------------------
// CPosPSYLog::MakeTestCaseHeaderL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLog::MakeTestCaseHeaderL(
    const TDesC& aTestName)
    {
    TTime time;         // time in microseconds since 0AD nominal Gregorian
	TDateTime dateTime; // year-month-day-hour-minute-second-microsecond
    TBuf<100> dateAndTime;
    _LIT(KFormat,"%d/%d/%d %d:%d:%d.");
    _LIT(KDateTime,"Test date and time : ");

	time.HomeTime(); // set time to home time
  	
	dateTime=time.DateTime();    // convert to fields
	
    dateAndTime.Format(
        KFormat,
		dateTime.Year(),
		TInt(dateTime.Month()+1),// <-- Print month as a TInt to preserve locale independence
		dateTime.Day()+1,        // <-- Day and month ranges begin at zero (0-30 and 0-11) so add one when printing
		dateTime.Hour(), dateTime.Minute(), dateTime.Second()
    );
    
    AppendLineL(KDelim);
    AppendLineL(aTestName);
    AppendLineL(KNewLine);
    AppendLineL(KDateTime);
    AppendLineL(dateAndTime);
    AppendLineL(KNewLine);
    AppendLineL(KDelim);
    }

// ---------------------------------------------------------
// CPosPSYLog::WriteSummaryAndCloseL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLog::WriteSummaryAndCloseL(TInt aStatus)
    {
    _LIT(KEndDelim, "##########################################################################################\r\n");
    _LIT(KSummary,"Testing completed! Number of Errors = %d | Number of Warnings = %d | Number of Infos = %d\r\n");
    _LIT(KCancelled,"Testing cancelled! Number of Errors = %d | Number of Warnings = %d | Number of Infos = %d\r\n");
	TBuf<KSummaryBufSize> summary;

	if (aStatus == KErrCancel)
		{
		summary.Format(KCancelled, iError, iWarning, iInfo);
		}
	else 
		{
		summary.Format(KSummary, iError, iWarning, iInfo);
		}
	AppendLineL(KNewLine);
    AppendLineL(KEndDelim);
    AppendLineL(KEndDelim);
    AppendLineL(summary);
    AppendLineL(KEndDelim);
    AppendLineL(KEndDelim);
    iCurrentLogFile.Close();
    ResetResults();
    }

// ---------------------------------------------------------
// CPosPSYLog::OpenLog
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLog::OpenLog()
    {
    TInt error;
    error = iCurrentLogFile.Open(iFileSession, iFileName, EFileWrite | EFileShareExclusive);
    if(KErrNone == error)
    {
    	iCurrentLogFile.Seek(ESeekEnd, iCurrentPosition);	
    }
    
    }

// ---------------------------------------------------------
// CPosPSYLog::CloseLog
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLog::CloseLog()
    {
     iCurrentLogFile.Flush();
     iCurrentLogFile.Close();
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::AppendLineL(
    const TDesC& aLine)
    {
    HBufC8* buf;
    buf = HBufC8::NewLC(aLine.Length());
    TPtr8 ptr = buf->Des();
    ptr.Zero();
    ptr.Append(aLine);
    iCurrentLogFile.Write(ptr);
    CleanupStack::PopAndDestroy(buf);
    }

// ---------------------------------------------------------
// CPosPSYLog::AppendLineL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::AppendLineL(
    const TInt aNumber)
    {
    TBuf<KMaxNumberSize> number;
    number.AppendNum(aNumber);
    AppendLineL(number);
    }

// ---------------------------------------------------------
// CPosPSYLog::MakeTestHeaderL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::MakeTestHeaderL(
    const TPosPSYInfo& aInfo)
    {
    _LIT(KName, "PSY Name: ");
    _LIT(KUid, "PSY Uid: ");
    _LIT(KVersion, "PSY Version: ");
    
    AppendLineL(KDelim);
    AppendLineL(KName);
    AppendLineL(aInfo.PSYBaseInfo().Name());
    AppendLineL(KNewLine);
    
    AppendLineL(KUid);
    AppendLineL(aInfo.PSYBaseInfo().Uid().Name());
    AppendLineL(KNewLine);
    
    AppendLineL(KVersion);
    AppendLineL(aInfo.PSYBaseInfo().Version());
    AppendLineL(KNewLine);
    }

// ---------------------------------------------------------
// CPosPSYLog::ResetResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::ResetResults()
    {
    iError = 0;
    iWarning = 0;
    iInfo = 0;
    }

// ---------------------------------------------------------
// CPosPSYLog::GetLogsDirL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::GetLogsDirL(TDes& aLogsDir)
    {
    TDriveNumber cDrive;
    User::LeaveIfError(BaflUtils::GetSystemDrive(cDrive));
    TChar cDriveChar;
    User::LeaveIfError(RFs::DriveToChar(cDrive, cDriveChar));
    aLogsDir.Zero();
    aLogsDir.Append(cDriveChar);
    aLogsDir.Append(KLogsDir);
    }

// ---------------------------------------------------------
// CPosPSYLog::GetPsyTesterLogsDirL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//     
void CPosPSYLog::GetPsyTesterLogsDirL(TDes& aTesterLogsDir)
    {
    TDriveNumber cDrive;
    User::LeaveIfError(BaflUtils::GetSystemDrive(cDrive));
    TChar cDriveChar;
    User::LeaveIfError(RFs::DriveToChar(cDrive, cDriveChar));
    aTesterLogsDir.Zero();
    aTesterLogsDir.Append(cDriveChar);
    aTesterLogsDir.Append(KPsyTesterDir);
    }

//  End of File
