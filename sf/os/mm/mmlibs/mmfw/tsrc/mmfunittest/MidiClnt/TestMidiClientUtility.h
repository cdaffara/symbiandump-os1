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
// Header file: Basic  tests.
// 
//

/**
 @file TESTMIDICLIENTUTILITY.h
*/

#ifndef TESTMIDICLIENTUTILITY_H__
#define TESTMIDICLIENTUTILITY_H__

#include <caf/caf.h>
#include "TSU_MMFMIDICLNT.h"

_LIT(KMidiFileName,"C:\\MidiClntITestData\\midi.mid");

enum TMmcuoCallbacks
{
	EMmcuoTempoChanged,
	EMmcuoVolumeChanged,
	EMmcuoMuteChanged,
	EMmcuoPolyphonyChanged,
	EMmcuoInstrumentChanged,
	EMmcuoSyncUpdate,
	EMmcuoMetaDataEntryFound,
	EMmcuoMipMessageReceived,
	EMmcuoInvalid
};


/**
 * Load and initialise a midi file.
 *
 * @class CTestMidiClntOpenFile
 *
 * Req. under test 
 */
class CTestMidiClntOpenFile : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntOpenFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	~CTestMidiClntOpenFile() {};
	static CTestMidiClntOpenFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	// From Base Class
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	const TBool iPlay;						// Does this test require playing the Audio file 
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file

	};

/**
 * Load and initialise a midi file.
 *
 * @class CTestMidiClntOpenDes
 *
 * Req. under test 
 */
class CTestMidiClntOpenDes : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntOpenDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	~CTestMidiClntOpenDes() {};
	static CTestMidiClntOpenDes* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	// From Base Class
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	const TBool iPlay;						// Does this test require playing the Audio file 
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file
	HBufC8* iAudio;							// Buffer for holding audio descriptor data
	};

/**
 * Load and initialise a midi file.
 *
 * @class CTestMidiClntOpenUrl
 *
 * Req. under test 
 */
class CTestMidiClntOpenUrl : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntOpenUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	~CTestMidiClntOpenUrl() {};
	static CTestMidiClntOpenUrl* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	// From Base Class
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	const TBool iPlay;						// Does this test require playing the Audio file 
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file
	};

/**
 * Close a midi file.
 *
 * @class CTestMidiClntClose
 *
 * Req. under test 
 */
class CTestMidiClntClose : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntClose* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntClose(const TDesC& aTestName);
	~CTestMidiClntClose() {};
	};

/**
 * Play a midi file.
 *
 * @class CTestMidiClntPlay
 *
 * Req. under test 
 */
class CTestMidiClntPlay : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntPlay* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntPlay(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntPlay() {};
	};

/**
 * Stop a midi file.
 *
 * @class CTestMidiClntStop
 *
 * Req. under test 
 */
class CTestMidiClntStop : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntStop* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntStop(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntStop() {};
	};

/**
 * Returns the state of the midi device.
 *
 * @class CTestMidiClntGetState
 *
 * Req. under test 
 */
class CTestMidiClntGetState : public CTestMidiClntOpenFile // ** N.B.
	{
public:
	static CTestMidiClntGetState* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,
									   const TTestStepType aTestType, const TBool aPlay);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntGetState(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,
						  const TTestStepType aTestType, const TBool aPlay);
	~CTestMidiClntGetState() {};
	};

/**
 * Play midi note.
 *
 * @class CTestMidiClntPlayNote
 *
 * Req. under test 
 */
class CTestMidiClntPlayNote : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntPlayNote* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntPlayNote(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntPlayNote() {};
	};

/**
 * Play midi note with start time.
 *
 * @class CTestMidiClntPlayNoteStartTime
 *
 * Req. under test 
 */
class CTestMidiClntPlayNoteStartTime : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntPlayNoteStartTime* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntPlayNoteStartTime(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntPlayNoteStartTime() {};
	};

/**
 * Stop midi note.
 *
 * @class CTestMidiClntStopNotes
 *
 * Req. under test 
 */
class CTestMidiClntStopNotes : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntStopNotes* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntStopNotes(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntStopNotes() {};
	};

/**
 * Midi note on.
 *
 * @class CTestMidiClntNoteOn
 *
 * Req. under test 
 */
class CTestMidiClntNoteOn : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntNoteOn* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntNoteOn(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntNoteOn() {};
	};

