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
* Description:  AudioroutingControl test module
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "AudioRoutingTestClass.h"
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
// CAudioAudioRoutingTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
TInt CAudioRoutingTestClass::Delete()
{
	TInt error= KErrNone;
    FTRACE(FPrint(_L("CAudioRoutingTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));


	if (iAudioInput)
		{
		delete iAudioInput;
		}

	if (iAudioOutput)
		{
		delete iAudioOutput;
		}

	if (iAudioTone)
		{
		delete iAudioTone;
		}

	if (iAudioInputStream)
		{
		delete iAudioInputStream;
		}

	if (iAudioOutputStream)
		{
		delete iAudioOutputStream;
		}

	if (iCustomCommandUtility)
		{
		delete iCustomCommandUtility;
		}

	if (iMidiClientUtility)
		{
		iMidiClientUtility->Close();
		delete iMidiClientUtility;
		}

	if (iRecorder)
		{
		delete iRecorder;
		}

	if (iUtil)
		{
		delete iUtil;
		}

	if (iDrmPlayerUtility)
		{
		delete iDrmPlayerUtility;
		}


	if (iVideoPlayerUtility)
		{
		delete iVideoPlayerUtility;
		DeleteVideoWindow();
		}
	if (iDevSound)
		{
		delete iDevSound;
		}
	REComSession::FinalClose();
	return error;
}

// -----------------------------------------------------------------------------
// CAudioAudioRoutingTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CAudioRoutingTestClass::RunMethodL( CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		ENTRY( "AudioInput", CAudioRoutingTestClass::CreateAudioInput),
		ENTRY( "CreateObject", CAudioRoutingTestClass::CreateObject),
		ENTRY( "AudioInputs", CAudioRoutingTestClass::AudioInput),
		ENTRY( "SetAudioInput", CAudioRoutingTestClass::SetAudioInputL),
		ENTRY( "AudioOutput", CAudioRoutingTestClass::AudioOutput),
		ENTRY( "DefaultAudioOutput", CAudioRoutingTestClass::DefaultAudioOutput),
		ENTRY( "RegisterObserver", CAudioRoutingTestClass::RegisterObserverL),
		ENTRY( "SecureOutput", CAudioRoutingTestClass::SecureOutput),
		ENTRY( "SetAudioOutput", CAudioRoutingTestClass::SetAudioOutputL),
		ENTRY( "SetSecureOutput", CAudioRoutingTestClass::SetSecureOutputL),
		ENTRY( "UnregisterObserver", CAudioRoutingTestClass::UnregisterObserver),
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CAudioAudioRoutingTest::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CAudioRoutingTestClass::EventName( TInt aKey )
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
// CAudioAudioRoutingTest::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CAudioRoutingTestClass::AddExpectedEvent(TG711ExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::AddExpectedEvent")));
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
// CAudioAudioRoutingTest::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CAudioRoutingTestClass::RemoveExpectedEvent(TG711ExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::RemoveExpectedEvent")));
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
// CAudioAudioRoutingTest::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CAudioRoutingTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CAudioAudioRoutingTest::ProcessEvent
// -----------------------------------------------------------------------------
void CAudioRoutingTestClass::ProcessEvent(TG711ExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::ProcessEvent")));
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
// CAudioAudioRoutingTest::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CAudioRoutingTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::HandleTimeout")));
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
// CAudioAudioRoutingTest::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CAudioRoutingTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::SetTimeout")));
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
TInt CAudioRoutingTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::SetAllowedPanic")));
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
// CAudioAudioRoutingTest::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CAudioRoutingTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CAudioRoutingTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
		{
		AddExpectedEvent(static_cast<TG711ExpectedEvent>(event), 0); // Default timeout value
		}
	return error;
}


// -----------------------------------------------------------------------------
// CAudioAudioRoutingTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAudioRoutingTestClass::ExampleL( CStifItemParser& aItem )
    {
    _LIT( KAudioRoutingTestClass, "AudioRoutingTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KAudioRoutingTestClass, KExample );

    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KAudioRoutingTestClass, KParam, i, &string );
        i++;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CG711DecoderIntfcTestClass::CreateObject
// Create Factory
// -----------------------------------------------------------------------------
//


TInt CAudioRoutingTestClass::CreateObject( CStifItemParser& aItem )
	{
	FTRACE(FPrint(_L("CAudioAudioRoutingTest::CreateObject")));
	iLog->Log(_L("CAudioAudioRoutingTest::CreateObject"));
	TInt error = KErrNone;

	TPtrC object;
	aItem.GetNextString ( object );

	iObject.Set(object);
	iLog->Log(_L("the object to create is [%S]"),&object);

	if (object == KTagDevSound)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		if (error == KErrNone)
		    {
		    TRAP(error,iDevSound->InitializeL(*this,KMMFFourCCCodeAMR,EMMFStatePlaying));
		    }
		}
	else if (object == KTagRecorder)
		{
		iInputOutput = ETrue;
		CreateInput();
		}
	else if (object== KTagCustomCommand)
		{
		TPtrC type;
		aItem.GetNextString ( type );
		if (type == KTagAudioPlayer)
			{
			TRAP(error,iCustomCommandUtility = CCustomCommandUtility::NewL(*iUtil));
			}
		else
			{
			TRAP(error,iCustomCommandUtility = CCustomCommandUtility::NewL(*iRecorder,ETrue));
			}
		}
	else if (object == KTagCustomInterface)
		{
		TPtrC type;
		aItem.GetNextString ( type );
		if (type == KTagAudioPlayer)
			{
			TRAP(error,iCustomInterfaceUtility = CCustomInterfaceUtility::NewL(*iUtil));
			}
		else if (type == KTagCustomCommand)
			{
			TRAP(error,iCustomInterfaceUtility = CCustomInterfaceUtility::NewL(*iCustomCommandUtility));
			}
		else if (type == KTagRecorder)
			{
			TRAP(error,iCustomInterfaceUtility = CCustomInterfaceUtility::NewL(*iRecorder,ETrue));
			}
		}
	else if (object == KTagInputStream)
		{
		TRAP(error,iAudioInputStream = CMdaAudioInputStream::NewL(*this));
		}
	else if (object == KTagAudioPlayer)
		{
		CreateOutput();
		}
	else if (object == KTagOuputRecord)
		{
		iInputOutput = EFalse;
		CreateInput();
		}
	else if (object == KTagOutputStream)
		{
		iStreamSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		iStreamSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
		iStreamSettings.iVolume = 1;
		TRAPD(error, iAudioOutputStream = CMdaAudioOutputStream::NewL(*this));
		if (error == KErrNone)
			{
			iAudioOutputStream->Open(&iStreamSettings);
			}
		}
	else if (object == KTagToneUtility)
		{
		TRAP(error,iAudioTone = CMdaAudioToneUtility::NewL(*this));
		}
	else if (object == KTagDevSound1)
		{
	    TRAP(error, iDevSound = CMMFDevSound::NewL());
		}
	else if (object == KTagMidiClient)
		{
	    TRAP(error, iMidiClientUtility = CMidiClientUtility::NewL(*this,80,TMdaPriorityPreference(0x00000000),EFalse));
	    //TRAP(error, iMidiClientUtility = CMidiClientUtility::NewL(*this));
	    if(error == KErrNone)
	    	{
	    	isInput = EFalse;
	    	//iMidiClientUtility->OpenFile(KAudioMidiFile);
	    	}
		}
	else if (object == KTagDrmPlayer)
		{
	    TRAP(error, iDrmPlayerUtility = CDrmPlayerUtility::NewL(*this,EMdaPriorityNormal,TMdaPriorityPreference(0x00000000)));
	    if (error == KErrNone)
	    	{
	    	iDrmPlayerUtility->OpenFileL(KAudioDrmFile);
	    	}
		}
	else if (object == KTagVideoPlayer)
		{
		aItem.GetNextString ( object );
		error = KErrNone;
		TRAP(error, CreateSessionAndScreen());
		TRAP(error, CreateWindowGroup());
		TRAP(error, CreateVideoWindow());
		TRAP(error, CreateVideoObject());	    
		}
	else if(object == KTagVideoRecord)
	    {
        TRAP(error, CreateVideoRecorderObject());        
	    }
	if (error == KErrNone)
		{
	    AddExpectedEvent(EInitializeComplete, KMediumTimeout);
		}
	else
	    {
	    iLog->Log(_L("CAudioRoutingTestClass::CreateObject Error [%d] - [%S]"),error,&object);
	    FTRACE(FPrint(_L("CAudioRoutingTestClass::CreateObject Error [%d] - [%S]"),error,&object));
	    }
	return error;
	}

TInt CAudioRoutingTestClass::CreateAudioInput( CStifItemParser& aItem )
	{
	TInt error = KErrNone;

	TPtrC object;
	RArray<CAudioInput::TAudioInputPreference> inputArray;

	aItem.GetNextString ( object );
	iLog->Log(_L("The AudioInput to create is with [%S]"),&object);

	if ( object == KTagVideoRecord )
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput"));		
		TRAP(error, iAudioInput = CAudioInput::NewL(*iVideoRecord));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioInput::NewL(*iVideoRecord) OK"));
			}
		}

	else if ( object == KTagDevSound )
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput"));
		TRAP(error, iAudioInput = CAudioInput::NewL(*iDevSound));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioInput::NewL(*iDevSound) OK"));
			}
		}
	else if (object == KTagRecorder)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput"));
		TRAP(error, iAudioInput = CAudioInput::NewL(*iRecorder));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioInput::NewL(*iRecorder) OK"));
			}
		}
	else if (object == KTagCustomCommand)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput - iCustomCommandUtility")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput - iCustomCommandUtility"));
		TRAP(error, iAudioInput = CAudioInput::NewL(*iCustomCommandUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioInput::NewL(*iCustomCommandUtility) OK"));
			}
		}
	else if (object == KTagCustomInterface)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput - CustomInterfaceUtility")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput - CustomInterfaceUtility"));
		TRAP(error, iAudioInput = CAudioInput::NewL(*iCustomInterfaceUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioInput::NewL(*iCustomInterfaceUtility) OK"));
			}
		}
	else if (object == KTagInputStream)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput - InputStream")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput - InputStream"));
		TRAP(error, iAudioInput = CAudioInput::NewL(*iAudioInputStream));
		if (error == KErrNone)
			{
			iLog->Log(_L(" CAudioInput::NewL(*iAudioInputStream) OK"));
			}
		}
	else if (object == KTagOuputRecord)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioInput - InputStream")));
		iLog->Log(_L("CAudioRoutingTest::CAudioInput - InputStream"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iRecorder,ETrue));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iRecorder,ETrue OK"));
			}
		}
	else if (object == KTagOutputStream)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - OutputStream")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - OutputStream"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iAudioOutputStream));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iAudioOutputStream) OK"));
			}
		}
	else if (object == KTagToneUtility)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - AudioTone")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - AudioTone"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iAudioTone));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iAudioTone) OK"));
			}
		}
	else if (object == KTagDevSound1)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - DevSound")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - DevSound"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iDevSound));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iDevSound) OK"));
			}
		}
	else if (object == KTagCustomInterfaceOutput)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - CustomInterfaceOutput")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - CustomInterfaceOutput"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iCustomInterfaceUtility));
		}
	else if (object == KTagAudioPlayer)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - AudioPlayer")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - AudioPlayer"));
		TRAPD(error,iAudioOutput = CAudioOutput::NewL(*iUtil));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iUtil) OK"));
			}
		}
	else if (object == KTagCustomCommandOutput)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - iCustomCommandOutput")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - iCustomCommandOutput"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iCustomCommandUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iCustomCommandUtility) OK"));
			}
		}

	else if (object == KTagMidiClient)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - CMidiClientUtility")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - CMidiClientUtility"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iMidiClientUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iMidiClientUtility) OK"));
			}
		}
	else if (object == KTagDrmPlayer)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - CDrmPlayerUtility")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - CDrmPlayerUtility"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iDrmPlayerUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iDrmPlayerUtility) OK"));
			}
		}
	else if (object == KTagVideoPlayer)
		{
		FTRACE(FPrint(_L("CAudioRoutingTest::CAudioOutput - VideoPlayer")));
		iLog->Log(_L("CAudioRoutingTest::CAudioOutput - VideoPlayer"));
		TRAP(error, iAudioOutput = CAudioOutput::NewL(*iVideoPlayerUtility));
		if (error == KErrNone)
			{
			iLog->Log(_L("CAudioOutput::NewL(*iVideoPlayerUtility) OK"));
			}
		}
	else
		{
		iLog->Log(_L(" No Intut Data"));
		}
	if (error != KErrNone)
	    {
	     iLog->Log(_L("CAudioRoutingTestClass::CreateAudioInput ERROR [%d] for [%s]"),error,&object);
	    }
	return error;
}

