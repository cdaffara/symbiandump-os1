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
* Description: WmaDecoderIntfcTest
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "WmaDecoderIntfcTestClass.h"
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
// CWmaDecoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CWmaDecoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	iSupportedFormats.Close();
	iSupportedTools.Close();
	iControllableTools.Close();

	if(iWmaDecoder)
    {
    	delete iWmaDecoder;
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
// CWmaDecoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CWmaDecoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		ENTRY( "CreateDevSoundInstance", CWmaDecoderIntfcTestClass::CreateDevSoundInstance ),
		ENTRY( "CreateInputStreamInstance", CWmaDecoderIntfcTestClass::CreateInputStreamInstance ),
		ENTRY( "GetFormat", CWmaDecoderIntfcTestClass::GetFormat),
		ENTRY( "GetBitsPerSampleIn", CWmaDecoderIntfcTestClass::GetBitsPerSampleIn),
		ENTRY( "GetNumOfChannelsIn", CWmaDecoderIntfcTestClass::GetNumOfChannelsIn),
		ENTRY( "GetSamplesPerSec", CWmaDecoderIntfcTestClass::GetSamplesPerSec),
		ENTRY( "GetAvgBytesPerSec", CWmaDecoderIntfcTestClass::GetAvgBytesPerSec),
		ENTRY( "GetBlockAlign", CWmaDecoderIntfcTestClass::GetBlockAlign),
		ENTRY( "GetEncodeOptions", CWmaDecoderIntfcTestClass::GetEncodeOptions),
		ENTRY( "GetEncodeOptions1", CWmaDecoderIntfcTestClass::GetEncodeOptions1),
		ENTRY( "GetEncodeOptions2", CWmaDecoderIntfcTestClass::GetEncodeOptions2),
		ENTRY( "GetChannelMaskIn", CWmaDecoderIntfcTestClass::GetChannelMaskIn),
		ENTRY( "GetSupportedFormats", CWmaDecoderIntfcTestClass::GetSupportedFormats),
		ENTRY( "GetSupportedTools", CWmaDecoderIntfcTestClass::GetSupportedTools),
		ENTRY( "GetSupportedMaxChannelsIn", CWmaDecoderIntfcTestClass::GetSupportedMaxChannelsIn),
		ENTRY( "GetSupportedMaxSampleRate", CWmaDecoderIntfcTestClass::GetSupportedMaxSampleRate),
		ENTRY( "GetSupportedMaxBitrate", CWmaDecoderIntfcTestClass::GetSupportedMaxBitrate),
		ENTRY( "GetControllableTools", CWmaDecoderIntfcTestClass::GetControllableTools),
		ENTRY( "GetTool", CWmaDecoderIntfcTestClass::GetTool),
		ENTRY( "SetFormat", CWmaDecoderIntfcTestClass::SetFormat),
		ENTRY( "SetBitsPerSampleIn", CWmaDecoderIntfcTestClass::SetBitsPerSampleIn),
		ENTRY( "SetNumChannelsIn", CWmaDecoderIntfcTestClass::SetNumChannelsIn),
		ENTRY( "SetSamplesPerSec", CWmaDecoderIntfcTestClass::SetSamplesPerSec),
		ENTRY( "SetAvgBytesPerSec", CWmaDecoderIntfcTestClass::SetAvgBytesPerSec),
		ENTRY( "SetBlockAlign", CWmaDecoderIntfcTestClass::SetBlockAlign),
		ENTRY( "SetEncodeOptions", CWmaDecoderIntfcTestClass::SetEncodeOptions),
		ENTRY( "SetEncodeOptions1", CWmaDecoderIntfcTestClass::SetEncodeOptions1),
		ENTRY( "SetEncodeOptions2", CWmaDecoderIntfcTestClass::SetEncodeOptions2),
		ENTRY( "SetChannelMaskIn", CWmaDecoderIntfcTestClass::SetChannelMaskIn),
		ENTRY( "EnableTool", CWmaDecoderIntfcTestClass::EnableTool),
		ENTRY( "DisableTool", CWmaDecoderIntfcTestClass::DisableTool),
		ENTRY( "ApplyConfig", CWmaDecoderIntfcTestClass::ApplyConfig),
		ENTRY( "CreateWmaDecoderIntfc", CWmaDecoderIntfcTestClass::CreateWmaDecoderIntfc ),
        ENTRY( "Example", CWmaDecoderIntfcTestClass::ExampleL ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CWmaDecoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CWmaDecoderIntfcTestClass::EventName( TInt aKey )
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
// CWmaDecoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CWmaDecoderIntfcTestClass::AddExpectedEvent(TWmaExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::AddExpectedEvent")));
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
// CWmaDecoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CWmaDecoderIntfcTestClass::RemoveExpectedEvent(TWmaExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::RemoveExpectedEvent")));
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
// CWmaDecoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CWmaDecoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CWmaDecoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CWmaDecoderIntfcTestClass::ProcessEvent(TWmaExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::ProcessExpectedEvent")));
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
// CWmaDecoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CWmaDecoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::HandleTimeout")));
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
// CWmaDecoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CWmaDecoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetTimeout")));
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
TInt CWmaDecoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetAllowedPanic")));
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
// CWmaDecoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CWmaDecoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
	{
		AddExpectedEvent(static_cast<TWmaExpectedEvent>(event), 0); // Default timeout value
	}
	return error;
}


// -----------------------------------------------------------------------------
// CWmaDecoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CWmaDecoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KWmaDecoderIntfcTestClass, "WmaDecoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KWmaDecoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KWmaDecoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }



//////////////////////////
// GetData
//////////////////////////


TInt CWmaDecoderIntfcTestClass::GetFormat( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetFormat")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetFormat"));
	TInt error = KErrNone;

	CWmaDecoderIntfc::TFormat aa;
	error = iWmaDecoder->GetFormat(aa);

	return error;
}