/**
 * Midi note off.
 *
 * @class CTestMidiClntNoteOff
 *
 * Req. under test 
 */
class CTestMidiClntNoteOff : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntNoteOff* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntNoteOff(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntNoteOff() {};
	};

/**
 * Returns play back rate of the midi device.
 *
 * @class CTestMidiClntReturnPlaybackRate
 *
 * Req. under test 
 */
class CTestMidiClntReturnPlaybackRate : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntReturnPlaybackRate* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntReturnPlaybackRate(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntReturnPlaybackRate() {};
	};

/**
 * Sets the play back rate of the midi device.
 *
 * @class CTestMidiClntSetPlaybackRate
 *
 * Req. under test 
 */
class CTestMidiClntSetPlaybackRate : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetPlaybackRate* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetPlaybackRate(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetPlaybackRate() {};
	};

/**
 * Returns the max play rate of the midi device.
 *
 * @class CTestMidiClntReturnMaxPlayRate
 *
 * Req. under test 
 */
class CTestMidiClntReturnMaxPlayRate : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntReturnMaxPlayRate* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntReturnMaxPlayRate(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntReturnMaxPlayRate() {};
	};

/**
 * Returns the min play rate of the midi device.
 *
 * @class CTestMidiClntReturnMinPlayRate
 *
 * Req. under test 
 */
class CTestMidiClntReturnMinPlayRate : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntReturnMinPlayRate* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntReturnMinPlayRate(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntReturnMinPlayRate() {};
	};

/**
 * Sets the micro bpm (beats per minute)
 *
 * @class CTestMidiClntTempoMicroBeatsPerMinute
 *
 * Req. under test 
 */
class CTestMidiClntTempoMicroBeatsPerMinute : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntTempoMicroBeatsPerMinute* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntTempoMicroBeatsPerMinute(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntTempoMicroBeatsPerMinute() {};
	};

/**
 * Sets the tempo micro bpm (beats per minute)
 *
 * @class CTestMidiClntSetTempo
 *
 * Req. under test 
 */
class CTestMidiClntSetTempo : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetTempo* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetTempo(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetTempo() {};
	};

/**
 * Get Pitch Transposition in Cents
 *
 * @class CTestMidiClntGetPitchTranspositionCents
 *
 * Req. under test 
 */
class CTestMidiClntGetPitchTranspositionCents : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntGetPitchTranspositionCents* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntGetPitchTranspositionCents(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntGetPitchTranspositionCents() {};
private:
	TInt iTestType;
	};

/**
 * Set Pitch Transposition in Cents
 *
 * @class CTestMidiClntSetPitchTransposition
 *
 * Req. under test 
 */
class CTestMidiClntSetPitchTransposition : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetPitchTransposition* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetPitchTransposition(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetPitchTransposition() {};
	};

/**
 * Return duration in micro seconds
 *
 * @class CTestMidiClntDurationMicroSeconds
 *
 * Req. under test 
 */
class CTestMidiClntDurationMicroSeconds : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntDurationMicroSeconds* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntDurationMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntDurationMicroSeconds() {};
	};

/**
 * Return duration in micro beats
 *
 * @class CTestMidiClntDurationMicroBeats
 *
 * Req. under test 
 */
class CTestMidiClntDurationMicroBeats : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntDurationMicroBeats* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntDurationMicroBeats(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntDurationMicroBeats() {};
	};

/**
 * Return number of tracks
 *
 * @class CTestMidiClntNumTracks
 *
 * Req. under test 
 */
class CTestMidiClntNumTracks : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntNumTracks* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntNumTracks(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntNumTracks() {};
	};

/**
 * Mute track
 *
 * @class CTestMidiClntSetTrackMute
 *
 * Req. under test 
 */
class CTestMidiClntSetTrackMute : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetTrackMute* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetTrackMute(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetTrackMute() {};
	};

/**
 * Returns MimeType
 *
 * @class CTestMidiClntMimeType
 *
 * Req. under test 
 */
class CTestMidiClntMimeType : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntMimeType* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntMimeType(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntMimeType() {};
	};

/**
 * Returns MimeTypes Supported
 *
 * @class CTestMidiClntGetMimeTypesSupported
 *
 * Req. under test 
 */