void CAudioRoutingTestClass::CreateInput()
   {
   //TInt error = KErrNone;
   isInput = ETrue;
   iUtil = CMdaAudioPlayerUtility::NewFilePlayerL(KMp3TestFile, *this);
   TRAPD(error, iRecorder = CMdaAudioRecorderUtility::NewL(*this));
	if (error == KErrNone)
		{
		iLog->Log(_L("CMdaAudioRecorderUtility::NewL(*this) OK"));
		}
   }

void CAudioRoutingTestClass::CreateOutput()
   {
   isInput = EFalse;
	iUtil = CMdaAudioPlayerUtility::NewFilePlayerL(KMp3TestFile, *this);
   }

TInt CAudioRoutingTestClass::CreateVideoInput()
   {
  // TInt error1 = KErrNone;
   TInt error = KErrNone;
   isInput = ETrue;
   iUtil = CMdaAudioPlayerUtility::NewFilePlayerL(KMp3TestFile,*this);
   if (iUtil)
	   {
	   iRecorder = CMdaAudioRecorderUtility::NewL(*this);
	   if (iRecorder)
		   {
		   iLog->Log(_L("CMdaAudioRecorderUtility::NewL(*this) OK"));
		   }
	   }
	return error;
   }

TInt CAudioRoutingTestClass::CreateVideoWindow()
	{
	TInt error = KErrNone;
	TRAP(error,iWindow = RWindow( iWindowSession ));
	if (error == KErrNone)
		{
		TRAP(error,User::LeaveIfError(iWindow.Construct(iWindowGroup, reinterpret_cast<TUint32>(&iWindowGroup) + 1)));
		if (error == KErrNone)
			{
			iWindow.SetBackgroundColor(TRgb(0x90, 0x90, 0x90));
			iWindow.Activate();
			iWindow.SetExtent(TPoint(0, 0), TSize(screenWidth, screenHeight));
			iWindow.SetVisible(ETrue);
			iWindowGc->Activate(iWindow);
			iRect = TRect(iWindow.Size());
			iWindow.Invalidate(iRect);
			iWindow.BeginRedraw(iRect);
			}
		}
	return error;
	}

