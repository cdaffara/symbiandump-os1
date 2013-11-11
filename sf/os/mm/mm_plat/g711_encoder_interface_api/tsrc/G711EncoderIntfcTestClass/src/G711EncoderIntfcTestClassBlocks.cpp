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
* Description: G711EncoderIntfc Test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "G711EncoderIntfcTestClass.h"
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
// CG711EncoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CG711EncoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iG711Encoder)
		{
    	delete iG711Encoder;
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
// CG711EncoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CG711EncoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateG711EncoderIntfc", CG711EncoderIntfcTestClass::CreateG711EncoderIntfc ),
		ENTRY( "CreateDevSoundInstance", CG711EncoderIntfcTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateInputStreamInstance", CG711EncoderIntfcTestClass::CreateInputStreamInstance),
  		ENTRY( "SetEncoderMode", CG711EncoderIntfcTestClass::SetEncoderMode ),
		ENTRY( "SetVadMode", CG711EncoderIntfcTestClass::SetVadMode ),
		ENTRY( "GetVadMode", CG711EncoderIntfcTestClass::GetVadMode ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CG711EncoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CG711EncoderIntfcTestClass::EventName( TInt aKey )
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
// CG711EncoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CG711EncoderIntfcTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::AddExpectedEvent")));
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
// CG711EncoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CG711EncoderIntfcTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::RemoveExpectedEvent")));
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
// CG711EncoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CG711EncoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CG711EncoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CG711EncoderIntfcTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::ProcessExpectedEvent")));
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
// CG711EncoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CG711EncoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::HandleTimeout")));
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
// CG711EncoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CG711EncoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetTimeout")));
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
TInt CG711EncoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetAllowedPanic")));
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
// CG711EncoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CG711EncoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CG711EncoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CG711EncoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KG711EncoderIntfcTestClass, "G711EncoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KG711EncoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KG711EncoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CG711EncoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetEncoderMode( );
    SetVadMode( );

	return error;
}



TInt CG711EncoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetVadMode();
    return error;
}




TInt CG711EncoderIntfcTestClass::SetEncoderMode( )
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetEncoderMode")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::SetEncoderMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CG711EncoderIntfcTestClass::SetEncoderMode - G711"));
	error = iG711Encoder->SetEncoderMode(CG711EncoderIntfc::EEncULaw);

	return error;
}



TInt CG711EncoderIntfcTestClass::SetVadMode( )
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::SetVadMode")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::SetVadMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CG711EncoderIntfcTestClass::SetVadMode - %d"), EFalse);
	error = iG711Encoder->SetVadMode(EFalse);

	return error;
}



//////////////////////////
// GetData
//////////////////////////

TInt CG711EncoderIntfcTestClass::GetVadMode()
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::GetVadMode")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::GetVadMode"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iG711Encoder->GetVadMode(returnOutput);

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
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//


TInt CG711EncoderIntfcTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::CreateDevSoundInstance"));
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
	        iLog->Log(_L("CG711EncoderIntfcTestClass::InitializeL Error [%d]"),error);
	        }
	    }
    else
        {
        iLog->Log(_L("CG711EncoderIntfcTestClass::CreateDevSoundInstance Error [%d]"),error);
        }
	return error;
	}

TInt CG711EncoderIntfcTestClass::CreateInputStreamInstance( CStifItemParser& /*aItem */)
	{

	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::CreateInputStreamInstance")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::CreateInputStreamInstance"));
	TInt error = KErrNone;
    TFourCC KG711FourCC('G','7','1','1');
    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this));
    if (error == KErrNone)
    	{
    	TRAP(error, iAudioInputStream->SetDataTypeL(KG711FourCC));
        iLog->Log(_L("iAudioInputStream->SetDataTypeL(KG711FourCC) [%d]"),error);
    	TMdaAudioDataSettings 	iStreamSettings;
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iVolume = 1;
		iAudioInputStream->Open(&iStreamSettings);
	    iLog->Log(_L("iAudioInputStream->Open(&iStreamSettings); [%d]"),error);

	    if (error == KErrNone)
		    {
	        AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout);
		    iLog->Log(_L(" AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout)"));
		    }
	    else
	        {
	        iLog->Log(_L("CG711EncoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
	        }
    	}
    else
    	{
        iLog->Log(_L("CG711EncoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
    	}
	return error;
	}
// -----------------------------------------------------------------------------
// CG711EncoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CG711EncoderIntfcTestClass::CreateG711EncoderIntfc( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CG711EncoderIntfcTestClass::CreateG711EncoderIntfc")));
	iLog->Log(_L("CG711EncoderIntfcTestClass::CreateG711EncoderIntfc"));
	TInt error = KErrNone;
	if (iDevSound)
		{
		TRAP(error, iG711Encoder = CG711EncoderIntfc::NewL(*iDevSound));
		}
	else if (iAudioInputStream)
		{
		FTRACE(FPrint(_L("CG711EncoderIntfc::NewL(*iAudioInputStream)")));
		iLog->Log(_L("CG711EncoderIntfc::NewL(*iAudioInputStream)"));

		TRAP(error, iG711Encoder = CG711EncoderIntfc::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CG711EncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CG711EncoderIntfc FAILED %d"),error);
		}

	return error;
}

//  End of File
