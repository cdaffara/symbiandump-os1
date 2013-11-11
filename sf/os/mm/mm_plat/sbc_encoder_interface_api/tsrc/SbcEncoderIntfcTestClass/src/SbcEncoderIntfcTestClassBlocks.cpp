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
* Description: SbcEncoderIntfcTest
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "SbcEncoderIntfcTestClass.h"
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
// CSbcEncoderIntfcTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSbcEncoderIntfcTestClass::Delete()
{

    FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
    iLog->Log(_L(""));
    iLog->Log(_L(""));

	iSupportedSamplingFrequencies.Close();
	iSupportedChannelModes.Close();
	iSupportedNumOfBlocks.Close();
	iSupportedNumOfSubbands.Close();
	iSupportedAllocationMethods.Close();

	if(iSbcEncoder)
		{
		delete iSbcEncoder;
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
// CSbcEncoderIntfcTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSbcEncoderIntfcTestClass::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "InitializeDevSound", CSbcEncoderIntfcTestClass::InitializeDevSound ),
		ENTRY( "CreateObject", CSbcEncoderIntfcTestClass::CreateObject ),
		ENTRY( "SetDataTypeInputStream", CSbcEncoderIntfcTestClass::SetDataTypeInputStream),
		ENTRY( "CreateSbcEncoderIntfc", CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc ),
  		ENTRY( "SetSamplingFrequency", CSbcEncoderIntfcTestClass::SetSamplingFrequency ),
		ENTRY( "SetChannelMode", CSbcEncoderIntfcTestClass::SetChannelMode ),
		ENTRY( "SetNumOfSubbands", CSbcEncoderIntfcTestClass::SetNumOfSubbands ),
  		ENTRY( "SetNumOfBlocks", CSbcEncoderIntfcTestClass::SetNumOfBlocks ),
		ENTRY( "SetAllocationMethod", CSbcEncoderIntfcTestClass::SetAllocationMethod ),
		ENTRY( "ApplyConfig", CSbcEncoderIntfcTestClass::ApplyConfig ),
  		ENTRY( "SetBitpoolSize", CSbcEncoderIntfcTestClass::SetBitpoolSize ),

		ENTRY( "GetSupportedSamplingFrequencies", CSbcEncoderIntfcTestClass::GetSupportedSamplingFrequencies ),
		ENTRY( "GetSupportedChannelModes", CSbcEncoderIntfcTestClass::GetSupportedChannelModes ),
  		ENTRY( "GetSupportedNumOfBlocks", CSbcEncoderIntfcTestClass::GetSupportedNumOfBlocks ),
		ENTRY( "GetSupportedNumOfSubbands", CSbcEncoderIntfcTestClass::GetSupportedNumOfSubbands ),
		ENTRY( "GetSupportedAllocationMethods", CSbcEncoderIntfcTestClass::GetSupportedAllocationMethods ),
  		ENTRY( "GetSupportedBitpoolRange", CSbcEncoderIntfcTestClass::GetSupportedBitpoolRange ),
		ENTRY( "GetSamplingFrequency", CSbcEncoderIntfcTestClass::GetSamplingFrequency ),
		ENTRY( "CreateSbcEncoderIntfc", CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc ),
  		ENTRY( "GetChannelMode", CSbcEncoderIntfcTestClass::GetChannelMode ),
		ENTRY( "GetNumOfSubbands", CSbcEncoderIntfcTestClass::GetNumOfSubbands ),
		ENTRY( "GetNumOfBlocks", CSbcEncoderIntfcTestClass::GetNumOfBlocks ),
  		ENTRY( "GetAllocationMethod", CSbcEncoderIntfcTestClass::GetAllocationMethod ),
		ENTRY( "GetBitpoolSize", CSbcEncoderIntfcTestClass::GetBitpoolSize )

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CSbcEncoderIntfcTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CSbcEncoderIntfcTestClass::EventName( TInt aKey )
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
// CSbcEncoderIntfcTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CSbcEncoderIntfcTestClass::AddExpectedEvent(TWmaExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::AddExpectedEvent")));
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
// CSbcEncoderIntfcTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CSbcEncoderIntfcTestClass::RemoveExpectedEvent(TWmaExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::RemoveExpectedEvent")));
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
// CSbcEncoderIntfcTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CSbcEncoderIntfcTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CSbcEncoderIntfcTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CSbcEncoderIntfcTestClass::ProcessEvent(TWmaExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::ProcessExpectedEvent")));
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
// CSbcEncoderIntfcTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CSbcEncoderIntfcTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::HandleTimeout")));
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
// CSbcEncoderIntfcTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CSbcEncoderIntfcTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetTimeout")));
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
TInt CSbcEncoderIntfcTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetAllowedPanic")));
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
// CSbcEncoderIntfcTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CSbcEncoderIntfcTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TWmaExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CSbcEncoderIntfcTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSbcEncoderIntfcTestClass::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KSbcEncoderIntfcTestClass, "SbcEncoderIntfcTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KSbcEncoderIntfcTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KSbcEncoderIntfcTestClass,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }



//////////////////////////
// GetData
//////////////////////////

TInt CSbcEncoderIntfcTestClass::SetConfigureData()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetConfigureData")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetConfigureData"));
	TInt error = KErrNone;

	SetSamplingFrequency();
	SetChannelMode();
	SetNumOfSubbands();
	SetNumOfBlocks();
	SetAllocationMethod();
	SetBitpoolSize();
	ApplyConfig();
	return error;
}