class CTestMidiClntGetMimeTypesSupported : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntGetMimeTypesSupported* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntGetMimeTypesSupported(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntGetMimeTypesSupported() {};
	};

/**
 * Sets position in micro seconds
 *
 * @class CTestMidiClntPositionMicroSeconds
 *
 * Req. under test 
 */
class CTestMidiClntPositionMicroSeconds : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntPositionMicroSeconds* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntPositionMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntPositionMicroSeconds() {};
	};

/**
 * Returns position in micro seconds
 *
 * @class CTestMidiClntReturnPositionMicroSeconds
 *
 * Req. under test 
 */
class CTestMidiClntReturnPositionMicroSeconds : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntReturnPositionMicroSeconds* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntReturnPositionMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntReturnPositionMicroSeconds() {};
	};

/**
 * Sets position in micro seconds
 *
 * @class CTestMidiClntSetPositionMicroSeconds
 *
 * Req. under test 
 */
class CTestMidiClntSetPositionMicroSeconds : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetPositionMicroSeconds* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetPositionMicroSeconds(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetPositionMicroSeconds() {};
	};

/**
 * Returns position in micro beats
 *
 * @class CTestMidiClntReturnsPositionMicroBeats
 *
 * Req. under test 
 */
class CTestMidiClntReturnsPositionMicroBeats : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntReturnsPositionMicroBeats* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntReturnsPositionMicroBeats(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntReturnsPositionMicroBeats() {};
	};

/**
 * Sets position in micro beats
 *
 * @class CTestMidiClntSetPositionMicroBeats
 *
 * Req. under test 
 */
class CTestMidiClntSetPositionMicroBeats : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntSetPositionMicroBeats* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntSetPositionMicroBeats(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntSetPositionMicroBeats() {};
	};

/**
 * Sets the sync update call back interval
 *
 * @class CTestSetSyncUpdateCallbackInterval
 *
 * Req. under test 
 */
