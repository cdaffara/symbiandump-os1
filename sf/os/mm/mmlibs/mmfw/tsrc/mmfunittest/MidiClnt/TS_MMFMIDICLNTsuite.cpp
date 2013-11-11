
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "TSU_MMFMIDICLNT.h"
#include "TS_MMFMIDICLNTsuite.h"
#include "TestMidiClientUtility.h"
#include "TestStepMidiClntAllocFailNew.h"
#include "TestStepMidiClntAllocFailOpen.h"
#include "TestMidiClientSharedHeap.h"


#define KFileNameKey _S("MidiDrmContent")
#define KUnlimitPlayContent _S("midiIdPlayUnlim")

const TDrmTestParams KDrm0321 =
	{
	KFileNameKey, 		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EUnknown,	//iIntent;
	KErrCANoPermission,			//iExpectedError;	
	TDrmTestParams::EFileName					// iSourceType;
	};
	
const TDrmTestParams KDrm0322 =
	{
	KFileNameKey,				//iFileNameKey
	_S("midiIdNoRights"),		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrCANoRights,			//iExpectedError;	
	TDrmTestParams::EFileName					// iSourceType;
	};
	
const TDrmTestParams KDrm0323 =
	{
	KFileNameKey,		//iFileNameKey
	_S("midiIdPlay01"),		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrCANoPermission,			//iExpectedError;	
	TDrmTestParams::EFileName,	// iSourceType;
	2							// iNumPlayLoops
	};
		
const TDrmTestParams KDrm0324 =
	{
	KFileNameKey,	//iFileNameKey
	_S("Wrong Id"),				//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNotFound,			//iExpectedError;	
	TDrmTestParams::EFileName					// iSourceType;
	};

const TDrmTestParams KDrm1011 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,			//iExpectedError;	
	TDrmTestParams::EFileName					// iSourceType;
	};

const TDrmTestParams KDrm1012 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,			//iExpectedError;	
	TDrmTestParams::EFileHandle					// iSourceType;
	};
	
const TDrmTestParams KDrm1013 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,			//iExpectedError;	
	TDrmTestParams::EFileName					// iSourceType;
	};
	
const TDrmTestParams KDrm1014 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,			//iExpectedError;	
	TDrmTestParams::EFileName,	// iSourceType;
	1,							// iNumPlayLoops
	TDrmTestParams::EAlloc			// iTestMode
	};
	
const TDrmTestParams KDrm1015 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,					//iExpectedError;	
	TDrmTestParams::EFileHandle,// iSourceType;
	1,							// iNumPlayLoops							
	TDrmTestParams::EAlloc		// iTestMode
	};
	
const TDrmTestParams KDrm1016 =
	{
	KFileNameKey,		//iFileNameKey
	KUnlimitPlayContent,		//iUniqueId;
	ContentAccess::EPlay,		//iIntent;
	KErrNone,					//iExpectedError;	
	TDrmTestParams::EFileHandle,// iSourceType;
	1							// iNumPlayLoops							
	};	


/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfMidiClntSuite* result = new (ELeave) CTestMmfMidiClntSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfMidiClntSuite::~CTestMmfMidiClntSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfMidiClntSuite::GetVersion()
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfMidiClntSuite 
 */
void CTestMmfMidiClntSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("MMF_MIDICLNT");

	//TTimeIntervalMicroSeconds position = TInt64(0);

	_LIT(KSectionOne, "SectionOne");
	_LIT(KSectionTwo, "SectionTwo");
	_LIT(KFilename, "filename");

