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
* Description: ilbc encoder test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "IlbcEncoderIntfcTestClass.h"
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
// CIlbcEncoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIlbcEncoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iIlbcEncoder)
		{
		delete iIlbcEncoder;
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
// CIlbcEncoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIlbcEncoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateIlbcEncoderIntfc", CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc ),
		ENTRY( "CreateIlbcEncoderIntfcNull", CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfcNull ),
		ENTRY( "CreateDevSoundInstance", CIlbcEncoderIntfcTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateInputStreamInstance", CIlbcEncoderIntfcTestClass::CreateInputStreamInstance),
  		ENTRY( "SetEncoderMode", CIlbcEncoderIntfcTestClass::SetEncoderMode ),
		ENTRY( "SetVadMode", CIlbcEncoderIntfcTestClass::SetVadMode ),
		ENTRY( "GetVadMode", CIlbcEncoderIntfcTestClass::GetVadMode )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CIlbcEncoderIntfcTestClass::EventName( TInt aKey )
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
// CIlbcEncoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CIlbcEncoderIntfcTestClass::AddExpectedEvent(TIlbcExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::AddExpectedEvent")));
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
// CIlbcEncoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CIlbcEncoderIntfcTestClass::RemoveExpectedEvent(TIlbcExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::RemoveExpectedEvent")));
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
// CIlbcEncoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CIlbcEncoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CIlbcEncoderIntfcTestClass::ProcessEvent(TIlbcExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::ProcessExpectedEvent")));
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
// CIlbcEncoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CIlbcEncoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::HandleTimeout")));
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
// CIlbcEncoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CIlbcEncoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetTimeout")));
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
TInt CIlbcEncoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetAllowedPanic")));
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
// CIlbcEncoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CIlbcEncoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TIlbcExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIlbcEncoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KIlbcEncoderIntfcTestClass, "IlbcEncoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KIlbcEncoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KIlbcEncoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


//////////////////////////
// SetData
//////////////////////////

TInt CIlbcEncoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetEncoderMode( );
    SetVadMode( );

	return error;
}



TInt CIlbcEncoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetVadMode();
    return error;
}




TInt CIlbcEncoderIntfcTestClass::SetEncoderMode( )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetEncoderMode")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::SetEncoderMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CIlbcEncoderIntfcTestClass::SetEncoderMode - Ilbc"));
	error = iIlbcEncoder->SetEncoderMode(CIlbcEncoderIntfc::E30msFrame);

	return error;
}



TInt CIlbcEncoderIntfcTestClass::SetVadMode( )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::SetVadMode")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::SetVadMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CIlbcEncoderIntfcTestClass::SetVadMode - %d"), EFalse);
	error = iIlbcEncoder->SetVadMode(EFalse);

	return error;
}



//////////////////////////
// GetData
//////////////////////////

TInt CIlbcEncoderIntfcTestClass::GetVadMode()
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::GetVadMode")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::GetVadMode"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iIlbcEncoder->GetVadMode(returnOutput);

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


TInt CIlbcEncoderIntfcTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateDevSoundInstance"));
	TInt error = KErrNone;

    TRAP(error, iDevSound = CMMFDevSound::NewL());

	if (error == KErrNone)
	    {
	    TFourCC KILBCFourCC('i','L','B','C');
	    TRAP(error,iDevSound->InitializeL(*this,KILBCFourCC,EMMFStateRecording));
	    if(error == KErrNone)
	        {
	        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
	        }
	    else
	        {
	        iLog->Log(_L("CIlbcEncoderIntfcTestClass::InitializeL Error [%d]"),error);
	        }
	    }
    else
        {
        iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateDevSoundInstance Error [%d]"),error);
        }
	return error;
	}

TInt CIlbcEncoderIntfcTestClass::CreateInputStreamInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::CreateInputStreamInstance")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateInputStreamInstance"));
	TInt error = KErrNone;
    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this););

    if (error == KErrNone)
    	{
	    TFourCC KILBCFourCC('i','L','B','C');
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iVolume = 1;
    	TRAP(error, iAudioInputStream->SetDataTypeL(KILBCFourCC));
        iLog->Log(_L("iAudioInputStream->SetDataTypeL(KILBCFourCC) [%d]"),error);

		iAudioInputStream->Open(&iStreamSettings);
	    iLog->Log(_L("iAudioInputStream->Open(&iStreamSettings)"));
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
// CIlbcEncoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc"));
	TInt error = KErrNone;

	if (iDevSound)
		{
		TRAP(error, iIlbcEncoder = CIlbcEncoderIntfc::NewL(*iDevSound));
		}
	else if (iAudioInputStream)
		{
		TRAP(error, iIlbcEncoder = CIlbcEncoderIntfc::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateIlbcEncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateIlbcEncoderIntfc FAILED %d"),error);
		}


	return error;
}

TInt CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfcNull( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateIlbcEncoderIntfc"));
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
	    iDevSound = CMMFDevSound::NewL();
		TRAP(error, iIlbcEncoder = CIlbcEncoderIntfc::NewL(*iDevSound));
		}
	else if (object == KTagInputStream)
		{
	    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this););
		TRAP(error, iIlbcEncoder = CIlbcEncoderIntfc::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateIlbcEncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateIlbcEncoderIntfc FAILED %d"),error);
		}


	return error;
}

//  End of File
