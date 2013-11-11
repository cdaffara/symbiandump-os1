// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TEST_STEP_MIDICLNT_ALLOCFAIL_NEW__
#define __TEST_STEP_MIDICLNT_ALLOCFAIL_NEW__

 
/**
 *
 * XXX
 *
 * @lib ""
 *
 * @xxxx
 *
 */
class CTestStepMidiClntAllocFailNew :	public	CTestMmfMidiClntStep
	{
public:
	static CTestStepMidiClntAllocFailNew* NewL();
	CTestStepMidiClntAllocFailNew();
	~CTestStepMidiClntAllocFailNew();
	virtual TVerdict DoTestStepL();

	// from MMidiClientUtilityObserver
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private:
	CMidiClientUtility* iMidiClnt;
	};


#endif //(__TEST_STEP_MIDICLNT_ALLOCFAIL_NEW__)