TInt CAudioRoutingTestClass::DeleteVideoWindow()
	{
	TInt error = KErrNone;

	TRAP(error,iWindowSession.Close());

	if (iScreenDevice)
		{

		}
	if (iWindowGc)
		{

		}

    return error;
	}

TInt CAudioRoutingTestClass::CreateSessionAndScreen()
	{
	TInt error = KErrNone;
	error = User::LeaveIfError(iWindowSession.Connect());

	if (error == KErrNone)
		{
		TRAP(error,iScreenDevice = new(ELeave) CWsScreenDevice(iWindowSession));
		}

	if (error == KErrNone)
		{
	    TRAP(error, iScreenDevice->Construct());
		}

    return error;
	}

TInt CAudioRoutingTestClass::CreateVideoObject( )
	{
    TInt error = KErrNone;


    TRAP(error, iVideoPlayerUtility = CVideoPlayerUtility::NewL ( *this,
                    EMdaPriorityNormal,
                    EMdaPriorityPreferenceNone,
                    iWindowSession,
                    *iScreenDevice,
                    iWindow,
                    iRect,
                    iRect));
    if (error == KErrNone)
        {
        TRAP(error, iVideoPlayerUtility->OpenFileL(K3gpTestFile));
        //		AddExpectedEvent(EInitializeComplete, 6000);	
        CActiveScheduler::Start();
        }
    if (error == KErrNone)
        {
        EndDrawing();
        iWindow.Close();
        iWindowGroup.Close();
        }
    return error;

    }
