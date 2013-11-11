/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Ra8DecoderIntfc Test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "Ra8DecoderIntfcTestClass.h"
#include "debug.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name .
//
// Returns: ?value_1:
//          ?value_n: _line1
//                    _line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  //
    ?arg_type arg)  //
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CRa8DecoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iRa8Decoder)
    {
    delete iRa8Decoder;
    }

	if (iDevSound)
   	{
	delete iDevSound;
	}

}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CRa8DecoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateRa8DecoderIntfc", CRa8DecoderIntfcTestClass::CreateRa8DecoderIntfc ),
		ENTRY( "CreateDevSoundInstance", CRa8DecoderIntfcTestClass::CreateDevSoundInstance ),
  		ENTRY( "SetInitString", CRa8DecoderIntfcTestClass::SetInitString ),
		ENTRY( "FrameNumber", CRa8DecoderIntfcTestClass::FrameNumber ),
		ENTRY( "InitDevSound", CRa8DecoderIntfcTestClass::InitDevSound ),


        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CRa8DecoderIntfcTestClass::EventName( TInt aKey )
{
	static TText* const badKeyword = (TText*)L"BadKeyword";
	static TText* const keywords[] =
	{

		(TText*)L"EInitializeComplete",

	};



	if( (TUint)aKey >= (sizeof( keywords )/sizeof(TText*)) )
	{
		iLog->Log(_L("Keyword out of bounds"));
		TPtrC keyword( badKeyword );
		return keyword;
	}
	else
	{
		TPtrC keyword( keywords[aKey] );
		return keyword;
	}
}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CRa8DecoderIntfcTestClass::AddExpectedEvent(TRa8ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::AddExpectedEvent")));
	iExpectedEvents.Append(event);
	TPtrC eventName = EventName(event);
	iLog->Log(_L("Adding expected event:(0x%02x)%S Total=%d"), event, &eventName, iExpectedEvents.Count() );

	if ( iTimeoutController && !iTimeoutController->IsActive() )
	{
		if (ms > 0)
			{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(ms * 1000) );
			}
		else
			{
			iLog->Log(_L("Timeout with default value (1s)"));
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
			}
	}
}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CRa8DecoderIntfcTestClass::RemoveExpectedEvent(TRa8ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::RemoveExpectedEvent")));
	TBool match = EFalse;
	for (TUint i=0; i < iExpectedEvents.Count() ; i++)
	{
		if (iExpectedEvents[i] == aEvent)
		{
			iExpectedEvents.Remove(i);
			match = ETrue;
			break;
		}
	}

	return match;
}


// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CRa8DecoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CRa8DecoderIntfcTestClass::ProcessEvent(TRa8ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::ProcessExpectedEvent")));
	TPtrC nameEvent = EventName(aEvent);

	// Check for error
	if (aError == KErrNone)
	{
		// Remove the event
		if (RemoveExpectedEvent(aEvent))
		{
			iLog->Log(_L("Expected Event: (0x%02x)%S has ocurred Total=%d"), aEvent, &nameEvent,iExpectedEvents.Count());
		}
		else
		{
			iLog->Log(_L("Event: (0x%02x)%S has ocurred"), aEvent, &nameEvent);
			return;
		}



		// All expected events have ocurred
		if (iExpectedEvents.Count() == 0 )
		{
			Signal();
			iTimeoutController->Cancel();
		}


	}
	else
	{
		iLog->Log(_L("[Error] Event: (0x%02x)%S return with error code=%d"), aEvent, &nameEvent, aError);
		if (iExpectedEvents.Count() != 0 )
		{
			RemoveExpectedEvent(aEvent);
		}
		iTimeoutController->Cancel();
		Signal(KErrCallbackErrorCode);
	}
}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CRa8DecoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::HandleTimeout")));
	// All expected events have ocurred
	if (aError != KErrNone)
	{
		if (iExpectedEvents.Count() == 0 )
		{
			iLog->Log(_L("Timing out but events have ocurred"));
			Signal();
		}
		else
		{
			RemoveAllExpectedEvents();
			iLog->Log(_L("Timing out and events still pending"));
			Signal(KErrEventPending);
		}
	}
	else
	{
		iLog->Log(_L("Timing out return a error %d"), aError);
		Signal(aError);
	}
}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CRa8DecoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::SetTimeout")));
	TInt timeout=0;
	TInt error = aItem.GetNextInt(timeout) ;
	if ( iTimeoutController )
	{
		if ( timeout > 0 )
		{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(timeout*1000) );
		}
		else
		{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
		}
	}
	else
	{
		iLog->Log(_L("Timeout Controller doesn't exist"));
		error = KErrTimeoutController;
	}
	return error;
}


