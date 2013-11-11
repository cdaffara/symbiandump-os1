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
* Description:  AacDecoderConfigTestClass test module.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "AacDecoderConfigTestClass.h"
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
// CAacDecoderConfigTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CAacDecoderConfigTestClass::Delete()
{

    FTRACE(FPrint(_L("CAacDecoderConfigTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();
	iSupportedBitrates.Close();


    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iAacDecoder)
		{
		delete iAacDecoder;
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
// CAacDecoderConfigTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CAacDecoderConfigTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateAacDecoderConfig", CAacDecoderConfigTestClass::CreateAacDecoderConfig ),
  		ENTRY( "SetAudioConfig", CAacDecoderConfigTestClass::SetAudioConfig ),
		ENTRY( "CreateObject", CAacDecoderConfigTestClass::CreateObject ),
		ENTRY( "InitObject", CAacDecoderConfigTestClass::InitObject ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CAacDecoderConfigTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CAacDecoderConfigTestClass::EventName( TInt aKey )
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
// CAacDecoderConfigTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CAacDecoderConfigTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::AddExpectedEvent")));
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
// CAacDecoderConfigTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CAacDecoderConfigTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::RemoveExpectedEvent")));
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
// CAacDecoderConfigTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CAacDecoderConfigTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CAacDecoderConfigTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CAacDecoderConfigTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::ProcessExpectedEvent")));
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
// CAacDecoderConfigTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CAacDecoderConfigTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::HandleTimeout")));
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
// CAacDecoderConfigTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CAacDecoderConfigTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::SetTimeout")));
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
TInt CAacDecoderConfigTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::SetAllowedPanic")));
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
// CAacDecoderConfigTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CAacDecoderConfigTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CAacDecoderConfigTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAacDecoderConfigTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KAacDecoderConfigTestClass, "AacDecoderConfigTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KAacDecoderConfigTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KAacDecoderConfigTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CAacDecoderConfigTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::SetConfigureData")));
	iLog->Log(_L("CAacDecoderConfigTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetAudioConfig();

	return error;
}



TInt CAacDecoderConfigTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::GetConfigureData")));
	iLog->Log(_L("CAacDecoderConfigTestClass::GetConfigureData"));
	TInt error = KErrNone;
    return error;
}

TInt CAacDecoderConfigTestClass::SetAudioConfig( )
{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::SetAudioConfig")));
	iLog->Log(_L("CAacDecoderConfigTestClass::SetAudioConfig"));
	TInt error = KErrNone;

	iLog->Log(_L("CAacDecoderConfigTestClass::SetAudioConfig "));
	TAudioConfig config;
	config.iAudioObjectType = TAudioConfig::EAacLc;

	error = iAacDecoder->SetAudioConfig(config);

	return error;
}

// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CAacDecoderConfigTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::CreateObject")));
	iLog->Log(_L("CAacDecoderConfigTestClass::CreateObject"));

	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		if (error == KErrNone)
		    {
		    iLog->Log(_L("iDevSound = CMMFDevSound::NewL() OK"));
		    }
	    else
	        {
	        iLog->Log(_L("CMMFDevSound::NewL Error [%d]"),error);
	        }
		}
	else if (object == KTagOutputStream)
		{
		TRAPD(error, iAudioOutputStream = CMdaAudioOutputStream::NewL(*this));
		if(error == KErrNone)
			{
			iLog->Log(_L("CMdaAudioOutputStream::NewL OK!"));
			}
		else
			{
			iLog->Log(_L("CMdaAudioOutputStream::NewL Error [%d]"),error);
			}
		}
	return error;
	}

TInt CAacDecoderConfigTestClass::InitObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CAacDecoderConfigTestClass::InitObject")));
	iLog->Log(_L("CAacDecoderConfigTestClass::InitObject"));
	TInt error = KErrNone;
	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		TRAP(error,iDevSound->InitializeL(*this,KMMFFourCCCodeAAC ,EMMFStatePlaying));
		if(error == KErrNone)
			{
		    iLog->Log(_L("CAacDecoderConfigTestClass::InitializeL OK [%d]"),error);
		    AddExpectedEvent(EInitializeComplete, KMediumTimeout);
		    }
		else
			{
		    iLog->Log(_L("CAacDecoderConfigTestClass::InitializeL ERROR [%d]"),error);
		    AddExpectedEvent(EInitializeComplete, KShortTimeout);
		    ProcessEvent(EInitializeComplete, error);
		    }
		}
	else if (object == KTagOutputStream)
		{
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
		iStreamSettings.iVolume = 1;
		TRAPD(error, iAudioOutputStream->SetDataTypeL(KMMFFourCCCodeAAC));
		if(error == KErrNone)
			{
			iAudioOutputStream->Open(&iStreamSettings);
			AddExpectedEvent(EMaoscOpenComplete, KMediumTimeout);
			iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
			}
		else
			{
			iLog->Log(_L("iAudioOutputStream->SetDataTypeL(KMMFFourCCCodeAAC) Error [%d]"),error);
			AddExpectedEvent(EMaoscOpenComplete, KShortTimeout);
		    ProcessEvent(EMaoscOpenComplete, error);
			}
		}
	return error;
	}

// -----------------------------------------------------------------------------
// CAacDecoderConfigTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CAacDecoderConfigTestClass::CreateAacDecoderConfig( CStifItemParser& aItem )
{
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		FTRACE(FPrint(_L("CAacDecoderConfigTestClass::CreateAacDecoderConfig")));
		iLog->Log(_L("CAacDecoderConfigTestClass::CreateAacDecoderConfig"));

		TRAP(error, iAacDecoder = CAacDecoderConfig::NewL(*iDevSound));
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L("CAacDecoderConfig::CreateAacDecoderConfig-CMdaAudioOutputStream")));
		iLog->Log(_L("CAacDecoderConfig::CreateAacDecoderConfig-CMdaAudioOutputStream"));

		TRAP(error, iAacDecoder = CAacDecoderConfig::NewL(*iAudioOutputStream));
		}

	if(error == KErrNone)
        {
        iLog->Log(_L("CAacDecoderConfigTestClass::NewL OK"));
        }
    else
        {
        iLog->Log(_L("CAacDecoderConfigTestClass::FAILED on NewL [%d]"),error);
        }


	return error;
}

//  End of File
