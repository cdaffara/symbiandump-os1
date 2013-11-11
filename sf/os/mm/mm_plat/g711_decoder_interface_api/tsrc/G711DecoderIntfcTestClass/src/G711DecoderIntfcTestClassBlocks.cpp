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
* Description:  G711DecoderIntfcTestClass test module.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "G711DecoderIntfcTestClass.h"
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
// CG711DecoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CG711DecoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
    iLog->Log(_L(""));
    iLog->Log(_L(""));

    if(iG711Decoder)
        {
        delete iG711Decoder;
        }
    if (iAudioOutputStream)
        {
        delete iAudioOutputStream;
        }

    if (iDevSound)
        {
        delete iDevSound;
        }
REComSession::FinalClose();

}

// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CG711DecoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateG711DecoderIntfc", CG711DecoderIntfcTestClass::CreateG711DecoderIntfc ),
 		ENTRY( "SetDecoderMode", CG711DecoderIntfcTestClass::SetDecoderMode ),
		ENTRY( "SetCng", CG711DecoderIntfcTestClass::SetCng ),
  		ENTRY( "GetCng", CG711DecoderIntfcTestClass::GetCng ),
		ENTRY( "SetPlc", CG711DecoderIntfcTestClass::SetPlc ),
		ENTRY( "CreateObject", CG711DecoderIntfcTestClass::CreateObject )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CG711DecoderIntfcTestClass::EventName( TInt aKey )
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
// CG711DecoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CG711DecoderIntfcTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::AddExpectedEvent")));
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
// CG711DecoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CG711DecoderIntfcTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
    FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::RemoveExpectedEvent")));
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
// CG711DecoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CG711DecoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CG711DecoderIntfcTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::ProcessExpectedEvent")));
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
// CG711DecoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CG711DecoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::HandleTimeout")));
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
// CG711DecoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CG711DecoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetTimeout")));
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
TInt CG711DecoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetAllowedPanic")));
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
// CG711DecoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CG711DecoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CG711DecoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KG711DecoderIntfcTestClass, "G711DecoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KG711DecoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KG711DecoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CG711DecoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetDecoderMode( );
    SetCng( );
	SetPlc();

	return error;
}



TInt CG711DecoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetCng();
    return error;
}




TInt CG711DecoderIntfcTestClass::SetDecoderMode( )
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetDecoderMode")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::SetDecoderMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CG711DecoderIntfcTestClass::SetDecoderMode - G711"));
	error = iG711Decoder->SetDecoderMode(CG711DecoderIntfc::EDecULaw);

	return error;
}



TInt CG711DecoderIntfcTestClass::SetCng( )
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetCng")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::SetCng"));
	TInt error = KErrNone;

	iLog->Log(_L("CG711DecoderIntfcTestClass::SetCng - %d"), EFalse);
	error = iG711Decoder->SetCng(EFalse);

	return error;
}



TInt CG711DecoderIntfcTestClass::SetPlc()
{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::SetPlc")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::SetPlc"));
	TInt error = KErrNone;

	iLog->Log(_L("CG711DecoderIntfcTestClass::SetPlc - %d"), ETrue);
	error = iG711Decoder->SetPlc(ETrue);

	return error;
}


//////////////////////////
// GetData
//////////////////////////

TInt CG711DecoderIntfcTestClass::GetCng()
	{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::GetCng")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::GetCng"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iG711Decoder->GetCng(returnOutput);

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
// CG711DecoderIntfcTestClass::CreateObject
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CG711DecoderIntfcTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CG711DecoderIntfcTestClass::CreateObject")));
	iLog->Log(_L("CG711DecoderIntfcTestClass::CreateObject"));
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
				iLog->Log(_L("CG711DecoderIntfcTestClass::InitializeL Error [%d]"),error);
				}
			}
		else
			{
			iLog->Log(_L("CG711DecoderIntfcTestClass::CreateObject Error [%d]"),error);
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
	return error;
	}

// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CG711DecoderIntfcTestClass::CreateG711DecoderIntfc( CStifItemParser& aItem )
	{
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		FTRACE(FPrint(_L("CG711DecoderIntfc::NewL(*iDevSound)")));
		iLog->Log(_L("CG711DecoderIntfc::NewL(*iDevSound)"));

		TRAP(error, iG711Decoder = CG711DecoderIntfc::NewL(*iDevSound));
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L("CG711DecoderIntfc::NewL(*iAudioOutputStream)")));
		iLog->Log(_L("CG711DecoderIntfc::NewL(*iAudioOutputStream)"));
		TRAP(error, iG711Decoder = CG711DecoderIntfc::NewL(*iAudioOutputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateG711DecoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateG711DecoderIntfc FAILED %d"),error);
		}
	return error;
	}

//  End of File
