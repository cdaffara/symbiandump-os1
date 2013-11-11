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
* Description:  ErrConcealmentIntfc
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ErrorConcealmentIntfcTestClass.h"
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
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CErrorConcealmentIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iErrorConcealment)
		{
		delete iErrorConcealment;
    	}

	if (iDevSound)
		{
   		delete iDevSound;
   		}

	if (iAudioOutputStream)
		{
		delete iAudioOutputStream;
		}
}

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CErrorConcealmentIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		//ENTRY( "CreateDevSoundInstance", CErrorConcealmentIntfcTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateErrorConcealmentIntfc", CErrorConcealmentIntfcTestClass::CreateErrorConcealmentIntfc ),
  		ENTRY( "ConcealErrorForNextBuffer", CErrorConcealmentIntfcTestClass::ConcealErrorForNextBuffer ),
		ENTRY( "SetFrameMode", CErrorConcealmentIntfcTestClass::SetFrameMode ),
		ENTRY( "FrameModeRqrdForEC", CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC ),
		ENTRY( "CreateObject", CErrorConcealmentIntfcTestClass::CreateObject ),

		ENTRY( "SetConfigureData", CErrorConcealmentIntfcTestClass::SetConfigureData ),
		ENTRY( "GetConfigureData", CErrorConcealmentIntfcTestClass::GetConfigureData ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CErrorConcealmentIntfcTestClass::EventName( TInt aKey )
{
	static TText* const badKeyword = (TText*)L"BadKeyword";
	static TText* const keywords[] =
		{

		(TText*)L"EInitializeComplete",
		(TText*)L"EMaoscOpenComplete",

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
// CErrorConcealmentIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CErrorConcealmentIntfcTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::AddExpectedEvent")));
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
// CErrorConcealmentIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CErrorConcealmentIntfcTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::RemoveExpectedEvent")));
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
// CErrorConcealmentIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CErrorConcealmentIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CErrorConcealmentIntfcTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::ProcessExpectedEvent")));
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
// CErrorConcealmentIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CErrorConcealmentIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::HandleTimeout")));
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
// CErrorConcealmentIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CErrorConcealmentIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::SetTimeout")));
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
TInt CErrorConcealmentIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::SetAllowedPanic")));
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
// CErrorConcealmentIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CErrorConcealmentIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CErrorConcealmentIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KErrorConcealmentIntfcTestClass, "ErrorConcealmentIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KErrorConcealmentIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KErrorConcealmentIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CErrorConcealmentIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetFrameMode();

	return error;
}



TInt CErrorConcealmentIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	FrameModeRqrdForEC();
	ConcealErrorForNextBuffer();
    return error;
}




TInt CErrorConcealmentIntfcTestClass::SetFrameMode()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::SetFrameMode")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::SetFrameMode"));
	TInt error = KErrNone;
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::SetFrameMode"));
	error = iErrorConcealment->SetFrameMode(ETrue);
	if (error != KErrNone)
		{
		iLog->Log(_L("CErrorConcealmentIntfcTestClass::SetFrameMode ERROR [%d]"),error);
		}
	return error;
}

//////////////////////////
// GetData
//////////////////////////

TInt CErrorConcealmentIntfcTestClass::ConcealErrorForNextBuffer()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::ConcealErrorForNextBuffer")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::ConcealErrorForNextBuffer"));
	TInt error = KErrNone;

	error = iErrorConcealment->ConcealErrorForNextBuffer();

	if (error != KErrNone)
		{
		iLog->Log(_L("CErrorConcealmentIntfcTestClass::ConcealErrorForNextBuffer ERROR [%d]"),error);
		}
    return error;
}


TInt CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC()
{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iErrorConcealment->FrameModeRqrdForEC(returnOutput);

	if (returnOutput)
		{
		iLog->Log(_L("CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC [%d]"),error);
		return error;
		}
	else
		{
		iLog->Log(_L("CErrorConcealmentIntfcTestClass::FrameModeRqrdForEC ERROR [%d]"),KErrUnexpectedValue);
		return KErrUnexpectedValue;
		}
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CErrorConcealmentIntfcTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CErrorConcealmentIntfcTestClass::CreateObject")));
	iLog->Log(_L("CErrorConcealmentIntfcTestClass::CreateObject"));

	TInt error = KErrNone;
    TFourCC KG711FourCC('G','7','1','1');
	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		if (error == KErrNone)
		    {

		    TRAP(error,iDevSound->InitializeL(*this,KG711FourCC,EMMFStatePlaying));
		    if(error == KErrNone)
		        {
		        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
		        }
		    else
		        {
		        iLog->Log(_L("CErrorConcealmentIntfcTestClass::InitializeL Error [%d]"),error);
		        }
		    }
	    else
	        {
	        iLog->Log(_L("CErrorConcealmentIntfcTestClass::CreateDevSoundInstance Error [%d]"),error);
	        }
		}
	else if (object == KTagOutputStream)
		{
		TRAPD(error, iAudioOutputStream = CMdaAudioOutputStream::NewL(*this));
		if(error == KErrNone)
			{
			iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
			iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
			iStreamSettings.iVolume = 1;
			TRAPD(error, iAudioOutputStream->SetDataTypeL(KG711FourCC));
			if(error == KErrNone)
				{
				iAudioOutputStream->Open(&iStreamSettings);
				AddExpectedEvent(EMaoscOpenComplete, KMediumTimeout);
				iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
				}
			else
				{
				iLog->Log(_L("CMdaAudioOutputStream::NewL Error [%d]"),error);
				}
			}
		}
	else
		{
		return KErrNotSupported;
		}
	return error;
	}

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CErrorConcealmentIntfcTestClass::CreateErrorConcealmentIntfc( CStifItemParser& aItem )
	{
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		FTRACE(FPrint(_L("CErrorConcealmentIntfc::NewL(*iDevSound)")));
		iLog->Log(_L("CErrorConcealmentIntfc::NewL(*iDevSound)"));

		TRAP(error, iErrorConcealment = CErrorConcealmentIntfc::NewL(*iDevSound));
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L("CErrorConcealmentIntfc::NewL(*iAudioOutputStream)")));
		iLog->Log(_L("CErrorConcealmentIntfc::NewL(*iAudioOutputStream)"));

		TRAP(error, iErrorConcealment = CErrorConcealmentIntfc::NewL(*iAudioOutputStream));
		}

	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateErrorConcealmentIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateErrorConcealmentIntfc FAILED %d"),error);
		}
	return error;
	}

//  End of File
