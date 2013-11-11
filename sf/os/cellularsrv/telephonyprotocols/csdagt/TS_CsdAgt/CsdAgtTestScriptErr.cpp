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
// CsdAgtTestStateMachineInit.cpp
// This contains CsdAgt TestCase Test 5.1 - 5.15
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <dummyetel.h> // include here to avoid picking up the real etel.h
#include <etelmm.h>
#include <nd_err.h>

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "TestSuiteCsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"
// EFinishedDialling
#include <csdprog.h>

// Class definitions
#include "CsdAgtTestScriptErr.h"


// The tests in this file test the following script processing conditions:
//
// KErrNoExpression
// KErrMissingQuote
// KErrVariableNotFound
// KErrInvalidNumber
// KErrNumberOutOfRange
// KErrNoEquals
// KErrNoOpenExpresion
// KErrNestedLoop
// KErrLoopCounterOutOfRange
// KErrMissingChevron
// KErrIllegalCommand
// KErrNoDropOrRaise
// KErrVariableNameTooLong
// KErrMissingBracket
// KErrExitScriptTimeOut
// KErrCharacterSetDoesNotExist
//
// In fact, apart from KErrExitScriptTimeOut, these are all error values internal to csdagt, and get reduced to 
// KErrExitScriptError before they are visible to the test harness.
// If you need to check that a specific error value was generated for a test, then the
// information is logged in Logs\Netdial\NetDial.txt
// You then need to check the error represented by the logged number in nd_std.h
//

void CCsdAgtTest5_xx::CustomiseLoginScriptL(const TDesC16& aLoginScript)
{
	csd = CDbCsdTable::NewL();
	CleanupStack::PushL(csd);

	csd->SetColumnBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	csd->SetColumnLongTextL(TPtrC(ISP_LOGIN_SCRIPT),aLoginScript);
}
void CCsdAgtTest5_xx::RevertCommdbChangesL()
{
	Log(_L("Reverting COMMDB changes"));

    csd->UndoDatabaseChangesL();

    CleanupStack::PopAndDestroy(csd);
}


enum TVerdict CCsdAgtTest5_xx::DoScriptErrTestL(TInt aTestNo, const TDesC& aLoginScript)
{
	char OK = 1;

	TInt ret = KErrGeneral;

	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(aTestNo);

	if(OK)
	{
	   TRAP(ret,SetGSMModeL(AgtNotify));
	   OK = (ret == KErrNone);
	}

	// Set up the login script to use.
	if(OK)
	   CustomiseLoginScriptL(aLoginScript);

	if(OK)
	{
		switch(aTestNo)
		{
		case 10515: // special case - timeout gets reported as such
		case 10517:
		case 10518:
			{
			TRAP(ret,ConnectExpectErrorL(KErrExitScriptTimeOut,ECsdFinishedLogIn));
			}
			break;
		case 10516: // no error expected here.
			{
	        TRAP(ret,NormalConnectL());
			}
			break;
		default:
			{
			TRAP(ret,ConnectExpectErrorL(KErrExitScriptError,ECsdFinishedLogIn));
			}
			break;
		}
	}		
	if(OK) OK = (ret == KErrNone);

	// Close database object, forcing it to undo all changes that were made
	RevertCommdbChangesL();

	return(OK ? EPass : EFail);
}
enum TVerdict CCsdAgtTest5_1::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10501, _L("SEND\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_2::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10502, _L("SEND \"AT+<0x0d>\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_3::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10503, _L("SEND login_init11$+<0x0d>\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_4::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10504, _L("LOOP 2147483648\n{\n}\nExit KErrNoModem$\nalive:\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_5::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10505, _L("WAIT 500001\n{\n\"OK\" alive\n}\nExit KErrNoModem$\nalive:\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_6::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10506, _L("SET err$ 0\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_7::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10507, _L("WAIT 10\n\"OK\" alive\n}\nExit KErrNoModem$\nalive:\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_8::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10508, _L("LOOP 2\n{\nSEND \"AT\"+<0xd>\nLOOP 4\n{\nSEND \"AT\"+<0xd>\n}\n}\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_9::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10509, _L("LOOP 10001\n{\nSEND \"AT\"+<0xd>\n}\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_10::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10510, _L("SEND \"AT\"+<0x0d\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_11::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10511, _L("WIAT 10\n{\n\"OK\" alive\n}\nExit KErrNoModem$\nalive:\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_12::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10512, _L("DTR\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_13::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10513, _L("SET e12345678901234567890$=\"hello\"\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_14::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10514, _L("SEND \"Hello\"+<0x0d> [UNICODE\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_15::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10515, _L("WAIT 3\n{\n}\nExit KErrTimeOut$\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_16::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10516, _L("SEND \"Hello\"+<0x0d>\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_17::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10517, _L("letsgo:\nWAIT 20\n{\n\"ogin:\" loginok\n}\nGOTO timeout\nloginok:\nSEND login_name$+<0x0d>\nWAIT 20\n{\n\"assword:\" passok\n}\nGOTO timeout\npassok:\nSEND login_pass$+<0x0d>\nWAIT 20\n{\n\"rotocol:\" protocolok\n}\nGOTO lofail\nprotocolok:\nSend \"PPP\"+<0x0d>\nEXIT\ntimeout:\nEXIT KErrTimeOut$\nlofail:\nEXIT KErrLoginFail$\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_18::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10518, _L("letsgo:\nWAIT 20\n	{\n\"ogin:\" loginok\n}\nGOTO timeout\nloginok:\n! READ\nWAIT 20\n{\n\"assword:\" passok\n}\nGOTO timeout\npassok:\n! READ\nWAIT 20\n{\n\"rotocol:\" protocolok\n}\nGOTO lofail\nprotocolok:\nSend \"PPP\"+<0x0d>\nEXIT\ntimeout:\nEXIT KErrTimeOut$\nlofail:\nEXIT KErrLoginFail$\n"));
	return(iTestStepResult);
	}
enum TVerdict CCsdAgtTest5_19::doCsdAgtTestStepL( void )
	{
	iTestStepResult = DoScriptErrTestL(10519, _L("CHARMAP [UNKNOWN]\nletsgo:\nWAIT 20\n{\n\"ogin:\" loginok\n}\nGOTO timeout\nloginok:\nWAIT 20\n{\n\"assword:\" passok\n}\nGOTO timeout\npassok:\nWAIT 20\n{\n\"rotocol:\" protocolok\n}\nGOTO lofail\nprotocolok:\nSend \"PPP\"+<0x0d>\nEXIT\ntimeout:\nEXIT KErrTimeOut$\nlofail:\nEXIT KErrLoginFail$\n"));
	return(iTestStepResult);
	}