TInt CAudioRoutingTestClass::CreateVideoRecorderObject()
    {
    TInt error = KErrNone;
    TRAP(error, iVideoRecord = CVideoRecorderUtility::NewL(*this));

            if (error == KErrNone)
                {
//                const TUid KVidTstControllerUid = {0x101F8503};
                TUid NULLUid ={0};
                
                TRAP(error,iVideoRecord->OpenFileL(K3gpTestFile, NULL, NULLUid, NULLUid));                
                }

    return error;
	}

TInt CAudioRoutingTestClass::CreateWindowGroup()
	{
	TInt error = KErrNone;
	TRAP(error, iWindowGroup = RWindowGroup(iWindowSession));
    if (error == KErrNone)
    	{
    	error = User::LeaveIfError(iWindowGroup.Construct(reinterpret_cast<TUint32>(&iWindowGroup), EFalse));
        if (error == KErrNone)
            {
            TRAP(error, iWindowGroup.SetOrdinalPosition(10, ECoeWinPriorityAlwaysAtFront));
            }
    	}
    else
    	{
		iLog->Log(_L("iWindowGroup = RWindowGroup(iWindowSession) Failed [%d]"),error);
    	}

    User::LeaveIfError(iScreenDevice->CreateContext(iWindowGc));

    return error;
	}