TInt CSbcEncoderIntfcTestClass::GetConfigureData()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetConfigureData")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetConfigureData"));
	TInt error = KErrNone;

	GetSupportedSamplingFrequencies();
	GetSupportedChannelModes();
	GetSupportedNumOfBlocks();
	GetSupportedNumOfSubbands();
	GetSupportedAllocationMethods();
	GetSupportedBitpoolRange();
	GetSamplingFrequency();
	GetChannelMode();
	GetNumOfSubbands();
	GetNumOfBlocks();
	GetAllocationMethod();
	GetBitpoolSize();
    return error;
}


//////////////////////////
// SetData
//////////////////////////



TInt CSbcEncoderIntfcTestClass::SetSamplingFrequency()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetSamplingFrequency")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetFoSetSamplingFrequency"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetSamplingFrequency 48000"));
	iSbcEncoder->SetSamplingFrequency(48000);

	return error;
}


TInt CSbcEncoderIntfcTestClass::SetChannelMode()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetChannelMode")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetChannelMode"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetChannelMode ESbcChannelDual"));
	iSbcEncoder->SetChannelMode(CSbcEncoderIntfc::ESbcChannelDual);

	return error;
}

TInt CSbcEncoderIntfcTestClass::SetNumOfSubbands()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetNumOfSubbands")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetNumOfSubbands"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetNumOfSubbands 4"));
	iSbcEncoder->SetNumOfSubbands(4);

	return error;
}


TInt CSbcEncoderIntfcTestClass::SetNumOfBlocks()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetNumOfBlocks")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetNumOfBlocks"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetNumOfBlocks 4"));
	iSbcEncoder->SetNumOfBlocks(4);

	return error;
}


TInt CSbcEncoderIntfcTestClass::SetAllocationMethod()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetAllocationMethod")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetAllocationMethod"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetAllocationMethod ESbcAllocationLoudness"));
	iSbcEncoder->SetAllocationMethod(CSbcEncoderIntfc::ESbcAllocationLoudness);

	return error;
}

TInt CSbcEncoderIntfcTestClass::SetBitpoolSize()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetBitpoolSize")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetBitpoolSize"));
	TInt error = KErrNone;

	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetBitpoolSize 100"));
	iSbcEncoder->SetBitpoolSize(32);

	return error;
}



TInt CSbcEncoderIntfcTestClass::GetSupportedSamplingFrequencies()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedSamplingFrequencies")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedSamplingFrequencies"));
	TInt error = KErrNone;

	error = iSbcEncoder->GetSupportedSamplingFrequencies(iSupportedSamplingFrequencies)  ;
	for (int i = 0; i<iSupportedSamplingFrequencies.Count(); ++i)
	{
	iLog->Log(_L("GetSupportedSamplingFrequencies - freq is %d"), iSupportedSamplingFrequencies[i]);
	}
	return error;
}


TInt CSbcEncoderIntfcTestClass::GetSupportedChannelModes()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedChannelModes")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedChannelModes"));
	TInt error = KErrNone;

	error = iSbcEncoder->GetSupportedChannelModes(iSupportedChannelModes)  ;
	for (int i = 0; i<iSupportedChannelModes.Count(); ++i)
	{
	iLog->Log(_L("GetSupportedChannelModes - freq is %d"), iSupportedChannelModes[i]);
	}
	return error;
}



TInt CSbcEncoderIntfcTestClass::GetSupportedNumOfBlocks()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedNumOfBlocks")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedNumOfBlocks"));
	TInt error = KErrNone;

	error = iSbcEncoder->GetSupportedNumOfBlocks(iSupportedNumOfBlocks)  ;
	for (int i = 0; i<iSupportedNumOfBlocks.Count(); ++i)
	{
	iLog->Log(_L("GetSupportedNumOfBlocks - supportedNumOfBlocks is %d"), iSupportedNumOfBlocks[i]);
	}
	return error;
}