// -----------------------------------------------------------------------------
// Uses the TestModuleBase API to allow a panic as exit reason for a test case
// -----------------------------------------------------------------------------
TInt CRa8DecoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::SetAllowedPanic")));
	TInt error = KErrNone;
	TInt panicCode;
	TPtrC panicType;
	if (  ( KErrNone == aItem.GetNextString(panicType) ) &&
	( KErrNone == aItem.GetNextInt(panicCode) )  )
	{
		iLog->Log(_L("Allowing panic: %S %d"), &panicType, panicCode);
		iTestModuleIf.SetExitReason( CTestModuleIf::EPanic, panicCode );
		iNormalExitReason = EFalse;
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}
	return error;
}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CRa8DecoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
	{
		AddExpectedEvent(static_cast<TRa8ExpectedEvent>(event), 0); // Default timeout value
	}
	return error;
}


// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRa8DecoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KRa8DecoderIntfcTestClass, "Ra8DecoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KRa8DecoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KRa8DecoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CRa8DecoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CRa8DecoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;
    error = SetInitString();

	return error;
}



TInt CRa8DecoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CRa8DecoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	FrameNumber();
    return error;
}


TInt CRa8DecoderIntfcTestClass::SetInitString()
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::SetInitString")));
	iLog->Log(_L("CRa8DecoderIntfcTestClass::SetInitString"));
	TInt error = KErrNone;

    TBuf8<20> testString;
    testString.Copy( _L( "abcd" ) );

	error = iRa8Decoder->SetInitString(testString);

	return error;
}
//////////////////////////
// GetData
//////////////////////////

TInt CRa8DecoderIntfcTestClass::FrameNumber()
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::FrameNumber")));
	iLog->Log(_L("CRa8DecoderIntfcTestClass::FrameNumber"));
	TInt error = KErrNone;

	TInt result = iRa8Decoder->FrameNumber();

	return error;
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create DevSound
// -----------------------------------------------------------------------------
//
TInt CRa8DecoderIntfcTestClass::InitDevSound( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::InitDevSound")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitDevSound"));
	TInt error = KErrNone;
	if (error == KErrNone)
	    {
	    TFourCC KRA8FourCC(' ','R','A','8');
	    TRAP(error,iDevSound->InitializeL(*this,KRA8FourCC,EMMFStatePlaying));
	    if(error == KErrNone)
	        {
	        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
	        }
	    else
	        {
	        iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitializeL Error [%d]"),error);
	        }
	    }
	return error;
	}


TInt CRa8DecoderIntfcTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::CreateDevSoundInstance"));
	TInt error = KErrNone;

    TRAP(error, iDevSound = CMMFDevSound::NewL());
	if (error == KErrNone)
	    {
	    iLog->Log(_L("CMMFDevSound::NewL() OK"));
	    }
    else
        {
        iLog->Log(_L("CMMFDevSound::NewL() Error [%d]"),error);
        }
	return error;
	}

// -----------------------------------------------------------------------------
// CRa8DecoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CRa8DecoderIntfcTestClass::CreateRa8DecoderIntfc( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRa8DecoderIntfcTestClass::CreateRa8DecoderIntfc")));
	iLog->Log(_L("CRa8DecoderIntfcTestClass::CreateRa8DecoderIntfc"));
	TInt error = KErrNone;
	TRAP(error, iRa8Decoder = CRa8CustomInterface::NewL(*iDevSound));
	if (error != KErrNone)
        {
        iLog->Log(_L("CRa8DecoderIntfcTestClass::CreateRa8DecoderIntfc Error [%d]"),error);
        }
	return error;
}

//  End of File