class CTestSetSyncUpdateCallbackInterval : public CTestMmfMidiClntStep
	{
public:
	static CTestSetSyncUpdateCallbackInterval* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetSyncUpdateCallbackInterval(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetSyncUpdateCallbackInterval() {};
	};

// -----------------------------------------------------------------------------------------------

/**
 * Send a midi message
 *
 * @class CTestSendMessage
 *
 * Req. under test 
 */
class CTestSendMessage : public CTestMmfMidiClntStep
	{
public:
	static CTestSendMessage* NewL(const TDesC& aTestName,
								  const TTestStepType aTestType, const TDesC8& aMessage);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSendMessage(const TDesC& aTestName,
					 const TTestStepType aTestType, const TDesC8& aMessage);
	~CTestSendMessage() {};

private:
	TBuf8<KNameBufSize> iMessage;
	};

// class CTestSendMessageTime incorporated in CTestSendMessage

/**
 * Send a Mip (Maximum Instantenous Polyphony) message
 *
 * @class CTestSendMipMessage
 *
 * Req. under test 
 */
class CTestSendMipMessage : public CTestMmfMidiClntStep
	{
public:
	static CTestSendMipMessage* NewL(const TDesC& aTestName,
								  const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSendMipMessage(const TDesC& aTestName,
					 const TTestStepType aTestType);
	~CTestSendMipMessage() {};

	};

/**
 * Number of banks
 *
 * @class CTestNumberOfBanks
 *
 * Req. under test 
 */
class CTestNumberOfBanks : public CTestMmfMidiClntStep
	{
public:
	static CTestNumberOfBanks* NewL(const TDesC& aTestName,
								  const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestNumberOfBanks(const TDesC& aTestName,
								  const TTestStepType aTestType);
	~CTestNumberOfBanks() {};
	};

/**
 * Return bank id
 *
 * @class CTestGetBankId
 *
 * Req. under test 
 */
class CTestGetBankId : public CTestMmfMidiClntStep
	{
public:
	static CTestGetBankId* NewL(const TDesC& aTestName,
								  const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestGetBankId(const TDesC& aTestName,
								  const TTestStepType aTestType);
	~CTestGetBankId() {};
	};

/**
 * Load custom bank
 *
 * @class CTestLoadCustomBank
 *
 * Req. under test 
 */
class CTestLoadCustomBank : public CTestMmfMidiClntStep
	{
public:
	static CTestLoadCustomBank* NewL(const TDesC& aTestName,
								  const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestLoadCustomBank(const TDesC& aTestName,
								  const TTestStepType aTestType);
	~CTestLoadCustomBank() {};
	};

/**
 * Unload custom bank
 *
 * @class CTestUnloadCustomBank
 *
 * Req. under test 
 */
class CTestUnloadCustomBank : public CTestMmfMidiClntStep
	{
public:
	static CTestUnloadCustomBank* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestUnloadCustomBank(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestUnloadCustomBank() {};
	};

/**
 * Unload all custom banks
 *
 * @class CTestUnloadAllCustomBanks
 *
 * Req. under test 
 */
class CTestUnloadAllCustomBanks : public CTestMmfMidiClntStep
	{
public:
	static CTestUnloadAllCustomBanks* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestUnloadAllCustomBanks(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestUnloadAllCustomBanks() {};
	};

/**
 * Return the number of instruments
 *
 * @class CTestNumberOfInstruments
 *
 * Req. under test 
 */
class CTestNumberOfInstruments : public CTestMmfMidiClntStep
	{
public:
	static CTestNumberOfInstruments* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestNumberOfInstruments(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestNumberOfInstruments() {};
	};

/**
 * Returns instrument id
 *
 * @class CTestGetInstrumentId
 *
 * Req. under test 
 */
class CTestGetInstrumentId : public CTestMmfMidiClntStep
	{
public:
	static CTestGetInstrumentId* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestGetInstrumentId(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestGetInstrumentId() {};
	};

/**
 * Returns instrument name
 *
 * @class CTestReturnsInstrumentName
 *
 * Req. under test 
 */
class CTestReturnsInstrumentName : public CTestMmfMidiClntStep
	{
public:
	static CTestReturnsInstrumentName* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestReturnsInstrumentName(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestReturnsInstrumentName() {};
	};

/**
 * Returns instrument
 *
 * @class CTestGetInstrument
 *
 * Req. under test 
 */
class CTestGetInstrument : public CTestMmfMidiClntStep
	{
public:
	static CTestGetInstrument* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestGetInstrument(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestGetInstrument() {};
	};

/**
 * Set instrument
 *
 * @class CTestSetInstrument
 *
 * Req. under test 
 */
class CTestSetInstrument : public CTestMmfMidiClntStep
	{
public:
	static CTestSetInstrument* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetInstrument(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetInstrument() {};
	};

/**
 * Load custom instrument
 *
 * @class CTestLoadCustomInstrument
 *
 * Req. under test 
 */
class CTestLoadCustomInstrument : public CTestMmfMidiClntStep
	{
public:
	static CTestLoadCustomInstrument* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestLoadCustomInstrument(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestLoadCustomInstrument() {};
	};


/**
 * Load custom instrument data
 *
 * @class CTestLoadCustomInstrumentData
 *
 * Req. under test 
 */
class CTestLoadCustomInstrumentData : public CTestMmfMidiClntStep
	{
public:
	static CTestLoadCustomInstrumentData* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestLoadCustomInstrumentData(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestLoadCustomInstrumentData() {};
	};


/**
 * Unload custom instrument
 *
 * @class CTestUnloadCustomInstrument
 *
 * Req. under test 
 */
class CTestUnloadCustomInstrument : public CTestMmfMidiClntStep
	{
public:
	static CTestUnloadCustomInstrument* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestUnloadCustomInstrument(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestUnloadCustomInstrument() {};
	};

/**
 * Get percussion key name
 *
 * @class CTestPercussionKeyName
 *
 * Req. under test 
 */
class CTestPercussionKeyName : public CTestMmfMidiClntStep
	{
public:
	static CTestPercussionKeyName* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestPercussionKeyName(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestPercussionKeyName() {};
	};

/**
 * Stop time
 *
 * @class CTestStopTime
 *
 * Req. under test 
 */
class CTestStopTime : public CTestMmfMidiClntStep
	{
public:
	static CTestStopTime* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestStopTime(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestStopTime() {};
	};

/**
 * Set stop time
 *
 * @class CTestSetStopTime
 *
 * Req. under test 
 */
class CTestSetStopTime : public CTestMmfMidiClntStep
	{
public:
	static CTestSetStopTime* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetStopTime(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetStopTime() {};
	};

/**
 * Set the number of repeats
 *
 * @class CTestSetRepeats
 *
 * Req. under test 
 */
class CTestSetRepeats : public CTestMmfMidiClntStep
	{
public:
	static CTestSetRepeats* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetRepeats(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetRepeats() {};
	};

/**
 * Returns the number of simultaneous voices a synth is able to play (polyphony)
 *
 * @class CTestPolyphony
 *
 * Req. under test 
 */
class CTestPolyphony : public CTestMmfMidiClntStep
	{
public:
	static CTestPolyphony* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestPolyphony(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestPolyphony() {};
	};

/**
 * Returns channels supported
 *
 * @class CTestChannelsSupported
 *
 * Req. under test 
 */
class CTestChannelsSupported : public CTestMmfMidiClntStep
	{
public:
	static CTestChannelsSupported* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestChannelsSupported(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestChannelsSupported() {};
	};

/**
 * Returns channel volume
 *
 * @class CTestReturnChannelVolume
 *
 * Req. under test 
 */
class CTestReturnChannelVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestReturnChannelVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestReturnChannelVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestReturnChannelVolume() {};
	};

/**
 * Returns max channel volume
 *
 * @class CTestMaxChannelVolume
 *
 * Req. under test 
 */
class CTestMaxChannelVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestMaxChannelVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMaxChannelVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMaxChannelVolume() {};
	};

/**
 * Sets channel volume
 *
 * @class CTestSetChannelVolume
 *
 * Req. under test 
 */
class CTestSetChannelVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestSetChannelVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetChannelVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetChannelVolume() {};
	};

/**
 * Mute channel volume
 *
 * @class CTestSetChannelMute
 *
 * Req. under test 
 */
class CTestSetChannelMute : public CTestMmfMidiClntStep
	{
public:
	static CTestSetChannelMute* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetChannelMute(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetChannelMute() {};
	};

/**
 * Return volume
 *
 * @class CTestReturnVolume
 *
 * Req. under test 
 */
class CTestReturnVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestReturnVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestReturnVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestReturnVolume() {};
	};

/**
 * Return Max volume
 *
 * @class CTestMaxVolume
 *
 * Req. under test 
 */
class CTestReturnMaxVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestReturnMaxVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestReturnMaxVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestReturnMaxVolume() {};
	};

/**
 * Set volume
 *
 * @class CTestSetVolume
 *
 * Req. under test 
 */
class CTestSetVolume : public CTestMmfMidiClntStep
	{
public:
	static CTestSetVolume* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetVolume(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetVolume() {};
	};

/**
 * Set volume ramp
 *
 * @class CTestSetVolumeRamp
 *
 * Req. under test 
 */
class CTestSetVolumeRamp : public CTestMmfMidiClntStep
	{
public:
	static CTestSetVolumeRamp* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetVolumeRamp(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetVolumeRamp() {};
	};

/**
 * Returns balance
 *
 * @class CTestGetBalance
 *
 * Req. under test 
 */
class CTestGetBalance : public CTestMmfMidiClntStep
	{
public:
	static CTestGetBalance* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestGetBalance(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestGetBalance() {};
	};

/**
 * Set balance
 *
 * @class CTestSetBalance
 *
 * Req. under test 
 */
class CTestSetBalance : public CTestMmfMidiClntStep
	{
public:
	static CTestSetBalance* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetBalance(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetBalance() {};
	};

/**
 * Set priority
 *
 * @class CTestSetPriority
 *
 * Req. under test 
 */
class CTestSetPriority : public CTestMmfMidiClntStep
	{
public:
	static CTestSetPriority* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestSetPriority(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestSetPriority() {};
	};

/**
 * Returns number of xmf meta data entries
 *
 * @class CTestNumberOfXmfMetaDataEntries
 *
 * Req. under test 
 */
class CTestNumberOfXmfMetaDataEntries : public CTestMmfMidiClntStep
	{
public:
	static CTestNumberOfXmfMetaDataEntries* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestNumberOfXmfMetaDataEntries(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestNumberOfXmfMetaDataEntries() {};
	};

/**
 * Returns xmf meta data entry
 *
 * @class CTestGetXmfMetaDataEntry
 *
 * Req. under test 
 */
class CTestGetXmfMetaDataEntry : public CTestMmfMidiClntStep
	{
public:
	static CTestGetXmfMetaDataEntry* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestGetXmfMetaDataEntry(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestGetXmfMetaDataEntry() {};
	};

/**
 * Loads instrument data
 *
 * @class CTestLoadInstrumentData
 *
 * Req. under test 
 */
class CTestLoadInstrumentData : public CTestMmfMidiClntStep
	{
public:
	static CTestLoadInstrumentData* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestLoadInstrumentData(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestLoadInstrumentData() {};
	};


/**
 * Checks that we call the right function in the Midi test controller
 *
 * @class CTestCheckInterface
 *
 * Req. under test 
 */
class CTestCheckInterface : public CTestMmfMidiClntStep
	{
public:
	static CTestCheckInterface* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestCheckInterface(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestCheckInterface();
	void Close();
	TInt CompareInterface(const TDesC8& aInterfaceName);
	void ConstructL(void);
private:
	RChunk iChunk;
	RSemaphore iSemaphore;
	};

/**
 * Load and initialise a midi file.
 *
 * @class CTestMidiClntOpenFileByHandle
 *
 * Req. under test 
 */
class CTestMidiClntOpenFileByHandle : public CTestMmfMidiClntStep
	{
public:
	~CTestMidiClntOpenFileByHandle() {};
	static CTestMidiClntOpenFileByHandle* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TTestStepType aTestType);
	// From Base Class
	virtual TVerdict DoTestStepL();
private:
	CTestMidiClntOpenFileByHandle(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TTestStepType aTestType);
protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	const TBool iPlay;						// Does this test require playing the Audio file 
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file
	};

/**
 * Play a midi file.
 *
 * @class CTestMidiClntPlayFileByHandle
 *
 * Req. under test 
 */
class CTestMidiClntPlayFileByHandle : public CTestMmfMidiClntStep
	{
public:
	~CTestMidiClntPlayFileByHandle() {};
	static CTestMidiClntPlayFileByHandle* NewL(const TDesC& aTestName);
	// From Base Class
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestMidiClntPlayFileByHandle(const TDesC& aTestName);
	};
	
	
/**
 * Play a midi file and verify that no events are getting lost
 *
 * @class CTestMidiClntThirteenMidiEvents
 *
 * Req. under test 
 */
class CTestMidiClntThirteenMidiEvents : public CTestMmfMidiClntStep
	{
public:
	static CTestMidiClntThirteenMidiEvents* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

	void InitialiseExpectedTransArrayL();
private:
	CTestMidiClntThirteenMidiEvents(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestMidiClntThirteenMidiEvents() {};
	
	RArray<TMmcuoCallbacks> expectedTransitions;
	TUint8 iCurrentTransion;
	};
	
	
/**
 * Test the MIDI pluging state machine: transition from EMidiStateClosedDisengaged to EMidiStateClosedEngaged
 *
 * @class CTestTrasitionFromEClosedToEClosedEngaged
 *
 * Req. under test 
 */
class CTestTrasitionFromEClosedToEClosedEngaged : public CTestMmfMidiClntStep
	{
public:
	static CTestTrasitionFromEClosedToEClosedEngaged* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	
private:
	CTestTrasitionFromEClosedToEClosedEngaged(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestTrasitionFromEClosedToEClosedEngaged() {};
	};
	

/**
 * Test the MIDI pluging state machine: transition from EMidiStateClosedDisengaged to EMidiStateOpenEngaged
 *
 * @class CTestTrasitionFromEClosedToEOpenEngaged
 *
 * Req. under test 
 */
class CTestTrasitionFromEClosedToEOpenEngaged : public CTestMmfMidiClntStep
	{
public:
	static CTestTrasitionFromEClosedToEOpenEngaged* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	
private:
	CTestTrasitionFromEClosedToEOpenEngaged(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestTrasitionFromEClosedToEOpenEngaged() {};
	};


	
/**
 * Test the MIDI pluging state machine: transition from EMidiStateOpenEngaged to EMidiStateClosedEngaged
 *
 * @class CTestTrasitionFromEOpenEngagedToEClosedEngaged
 *
 * Req. under test 
 */
class CTestTrasitionFromEOpenEngagedToEClosedEngaged : public CTestMmfMidiClntStep
	{
public:
	static CTestTrasitionFromEOpenEngagedToEClosedEngaged* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	
private:
	CTestTrasitionFromEOpenEngagedToEClosedEngaged(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestTrasitionFromEOpenEngagedToEClosedEngaged() {};
	};


/**
 * Test the MIDI pluging state machine: transition from EMidiStateClosedEngaged to EMidiStateClosedDisengaged
 *
 * @class CTestTrasitionFromEClosedEngagedToEClosed
 *
 * Req. under test 
 */
class CTestTrasitionFromEClosedEngagedToEClosed : public CTestMmfMidiClntStep
	{
public:
	static CTestTrasitionFromEClosedEngagedToEClosed* NewL(const TDesC& aTestName, const TTestStepType aTestType);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	
private:
	CTestTrasitionFromEClosedEngagedToEClosed(const TDesC& aTestName, const TTestStepType aTestType);
	~CTestTrasitionFromEClosedEngagedToEClosed() {};
	};

struct TDrmTestParams
	{
	enum 
		{
		KNoIntent=-1
		};
		
	enum TSourceType
		{
		EFileName=0,
		EFileHandle
		};
	enum TTestMode
		{
		ENormal = 0,
		EAlloc
		};
		
	const TText*			iFileNameKey;
	const TText*			iUniqueId;
	ContentAccess::TIntent	iIntent;
	TInt					iExpectedError;	
	TSourceType				iSourceType;
	TInt					iNumPlayLoops;
	TTestMode				iTestMode;
	};
	
class CCallbackTimer;	
	
class CTestStepDrm : public CTestMmfMidiClntStep 
	{
protected:

	virtual void OnTimer();

	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	TVerdict OpenFileL(CMidiClientUtility* aMidi);
	CTestStepDrm(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	
	const TDrmTestParams& iDrmParams;
	RFs iFs;
	RFile iFile;
	CCallbackTimer* iTimer;
private:
static TInt TimerGate(TAny* aPtr);	
	};
/**
 * DRM-related negative test 
 *
 *
 * Req. under test 
 */
class CTestDrmNegative : public CTestStepDrm
	{
public:
	static CTestDrmNegative* NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	virtual void MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError);
private:
	CTestDrmNegative(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	};

class CTestDrmPlay : public CTestStepDrm
	{
public:
	static CTestDrmPlay* NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	
	virtual TVerdict PlayL(CMidiClientUtility* aMidi, TTimeIntervalMicroSeconds32 aStopAfter);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	virtual void MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError);
	
protected:
	CTestDrmPlay(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);

protected:
	TMidiState iLatestState; 	
	
	};


class CTestDrmPlayStop : public CTestDrmPlay
	{
public:
	static CTestDrmPlayStop* NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	
	virtual void OnTimer();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
private:
	CTestDrmPlayStop(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	CMidiClientUtility* iMidi;
	TBool iStopping;
	};
		

class CTestDrmPlayWithUI : public CTestStepDrm
	{
public:
	static CTestDrmPlayWithUI* NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	
	virtual TVerdict PlayL(CMidiClientUtility* aMidi, TTimeIntervalMicroSeconds32 aStopAfter);
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	virtual void MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError);
	
protected:
	TMidiState iLatestState; 	
	
private:
	CTestDrmPlayWithUI(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams);
	};

/**
 * Open a midi file more than once using FileHandle
 *
 * @class CTestMidiOpenFileHandleRepeat
 *
 * Req. under test 
 */
class CTestMidiOpenFileHandleRepeat : public CTestMmfMidiClntStep
	{
public:
	CTestMidiOpenFileHandleRepeat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	~CTestMidiOpenFileHandleRepeat() {};
	static CTestMidiOpenFileHandleRepeat* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	// From Base Class
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	const TBool iPlay;						// Does this test require playing the Audio file 
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file

	};
	
/**
 * Play without opening a file.
 *
 * @class CTestMidiClntPlayWithoutFile
 *
 * 
 */
class CTestMidiClntPlayWithoutFile : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntPlayWithoutFile(const TDesC& aTestName);
	~CTestMidiClntPlayWithoutFile() {};
	static CTestMidiClntPlayWithoutFile * NewL(const TDesC& aTestName);
	// From Base Class
	virtual TVerdict DoTestStepL();
	void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
protected:
    TInt iError;
	};


/**
 * Opening a midifile and playing it.
 *
 * @class CTestMidiClntOpenAndPlayFile
 *
 * 
 */
class CTestMidiClntOpenAndPlayFile : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntOpenAndPlayFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	~CTestMidiClntOpenAndPlayFile() {};
	static CTestMidiClntOpenAndPlayFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	// From Base Class
	virtual TVerdict DoTestStepL();
	void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
protected:
    TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
    TInt iError;
	};
#endif

