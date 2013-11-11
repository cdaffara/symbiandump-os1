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
* Description: g729 encoder interface test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "G729EncoderIntfcTestClass.h"
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
// CG729EncoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CG729EncoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iG729Encoder)
		{
		delete iG729Encoder;
		}
	if (iAudioInputStream)
	   	{
	   	delete iAudioInputStream;
		}
    if (iDevSound)
        {
        delete iDevSound;
        }
    REComSession::FinalClose();
}

// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CG729EncoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateG729EncoderIntfc", CG729EncoderIntfcTestClass::CreateG729EncoderIntfc ),
		ENTRY( "CreateDevSoundInstance", CG729EncoderIntfcTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateInputStreamInstance", CG729EncoderIntfcTestClass::CreateInputStreamInstance),
  		ENTRY( "SetVadMode", CG729EncoderIntfcTestClass::SetVadMode ),
		ENTRY( "GetVadMode", CG729EncoderIntfcTestClass::GetVadMode ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CG729EncoderIntfcTestClass::EventName( TInt aKey )
{
	static TText* const badKeyword = (TText*)L"BadKeyword";
	static TText* const keywords[] =
		{

		(TText*)L"EInitializeComplete",
		(TText*)L"EMaiscOpenComplete",

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
// CG729EncoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CG729EncoderIntfcTestClass::AddExpectedEvent(TG729ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::AddExpectedEvent")));
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
// CG729EncoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CG729EncoderIntfcTestClass::RemoveExpectedEvent(TG729ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::RemoveExpectedEvent")));
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
// CG729EncoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CG729EncoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CG729EncoderIntfcTestClass::ProcessEvent(TG729ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::ProcessExpectedEvent")));
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
// CG729EncoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CG729EncoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::HandleTimeout")));
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
// CG729EncoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CG729EncoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::SetTimeout")));
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
TInt CG729EncoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::SetAllowedPanic")));
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
// CG729EncoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CG729EncoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG729ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CG729EncoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KG729EncoderIntfcTestClass, "G729EncoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KG729EncoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KG729EncoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CG729EncoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

    SetVadMode( );

	return error;
}



TInt CG729EncoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetVadMode();
    return error;
}


TInt CG729EncoderIntfcTestClass::SetVadMode( )
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::SetVadMode")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::SetVadMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CG729EncoderIntfcTestClass::SetVadMode - %d"), EFalse);
	error = iG729Encoder->SetVadMode(EFalse);

	return error;
}



//////////////////////////
// GetData
//////////////////////////

TInt CG729EncoderIntfcTestClass::GetVadMode()
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::GetVadMode")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::GetVadMode"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iG729Encoder->GetVadMode(returnOutput);

	if (!returnOutput)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}


// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::CreateFactory
// Create Factory
// -----------------------------------------------------------------------------
//


TInt CG729EncoderIntfcTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::CreateDevSoundInstance"));
	TInt error = KErrNone;

    TRAP(error, iDevSound = CMMFDevSound::NewL());
	if (error == KErrNone)
	    {
	    TFourCC KG729FourCC('G','7','2','9');
	    TRAP(error,iDevSound->InitializeL(*this,KG729FourCC,EMMFStateRecording));
	    if(error == KErrNone)
	        {
	        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
	        }
	    else
	        {
	        iLog->Log(_L("CG729EncoderIntfcTestClass::InitializeL Error [%d]"),error);
	        }
	    }
    else
        {
        iLog->Log(_L("CG729EncoderIntfcTestClass::CreateDevSoundInstance Error [%d]"),error);
        }
	return error;
	}

TInt CG729EncoderIntfcTestClass::CreateInputStreamInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance"));
	TInt error = KErrNone;
    TFourCC KG729FourCC('G','7','2','9');

    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this));
    if (error == KErrNone)
    	{
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iVolume = 1;

    	TRAP(error, iAudioInputStream->SetDataTypeL(KG729FourCC));
        iLog->Log(_L("iAudioInputStream->SetDataTypeL(KG729FourCC) [%d]"),error);
		iAudioInputStream->Open(&iStreamSettings);
	    if (error == KErrNone)
		    {
	        AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout);
		    iLog->Log(_L(" AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout)"));
		    }
	    else
	        {
	        iLog->Log(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
	        }
    	}
    else
    	{
        iLog->Log(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
    	}

	return error;
	}

// -----------------------------------------------------------------------------
// CG729EncoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CG729EncoderIntfcTestClass::CreateG729EncoderIntfc( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::CreateG729EncoderIntfc")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::CreateG729EncoderIntfc"));
	TInt error = KErrNone;

	if (iDevSound)
		{
		TRAP(error, iG729Encoder = CG729EncoderIntfc::NewL(*iDevSound));
		}
	else if (iAudioInputStream)
		{
		TRAP(error, iG729Encoder = CG729EncoderIntfc::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateG729EncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateG729EncoderIntfc FAILED %d"),error);
		}


	return error;
}

//  End of File
