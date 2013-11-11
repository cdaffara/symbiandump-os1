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
* Description:  AudioVisualization test module
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "AudioVisualizationTestClass.h"
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
// CAudioVisualizationTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CAudioVisualizationTestClass::Delete()
{

    FTRACE(FPrint(_L("CAudioVisualizationTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	if(iAudioVisualization)
		{
    	delete iAudioVisualization;
		}
}

// -----------------------------------------------------------------------------
// CAudioVisualizationTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CAudioVisualizationTestClass::RunMethodL( CStifItemParser& aItem )
    {
    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		ENTRY( "CreateAudioVisualization", CAudioVisualizationTestClass::CreateAudioVisualization ),
		ENTRY( "DataRate", CAudioVisualizationTestClass::DataRate ),
		ENTRY( "SetDataRate", CAudioVisualizationTestClass::SetDataRate ),
		ENTRY( "DataRateRange", CAudioVisualizationTestClass::SetDataRateRange ),
		ENTRY( "EnableWaveFormat", CAudioVisualizationTestClass::EnableWaveformData ),
		ENTRY( "StartVizualization", CAudioVisualizationTestClass::StartVizualization ),
		ENTRY( "StopVizualization", CAudioVisualizationTestClass::StopVizualization ),
		ENTRY( "EnableFrequencyData", CAudioVisualizationTestClass::EnableFrequencyData ),
		ENTRY( "FreqencyBandCount", CAudioVisualizationTestClass::FreqencyBandCount ),
		ENTRY( "FrequencyBandCountRange", CAudioVisualizationTestClass::FrequencyBandCountRange ),
		ENTRY( "IsFrequencyDataEnabled", CAudioVisualizationTestClass::IsFrequencyDataEnabled ),
		ENTRY( "IsWaveformDataEnabled", CAudioVisualizationTestClass::IsWaveformDataEnabled ),
		ENTRY( "SamplingRate", CAudioVisualizationTestClass::SamplingRate ),
		ENTRY( "SetFrequencyBandCount", CAudioVisualizationTestClass::SetFrequencyBandCount ),
		ENTRY( "SetWaveformDataLength", CAudioVisualizationTestClass::SetWaveformDataLength ),
		ENTRY( "WaveformDataLength", CAudioVisualizationTestClass::WaveformDataLength ),
		ENTRY( "WaveformDataLengthRange", CAudioVisualizationTestClass::WaveformDataLengthRange ),


        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CAudioVisualizationTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CAudioVisualizationTestClass::EventName( TInt aKey )
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
// CAudioVisualizationTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CAudioVisualizationTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::AddExpectedEvent")));
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
// CAudioVisualizationTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CAudioVisualizationTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::RemoveExpectedEvent")));
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
// CAudioVisualizationTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CAudioVisualizationTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CAudioVisualizationTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CAudioVisualizationTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::ProcessExpectedEvent")));
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
// CAudioVisualizationTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CAudioVisualizationTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::HandleTimeout")));
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
// CAudioVisualizationTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CAudioVisualizationTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::SetTimeout")));
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
TInt CAudioVisualizationTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::SetAllowedPanic")));
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
// CAudioVisualizationTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CAudioVisualizationTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioVisualizationTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
	{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
	}
	return error;
}


// -----------------------------------------------------------------------------
// CAudioVisualizationTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAudioVisualizationTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KAudioVisualizationTestClass, "AudioVisualizationTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KAudioVisualizationTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KAudioVisualizationTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//


// -----------------------------------------------------------------------------
// CAudioVisualizationTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CAudioVisualizationTestClass::CreateAudioVisualization( CStifItemParser& /*aItem*/ )
	{
	TInt error = KErrNone;
	FTRACE(FPrint(_L("CreateAudioVisualization::CreateAudioVisualization")));
	iLog->Log(_L("CreateAudioVisualization::CreateAudioVisualization"));
	TRAP(error, iAudioVisualization = CVisualization::NewL(*this));
    if(error == KErrNone)
        {
        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
        iLog->Log(_L("CreateAudioVisualization::InitializeL OK"));
        }
    else
        {
        iLog->Log(_L("CreateAudioVisualization::InitializeL Error [%d]"),error);
        }
	return error;
	}

TInt CAudioVisualizationTestClass::DataRate()
	{
	TInt error = KErrNone;
	TUint32 theDataRate = 0;
	//get data rate
	TRAP(error, theDataRate = iAudioVisualization->DataRate()); //in Hz
    if(error == KErrNone)
    	{

    	FTRACE(FPrint(_L("CVisualization::DataRate = %d"),theDataRate));
    	iLog->Log(_L(" CVisualization::DataRate = %d"), theDataRate);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::DataRate FAILED = %d"),error));
    	iLog->Log(_L("CVisualization::DataRate FAILED = %d"),error );
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::SetDataRate(CStifItemParser& aItem )
	{
	TInt error = KErrNone;
	TUint32 theDataRate;
	TInt dataRate;
	error = aItem.GetNextInt(dataRate);
	theDataRate = (TUint32)dataRate;
	//get data rate
	TRAP(error, iAudioVisualization->SetDataRateL(theDataRate)); //in Hz
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::SetDataRateL %d"),theDataRate));
    	iLog->Log(_L("CVisualization::SetDataRateL %d"), theDataRate);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::SetDataRateL FAILED %d"),error));
    	iLog->Log(_L("CVisualization::SetDataRateL FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::SetDataRateRange()
	{
	TInt error = KErrNone;
	TUint32 theDataRateMin;
	TUint32 theDataRateMax;
	//set data rate range
	TRAP(error,iAudioVisualization->DataRateRange(theDataRateMin, theDataRateMax));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::DataRateRange ----> %x - %x"),theDataRateMin, theDataRateMax));
    	iLog->Log(_L("CVisualization::DataRateRange  ----> %x - %x"), (TUint32)theDataRateMin, (TUint32)theDataRateMax);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::DataRateRange FAILED %d"),error));
    	iLog->Log(_L("CVisualization::DataRateRange FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::EnableWaveformData()
	{
	TInt error = KErrNone;
	//set data rate range
	TRAP(error,iAudioVisualization->EnableWaveformData(ETrue));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::EnableWaveformData")));
    	iLog->Log(_L("CVisualization::EnableWaveformData"));
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::EnableWaveformData FAILED %d"),error));
    	iLog->Log(_L("CVisualization::EnableWaveformData FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::StartVizualization()
	{
	TInt error = KErrNone;
	//set data rate range
	TRAP(error,iAudioVisualization->StartL());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::StartVizualization")));
    	iLog->Log(_L("CVisualization::StartVizualization"));
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::StartVizualization FAILED %d"),error));
    	iLog->Log(_L("CVisualization::StartVizualization FAILED %d"),error);
    	}
	return error;
	}


TInt CAudioVisualizationTestClass::StopVizualization()
	{
	TInt error = KErrNone;
	//set data rate range
	TRAP(error,iAudioVisualization->StopL());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::StopVizualization")));
    	iLog->Log(_L("CVisualization::StopVizualization"));
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::StopVizualization FAILED %d"),error));
    	iLog->Log(_L("CVisualization::StopVizualization FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::EnableFrequencyData()
	{
	TInt error = KErrNone;
	//set data rate range
	TRAP(error,iAudioVisualization->EnableFrequencyData(ETrue));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::EnableFrequencyData")));
    	iLog->Log(_L("CVisualization::EnableFrequencyData"));
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::EnableFrequencyData FAILED %d"),error));
    	iLog->Log(_L("CVisualization::EnableFrequencyData FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::FreqencyBandCount()
	{
	TInt error = KErrNone;
	//This function is not available with the hardware
	//TRAP(error,iAudioVisualization->FreqencyBandCount());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::FreqencyBandCount")));
    	iLog->Log(_L("CVisualization::FreqencyBandCount"));
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::FreqencyBandCount FAILED %d"),error));
    	iLog->Log(_L("CVisualization::FreqencyBandCount FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::FrequencyBandCountRange()
	{
	TInt error = KErrNone;
	TUint32 theDataRateMin;
	TUint32 theDataRateMax;
	//set data rate range
	TRAP(error,iAudioVisualization->FrequencyBandCountRange(theDataRateMin, theDataRateMax));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::FrequencyBandCountRange ----> %x - %x"),theDataRateMin, theDataRateMax));
    	iLog->Log(_L("CVisualization::FrequencyBandCountRange  ----> %x - %x"), (TUint32)theDataRateMin, (TUint32)theDataRateMax);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::FrequencyBandCountRange FAILED %d"),error));
    	iLog->Log(_L("CVisualization::FrequencyBandCountRange FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::IsFrequencyDataEnabled()
	{
	TInt error = KErrNone;
	TBool enabled = EFalse;
	//set data rate range
	TRAP(error,enabled = iAudioVisualization->IsFrequencyDataEnabled());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::IsFrequencyDataEnabled %d"),enabled));
    	iLog->Log(_L("CVisualization::IsFrequencyDataEnabled %d"),enabled);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::IsFrequencyDataEnabled FAILED %d"),error));
    	iLog->Log(_L("CVisualization::IsFrequencyDataEnabled FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::IsWaveformDataEnabled()
	{
	TInt error = KErrNone;
	TBool enabled = EFalse;
	//set data rate range
	TRAP(error,enabled = iAudioVisualization->IsWaveformDataEnabled());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::IsWaveformDataEnabled %d"),enabled));
    	iLog->Log(_L("CVisualization::IsWaveformDataEnabled %d"),enabled);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::IsWaveformDataEnabled FAILED %d"),error));
    	iLog->Log(_L("CVisualization::IsWaveformDataEnabled FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::SamplingRate()
	{
	TInt error = KErrNone;
	TUint32 sampleRate = 0;
	//set data rate range
	TRAP(error,sampleRate = iAudioVisualization->SamplingRate());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::SamplingRate %d"),sampleRate));
    	iLog->Log(_L("CVisualization::SamplingRate %d"),sampleRate);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::SamplingRate FAILED %d"),error));
    	iLog->Log(_L("CVisualization::SamplingRate FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::SetFrequencyBandCount( CStifItemParser& aItem )
	{
	TInt dataFrequency;
	TInt error = KErrNone;
	error = aItem.GetNextInt(dataFrequency);
	TUint32 frequencyBandCount=(TUint32)dataFrequency;
	//set data rate range

	TRAP(error,iAudioVisualization->SetFrequencyBandCountL(frequencyBandCount));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::SetFrequencyBandCount %d"),frequencyBandCount));
    	iLog->Log(_L("CVisualization::SetFrequencyBandCount %d"),frequencyBandCount);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::SetFrequencyBandCount FAILED %d"),error));
    	iLog->Log(_L("CVisualization::SetFrequencyBandCount FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::SetWaveformDataLength( CStifItemParser& aItem )
	{
	TInt dataLength;
	TInt error = KErrNone;
	error = aItem.GetNextInt(dataLength);
	TUint32 SetWaveformDataLength=(TUint32)dataLength;

	//set data rate range
	TRAP(error,iAudioVisualization->SetWaveformDataLengthL(SetWaveformDataLength));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::SetWaveformDataLength %d"),SetWaveformDataLength));
    	iLog->Log(_L("CVisualization::SetWaveformDataLength %d"),SetWaveformDataLength);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::SetWaveformDataLength FAILED %d"),error));
    	iLog->Log(_L("CVisualization::SetWaveformDataLength FAILED %d"),error);
    	}
    return error;
	}


TInt CAudioVisualizationTestClass::WaveformDataLength()
	{
	TInt error = KErrNone;
	TUint32 waveData = 0;
	//set data rate range
	TRAP(error,waveData = iAudioVisualization->WaveformDataLength());
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::WaveformDataLength %d"),waveData));
    	iLog->Log(_L("CVisualization::WaveformDataLength %d"),waveData);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::WaveformDataLength FAILED %d"),error));
    	iLog->Log(_L("CVisualization::WaveformDataLength FAILED %d"),error);
    	}
	return error;
	}

TInt CAudioVisualizationTestClass::WaveformDataLengthRange()
	{
	TInt error = KErrNone;
	TUint32 theDataLengthMin;
	TUint32 theDataLengthMax;
	//set data rate range
	TRAP(error,iAudioVisualization->WaveformDataLengthRange(theDataLengthMin, theDataLengthMax));
    if(error == KErrNone)
    	{
    	FTRACE(FPrint(_L("CVisualization::WaveformDataLengthRange ----> %x - %x"),theDataLengthMin, theDataLengthMax));
    	iLog->Log(_L("CVisualization::WaveformDataLengthRange  ----> %x - %x"), (TUint32)theDataLengthMin, (TUint32)theDataLengthMax);
    	}
    else
    	{
    	FTRACE(FPrint(_L("CVisualization::WaveformDataLengthRange FAILED %d"),error));
    	iLog->Log(_L("CVisualization::WaveformDataLengthRange FAILED %d"),error);
    	}
	return error;
	}

//  End of File
