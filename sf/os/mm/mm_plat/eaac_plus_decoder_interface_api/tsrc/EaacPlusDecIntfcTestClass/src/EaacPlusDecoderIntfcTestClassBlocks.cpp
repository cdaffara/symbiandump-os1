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
* Description:  EAacPlusDecoderIntfc Test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "EaacPlusDecoderIntfcTestClass.h"
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
// CEaacPlusDecoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CEaacPlusDecoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iEaacPlusDecoder)
        {
        delete iEaacPlusDecoder;
        iEaacPlusDecoder = NULL;
        }

	if (iDevSound)
       	{
    	delete iDevSound;
    	iDevSound = NULL;
    	}

	if (iAudioOutputStream)
		{
		delete iAudioOutputStream;
		iAudioOutputStream = NULL;
		}

}

// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CEaacPlusDecoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateEaacPlusDecoderIntfc", CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc ),
  		ENTRY( "SetAudioObjectType", CEaacPlusDecoderIntfcTestClass::SetAudioObjectType ),
		ENTRY( "SetInputSamplingFrequency", CEaacPlusDecoderIntfcTestClass::SetInputSamplingFrequency ),
  		ENTRY( "SetNumOfChannels", CEaacPlusDecoderIntfcTestClass::SetNumOfChannels ),
  		ENTRY( "SetSbr", CEaacPlusDecoderIntfcTestClass::SetSbr ),
  		ENTRY( "SetDownSampledMode", CEaacPlusDecoderIntfcTestClass::SetDownSampledMode ),
  		ENTRY( "ApplyConfig", CEaacPlusDecoderIntfcTestClass::ApplyConfig ),
  		ENTRY( "GetAudioObjectType", CEaacPlusDecoderIntfcTestClass::GetAudioObjectType ),
  		ENTRY( "GetInputSamplingFrequency", CEaacPlusDecoderIntfcTestClass::GetInputSamplingFrequency ),
  		ENTRY( "GetNumOfChannels", CEaacPlusDecoderIntfcTestClass::GetNumOfChannels ),
  		ENTRY( "GetSbr", CEaacPlusDecoderIntfcTestClass::GetSbr ),
  		ENTRY( "GetDownSampledMode", CEaacPlusDecoderIntfcTestClass::GetDownSampledMode ),
  		ENTRY( "CreateObject", CEaacPlusDecoderIntfcTestClass::CreateObject )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CEaacPlusDecoderIntfcTestClass::EventName( TInt aKey )
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
// CEaacPlusDecoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CEaacPlusDecoderIntfcTestClass::AddExpectedEvent(TEaacPlusExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::AddExpectedEvent")));
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
// CEaacPlusDecoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CEaacPlusDecoderIntfcTestClass::RemoveExpectedEvent(TEaacPlusExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::RemoveExpectedEvent")));
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
// CEaacPlusDecoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CEaacPlusDecoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CEaacPlusDecoderIntfcTestClass::ProcessEvent(TEaacPlusExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::ProcessExpectedEvent")));
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
// CEaacPlusDecoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CEaacPlusDecoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::HandleTimeout")));
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
// CEaacPlusDecoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CEaacPlusDecoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetTimeout")));
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
TInt CEaacPlusDecoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetAllowedPanic")));
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
// CEaacPlusDecoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CEaacPlusDecoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TEaacPlusExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEaacPlusDecoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KEaacPlusDecoderIntfcTestClass, "EaacPlusDecoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KEaacPlusDecoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KEaacPlusDecoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CEaacPlusDecoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetAudioObjectType( );
    SetInputSamplingFrequency( );
	SetNumOfChannels();
	SetSbr();
	SetDownSampledMode();
	ApplyConfig();

	return error;
}



TInt CEaacPlusDecoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetAudioObjectType();
    GetInputSamplingFrequency();
    GetNumOfChannels();
    GetSbr();
    GetDownSampledMode();

    return error;
}




TInt CEaacPlusDecoderIntfcTestClass::SetAudioObjectType( )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetAudioObjectType")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetAudioObjectType"));
	TInt error = KErrNone;

	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetAudioObjectType - EEaacPlus"));
	iEaacPlusDecoder->SetAudioObjectType(CEAacPlusDecoderIntfc::EAacLc);

	return error;
}



TInt CEaacPlusDecoderIntfcTestClass::SetInputSamplingFrequency( )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetInputSamplingFrequency")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetInputSamplingFrequency"));
	TInt error = KErrNone;

	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetInputSamplingFrequency - %d"), 4000);
	iEaacPlusDecoder->SetInputSamplingFrequency(4000);

	return error;
}



TInt CEaacPlusDecoderIntfcTestClass::SetNumOfChannels()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetNumOfChannels")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetNumOfChannels"));
	TInt error = KErrNone;

	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetNumOfChannels - %d"), 2);
	iEaacPlusDecoder->SetNumOfChannels(2);

	return error;
}



TInt CEaacPlusDecoderIntfcTestClass::SetSbr()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetSbr")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetSbr"));
	TInt error = KErrNone;

	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetSbr - %d") , 0);
	iEaacPlusDecoder->SetSbr(ETrue);

	return error;
}



