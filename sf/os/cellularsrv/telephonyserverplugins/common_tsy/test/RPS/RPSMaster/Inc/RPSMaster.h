// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
   
#ifndef __RPSMaster_H__
#define __RPSMaster_H__

#include <test/testexecutestepbase.h>   // CTestStep
#include "rpsasciirqstdefs.h"
#include <etel.h>

//
// CRPSMaster.
// This class offers the exported API's for the Remote Party Simulator functionality.
// The API's are used by clients to issue commands to a remote server which executes the commands.
//
class MRPSMBase;
class CRPSMaster //: public CRPSMBase
	{
public:
	IMPORT_C static CRPSMaster* NewL();
	IMPORT_C static CRPSMaster* NewLC();
	IMPORT_C ~CRPSMaster();
	
	// Requests to Slave
	// These return synchronously once the Slave ETEL rqst has been issued to the remote party.
	
	// Single requests...
	IMPORT_C void CallNumberL(const TDesC& aNumToDial, TEtelLine aLine);
	
	IMPORT_C void AnswerNextCallL(TEtelLine aLine);
	IMPORT_C void RejectNextCallL(TEtelLine aLine);
	IMPORT_C void IgnoreNextCallL(TEtelLine aLine);
	
	IMPORT_C void HangUpL(TEtelLine aLine, TInt aDelayInSec=0);
	IMPORT_C void HoldL(TEtelLine aLine, TInt aDelayInSec=0);
	IMPORT_C void ResumeL(TEtelLine aLine, TInt aDelayInSec=0);
	
	IMPORT_C void SendDataBackOnDataPortL(const TDesC& aDataToSend);
	
	// Ganged requests...
	IMPORT_C void CallNumberHangUpL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec=0);
	IMPORT_C void CallNumberHangUpBeforeConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec=0);
	IMPORT_C void CallNumberHangUpAfterConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec=0);
	
	IMPORT_C void AnswerNextCallHangUpL(TEtelLine aLine, TInt aDelayInSec=0);
	IMPORT_C void AnswerNextCallHoldL(TEtelLine aLine, TInt aDelayInSec=0);
	
	IMPORT_C void HangUpNextCallBeforeConnectL(TEtelLine aLine);
	
	IMPORT_C void CleanupL();
	
	// Check for RPS presence
	IMPORT_C void CheckRpsPresentL(TBool& aPresent);
	
	IMPORT_C const TDesC16& RpsSlaveTelNum();
	IMPORT_C const TDesC16& RpsSlaveTelNumData();
	IMPORT_C const TDesC16& RpsSlaveTelNumFax();
	
protected:
	CRPSMaster();
	void ConstructL();
	
private:
	MRPSMBase* 		iImpl;
	};

#endif  // __RPSMaster_H__