TInt CAudioRoutingTestClass::EndDrawing()
	{
	TInt error = KErrNone;
	TRAP(error,{iWindow.EndRedraw();iWindowGc->Deactivate();iWindowSession.Flush();});
	if (error!=KErrNone)
		{
		iLog->Log(_L("CAudioRoutingTestClass::EndDrawing Failed [%d]"),error);
		}
    return error;
	}

TInt CAudioRoutingTestClass::AudioInput()
   {
   if (iAudioInput)
	   {
		CAudioInput::TAudioInputArray currentInputs = iAudioInput->AudioInput();
		CAudioInput::TAudioInputPreference currentInput = currentInputs[0];
		iLog->Log(_L("AudioInput - currentInput = %d"),currentInput);
	   }
   return KErrNone;
   }

TInt CAudioRoutingTestClass::SetAudioInputL()
	{
	if (iAudioInput)
		{
		RArray<CAudioInput::TAudioInputPreference> inputArray;
		inputArray.Append(CAudioInput::EDefaultMic);
		iAudioInput->SetAudioInputL(inputArray.Array());
		inputArray.Reset();
		}
	iLog->Log(_L("AudioInput - SetAudioInputL "));
	return KErrNone;
	}

TInt CAudioRoutingTestClass::AudioOutput()
   {
   if (iAudioOutput)
		{
		iAudioOutput->AudioOutput();
		}
	iLog->Log(_L("AudioOutput - AudioOutput "));
	return KErrNone;
   }
TInt CAudioRoutingTestClass::DefaultAudioOutput()
   {
   if (iAudioOutput)
	   {
	   iAudioOutput->DefaultAudioOutput();
	   }
	iLog->Log(_L("iAudioOutput - DefaultAudioOutput "));
	return KErrNone;
   }

TInt CAudioRoutingTestClass::RegisterObserverL()
   {
  	if(iAudioOutput)
		{
		iAudioOutput->RegisterObserverL(*this);
		}
	iLog->Log(_L("iAudioOutput - RegisterObserverL "));
	return KErrNone;
   }

TInt CAudioRoutingTestClass::SecureOutput()
   {
   if (iAudioOutput)
	   {
	   TBool secureOutput = iAudioOutput->SecureOutput();
	   iLog->Log(_L("CAudioRoutingTestClass::SecureOutput - SecureOutput = %d"),secureOutput);
	   }
	iLog->Log(_L("iAudioOutput - SecureOutput "));
	return KErrNone;
   }


TInt CAudioRoutingTestClass::SetAudioOutputL()
   {
	if(iAudioOutput)
		{
		iAudioOutput->SetAudioOutputL(CAudioOutput::EPublic);
		}
	iLog->Log(_L("iAudioOutput - SetAudioOutputL "));
	return KErrNone;
   }

TInt CAudioRoutingTestClass::SetSecureOutputL()
   {
        TInt err;
	if(iAudioOutput)
	   {
	    TRAP(err,iAudioOutput->SetSecureOutputL(EFalse));
	   }
	if(err == KErrNotSupported)
	   {
	   iLog->Log(_L("SetsecureoutputL is deprecated,so not supported"));
	   return KErrNone;
	   }
	else
	   {
	   return KErrGeneral;
	   }
	
   }

TInt CAudioRoutingTestClass::UnregisterObserver()
   {
	iLog->Log(_L("iAudioOutput - UnregisterObserver INIT"));
	if(iAudioOutput)
		{
		iAudioOutput->UnregisterObserver(*this);
		}
	iLog->Log(_L("iAudioOutput - UnregisterObserver "));
	return KErrNone;
}
//  End of File