TInt CWmaDecoderIntfcTestClass::GetBitsPerSampleIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetBitsPerSampleIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetBitsPerSampleIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetBitsPerSampleIn(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetNumOfChannelsIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetNumOfChannelsIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetNumOfChannelsIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetNumOfChannelsIn(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}

}



TInt CWmaDecoderIntfcTestClass::GetSamplesPerSec( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSamplesPerSec")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSamplesPerSec"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetSamplesPerSec(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetAvgBytesPerSec( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetAvgBytesPerSec")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetAvgBytesPerSec"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetAvgBytesPerSec(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetBlockAlign( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetBlockAlign")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetBlockAlign"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetBlockAlign(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetEncodeOptions( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetEncodeOptions(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetEncodeOptions1( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions1")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions1"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetEncodeOptions1(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetEncodeOptions2( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions2")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetEncodeOptions2"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetEncodeOptions2(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetChannelMaskIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetChannelMaskIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetChannelMaskIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	TUint returnOutput;
	error = iWmaDecoder->GetChannelMaskIn(returnOutput);

	if (inputParam == returnOutput)
	{
		return error;
	}
	else
	{
		return KErrUnexpectedValue;
	}
}



TInt CWmaDecoderIntfcTestClass::GetSupportedFormats( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSupportedFormats")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSupportedFormats"));
	TInt error = KErrNone;

	CWmaDecoderIntfc::TFormat f1;
	error = iWmaDecoder->GetSupportedFormats(iSupportedFormats)  ;
	for (int i = 0; i<iSupportedFormats.Count(); ++i)
	{
		f1 = iSupportedFormats[i];
		if ( f1 == CWmaDecoderIntfc::EWma)
		{
			iLog->Log(_L("GetSupportedFormats - support EWma format"));
		}
		else if ( f1 == CWmaDecoderIntfc::EWmaPro)
		{
			iLog->Log(_L("GetSupportedFormats - support EWmaPro format"));
		}
		else
		{
			iLog->Log(_L("GetSupportedFormats - support unknown format"));
		}
	}
	return error;
}

TInt CWmaDecoderIntfcTestClass::GetSupportedTools( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSupportedTools")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSupportedTools"));
	TInt error = KErrNone;

	CWmaDecoderIntfc::TTool tool;
	error = iWmaDecoder->GetControllableTools(iSupportedTools)  ;
  for (int k = 0; k<iSupportedTools.Count(); ++k)
	{
		tool = iSupportedTools[k];
		if (tool == CWmaDecoderIntfc::EToolOutput32Bit)
		{
			iLog->Log(_L("GetSupportedTools - support EToolOutput32Bit"));
		}
		else if (tool == CWmaDecoderIntfc::EDownMixToStereo)
		{
			iLog->Log(_L("GetSupportedTools - support EDownMixToStereo"));
		}
		else if (tool == CWmaDecoderIntfc::EDownMixToStereo)
		{
			iLog->Log(_L("GetSupportedTools - support ELostDataConcealment"));
		}
		else
		{
			iLog->Log(_L("GetSupportedTools - support unknown tool"));
		}
	}

	return error;
}

TInt CWmaDecoderIntfcTestClass::GetSupportedMaxChannelsIn( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxChannelsIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxChannelsIn"));
	TInt error = KErrNone;

	TUint aSupportedMaxChannelsIn;
  error = iWmaDecoder->GetSupportedMaxChannelsIn(aSupportedMaxChannelsIn);
	iLog->Log(_L("Returned SupportedMaxChannelsIn = %d"), aSupportedMaxChannelsIn);

	return error;
}


TInt CWmaDecoderIntfcTestClass::GetSupportedMaxBitrate( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxBitrate")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxBitrate"));
	TInt error = KErrNone;

	TUint aSupportedMaxBitrate;
  error = iWmaDecoder->GetSupportedMaxBitrate(aSupportedMaxBitrate)  ;
	iLog->Log(_L("Returned SupportedMaxBitrate = %d"), aSupportedMaxBitrate);

	return error;
}


TInt CWmaDecoderIntfcTestClass::GetSupportedMaxSampleRate( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxSampleRate")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetSupportedMaxSampleRate"));
	TInt error = KErrNone;

	TUint aSupportedmaxSamplerate;
	error = iWmaDecoder->GetSupportedMaxSampleRate(aSupportedmaxSamplerate)  ;
	iLog->Log(_L("Returned SupportedmaxSamplerate = %d"), aSupportedmaxSamplerate);

	return error;
}


TInt CWmaDecoderIntfcTestClass::GetControllableTools( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetControllableTools")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetControllableTools"));
	TInt error = KErrNone;

	CWmaDecoderIntfc::TTool tool;
	error = iWmaDecoder->GetControllableTools(iControllableTools)  ;
  for (int k = 0; k<iControllableTools.Count(); ++k)
	{
		tool = iControllableTools[k];
		if (tool == CWmaDecoderIntfc::EToolOutput32Bit)
		{
			iLog->Log(_L("GetControllableTools - support EToolOutput32Bit"));
		}
		else if (tool == CWmaDecoderIntfc::EDownMixToStereo)
		{
			iLog->Log(_L("GetControllableTools - support EDownMixToStereo"));
		}
		else if (tool == CWmaDecoderIntfc::EDownMixToStereo)
		{
			iLog->Log(_L("GetControllableTools - support ELostDataConcealment"));
		}
		else
		{
			iLog->Log(_L("GetControllableTools - support unknown tool"));
		}
	}
	return error;
}



TInt CWmaDecoderIntfcTestClass::GetTool( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::GetTool")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool"));
	TInt error = KErrNone;

	TPtrC ToolType;
	error = aItem.GetNextString(ToolType);


	TBool aEnabledStatus;

	CWmaDecoderIntfc::TTool tool7 = CWmaDecoderIntfc::EToolOutput32Bit;
	CWmaDecoderIntfc::TTool tool2 = CWmaDecoderIntfc::EDownMixToStereo;
	CWmaDecoderIntfc::TTool tool3 = CWmaDecoderIntfc::ELostDataConcealment;

	if (error == KErrNone)
	{
		if (ToolType == KTagOutput32Bit)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool - Output32Bit"));
			iWmaDecoder->GetTool(tool7, aEnabledStatus);
		}
		else if (ToolType == KTagDownMixToStereo)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool - DownMixToStereo"));
			iWmaDecoder->GetTool(tool2, aEnabledStatus);
		}
		else if (ToolType == KTagLostDataConcealment)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool - LostDataConcealment"));
			iWmaDecoder->GetTool(tool3, aEnabledStatus);
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
			return error;
		}

		TPtrC ExpectedStatus;
		error = aItem.GetNextString(ExpectedStatus);
		if (error == KErrNone)
		{
			if ( (ExpectedStatus == KTagTrue) && aEnabledStatus || (ExpectedStatus == KTagFalse) && !aEnabledStatus )
			{
				iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool OK"));
			}
			else
			{
				iLog->Log(_L("CWmaDecoderIntfcTestClass::GetTool failed. aEnabledStatus = %d"), aEnabledStatus);
				error = KErrUnexpectedValue;
			}
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
		}

	}

	return error;
}


