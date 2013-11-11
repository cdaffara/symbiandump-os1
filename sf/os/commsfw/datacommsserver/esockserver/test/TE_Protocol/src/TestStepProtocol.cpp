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
// Contains definition of CTestStepProtocol which is the base class 
// for all the Protocol Test Step classes  		
// 
//

/**
 @file TestStepProtocol.cpp
*/

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <test/es_dummy.h>

// Test system includes
#include "TestStepProtocol.h"
#include "ProtocolTestSection.h"

// static consts
const TUint CTestStepProtocol::KNumTestSockets = 10;
const TInt CTestStepProtocol::KNumExhaustiveSockets = 100;
const TInt CTestStepProtocol::KNumStretchOpens = 16;



// constructor
CTestStepProtocol::CTestStepProtocol() : iErrText()
	{
	}

// destructor
CTestStepProtocol::~CTestStepProtocol()
	{
	delete iDummy;
	}


enum TVerdict CTestStepProtocol::doTestStepL()
	{
	
	TRAPD(trapRet,
		Logger().ShareAuto();
	enum TVerdict stepRet = InternalDoTestStepL();
	if (stepRet !=EPass)
		{
		SetTestStepResult(stepRet);
		}
		)
		
		if (trapRet != KErrNone)
			{
			SetTestStepResult(EFail);
			return EFail;
			}
		return EPass;
	};


// extra logging methods
const TPtrC& CTestStepProtocol::EpocErrorToText(const TInt aErrCode)
/**
Convert a Error code to text.

  @param aError error code to display as text.
  @return Text describing the error.
  */
	{
	//	TPtr errText;
	switch (aErrCode)
		{
		case KErrNone:
			iErrText.Set(_L("KErrNone"));
			break;
		case KErrNotFound:
			iErrText.Set(_L("KErrNotFound"));
			break;
		case KErrGeneral:
			iErrText.Set(_L("KErrGeneral"));
			break;
		case KErrCancel:
			iErrText.Set(_L("KErrCancel"));
			break;
		case KErrNoMemory:
			iErrText.Set(_L("KErrNoMemory"));
			break;
		case KErrNotSupported:
			iErrText.Set(_L("KErrNotSupported"));
			break;
		case KErrArgument:
			iErrText.Set(_L("KErrArgument"));
			break;
		case KErrTotalLossOfPrecision:
			iErrText.Set(_L("KErrTotalLossOfPrecision"));
			break;
		case KErrBadHandle:
			iErrText.Set(_L("KErrBadHandle"));
			break;
		case KErrOverflow:
			iErrText.Set(_L("KErrOverflow"));
			break;
		case KErrUnderflow:
			iErrText.Set(_L("KErrUnderflow"));
			break;
		case KErrAlreadyExists:
			iErrText.Set(_L("KErrAlreadyExists"));
			break;
		case KErrPathNotFound:
			iErrText.Set(_L("KErrPathNotFound"));
			break;
		case KErrDied:
			iErrText.Set(_L("KErrDied"));
			break;
		case KErrInUse:
			iErrText.Set(_L("KErrInUse"));
			break;
		case KErrServerTerminated:
			iErrText.Set(_L("KErrServerTerminated"));
			break;
		case KErrServerBusy:
			iErrText.Set(_L("KErrServerBusy"));
			break;
		case KErrCompletion:
			iErrText.Set(_L("KErrCompletion"));
			break;
		case KErrNotReady:
			iErrText.Set(_L("KErrNotReady"));
			break;
		case KErrUnknown:
			iErrText.Set(_L("KErrUnknown"));
			break;
		case KErrCorrupt:
			iErrText.Set(_L("KErrCorrupt"));
			break;
		case KErrAccessDenied:
			iErrText.Set(_L("KErrAccessDenied"));
			break;
		case KErrLocked:
			iErrText.Set(_L("KErrLocked"));
			break;
		case KErrWrite:
			iErrText.Set(_L("KErrWrite"));
			break;
		case KErrDisMounted:
			iErrText.Set(_L("KErrDisMounted"));
			break;
		case KErrEof:
			iErrText.Set(_L("KErrEof"));
			break;
		case KErrDiskFull:
			iErrText.Set(_L("KErrDiskFull"));
			break;
		case KErrBadDriver:
			iErrText.Set(_L("KErrBadDriver"));
			break;
		case KErrBadName:
			iErrText.Set(_L("KErrBadName"));
			break;
		case KErrCommsLineFail:
			iErrText.Set(_L("KErrCommsLineFail"));
			break;
		case KErrCommsFrame:
			iErrText.Set(_L("KErrCommsFrame"));
			break;
		case KErrCommsOverrun:
			iErrText.Set(_L("KErrCommsOverrun"));
			break;
		case KErrCommsParity:
			iErrText.Set(_L("KErrCommsParity"));
			break;
		case KErrTimedOut:
			iErrText.Set(_L("KErrTimedOut"));
			break;
		case KErrCouldNotConnect:
			iErrText.Set(_L("KErrCouldNotConnect"));
			break;
		case KErrCouldNotDisconnect:
			iErrText.Set(_L("KErrCouldNotDisconnect"));
			break;
		case KErrDisconnected:
			iErrText.Set(_L("KErrDisconnected"));
			break;
		case KErrBadLibraryEntryPoint:
			iErrText.Set(_L("KErrBadLibraryEntryPoint"));
			break;
		case KErrBadDescriptor:
			iErrText.Set(_L("KErrBadDescriptor"));
			break;
		case KErrAbort:
			iErrText.Set(_L("KErrAbort"));
			break;
		case KErrTooBig:
			iErrText.Set(_L("KErrTooBig"));
			break;
		case KErrCannotFindProtocol:
			iErrText.Set(_L("KErrCannotFindProtocol"));
			break;
		case KErrPermissionDenied:
			iErrText.Set(_L("KErrPermissionDenied"));
			break;	
		default:
			iErrBuf.Format(_L(" %d"),aErrCode);
			iErrText.Set(iErrBuf.Ptr());
			break;
		}
		return iErrText;
	}	