TInt CEaacPlusDecoderIntfcTestClass::SetDownSampledMode()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::SetDownSampledMode")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetDownSampledMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::SetDownSampledMode - %d"), 0);
    iEaacPlusDecoder->SetDownSampledMode(ETrue);

	return error;
}


TInt CEaacPlusDecoderIntfcTestClass::ApplyConfig( )
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::ApplyConfig")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::ApplyConfig"));
	TInt error = KErrNone;

	error = iEaacPlusDecoder->ApplyConfig();

	return error;
}




//////////////////////////
// GetData
//////////////////////////

TInt CEaacPlusDecoderIntfcTestClass::GetAudioObjectType()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetAudioObjectType")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetAudioObjectType"));
	TInt error = KErrNone;

	CEAacPlusDecoderIntfc::TAudioObjectType returnOutput;
	error = iEaacPlusDecoder->GetAudioObjectType(returnOutput);

	if (returnOutput == CEAacPlusDecoderIntfc::EAacLc)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CEaacPlusDecoderIntfcTestClass::GetInputSamplingFrequency()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetInputSamplingFrequency")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetInputSamplingFrequency"));
	TInt error = KErrNone;


	TUint returnOutput;
	error = iEaacPlusDecoder->GetInputSamplingFrequency(returnOutput);

	if (returnOutput == 4000)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CEaacPlusDecoderIntfcTestClass::GetNumOfChannels()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetNumOfChannels")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetNumOfChannels"));
	TInt error = KErrNone;

	TUint returnOutput;
	error = iEaacPlusDecoder->GetNumOfChannels(returnOutput);

	if (returnOutput == 2)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CEaacPlusDecoderIntfcTestClass::GetSbr()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetSbr")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetSbr"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iEaacPlusDecoder->GetSbr(returnOutput);

	if (returnOutput)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CEaacPlusDecoderIntfcTestClass::GetDownSampledMode()
{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::GetDownSampledMode")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::GetDownSampledMode"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iEaacPlusDecoder->GetDownSampledMode(returnOutput);

	if (returnOutput)
		{
		return error;
		}
	else
		{
		return KErrUnexpectedValue;
		}
}


// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::CreateFactory
// Create Factory
// -----------------------------------------------------------------------------
//
TInt CEaacPlusDecoderIntfcTestClass::InitObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::InitObject")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitObject"));
	TInt error = KErrNone;
    TFourCC KEAACFourCC(' ','E','A','C');
	TPtrC object;
	aItem.GetNextString(object);
	if (object == KTagDevSound)
		{
		TRAP(error,iDevSound->InitializeL(*this,KEAACFourCC,EMMFStatePlaying));
		if(error == KErrNone)
			{
			iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitializeL OK [%d]"),error);
			AddExpectedEvent(EInitializeComplete, KMediumTimeout);
			}
		else
			{
		    iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitializeL Error [%d]"),error);
		    AddExpectedEvent(EInitializeComplete, KShortTimeout);
		    ProcessEvent(EInitializeComplete, error);
			}
		}
	else if (object == KTagOutputStream)
		{
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
		iStreamSettings.iVolume = 1;
		TRAPD(error, iAudioOutputStream->SetDataTypeL(KEAACFourCC));
		if(error == KErrNone)
			{
			iAudioOutputStream->Open(&iStreamSettings);
			AddExpectedEvent(EMaoscOpenComplete, KMediumTimeout);
			iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
			}
		else
			{
			iLog->Log(_L("iAudioOutputStream->SetDataTypeL(KEAACFourCC) Error [%d]"),error);
			AddExpectedEvent(EMaoscOpenComplete, KShortTimeout);
		       ProcessEvent(EMaoscOpenComplete, error);
			}
		}
	return error;
	}


TInt CEaacPlusDecoderIntfcTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::CreateObject")));
	iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::CreateObject"));

	TInt error = KErrNone;
	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		if (error == KErrNone)
		    {
		    iLog->Log(_L("CMMFDevSound::NewL OK "));
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
			iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
			}
		else
			{
			iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL Error [%d]"),error);
			}
		}
	return error;
	}

// -----------------------------------------------------------------------------
// CEaacPlusDecoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc( CStifItemParser& aItem )
{
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc")));
		iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc"));

		TRAP(error,iEaacPlusDecoder = CEAacPlusDecoderIntfc::NewL(*iDevSound));
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L("CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc-CMdaAudioOutputStream")));
		iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::CreateEaacPlusDecoderIntfc-CMdaAudioOutputStream"));

		TRAP(error, iEaacPlusDecoder = CEAacPlusDecoderIntfc::NewL(*iAudioOutputStream));
		}
	if(error == KErrNone)
        {
        iLog->Log(_L("CreateEaacPlusDecoderIntfc::NewL OK"));
        }
    else
        {
        iLog->Log(_L("CAacDecoderConfigTestClass::FAILED on NewL [%d]"),error);
        }
	return error;
}

//  End of File