// POSITIVE TEST
	
	//Return the state
	AddTestStepL( CTestMidiClntGetState::NewL( _L("MM-MMF-MIDICLNT-U-0016"), KSectionOne, KFilename, ETestValid, ETrue));

	//Open 'maintest.wav'
	AddTestStepL( CTestMidiClntOpenFile::NewL( _L("MM-MMF-MIDICLNT-U-0018"), KSectionOne, KFilename, ETrue) );
	
	//Open Descriptor
	AddTestStepL( CTestMidiClntOpenDes::NewL( _L("MM-MMF-MIDICLNT-U-0020"), KSectionOne, KFilename, ETrue) );

	//Open valid URL - www.yahoo.co.uk
	AddTestStepL( CTestMidiClntOpenUrl::NewL( _L("MM-MMF-MIDICLNT-U-0023"), KSectionOne, KFilename, ETrue) );

	//Closes any currently open resource and deletes any plugins in use
	AddTestStepL( CTestMidiClntClose::NewL( _L("MM-MMF-MIDICLNT-U-0027")));

	//Initialize or resume playback
	AddTestStepL( CTestMidiClntPlay::NewL( _L("MM-MMF-MIDICLNT-U-0029"),  ETestValid));

	//Stops playback of a resource
	AddTestStepL( CTestMidiClntStop::NewL( _L("MM-MMF-MIDICLNT-U-0035"), ETestValid));

	//Play a single note
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0038"), ETestValid));

	//Stop all notes
	AddTestStepL( CTestMidiClntStopNotes::NewL( _L("MM-MMF-MIDICLNT-U-0047"), ETestValid));

	//Commence playback of a note
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0050"), ETestValid));

	//Terminate playback of a note
	AddTestStepL( CTestMidiClntNoteOff::NewL( _L("MM-MMF-MIDICLNT-U-0056"), ETestValid));

	//Returns the current tempo
	AddTestStepL( CTestMidiClntTempoMicroBeatsPerMinute::NewL( _L("MM-MMF-MIDICLNT-U-0060"), ETestValid));

	//Sets the tempo
	AddTestStepL( CTestMidiClntSetTempo::NewL( _L("MM-MMF-MIDICLNT-U-0062"), ETestValid));

	//Returns the pitch shift
	AddTestStepL( CTestMidiClntGetPitchTranspositionCents::NewL( _L("MM-MMF-MIDICLNT-U-0066"),ETestValid));

	//Set the pitch shift
	AddTestStepL( CTestMidiClntSetPitchTransposition::NewL( _L("MM-MMF-MIDICLNT-U-0072"),ETestValid));

	//Returns the length of midi resource
	AddTestStepL( CTestMidiClntDurationMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0076"), ETestValid));

	//The length of midi resource by Beats
	AddTestStepL( CTestMidiClntDurationMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0078"), ETestValid));

	//Number of tracks
	AddTestStepL( CTestMidiClntNumTracks::NewL( _L("MM-MMF-MIDICLNT-U-0080"), ETestValid));

	//Mutes a particular track
	AddTestStepL( CTestMidiClntSetTrackMute::NewL( _L("MM-MMF-MIDICLNT-U-0082"), ETestValid));

	//Channels Supported
	AddTestStepL( CTestChannelsSupported::NewL( _L("MM-MMF-MIDICLNT-U-0085"), ETestValid));

	//Queries the type
	AddTestStepL( CTestMidiClntMimeType::NewL( _L("MM-MMF-MIDICLNT-U-0087"), ETestValid));

	//Current temporal position
	AddTestStepL( CTestMidiClntReturnPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0092"), ETestValid));
	
	//Change the position
	AddTestStepL( CTestMidiClntSetPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0095"), ETestValid));

	//Current metrical position
	AddTestStepL( CTestMidiClntReturnsPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0099"), ETestValid));

	//Change the metrical position
	AddTestStepL( CTestMidiClntSetPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0102"), ETestValid));

	//Sets the frequency
	AddTestStepL( CTestSetSyncUpdateCallbackInterval::NewL( _L("MM-MMF-MIDICLNT-U-0106"), ETestValid));

	//Sends a midi message 
	AddTestStepL( CTestSendMessage::NewL( _L("MM-MMF-MIDICLNT-U-0111"), ETestValid, _L8("MIDI Message")) );

	//Sends a mip message 
	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0115"), ETestValid) );
	
	//The number of sound banks
	AddTestStepL( CTestNumberOfBanks::NewL( _L("MM-MMF-MIDICLNT-U-0121"),ETestValid));

	//The identifier of a sound bank
	AddTestStepL( CTestGetBankId::NewL( _L("MM-MMF-MIDICLNT-U-0124"),ETestValid));

	//Loads a custom sound bank
	AddTestStepL( CTestLoadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0129"),ETestValid));

	//Removes a custom sound bank
//	AddTestStepL( CTestUnloadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0133"),ETestValid));

	//Unload all custom banks  -- DEFERED (No suitable Midi Controller)
//	AddTestStepL( CTestUnloadAllCustomBanks::NewL( _L("MM-MMF-MIDICLNT-U-0135"),ETestValid));

	//Gets the number of instruments
	AddTestStepL( CTestNumberOfInstruments::NewL( _L("MM-MMF-MIDICLNT-U-0137"),ETestValid));

	//Gets the identifier of a instrument
	AddTestStepL( CTestGetInstrumentId::NewL( _L("MM-MMF-MIDICLNT-U-0141"),ETestValid));

	//Gets the name of a instrument
	AddTestStepL( CTestReturnsInstrumentName::NewL( _L("MM-MMF-MIDICLNT-U-0146"),ETestValid));

	//Sets a channel to use the instrument
	AddTestStepL( CTestSetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0148"),ETestValid));

	//Loads a custom Instrument
	AddTestStepL( CTestLoadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0151"),ETestValid));

	//Removes a instrument
	AddTestStepL( CTestUnloadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0155"),ETestValid ));

	//Queries the name of a particular percussion key
	AddTestStepL( CTestPercussionKeyName::NewL( _L("MM-MMF-MIDICLNT-U-0159"),ETestValid));
	
	//Stop Time
	AddTestStepL( CTestStopTime::NewL( _L("MM-MMF-MIDICLNT-U-0161"),ETestValid));

	//Set Stop Time
	AddTestStepL( CTestSetStopTime::NewL( _L("MM-MMF-MIDICLNT-U-0163"),ETestValid));

	//Set the number of times to repeat
	AddTestStepL( CTestSetRepeats::NewL( _L("MM-MMF-MIDICLNT-U-0168"),ETestValid));

	//Gets the polyphony
	AddTestStepL( CTestPolyphony::NewL( _L("MM-MMF-MIDICLNT-U-0173"),ETestValid));

	//volume setting of a channel
	AddTestStepL( CTestReturnChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0174"), ETestValid));

	//Set the volume of a channel
	AddTestStepL( CTestSetChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0177"), ETestValid));

	//Set the muting state of a channel 
	AddTestStepL( CTestSetChannelMute::NewL( _L("MM-MMF-MIDICLNT-U-0181"), ETestValid));

	//Get the overall volume of the midi client
	AddTestStepL( CTestReturnVolume::NewL( _L("MM-MMF-MIDICLNT-U-0183"), ETestValid));

	//Maximum volume setting
	AddTestStepL( CTestReturnMaxVolume::NewL( _L("MM-MMF-MIDICLNT-U-0184"), ETestValid));

	//Set the overall volume
	AddTestStepL( CTestSetVolume::NewL( _L("MM-MMF-MIDICLNT-U-0185"), ETestValid));

	//Length of time over which the volume is faded up
	AddTestStepL( CTestSetVolumeRamp::NewL( _L("MM-MMF-MIDICLNT-U-0188"), ETestValid));

	//Get the stereo balance value
	AddTestStepL( CTestGetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0191"), ETestValid));

	//Set the stereo balance value
	AddTestStepL( CTestSetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0193"), ETestValid));

	//Set the priority 
	AddTestStepL( CTestSetPriority::NewL( _L("MM-MMF-MIDICLNT-U-0197"), ETestValid));

	//Get the number of meta data entries
	AddTestStepL( CTestNumberOfXmfMetaDataEntries::NewL( _L("MM-MMF-MIDICLNT-U-0199"), ETestValid));

	//Retrieve the specified meta data entry
//	AddTestStepL( CTestGetXmfMetaDataEntry::NewL( _L("MM-MMF-MIDICLNT-U-0201"), ETestValid));

	//Returns the current playback rate
	AddTestStepL( CTestMidiClntReturnPlaybackRate::NewL( _L("MM-MMF-MIDICLNT-U-0210"), ETestValid));

	//Sets the playback rate
	AddTestStepL( CTestMidiClntSetPlaybackRate::NewL( _L("MM-MMF-MIDICLNT-U-0211"), ETestValid));

	//Sets the playback rate [No Resource]	- DEFERRED
	//AddTestStepL( CTestMidiClntSetPlaybackRate::NewL( _L("MM-MMF-MIDICLNT-U-0226"), ETestNoResource));
	
	//Sets the playback rate [Out of range high] - DEFERRED
	//AddTestStepL( CTestMidiClntSetPlaybackRate::NewL( _L("MM-MMF-MIDICLNT-U-0227"), ETestOutOfRangeHigh));

	//Sets the playback rate [Out of range low] - DEFERRED
	//AddTestStepL( CTestMidiClntSetPlaybackRate::NewL( _L("MM-MMF-MIDICLNT-U-0228"), ETestOutOfRangeLow));

	//Query the maximum playback rate
	AddTestStepL( CTestMidiClntReturnMaxPlayRate::NewL( _L("MM-MMF-MIDICLNT-U-0213"), ETestValid));

	//Query the minimum playback rate
	AddTestStepL( CTestMidiClntReturnMinPlayRate::NewL( _L("MM-MMF-MIDICLNT-U-0214"), ETestValid));

	//Plugin required to do negative tests//
	//Query the instrument assigned to a specified channel.
	AddTestStepL( CTestGetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0215"), ETestValid));

	//Loads an individual instrument from descriptor
	AddTestStepL( CTestLoadCustomInstrumentData::NewL( _L("MM-MMF-MIDICLNT-U-0219"), ETestValid));

	// Returns the maximum volume setting
	AddTestStepL( CTestMaxChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0225"), ETestValid));
	
	// Checks that the client can hold more than 10 events in its queue
	AddTestStepL( CTestMidiClntThirteenMidiEvents::NewL( _L("MM-MMF-MIDICLNT-U-0301"), ETestValid));
	
	// Checks that the transtion from EMidiStateClosedDisengaged to EMidiStateClosedEngaged is made without any problems.
	AddTestStepL( CTestTrasitionFromEClosedToEClosedEngaged::NewL( _L("MM-MMF-MIDICLNT-U-0302"), ETestValid));

	// Checks that the transtion from EMidiStateClosedDisengaged to ECOpenEngaged is made without any problems.
	AddTestStepL( CTestTrasitionFromEClosedToEOpenEngaged::NewL( _L("MM-MMF-MIDICLNT-U-0303"), ETestValid));

	// Checks that the transtion from EMidiStateOpenEngaged to EMidiStateClosedEngaged is made without any problems.
	AddTestStepL( CTestTrasitionFromEOpenEngagedToEClosedEngaged::NewL( _L("MM-MMF-MIDICLNT-U-0304"), ETestValid));
	
	// Checks that the transtion from EMidiStateClosedEngaged to EMidiStateClosedDisengaged is made without any problems (using Stop()).
	AddTestStepL( CTestTrasitionFromEClosedEngagedToEClosed::NewL( _L("MM-MMF-MIDICLNT-U-0305"), ETestValid));

	AddTestStepL( CTestMidiOpenFileHandleRepeat::NewL( _L("MM-MMF-MIDICLNT-U-0306"), KSectionOne, KFilename, ETrue) );

// NEGATIVE TEST

	//Return the state before any initialisation has been done
//	AddTestStepL( CTestMidiClntGetState::NewL( _L("MM-MMF-MIDICLNT-U-0017"), KSectionOne, KFilename, ETestNegative, ETrue));

	//Open 'Invalid File Name'
	AddTestStepL( CTestMidiClntOpenFile::NewL( _L("MM-MMF-MIDICLNT-U-0019"), KSectionOne, KFilename, ETrue) );

	//Open NULL Descriptor
	AddTestStepL( CTestMidiClntOpenDes::NewL( _L("MM-MMF-MIDICLNT-U-0021"), KSectionOne, KFilename, ETrue) );

	//Open Invalid Data Descriptor
	AddTestStepL( CTestMidiClntOpenDes::NewL( _L("MM-MMF-MIDICLNT-U-0022"), KSectionOne, KFilename, ETrue) );

	//Open invalid URL - www.yahoo.coo.uuk
	AddTestStepL( CTestMidiClntOpenUrl::NewL( _L("MM-MMF-MIDICLNT-U-0024"), KSectionOne, KFilename, ETrue) );

	//Open invalid IAP - TInt aIapId = KUseInvalidIap
	AddTestStepL( CTestMidiClntOpenUrl::NewL( _L("MM-MMF-MIDICLNT-U-0025"), KSectionOne, KFilename, ETrue) );

	//Open invalid MIME type - TDesC8& = Invalid MIME Type
	AddTestStepL( CTestMidiClntOpenUrl::NewL( _L("MM-MMF-MIDICLNT-U-0026"), KSectionOne, KFilename, ETrue) );

	//Closes any currently open resource and deletes any plugins in use (No plugin available)
	AddTestStepL( CTestMidiClntClose::NewL( _L("MM-MMF-MIDICLNT-U-0028")));

	//Initiate or resume playback (No plugin available)
//	AddTestStepL( CTestMidiClntPlay::NewL( _L("MM-MMF-MIDICLNT-U-0030"),  ETestNoPlugin));
	
	//Initiate or resume playback (Invalid System State)
//	AddTestStepL( CTestMidiClntPlay::NewL( _L("MM-MMF-MIDICLNT-U-0031"), ETestInvalidState));

	// Disabled until real MIDI controller available
	//Stops playback of a resource (No Resource Is Playing)
	//AddTestStepL( CTestMidiClntStop::NewL( _L("MM-MMF-MIDICLNT-U-0036"), ETestNoResource));

	//Stops playback of a resource (No Plugin Available)
	AddTestStepL( CTestMidiClntStop::NewL( _L("MM-MMF-MIDICLNT-U-0037"), ETestNoPlugin));

	//Play a single note (Zero duration)
//	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0039"), ETestZeroDurationOutOfRange));

	//Play a single note (Invalid Channel)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0040"), ETestInvalidChannelOutOfRange));

	//Play a single note (Note out of range high)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0041"), ETestNoteHighOutOfRange));

	//Play a single note (Note out of range low)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0042"), ETestNoteLowOutOfRange));

	//Play a single note (Attack out of range high)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0043"), ETestAttackHighOutOfRange));

	//Play a single note (Attack out of range low)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0044"), ETestAttackLowOutOfRange));

	//Play a single note (Release out of range high)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0045"), ETestReleaseHighOutOfRange));

	//Play a single note (Release out of range low)
	AddTestStepL( CTestMidiClntPlayNote::NewL( _L("MM-MMF-MIDICLNT-U-0046"), ETestReleaseLowOutOfRange));

	//Stop all notes (Invalid Channel Number)
	AddTestStepL( CTestMidiClntStopNotes::NewL( _L("MM-MMF-MIDICLNT-U-0048"), ETestInvalidChannelOutOfRange));

	//Stop all notes (No Note Available)
	AddTestStepL( CTestMidiClntStopNotes::NewL( _L("MM-MMF-MIDICLNT-U-0049"), ETestNoNoteAvailable));

	//Commence playback of a note (Invalid Channel Number)
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0051"), ETestInvalidChannelOutOfRange));

	//Commence playback of a note (Out of Range Low Note)
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0052"), ETestNoteLowOutOfRange));

	//Commence playback of a note (Out of Range High Note)
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0053"), ETestNoteHighOutOfRange));

	//Commence playback of a note (Out of Range Low Velocity)
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0054"), ETestVelocityLowOutOfRange));

	//Commence playback of a note (Out of Range High Velocity)
	AddTestStepL( CTestMidiClntNoteOn::NewL( _L("MM-MMF-MIDICLNT-U-0055"), ETestVelocityHighOutOfRange));

	//Terminate playback of a note (Invalid Channel Number)
	AddTestStepL( CTestMidiClntNoteOff::NewL( _L("MM-MMF-MIDICLNT-U-0057"), ETestInvalidChannelOutOfRange));

	//Terminate playback of a note (No Note Currently Playing)
	AddTestStepL( CTestMidiClntNoteOff::NewL( _L("MM-MMF-MIDICLNT-U-0058"), ETestNoNoteAvailable));

	//Terminate playback of a note (Selected Note / Velocity Set is different to those playing)
//	AddTestStepL( CTestMidiClntNoteOff::NewL( _L("MM-MMF-MIDICLNT-U-0059"), ETestDifferentNoteVelocity));

	//Returns the current tempo (No Resource is Open)
//	AddTestStepL( CTestMidiClntTempoMicroBeatsPerMinute::NewL( _L("MM-MMF-MIDICLNT-U-0061"), ETestNoResource ));

	//Sets the tempo (Beats Out of Range Low)
	AddTestStepL( CTestMidiClntSetTempo::NewL( _L("MM-MMF-MIDICLNT-U-0063"), ETestBeatsLowOutOfRange));

// Does not calculate errors on out of range values, all values above 0 are a pass, Invalid test
	//Sets the tempo (Beats Out of Range High)
//	AddTestStepL( CTestMidiClntSetTempo::NewL( _L("MM-MMF-MIDICLNT-U-0064"), ETestBeatsHighOutOfRange));

	//Sets the tempo (No Resource is Open)
//	AddTestStepL( CTestMidiClntSetTempo::NewL( _L("MM-MMF-MIDICLNT-U-0065"), ETestNoResource));

	//Returns the pitch shift (No Resource is Open)
	//AddTestStepL( CTestMidiClntGetPitchTranspositionCents::NewL( _L("MM-MMF-MIDICLNT-U-0067"),ETestNoResource ));

	//Set the pitch shift (No Resource is Open)
	//AddTestStepL( CTestMidiClntSetPitchTransposition::NewL( _L("MM-MMF-MIDICLNT-U-0073"),ETestNoResource ));

	//Set the pitch shift (Out of Range Value High)
	AddTestStepL( CTestMidiClntSetPitchTransposition::NewL( _L("MM-MMF-MIDICLNT-U-0074"),ETestOutOfRangeHigh ));

	//Set the pitch shift (Out of Range Value Low)
//	AddTestStepL( CTestMidiClntSetPitchTransposition::NewL( _L("MM-MMF-MIDICLNT-U-0075"),ETestOutOfRangeLow ));

	//Returns the length of midi resource (No Resource is Open)
//	AddTestStepL( CTestMidiClntDurationMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0077"), ETestNoResource));

	//The length of midi resource by Beats (No Resource is Open)
//	AddTestStepL( CTestMidiClntDurationMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0079"), ETestNoResource));

	// Disabled until real MIDI controller available
	//Number of tracks (No Resource is Open)
	//AddTestStepL( CTestMidiClntNumTracks::NewL( _L("MM-MMF-MIDICLNT-U-0081"),ETestNoResource));

	//Mutes a particular track (Invalid Track)
	AddTestStepL( CTestMidiClntSetTrackMute::NewL( _L("MM-MMF-MIDICLNT-U-0083"), ETestInvalidTrack));

	//Mutes a particular track (No Resource is Open)
//	AddTestStepL( CTestMidiClntSetTrackMute::NewL( _L("MM-MMF-MIDICLNT-U-0084"), ETestNoResource));

	// Disabled until real MIDI controller available
	//Number of channels (No Resource is Open)
	//AddTestStepL( CTestChannelsSupported::NewL( _L("MM-MMF-MIDICLNT-U-0086"),ETestNoResource));

	//Queries the type (No Resource is Open)
//	AddTestStepL( CTestMidiClntMimeType::NewL( _L("MM-MMF-MIDICLNT-U-0088"), ETestNoResource));

	//Current temporal position (No Resource is Open)
//	AddTestStepL( CTestMidiClntReturnPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0093"), ETestNoResource));

	//Current temporal position (Resource is not Playing)
//	AddTestStepL( CTestMidiClntReturnPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0094"), ETestNoResourcePlaying));

	//Change the position (No Resource is Open)
//	AddTestStepL( CTestMidiClntSetPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0096"), ETestNoResource));

//Taken out as controller does not check values
	//Change the position (Out of Range Value Low)
//	AddTestStepL( CTestMidiClntSetPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0097"), ETestMicrosecondsLowOutOfRange));

//Taken out as controller does not check values
	//Change the position (Out of Range Value High)
//	AddTestStepL( CTestMidiClntSetPositionMicroSeconds::NewL( _L("MM-MMF-MIDICLNT-U-0098"), ETestMicrosecondsHighOutOfRange));

	//Current metrical position (No Resource is Open)
//	AddTestStepL( CTestMidiClntReturnsPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0100"), ETestNoResource));

	//Current metrical position (Resource is not Playing)
//	AddTestStepL( CTestMidiClntReturnsPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0101"), ETestNoResourcePlaying));

	//Change the metrical position (No Resource is Open)
//	AddTestStepL( CTestMidiClntSetPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0103"), ETestNoResource));

//Taken out as controller does not check values
	//Change the metrical position (Out of Range Value Low)
//	AddTestStepL( CTestMidiClntSetPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0104"), ETestMicrobeatsLowOutOfRange));

//Taken out as controller does not check values
	//Change the metrical position (Out of Range Value High)
//	AddTestStepL( CTestMidiClntSetPositionMicroBeats::NewL( _L("MM-MMF-MIDICLNT-U-0105"), ETestMicrobeatsHighOutOfRange ));

	//Sets the frequency (Both Arguments are Set to Zero)
//	AddTestStepL( CTestSetSyncUpdateCallbackInterval::NewL( _L("MM-MMF-MIDICLNT-U-0107"), ETestMicrosecondsMicrobeatsZero));

	//Sets the frequency (Microseconds Value is Out of Range)
	AddTestStepL( CTestSetSyncUpdateCallbackInterval::NewL( _L("MM-MMF-MIDICLNT-U-0108"), ETestMicrosecondsLowOutOfRange));

	//Sets the frequency (Out of Range Interval Low)
	AddTestStepL( CTestSetSyncUpdateCallbackInterval::NewL( _L("MM-MMF-MIDICLNT-U-0109"), ETestMicrobeatsLowOutOfRange));

// Does not calculate errors on out of range values, all values above 0 are a pass, Invalid test
	//Sets the frequency (Out of Range Interval High)
//	AddTestStepL( CTestSetSyncUpdateCallbackInterval::NewL( _L("MM-MMF-MIDICLNT-U-0110"), ETestMicrobeatsHighOutOfRange));

	//Sends a midi message (Call With NULL Message)
//	AddTestStepL( CTestSendMessage::NewL( _L("MM-MMF-MIDICLNT-U-0112"), ETestNullMessage, KNullDesC8) );

	//Sends a midi message (Call With Unsupported Message)
//	AddTestStepL( CTestSendMessage::NewL( _L("MM-MMF-MIDICLNT-U-0113"), ETestUnsupported, _L8("Unsupported MIDI Message")) );

	//Sends a midi message (Call With Corrupt Message)
//	AddTestStepL( CTestSendMessage::NewL( _L("MM-MMF-MIDICLNT-U-0114"), ETestCorrupt, _L8("Corrupt MIDI Message")) );

	//Sends a mip message (Invalid Device ID)
//	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0116"), ETestInvalidId ));

	//Sends a mip message  (NULL Message)
//	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0117"), ETestNullMessage) );

	//Sends a mip message (Unsupported Message)
//	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0118"), ETestUnsupported) );

	// Duplicate requirement
	//Sends a mip message (Unsupported Message)
//	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0119"), ETestUnsupported ));

	//Sends a mip message (Corrupt Message)
//	AddTestStepL( CTestSendMipMessage::NewL( _L("MM-MMF-MIDICLNT-U-0120"), ETestCorrupt) );

	//The number of sound banks (No Custom banks Exist)
//	AddTestStepL( CTestNumberOfBanks::NewL( _L("MM-MMF-MIDICLNT-U-0122"), ETestNoCustomBanks) );

	//The number of sound banks (No Standard bank Exist)
//	AddTestStepL( CTestNumberOfBanks::NewL( _L("MM-MMF-MIDICLNT-U-0123"), ETestNoStandardBanks ));

	//The identifier of a sound bank (Invalid Bank Index)
//	AddTestStepL( CTestGetBankId::NewL( _L("MM-MMF-MIDICLNT-U-0125"), ETestInvalidIndex) );

	//The identifier of a sound bank (No Custom Banks Exist)
//	AddTestStepL( CTestGetBankId::NewL( _L("MM-MMF-MIDICLNT-U-0126"), ETestNoCustomBanks) );

	//The identifier of a sound bank  (No Standard Banks Exist)
//	AddTestStepL( CTestGetBankId::NewL( _L("MM-MMF-MIDICLNT-U-0127"), ETestNoStandardBanks ));

	//Loads a custom sound bank (Null Filename)
//	AddTestStepL( CTestLoadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0130"), ETestNullMessage) );

	//Loads a custom sound bank (lnvalid Filename)
//	AddTestStepL( CTestLoadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0131"), ETestInvalidMessage) );

	//Loads a custom sound bank (Unsupported File)
//	AddTestStepL( CTestLoadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0132"), ETestUnsupported) );

	//Removes a custom sound bank (ID of Bank Not Loaded)
//	AddTestStepL( CTestUnloadCustomBank::NewL( _L("MM-MMF-MIDICLNT-U-0134"), ETestNotLoaded ));

	//Gets the number of instruments  (Invalid Bank Index)
//	AddTestStepL( CTestNumberOfInstruments::NewL( _L("MM-MMF-MIDICLNT-U-0138"), ETestInvalidIndex ));

	//Gets the number of instruments  (No Custome Banks Exist)
//	AddTestStepL( CTestNumberOfInstruments::NewL( _L("MM-MMF-MIDICLNT-U-0139"), ETestNoCustomBanks ));

	//Gets the number of instruments  (No Standard Banks Exist)
//	AddTestStepL( CTestNumberOfInstruments::NewL( _L("MM-MMF-MIDICLNT-U-0140"), ETestNoStandardBanks ));

	//Gets the identifier of a instrument  (Invalid Bank ID)
//	AddTestStepL( CTestGetInstrumentId::NewL( _L("MM-MMF-MIDICLNT-U-0142"), ETestInvalidId ));

	//Gets the identifier of a instrument (No Custome Banks Exist)
//	AddTestStepL( CTestGetInstrumentId::NewL( _L("MM-MMF-MIDICLNT-U-0143"), ETestNoCustomBanks ));

	//Gets the identifier of a instrument (No Standard Banks Exist)
//	AddTestStepL( CTestGetInstrumentId::NewL( _L("MM-MMF-MIDICLNT-U-0144"), ETestNoStandardBanks ));

	//Gets the identifier of a instrument (Invalid Program/Instrument Index)
//	AddTestStepL( CTestGetInstrumentId::NewL( _L("MM-MMF-MIDICLNT-U-0145"), ETestInvalidIndex ));

//Check this//
	//Change it to return EFail for failing negative test
	//Gets the name of a instrument (Invalid Program/Instrument Information)
	AddTestStepL( CTestReturnsInstrumentName::NewL( _L("MM-MMF-MIDICLNT-U-0147"), ETestNegative ));
//

	//Sets a channel to use the instrument  (Invalid Channel)
//	AddTestStepL( CTestSetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0149"), ETestInvalidChannel ));

	//Sets a channel to use the instrument (Invalid Program/Instrument ID)
//	AddTestStepL( CTestSetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0150"), ETestInvalidId ));

	//Loads a custom Instrument (Invalid Bank ID)
//	AddTestStepL( CTestLoadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0152"), ETestInvalidId ));

	//Loads a custom Instrument (Load Program/Instrument Already Loaded)
//	AddTestStepL( CTestLoadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0153"), ETestAlreadyLoaded ));

	//Loads a custom Instrument (Load Unsupported Program/Instrument)
//	AddTestStepL( CTestLoadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0154"), ETestUnsupported ));

	//Removes a instrument (Invalid Bank ID)
//	AddTestStepL( CTestUnloadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0156"), ETestInvalidId ));

	//Removes a instrument (ID of Program not Loaded)
//	AddTestStepL( CTestUnloadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0157"), ETestNotLoaded ));

	//Removes a instrument (Non-unloadable Program/Instrument)
//	AddTestStepL( CTestUnloadCustomInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0158"), ETestNotUnloadable ));

	//Queries the name of a particular percussion key (Invalid Note)
//	AddTestStepL( CTestPercussionKeyName::NewL( _L("MM-MMF-MIDICLNT-U-0160"), ETestInvalidId ));

	// Disabled until real MIDI controller available
	//Stop Time (No Midi Resource is Open)
	//AddTestStepL( CTestStopTime::NewL( _L("MM-MMF-MIDICLNT-U-0162"),ETestNoResource));

	//Set Stop Time (Stop time out of range high)
	AddTestStepL( CTestSetStopTime::NewL( _L("MM-MMF-MIDICLNT-U-0164"),ETestOutOfRangeHigh));

	//Set Stop Time (Stop time out of range low)
	AddTestStepL( CTestSetStopTime::NewL( _L("MM-MMF-MIDICLNT-U-0165"),ETestOutOfRangeLow));

	//Set the number of times to repeat (Repeat: Out of Range High, Trailing Silence: Out of Range High)
	AddTestStepL( CTestSetRepeats::NewL( _L("MM-MMF-MIDICLNT-U-0169"), ETestOutOfRange ));

	//It gave no error even it is Channel16. So it should be because no plugin.
	//volume setting of a channel (Invalid Channel)  -- DEFERED (No suitable Midi Controller)
	AddTestStepL( CTestReturnChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0175"), ETestInvalidChannel ));

	//Set the volume of a channel (Invalid Channel Number)
	AddTestStepL( CTestSetChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0178"), ETestInvalidChannelOutOfRange));

	//Set the volume of a channel (Volume out of Range High)
	AddTestStepL( CTestSetChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0179"), ETestVolumeHighOutOfRange));

	//Set the volume of a channel (Volume out of Range Low)
	AddTestStepL( CTestSetChannelVolume::NewL( _L("MM-MMF-MIDICLNT-U-0180"), ETestVolumeLowOutOfRange));

//Check this
//Is the error message 'NotSupported' corect??
	//Set the muting state of a channel (Invalid Channel)
	AddTestStepL( CTestSetChannelMute::NewL( _L("MM-MMF-MIDICLNT-U-0182"), ETestInvalidChannelOutOfRange));
//

//Taken out as controller does not check values
	//Set the overall volume (Out of Range Volume Low) 
//	AddTestStepL( CTestSetVolume::NewL( _L("MM-MMF-MIDICLNT-U-0186"), ETestVolumeLowOutOfRange));

//Taken out as controller does not check values
	//Set the overall volume (Out of Range Volume High) 
//	AddTestStepL( CTestSetVolume::NewL( _L("MM-MMF-MIDICLNT-U-0187"), ETestVolumeHighOutOfRange));

//Taken out as controller does not check values
	//Length of time over which the volume is faded up (Out of Range Volume Low) 
//	AddTestStepL( CTestSetVolumeRamp::NewL( _L("MM-MMF-MIDICLNT-U-0189"), ETestRampDurationLowOutOfRange));

//Taken out as controller does not check values
	//Length of time over which the volume is faded up (Out of Range Volume High) 
//	AddTestStepL( CTestSetVolumeRamp::NewL( _L("MM-MMF-MIDICLNT-U-0190"), ETestRampDurationHighOutOfRange));

	//Get the stereo balance value (Plugin Does Not Support Balance)
//	AddTestStepL( CTestGetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0192"), ETestUnsupported));

	//Set the stereo balance value  (Plugin Does Not Support Balance)
//	AddTestStepL( CTestSetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0194"), ETestUnsupported));

//Taken out as controller does not check values
	//Set the stereo balance value (Out of Range Volume Low)
//	AddTestStepL( CTestSetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0195"), ETestBalanceLowOutOfRange));

//Taken out as controller does not check values
	//Set the stereo balance value (Out of Range Volume High)
//	AddTestStepL( CTestSetBalance::NewL( _L("MM-MMF-MIDICLNT-U-0196"), ETestBalanceHighOutOfRange));

//  We need this to confict and cause PreferenceConflictsCannotBeResolved
	//Set the priority (Preference Conflicts Cannot be Resolved)  -- DEFERED (Need another Midi Client)
//	AddTestStepL( CTestSetPriority::NewL( _L("MM-MMF-MIDICLNT-U-0198"), ETestPreferenceConflictsCannotBeResolved));

// Taken out we need plugin
	//Get the number of meta data entries  (Plugin Does Not Support Metadata)
//	AddTestStepL( CTestNumberOfXmfMetaDataEntries::NewL( _L("MM-MMF-MIDICLNT-U-0200"), ETestUnsupported));

//Causes Access Violation
	//Retrieve the specified meta data entry (Invalid Index)
//	AddTestStepL( CTestGetXmfMetaDataEntry::NewL( _L("MM-MMF-MIDICLNT-U-0202"), ETestMetaDataIndexInvalid));

//Causes Access Violation
	//Retrieve the specified meta data entry (Plugin Does Not Support Metadata)
//	AddTestStepL( CTestGetXmfMetaDataEntry::NewL( _L("MM-MMF-MIDICLNT-U-0203"), ETestUnsupported));
	
	//Query the instrument (Invalid channel) - DEFERRED
	//AddTestStepL( CTestGetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0216"), ETestInvalidChannel));

	//Query the instrument (Invalid Id) - DEFERRED
	//AddTestStepL( CTestGetInstrument::NewL( _L("MM-MMF-MIDICLNT-U-0217"), ETestInvalidId));

	//Loads an individual instrument from descriptor (Invalid ID) - DEFERRED
	//AddTestStepL( CTestLoadCustomInstrumentData::NewL( _L("MM-MMF-MIDICLNT-U-0220"), ETestInvalidId));

	//Loads an individual instrument from descriptor (Already loaded) - DEFERRED
	//AddTestStepL( CTestLoadCustomInstrumentData::NewL( _L("MM-MMF-MIDICLNT-U-0221"), ETestAlreadyLoaded));

	//Loads an individual instrument from descriptor (Unsupported / Bad instrument data) - DEFERRED
	//AddTestStepL( CTestLoadCustomInstrumentData::NewL( _L("MM-MMF-MIDICLNT-U-0222"), ETestUnsupported));
	
	AddTestStepL( CTestCheckInterface::NewL( _L("MM-MMF-MIDICLNT-U-0300"), ETestValid));

	AddTestStepL(CTestStepMidiClntAllocFailNew::NewL());
	AddTestStepL(CTestStepMidiClntAllocFailOpenFile::NewL(_L("MM-MMF-MIDICLNT-U-0200-CP")));
	AddTestStepL(CTestStepMidiClntAllocFailOpenDes::NewL());
	AddTestStepL(CTestStepMidiClntAllocFailOpenUrl::NewL());

	//MS 3.4 File Handles API
	//NEGATIVE TESTS
	//Open a MIDI File containing text data and try to perform initialisation ready for playback - (Invalid File).
	AddTestStepL( CTestMidiClntOpenFileByHandle::NewL( _L("MM-MMF-MIDICLNT-U-0223"), KSectionTwo, KFilename, ETrue, ETestValid) );
	
	//POSITIVE TESTS
	//Open a file containing midi data and perform initialisation ready for playback.
	AddTestStepL( CTestMidiClntOpenFileByHandle::NewL( _L("MM-MMF-MIDICLNT-U-1001"), KSectionOne, KFilename, ETrue, ETestValid) );
	//Initiate or resume playback of a previously opened resource.
	AddTestStepL( CTestMidiClntPlayFileByHandle::NewL( _L("MM-MMF-MIDICLNT-U-1002")));
	
	//ALLOC TESTS
	//Alloc test to Open a file containing midi data and perform initialisation ready for playback.
	AddTestStepL(CTestStepMidiClntAllocFailOpenFile::NewL(_L("MM-MMF-MIDICLNT-U-1003")));
	
	AddTestStepL(CTestDrmNegative::NewL(_L("MM-MMF-MIDICLNT-U-0321-HP"),ETestNegative ,KDrm0321));
	AddTestStepL(CTestDrmNegative::NewL(_L("MM-MMF-MIDICLNT-U-0322-HP"),ETestNegative ,KDrm0322));
	AddTestStepL(CTestDrmPlay::NewL(_L("MM-MMF-MIDICLNT-U-0323-HP"),ETestValid ,KDrm0323));
	AddTestStepL(CTestDrmNegative::NewL(_L("MM-MMF-MIDICLNT-U-0324-HP"),ETestNegative ,KDrm0324));
	AddTestStepL(CTestDrmPlay::NewL(_L("MM-MMF-MIDICLNT-U-1011-HP"),ETestValid ,KDrm1011));
	AddTestStepL(CTestDrmPlay::NewL(_L("MM-MMF-MIDICLNT-U-1012-HP"),ETestValid ,KDrm1012));
	AddTestStepL(CTestDrmPlayStop::NewL(_L("MM-MMF-MIDICLNT-U-1013-HP"),ETestValid ,KDrm1013));
	AddTestStepL(CTestDrmPlay::NewL(_L("MM-MMF-MIDICLNT-U-1014-HP"),ETestValid ,KDrm1014));
	AddTestStepL(CTestDrmPlay::NewL(_L("MM-MMF-MIDICLNT-U-1015-HP"),ETestValid ,KDrm1015));
	AddTestStepL(CTestDrmPlayWithUI::NewL(_L("MM-MMF-MIDICLNT-U-1016-HP"),ETestValid ,KDrm1016));
	
	//Play a midifile without opening it and check the status while playing and after it is been  stopped.
	AddTestStepL(CTestMidiClntPlayWithoutFile::NewL(_L("MM-MMF-MIDICLNT-U-1020-HP")));
	//Open and Play a midifile and check the status while playing and after it is been  stopped.
	AddTestStepL(CTestMidiClntOpenAndPlayFile::NewL(_L("MM-MMF-MIDICLNT-U-1021-HP"),KSectionOne, KFilename));
	
	//Open and play multiple concurrent midi objects, using shared heap mode, to check we don't run out of heaps
	AddTestStepL(CTestMidiClntMultiSharedHeap::NewL(_L("MM-MMF-MIDICLNT-U-2000-HP"),KSectionOne, KFilename, EFalse));
	AddTestStepL(CTestMidiClntMultiSharedHeap::NewL(_L("MM-MMF-MIDICLNT-U-2001-HP"),KSectionOne, KFilename, ETrue));
	
	// repeatedly create and delete multiple concurrent midi objects, using shared heap mode, to check no chunks are leaked
	AddTestStepL(CTestMidiClntRepeatMultiSharedHeap::NewL(_L("MM-MMF-MIDICLNT-U-2002-HP"),KSectionOne, KFilename, EFalse));
	AddTestStepL(CTestMidiClntRepeatMultiSharedHeap::NewL(_L("MM-MMF-MIDICLNT-U-2003-HP"),KSectionOne, KFilename, ETrue));
}


