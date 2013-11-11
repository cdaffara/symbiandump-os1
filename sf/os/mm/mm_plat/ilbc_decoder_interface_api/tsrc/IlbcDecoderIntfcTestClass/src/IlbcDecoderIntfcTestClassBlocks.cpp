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
* Description:  IlbcDecoderIntfcTest
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "IlbcDecoderIntfcTestClass.h"
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
// CIlbcDecoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIlbcDecoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iIlbcDecoder)
		{
		delete iIlbcDecoder;
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
// CIlbcDecoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIlbcDecoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateIlbcDecoderIntfc", CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc ),
  		ENTRY( "SetDecoderMode", CIlbcDecoderIntfcTestClass::SetDecoderMode ),
		ENTRY( "SetCng", CIlbcDecoderIntfcTestClass::SetCng ),
  		ENTRY( "GetCng", CIlbcDecoderIntfcTestClass::GetCng ),
		ENTRY( "CreateObject", CIlbcDecoderIntfcTestClass::CreateObject )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CIlbcDecoderIntfcTestClass::EventName( TInt aKey )
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
// CIlbcDecoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CIlbcDecoderIntfcTestClass::AddExpectedEvent(TIlbcExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::AddExpectedEvent")));
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
// CIlbcDecoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CIlbcDecoderIntfcTestClass::RemoveExpectedEvent(TIlbcExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::RemoveExpectedEvent")));
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
// CIlbcDecoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CIlbcDecoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CIlbcDecoderIntfcTestClass::ProcessEvent(TIlbcExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::ProcessExpectedEvent")));
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
// CIlbcDecoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CIlbcDecoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::HandleTimeout")));
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
// CIlbcDecoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CIlbcDecoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetTimeout")));
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
TInt CIlbcDecoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetAllowedPanic")));
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
// CIlbcDecoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CIlbcDecoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TIlbcExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIlbcDecoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
{

    // Print to UI
    _LIT( KIlbcDecoderIntfcTestClass, "IlbcDecoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KIlbcDecoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KIlbcDecoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

}


//////////////////////////
// SetData
//////////////////////////

TInt CIlbcDecoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetDecoderMode( );
    SetCng( );

	return error;
}



TInt CIlbcDecoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetCng();
    return error;
}




TInt CIlbcDecoderIntfcTestClass::SetDecoderMode( )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetDecoderMode")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::SetDecoderMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CIlbcDecoderIntfcTestClass::SetDecoderMode - Ilbc"));
	error = iIlbcDecoder->SetDecoderMode(CIlbcDecoderIntfc::E30msFrame);

	return error;
}



TInt CIlbcDecoderIntfcTestClass::SetCng( )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::SetCng")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::SetCng"));
	TInt error = KErrNone;

	iLog->Log(_L("CIlbcDecoderIntfcTestClass::SetCng - %d"), EFalse);
	error = iIlbcDecoder->SetCng(EFalse);

	return error;
}


//////////////////////////
// GetData
//////////////////////////

TInt CIlbcDecoderIntfcTestClass::GetCng()
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::GetCng")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::GetCng"));
	TInt error = KErrNone;

	TBool returnOutput;
	error = iIlbcDecoder->GetCng(returnOutput);

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
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CIlbcDecoderIntfcTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::CreateObject")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::CreateObject"));
	TInt error = KErrNone;
    TFourCC KILBCFourCC('i','L','B','C');
	TPtrC object;
	TInt objectN;
	aItem.GetNextString(object);
	aItem.GetNextInt(objectN);

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		if ((error == KErrNone) && !(objectN))
		    {
		    TRAP(error,iDevSound->InitializeL(*this,KILBCFourCC,EMMFStatePlaying));
		    if(error == KErrNone)
		        {
		        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
		        }
		    else
		        {
		        	iLog->Log(_L("CIlbcDecoderIntfcTestClass::InitializeL Error [%d]"),error);
		        	AddExpectedEvent(EInitializeComplete, KShortTimeout);
		        	ProcessEvent(EInitializeComplete, error);
		        }
		    }
	    else
	        {
	        if (error == KErrNone)
	        	{
	        	iLog->Log(_L("CIlbcDecoderIntfcTestClass::CreateDevSoundInstance Ok"));
	        	AddExpectedEvent(EInitializeComplete, KShortTimeout);
	        	}
	        else
	        	{
	        	iLog->Log(_L("CIlbcDecoderIntfcTestClass::CreateDevSoundInstance Error [%d]"),error);
	        	AddExpectedEvent(EInitializeComplete, KShortTimeout);
	        	ProcessEvent(EInitializeComplete, error);
	        	}
	        }
		}
	else if (object == KTagOutputStream)
		{
		TRAPD(error, iAudioOutputStream = CMdaAudioOutputStream::NewL(*this));
		if((error == KErrNone) && !(objectN))
			{
			iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
			iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
			iStreamSettings.iVolume = 1;
			TRAPD(error, iAudioOutputStream->SetDataTypeL(KILBCFourCC));
			if(error == KErrNone)
				{
				iAudioOutputStream->Open(&iStreamSettings);
				AddExpectedEvent(EMaoscOpenComplete, KMediumTimeout);
				iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
				}
			else
				{
				iLog->Log(_L("iAudioOutputStream->SetDataTypeL(KILBCFourCC) Error [%d]"),error);
				AddExpectedEvent(EMaoscOpenComplete, KShortTimeout);
		        ProcessEvent(EMaoscOpenComplete, error);
				}
			}
		else
			{
			if (error == KErrNone)
				{
				AddExpectedEvent(EMaoscOpenComplete, KMediumTimeout);
				iLog->Log(_L("iAudioOutputStream = CMdaAudioOutputStream::NewL OK!"));
				}
			else
	        	{
	        	iLog->Log(_L("CMdaAudioOutputStream::NewL Error [%d]"),error);
				AddExpectedEvent(EMaoscOpenComplete, KShortTimeout);
		        ProcessEvent(EMaoscOpenComplete, error);
	        	}
			}
		}

	return error;
	}
// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc")));
	iLog->Log(_L("CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc"));
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		FTRACE(FPrint(_L("CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc")));
		iLog->Log(_L("CIlbcDecoderIntfcTestClass::CreateIlbcDecoderIntfc"));

		TRAP(error, iIlbcDecoder = CIlbcDecoderIntfc::NewL(*iDevSound));
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L(" CIlbcDecoderIntfc::NewL(*iAudioOutputStream)")));
		iLog->Log(_L(" CIlbcDecoderIntfc::NewL(*iAudioOutputStream)"));

		TRAP(error, iIlbcDecoder = CIlbcDecoderIntfc::NewL(*iAudioOutputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CG729DecoderIntfc::CreateIlbcDecoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CG729DecoderIntfc::CreateIlbcDecoderIntfc FAILED %d"),error);
		}

	return error;
}

//  End of File
