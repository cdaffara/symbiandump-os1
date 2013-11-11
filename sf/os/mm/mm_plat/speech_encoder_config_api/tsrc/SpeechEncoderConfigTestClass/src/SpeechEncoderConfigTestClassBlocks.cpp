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
* Description: SpeechEncoderConfigTest
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "SpeechEncoderConfigTestClass.h"
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
// CSpeechEncoderConfigTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSpeechEncoderConfigTestClass::Delete()
{

    FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();
	iSupportedBitrates.Close();


    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iSpeechEncoder)
		{
		delete iSpeechEncoder;
		}

	if (iDevSound)
		{
		delete iDevSound;
		}

	if (iAudioInputStream)
	   	{
	   	delete iAudioInputStream;
		}
}

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSpeechEncoderConfigTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		ENTRY( "CreateDevSoundInstance", CSpeechEncoderConfigTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateInputStreamInstance", CSpeechEncoderConfigTestClass::CreateInputStreamInstance),
		ENTRY( "CreateSpeechEncoderConfig", CSpeechEncoderConfigTestClass::CreateSpeechEncoderConfig ),
  		ENTRY( "SetBitrate", CSpeechEncoderConfigTestClass::SetBitrate ),
		ENTRY( "SetVadMode", CSpeechEncoderConfigTestClass::SetVadMode ),
		ENTRY( "GetVadMode", CSpeechEncoderConfigTestClass::GetVadMode ),
  		ENTRY( "GetBitrate", CSpeechEncoderConfigTestClass::GetBitrate ),
		ENTRY( "GetSupportedBitrates", CSpeechEncoderConfigTestClass::GetSupportedBitrates )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CSpeechEncoderConfigTestClass::EventName( TInt aKey )
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
// CSpeechEncoderConfigTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CSpeechEncoderConfigTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::AddExpectedEvent")));
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
// CSpeechEncoderConfigTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CSpeechEncoderConfigTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::RemoveExpectedEvent")));
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
// CSpeechEncoderConfigTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CSpeechEncoderConfigTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CSpeechEncoderConfigTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::ProcessExpectedEvent")));
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
// CSpeechEncoderConfigTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CSpeechEncoderConfigTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::HandleTimeout")));
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
// CSpeechEncoderConfigTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CSpeechEncoderConfigTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetTimeout")));
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
TInt CSpeechEncoderConfigTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetAllowedPanic")));
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
// CSpeechEncoderConfigTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CSpeechEncoderConfigTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSpeechEncoderConfigTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KSpeechEncoderConfigTestClass, "SpeechEncoderConfigTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KSpeechEncoderConfigTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KSpeechEncoderConfigTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CSpeechEncoderConfigTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetConfigureData")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetBitrate();
    SetVadMode();

	return error;
}



TInt CSpeechEncoderConfigTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::GetConfigureData")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetVadMode();
	GetBitrate();

    return error;
}




TInt CSpeechEncoderConfigTestClass::SetBitrate( )
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetBitrate")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::SetBitrate"));
	TInt error = KErrNone;

	iLog->Log(_L("CSpeechEncoderConfigTestClass::SetBitrate 64000"));
	error = iSpeechEncoder->SetBitrate(64000);

	return error;
}



TInt CSpeechEncoderConfigTestClass::SetVadMode( )
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::SetVadMode")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::SetVadMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CSpeechEncoderConfigTestClass::SetVadMode - %d"), EFalse);
	error = iSpeechEncoder->SetVadMode(EFalse);

	return error;
}



//////////////////////////
// GetData
//////////////////////////

TInt CSpeechEncoderConfigTestClass::GetVadMode()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::GetVadMode")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::GetVadMode"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iSpeechEncoder->GetVadMode(returnOutput);

	if (!returnOutput)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}


TInt CSpeechEncoderConfigTestClass::GetBitrate()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::GetBitrate")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::GetBitrate"));
	TInt error = KErrNone;

	TUint returnOutput;
	error = iSpeechEncoder->GetBitrate(returnOutput);
	iLog->Log(_L("CSpeechEncoderConfigTestClass::GetBitrate %d"),returnOutput);
	if (returnOutput == 64000)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}


TInt CSpeechEncoderConfigTestClass::GetSupportedBitrates()
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::GetSupportedBitrates")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::GetSupportedBitrates"));
	TInt error = KErrNone;

	TUint supportedBitrates;
	error = iSpeechEncoder->GetSupportedBitrates(iSupportedBitrates)  ;
	for (int k = 0; k<iSupportedBitrates.Count(); ++k)
		{
		supportedBitrates = iSupportedBitrates[k];

		iLog->Log(_L("GetSupportedTools - supporttool %d"), supportedBitrates);
		}

	return error;
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//


TInt CSpeechEncoderConfigTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::CreateDevSoundInstance"));
	TInt error = KErrNone;

    TRAP(error, iDevSound = CMMFDevSound::NewL());

	if (error == KErrNone)
	    {
	    TFourCC KG711FourCC('G','7','1','1');
	    TRAP(error,iDevSound->InitializeL(*this,KG711FourCC,EMMFStateRecording));
	    if(error == KErrNone)
	        {
	        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
	        }
	    else
	        {
	        iLog->Log(_L("CSpeechEncoderConfigTestClass::InitializeL Error [%d]"),error);
	        }
	    }
    else
        {
        iLog->Log(_L("CSpeechEncoderConfigTestClass::CreateDevSoundInstance Error [%d]"),error);
        }

	return error;
	}

TInt CSpeechEncoderConfigTestClass::CreateInputStreamInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::CreateInputStreamInstance")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::CreateInputStreamInstance"));
	TInt error = KErrNone;

    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this));
    if (error == KErrNone)
    	{
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iVolume = 1;
	    TFourCC KG711FourCC('G','7','1','1');
    	TRAP(error, iAudioInputStream->SetDataTypeL(KG711FourCC));
        iLog->Log(_L("iAudioInputStream->SetDataTypeL(KG711FourCC) [%d]"),error);
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
// CSpeechEncoderConfigTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CSpeechEncoderConfigTestClass::CreateSpeechEncoderConfig( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CSpeechEncoderConfigTestClass::CreateSpeechEncoderConfig")));
	iLog->Log(_L("CSpeechEncoderConfigTestClass::CreateSpeechEncoderConfig"));
	TInt error = KErrNone;

	if (iDevSound)
		{
		TRAP(error, iSpeechEncoder = CSpeechEncoderConfig::NewL(*iDevSound));
		}
	else if (iAudioInputStream)
		{
		TRAP(error, iSpeechEncoder = CSpeechEncoderConfig::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateSpeechEncoderConfig-FAILED %d"),error));
		iLog->Log(_L("CreateSpeechEncoderConfig FAILED %d"),error);
		}
	return error;
}

//  End of File