TInt CSbcEncoderIntfcTestClass::GetSupportedNumOfSubbands()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedNumOfSubbands")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedNumOfSubbands"));
	TInt error = KErrNone;

	error = iSbcEncoder->GetSupportedNumOfSubbands(iSupportedNumOfSubbands)  ;
	for (int i = 0; i<iSupportedNumOfSubbands.Count(); ++i)
	{
	iLog->Log(_L("GetSupportedNumOfSubbands - supportedNumOfSubbands is %d"), iSupportedNumOfSubbands[i]);
	}
	return error;
}


TInt CSbcEncoderIntfcTestClass::GetSupportedAllocationMethods()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedAllocationMethods")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedAllocationMethods"));
	TInt error = KErrNone;

	error = iSbcEncoder->GetSupportedAllocationMethods(iSupportedAllocationMethods)  ;
	for (int i = 0; i<iSupportedAllocationMethods.Count(); ++i)
		{
		iLog->Log(_L("GetSupportedAllocationMethods - supportedAllocationMethods is %d"), iSupportedAllocationMethods[i]);
		}
	return error;
}




TInt CSbcEncoderIntfcTestClass::GetSupportedBitpoolRange( )
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSupportedBitpoolRange")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSupportedBitpoolRange"));
	TInt error = KErrNone;

    TUint minSize = 0, maxSize = 0;
	error = iSbcEncoder->GetSupportedBitpoolRange(minSize, maxSize);
	iLog->Log(_L("GetSupportedAllocationMethods - minSize is %d"), minSize);
	iLog->Log(_L("GetSupportedAllocationMethods - maxSize is %d"), maxSize);

	return error;

}



TInt CSbcEncoderIntfcTestClass::GetSamplingFrequency()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetSamplingFrequency")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetSamplingFrequency"));
	TInt error = KErrNone;

	TUint returnOutput = 0;
	error = iSbcEncoder->GetSamplingFrequency(returnOutput);
	iLog->Log(_L("GetSamplingFrequency - freq is %d"), returnOutput);
    if (returnOutput == 48000)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}

}


TInt CSbcEncoderIntfcTestClass::GetChannelMode()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetChannelMode")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetChannelMode"));
	TInt error = KErrNone;

    CSbcEncoderIntfc::TSbcChannelMode returnOutput;
	error = iSbcEncoder->GetChannelMode(returnOutput);
	iLog->Log(_L("GetChannelMode - channelMode is %d"), returnOutput);

    if (returnOutput == CSbcEncoderIntfc::ESbcChannelDual)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}
}




TInt CSbcEncoderIntfcTestClass::GetNumOfSubbands()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetNumOfSubbands")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetNumOfSubbands"));
	TInt error = KErrNone;

	TUint returnOutput = 0;
	error = iSbcEncoder->GetNumOfSubbands(returnOutput);
	iLog->Log(_L("GetNumOfSubbands - numOfSubbands is %d"), returnOutput);

    if (returnOutput == 4)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}
}


TInt CSbcEncoderIntfcTestClass::GetNumOfBlocks()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetNumOfBlocks")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetNumOfBlocks"));
	TInt error = KErrNone;

	TUint returnOutput = 0;
	error = iSbcEncoder->GetNumOfBlocks(returnOutput);
	iLog->Log(_L("GetNumOfBlocks - numOfBlocks is %d"), returnOutput);

    if (returnOutput == 4)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CSbcEncoderIntfcTestClass::GetAllocationMethod()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetAllocationMethod")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetAllocationMethod"));
	TInt error = KErrNone;

	CSbcEncoderIntfc::TSbcAllocationMethod returnOutput;
	error = iSbcEncoder->GetAllocationMethod(returnOutput);
	iLog->Log(_L("GetAllocationMethod - allocationMethod is %d"), returnOutput);

    if (returnOutput == CSbcEncoderIntfc::ESbcAllocationLoudness)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}
}



TInt CSbcEncoderIntfcTestClass::GetBitpoolSize()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::GetBitpoolSize")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::GetBitpoolSize"));
	TInt error = KErrNone;

	TUint returnOutput;
	error = iSbcEncoder->GetBitpoolSize(returnOutput);
	iLog->Log(_L("GetBitpoolSize - bitpoolSize is %d"), returnOutput);

    if (returnOutput == 32)
    	{
		return error;
    	}
	else
		{
		return KErrUnexpectedValue;
		}
}


