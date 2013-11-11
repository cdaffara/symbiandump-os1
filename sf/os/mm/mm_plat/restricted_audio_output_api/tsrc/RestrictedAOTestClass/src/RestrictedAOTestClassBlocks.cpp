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
* Description: Restricted Audio Output Test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <RestrictedAudioOutputProxy.h>
#include "RestrictedAOTestClass.h"
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
// CRestrictedAOTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CRestrictedAOTestClass::Delete()
{

    FTRACE(FPrint(_L("CRestrictedAOTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	  iLog->Log(_L(""));
	  iLog->Log(_L(""));

    if(iRestrictedAudioOutput)
    {
    	delete iRestrictedAudioOutput;
    }

    if (iDevSound)
        {
        delete iDevSound;
        iDevSound = NULL;
        }
REComSession::FinalClose();

}

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateFactory", CRestrictedAOTestClass::CreateFactory ),
		ENTRY( "CreateRestrictedAO", CRestrictedAOTestClass::CreateRestrictedAO ),
        ENTRY( "AppendAllowedOutput", CRestrictedAOTestClass::AppendAllowedOutput ),
        ENTRY( "RemoveAllowedOutput", CRestrictedAOTestClass::RemoveAllowedOutput ),
        ENTRY( "GetAllowedOutputCount", CRestrictedAOTestClass::GetAllowedOutputCount ),
        ENTRY( "GetAllowedOutput", CRestrictedAOTestClass::GetAllowedOutput ),
        ENTRY( "Reset", CRestrictedAOTestClass::Reset ),
        ENTRY( "Commit", CRestrictedAOTestClass::Commit ),

        ENTRY( "Example", CRestrictedAOTestClass::ExampleL ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CRestrictedAOTestClass::EventName( TInt aKey )
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
// CRestrictedAOTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CRestrictedAOTestClass::AddExpectedEvent(TROExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::AddExpectedEvent")));
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
// CRestrictedAOTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CRestrictedAOTestClass::RemoveExpectedEvent(TROExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::RemoveExpectedEvent")));
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
// CRestrictedAOTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CRestrictedAOTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CRestrictedAOTestClass::ProcessEvent(TROExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::ProcessExpectedEvent")));
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
// CRestrictedAOTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CRestrictedAOTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::HandleTimeout")));
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
// CRestrictedAOTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CRestrictedAOTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::SetTimeout")));
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
TInt CRestrictedAOTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::SetAllowedPanic")));
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
// CRestrictedAOTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CRestrictedAOTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
	{
		AddExpectedEvent(static_cast<TROExpectedEvent>(event), 0); // Default timeout value
	}
	return error;
}


// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KRestrictedAOTestClass, "RestrictedAOTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KRestrictedAOTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KRestrictedAOTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::AppendAllowedOutput
// Append AllowedOutput
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::AppendAllowedOutput( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::AppendAllowedOutput")));
	iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput"));
	TInt error = KErrNone;

	TPtrC AppendType;
	error = aItem.GetNextString(AppendType);
	if (error == KErrNone)
	{
		if (AppendType == KTagAll)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAllAudio"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAllAudio);
		}
		else if (AppendType == KTagAudioAnalog)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioAnalog"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioAnalog);
		}
		else if (AppendType == KTagFM)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioFMTransmitter"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioFMTransmitter);
		}
		else if (AppendType == KTagStereoBT)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioBtHFPHSP"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioBtHFPHSP);
		}
		else if (AppendType == KTagBTA2DP)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioBtA2DP"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioBtA2DP);
		}
		else if (AppendType == KTagAudioUplink)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioUplink"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioUplink);
		}
		else if (AppendType == KTagUSB)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioUsb"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioUsb);
		}
		else if (AppendType == KTagRecord)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioRecording"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioRecording);
		}
		else if (AppendType == KTagVisualization)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowVisualization"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowVisualization);
		}

		/**
		* RIM CR 417-7642: HDMI with HDCP to Resctricted Audio Output API
		* Adding new ENUMs to CRestrictedAudioOutput::TAllowedOutputPreference for HDMI and HDCP
		* EAllowAudioHDMI and EAllowAudioHdmiHdcpRequired
		*/
		else if (AppendType == KTagHDMI)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioHDMI"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioHDMI);
		}
		else if (AppendType == KTagHdmiHdcpRequired)
		{
			iLog->Log(_L("CRestrictedAOTestClass::AppendAllowedOutput - EAllowAudioHdmiHdcpRequired"));
			error = iRestrictedAudioOutput->AppendAllowedOutput(CRestrictedAudioOutput::EAllowAudioHdmiHdcpRequired);
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
		}

	}

	return error;
}


// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::RemoveAllowedOutput
// Remove AllowedOutput
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::RemoveAllowedOutput( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::RemoveAllowedOutput")));
	iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput"));
	TInt error = KErrNone;

	TPtrC RemoveType;
	error = aItem.GetNextString(RemoveType);
	if (error == KErrNone)
	{
		if (RemoveType == KTagAll)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAllAudio"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAllAudio);
		}
		else if (RemoveType == KTagAudioAnalog)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioAnalog"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioAnalog);
		}
		else if (RemoveType == KTagFM)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioFMTransmitter"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioFMTransmitter);
		}
		else if (RemoveType == KTagStereoBT)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioBtHFPHSP"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioBtHFPHSP);
		}
		else if (RemoveType == KTagBTA2DP)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioBtA2DP"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioBtA2DP);
		}
		else if (RemoveType == KTagAudioUplink)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioUplink"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioUplink);
		}
		else if (RemoveType == KTagUSB)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioUsb"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioUsb);
		}
		else if (RemoveType == KTagRecord)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowAudioRecording"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowAudioRecording);
		}
		else if (RemoveType == KTagVisualization)
		{
			iLog->Log(_L("CRestrictedAOTestClass::RemoveAllowedOutput - EAllowVisualization"));
			error = iRestrictedAudioOutput->RemoveAllowedOutput(CRestrictedAudioOutput::EAllowVisualization);
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
		}

	}

	return error;
}



// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::GetAllowedOutputCount
// Get AllowedOutput Count
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::GetAllowedOutputCount( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::GetAllowedOutputCount")));
	iLog->Log(_L("CRestrictedAOTestClass::GetAllowedOutputCount"));
	TInt error = KErrNone;

	TInt aSize;
	error = iRestrictedAudioOutput->GetAllowedOutputCount(aSize);
	if (error == KErrNone)
		error = aSize;

	return error;
}

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::GetAllowedOutput
// Get Allowed Output
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::GetAllowedOutput( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::GetAllowedOutput")));
	iLog->Log(_L("CRestrictedAOTestClass::GetAllowedOutput"));
	TInt error = KErrNone;
	TInt aCount;
	TInt index;
	TInt Found = 0;
	CRestrictedAudioOutput::TAllowedOutputPreference aOutput;

	error = iRestrictedAudioOutput->GetAllowedOutputCount(aCount);
	if (error != KErrNone)
	{
		iLog->Log(_L("GetAllowedOutputCount return error %d"), error);
		return error;
	}

	TPtrC ExpectedOutput;
	error = aItem.GetNextString(ExpectedOutput);
	if (error != KErrNone)
	{
		iLog->Log(_L("Missing parameter in config file"));
		return error;
	}

	for( index = 0; index < aCount; index++)
	{
		error = iRestrictedAudioOutput->GetAllowedOutput(index, aOutput);
		if (error == KErrNone)
		{
			switch (aOutput)
			{
				case 0:	//  ALL
					if (ExpectedOutput == KTagAll)
					{
						iLog->Log(_L("index = %d, Allow AllAudio"), index);
						Found = 1;
					}
					break;

				case 1:	// Audio Analog
					if (ExpectedOutput == KTagAudioAnalog)
					{
						iLog->Log(_L("index = %d, Allow AudioAnalog"), index);
						Found = 1;
					}
					break;

				case 2:	//  AudioFMTransmitter
					if (ExpectedOutput == KTagFM)
					{
						iLog->Log(_L("index = %d, Allow AudioFMTransmitter"), index);
						Found = 1;
					}
					break;

				case 3:	// AudioStereoBtHFP
					if (ExpectedOutput == KTagStereoBT)
					{
						iLog->Log(_L("index = %d, Allow AudioStereoBtHFP"), index);
						Found = 1;
					}
					break;

				case 4:	// AudioBtA2DP
					if (ExpectedOutput == KTagBTA2DP)
					{
						iLog->Log(_L("index = %d, Allow AudioBtA2DP"), index);
						Found = 1;
					}
					break;

				case 5:	// AudioUplink
					if (ExpectedOutput == KTagAudioUplink)
					{
						iLog->Log(_L("index = %d, Allow AudioUplink"), index);
						Found = 1;
					}
					break;

				case 6:	//  AudioUsb
					if (ExpectedOutput == KTagUSB)
					{
						iLog->Log(_L("index = %d, Allow AudioUsb"), index);
						Found = 1;
					}
					break;

				case 7:	// Audio Recording
					if (ExpectedOutput == KTagRecord)
					{
						iLog->Log(_L("index = %d, Allow AudioRecording"), index);
						Found = 1;
					}
					break;

				case 8:	// Visualization
					if (ExpectedOutput == KTagVisualization)
					{
						iLog->Log(_L("index = %d, Allow Visualization"), index);
						Found = 1;
					}
					break;

				default:
					iLog->Log(_L("unknown audio output"));
					break;
				} // switch
				if (Found == 1)
				{
					return error;
				}

		} // if
	} //for

	if (Found == 0)
	{
		error = KErrNotFound;
	}

	return error;
}

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::Reset
// Reset AllowedOutput
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::Reset( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::Reset")));
	iLog->Log(_L("CRestrictedAOTestClass::Reset"));
	TInt error = KErrNone;

	error = iRestrictedAudioOutput->Reset();

	return error;
}

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::Commit
// Commit AllowedOutput
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::Commit( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::Commit")));
	iLog->Log(_L("CRestrictedAOTestClass::Commit"));
	TInt error = KErrNone;

	error = iRestrictedAudioOutput->Commit();

	return error;
}


// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::CreateFactory
// Create Factory
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::CreateFactory( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::CreateFactory")));
	iLog->Log(_L("CRestrictedAOTestClass::CreateFactory"));
	TInt error = KErrNone;

	TRAP(error, iDevSound = CMMFDevSound::NewL()) ;
//	AddExpectedEvent(EInitializeComplete, KMediumTimeout);

	return error;
}

// -----------------------------------------------------------------------------
// CRestrictedAOTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CRestrictedAOTestClass::CreateRestrictedAO( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CRestrictedAOTestClass::CreateRestrictedAO")));
	iLog->Log(_L("CRestrictedAOTestClass::CreateRestrictedAO"));
	TInt error = KErrNone;

	if (iDevSound != NULL)
	{
            TRAP(error, iRestrictedAudioOutput = CRestrictedAudioOutputProxy::NewL(*iDevSound));
	}
	else
	{
			error = KErrNotSupported;
	}

	return error;
}

//  End of File