//////////////////////////
// SetData
//////////////////////////



TInt CWmaDecoderIntfcTestClass::SetFormat( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetFormat")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetFormat"));
	TInt error = KErrNone;

	TPtrC Format;
	error = aItem.GetNextString(Format);

	if (error == KErrNone)
	{
		if (Format == KTagWma)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::SetFormat - EWma"));
			iWmaDecoder->SetFormat(CWmaDecoderIntfc::EWma);
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
		}

	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetBitsPerSampleIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetBitsPerSampleIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetBitsPerSampleIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetBitsPerSampleIn - %d"), inputParam);
		iWmaDecoder->SetBitsPerSampleIn(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetNumChannelsIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetNumChannelsIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetNumChannelsIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetNumChannelsIn - %d"), inputParam);
		iWmaDecoder->SetNumChannelsIn(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetSamplesPerSec( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetSamplesPerSec")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetSamplesPerSec"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetSamplesPerSec - %d") , inputParam);
		iWmaDecoder->SetSamplesPerSec(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetAvgBytesPerSec( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetAvgBytesPerSec")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetAvgBytesPerSec"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetAvgBytesPerSec - %d"), inputParam);
		iWmaDecoder->SetAvgBytesPerSec(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetBlockAlign( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetBlockAlign")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetBlockAlign"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetBlockAlign - %d"), inputParam);
		iWmaDecoder->SetBlockAlign(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetEncodeOptions( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions - %d"), inputParam);
		iWmaDecoder->SetEncodeOptions(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetEncodeOptions1( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions1")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions1"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions1 - %d"), inputParam);
		iWmaDecoder->SetEncodeOptions1(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetEncodeOptions2( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions2")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions2"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetEncodeOptions2 - %d"), inputParam);
		iWmaDecoder->SetEncodeOptions2(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::SetChannelMaskIn( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::SetChannelMaskIn")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::SetChannelMaskIn"));
	TInt error = KErrNone;

	TInt inputParam = 0;
	error = aItem.GetNextInt(inputParam);

	if (error == KErrNone)
	{
		iLog->Log(_L("CWmaDecoderIntfcTestClass::SetChannelMaskIn - %d") ,inputParam );
		iWmaDecoder->SetChannelMaskIn(inputParam);
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}

	return error;
}




TInt CWmaDecoderIntfcTestClass::ApplyConfig( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::ApplyConfig")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::ApplyConfig"));
	TInt error = KErrNone;

	iWmaDecoder->ApplyConfig();

	return error;
}



TInt CWmaDecoderIntfcTestClass::EnableTool( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::EnableTool")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::EnableTool"));
	TInt error = KErrNone;

	TPtrC ToolType;
	error = aItem.GetNextString(ToolType);


	CWmaDecoderIntfc::TTool tool7 = CWmaDecoderIntfc::EToolOutput32Bit;
	CWmaDecoderIntfc::TTool tool2 = CWmaDecoderIntfc::EDownMixToStereo;
	CWmaDecoderIntfc::TTool tool3 = CWmaDecoderIntfc::ELostDataConcealment;

	if (error == KErrNone)
	{
		if (ToolType == KTagOutput32Bit)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::EnableTool - Output32Bit"));
			iWmaDecoder->EnableTool(tool7)  ;
		}
		else if (ToolType == KTagDownMixToStereo)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::EnableTool - DownMixToStereo"));
			iWmaDecoder->EnableTool(tool2)  ;
		}
		else if (ToolType == KTagLostDataConcealment)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::EnableTool - LostDataConcealment"));
			iWmaDecoder->EnableTool(tool3)  ;
		}
		else
		{
			iLog->Log(KMsgBadTestParameters);
			error = KErrBadTestParameter;
		}

	}

	return error;
}



TInt CWmaDecoderIntfcTestClass::DisableTool( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::DisableTool")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::DisableTool"));
	TInt error = KErrNone;

	TPtrC ToolType;
	error = aItem.GetNextString(ToolType);


	CWmaDecoderIntfc::TTool tool7 = CWmaDecoderIntfc::EToolOutput32Bit;
	CWmaDecoderIntfc::TTool tool2 = CWmaDecoderIntfc::EDownMixToStereo;
	CWmaDecoderIntfc::TTool tool3 = CWmaDecoderIntfc::ELostDataConcealment;

	if (error == KErrNone)
	{
		if (ToolType == KTagOutput32Bit)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::DisableTool - Output32Bit"));
			iWmaDecoder->DisableTool(tool7)  ;
		}
		else if (ToolType == KTagDownMixToStereo)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::DisableTool - DownMixToStereo"));
			iWmaDecoder->DisableTool(tool2)  ;
		}
		else if (ToolType == KTagLostDataConcealment)
		{
			iLog->Log(_L("CWmaDecoderIntfcTestClass::DisableTool - LostDataConcealment"));
			iWmaDecoder->DisableTool(tool3)  ;
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
// CG729DecoderIntfcTestClass::CreateFactory
// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CWmaDecoderIntfcTestClass::CreateDevSoundInstance( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CIlbcEncoderIntfcTestClass::CreateDevSoundInstance")));
	iLog->Log(_L("CIlbcEncoderIntfcTestClass::CreateDevSoundInstance"));
	TInt error = KErrNone;

    TRAP(error, iDevSound = CMMFDevSound::NewL());

	if (error == KErrNone)
	    {
	    TRAP(error,iDevSound->InitializeL(*this,KMMFFourCCCodeWMA,EMMFStateRecording));
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

TInt CWmaDecoderIntfcTestClass::CreateInputStreamInstance(CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance")));
	iLog->Log(_L("CG729EncoderIntfcTestClass::CreateInputStreamInstance"));
	TInt error = KErrNone;

    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this));
    if (error == KErrNone)
    	{
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iVolume = 1;

    	TRAP(error, iAudioInputStream->SetDataTypeL(KMMFFourCCCodeWMA));
        iLog->Log(_L("iAudioInputStream->SetDataTypeL(KMMFFourCCCodeWMA) [%d]"),error);
		iAudioInputStream->Open(&iStreamSettings);
	    if (error == KErrNone)
		    {
	        AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout);
		    iLog->Log(_L(" AddExpectedEvent(EMaiscOpenComplete, KMediumTimeout)"));
		    }
	    else
	        {
	        iLog->Log(_L("CWmaDecoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
	        }
    	}
    else
    	{
        iLog->Log(_L("CWmaDecoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
    	}
    return error;
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CWmaDecoderIntfcTestClass::CreateRestrictedAO
// Create RestrictedAO
// -----------------------------------------------------------------------------
//
TInt CWmaDecoderIntfcTestClass::CreateWmaDecoderIntfc( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CWmaDecoderIntfcTestClass::CreateWmaDecoderIntfc")));
	iLog->Log(_L("CWmaDecoderIntfcTestClass::CreateWmaDecoderIntfc"));
	TInt error = KErrNone;

	/* By FC
	 * Started on 06/12/08
	 * Finished on 06/12/08
	 */
	TPtrC Type;
	error = aItem.GetNextString(Type);

	if (iAudioInputStream)
		{
		TRAP (error, iWmaDecoder = CWmaDecoderIntfc::NewL(*iAudioInputStream));
		}
	else if (iDevSound)
		{
		TRAP (error, iWmaDecoder = CWmaDecoderIntfc::NewL(*iDevSound));
		}
	if(error != KErrNone)
		{
		iLog->Log(_L("CEaacPlusDecoderIntfcTestClass::InitializeL Error [%d]"),error);
		}

	return error;
}

//  End of File