TInt CSbcEncoderIntfcTestClass::ApplyConfig()
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::ApplyConfig")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::ApplyConfig"));
	TInt error = KErrNone;

	iSbcEncoder->ApplyConfig();

	return error;
}


// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateDevSoundInstance
// Create Factory
// -----------------------------------------------------------------------------
//

TInt CSbcEncoderIntfcTestClass::InitializeDevSound( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::InitializeDevSound")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::InitializeDevSound"));
	TInt error = KErrNone;
	    TRAP(error,iDevSound->InitializeL(*this,KMMFFourCCCodeSBC,EMMFStateRecording));
	    if(error != KErrNone)
	        {
	        iLog->Log(_L("CSbcEncoderIntfcTestClass::InitializeL Error [%d]"),error);
	        AddExpectedEvent(EInitializeComplete, KShortTimeout);
	        ProcessEvent(EInitializeComplete, error);
	        }
	    else
	    	{
	    	FTRACE(FPrint(_L("InitializeDevSound OK")));
	    	iLog->Log(_L("InitializeDevSound OK"));
	    	}
	return error;
	}

TInt CSbcEncoderIntfcTestClass::SetDataTypeInputStream( CStifItemParser& /*aItem */)
	{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::SetDataTypeInputStream")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::SetDataTypeInputStream"));
	TInt error = KErrNone;
	iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
	iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
	iStreamSettings.iVolume = 1;
    TRAP(error, iAudioInputStream->SetDataTypeL(KMMFFourCCCodeSBC));
    iLog->Log(_L("iAudioInputStream->SetDataTypeL(KMMFFourCCCodeSBC) [%d]"),error);
	if (error == KErrNone)
		{
		iAudioInputStream->Open(&iStreamSettings);
		iLog->Log(_L("iAudioInputStream->Open(&iStreamSettings)"));
		}
	else
		{
	    iLog->Log(_L("CSbcEncoderIntfcTestClass::CreateInputStreamInstance Error [%d]"),error);
	    AddExpectedEvent(EMaiscOpenComplete, KShortTimeout);
        ProcessEvent(EMaiscOpenComplete, error);
	    }
	return error;
	}

// -----------------------------------------------------------------------------
// CSbcEncoderIntfcTestClass::CreateObject
// Create Create the DevSound or Input Stream Instance
// -----------------------------------------------------------------------------
//
TInt CSbcEncoderIntfcTestClass::CreateObject( CStifItemParser& aItem )
{
	TPtrC object;
	aItem.GetNextString(object);

	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::CreateObject")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::CreateObject"));

	TInt error = KErrNone;

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
	    if (error == KErrNone)
	    	{
	        AddExpectedEvent(EInitializeComplete, KMediumTimeout);
	    	}
		}
	else if (object == KTagInputStream)
		{
	    TRAP(error, iAudioInputStream = CMdaAudioInputStream::NewL(*this););
	    if (error == KErrNone)
	    	{
		    AddExpectedEvent(EMaiscOpenComplete, KShortTimeout);
	    	}
		}
	else
		{
		error = KErrNotSupported;
		FTRACE(FPrint(_L("No Data Input %d"),error));
		iLog->Log(_L("No Data Input %d"),error);
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateSbcEncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateSbcEncoderIntfc FAILED %d"),error);
		}
	else
		{
		FTRACE(FPrint(_L("CreateObject OK")));
		iLog->Log(_L("CreateObject OK"));
		}
	return error;
}


// -----------------------------------------------------------------------------
// CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc
// Create CreateSbcEncoderIntfc
// -----------------------------------------------------------------------------
//
TInt CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc")));
	iLog->Log(_L("CSbcEncoderIntfcTestClass::CreateSbcEncoderIntfc"));
	TInt error = KErrNone;
	TPtrC object;
	aItem.GetNextString(object);

	if (object == KTagDevSound)
		{
		TRAP(error, iSbcEncoder = CSbcEncoderIntfc::NewL(*iDevSound));
		}
	else if (object == KTagInputStream)
		{
		TRAP(error, iSbcEncoder = CSbcEncoderIntfc::NewL(*iAudioInputStream));
		}
	if (error != KErrNone)
		{
		FTRACE(FPrint(_L("CreateSbcEncoderIntfc-FAILED %d"),error));
		iLog->Log(_L("CreateSbcEncoderIntfc FAILED %d"),error);
		}
	else
		{
		FTRACE(FPrint(_L("CreateSbcEncoderIntfc-OK ")));
		iLog->Log(_L("CreateSbcEncoderIntfc OK "));
		}
	return error;
}

//  End of File
