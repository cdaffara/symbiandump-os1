/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



#include "T_MMDataAudioToneUtility.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KState,							"state");
_LIT(KSequenceNumber,					"sequencenumber");
_LIT(KToneLength,						"tonelength");
_LIT(KToneOffLength,					"toneofflength");
_LIT(KPauseLength,						"pauselength");
_LIT(KDtmf,								"dtmf");
_LIT(KSequence,							"sequence");
_LIT(KFileName,							"filename");
_LIT(KFileHandle,						"filehandle");
_LIT(KFrequency,						"frequency");
_LIT(KFrequencyTwo,						"frequencytwo");
_LIT(KDuration,							"duration");
_LIT(KVolume,							"volume");
_LIT(KVolumeAdd,						"volumeAdd");
_LIT(KBalance,							"balance");
_LIT(KRepeat,							"repeat");
_LIT(KSilence,							"silence");
_LIT(KCustomUid,						"customUid");
_LIT(KSequenceCount,					"sequencecount");
_LIT(KSequenceName,						"sequencename");
_LIT(KCustomInterfaceExpected,			"customintexpected");

_LIT(KCmdNewL,							"NewL");
_LIT(KCmdState,							"State");
_LIT(KCmdFixedSequenceCount,			"FixedSequenceCount");
_LIT(KCmdFixedSequenceName,				"FixedSequenceName");
_LIT(KCmdSetDTMFLengths,				"SetDTMFLengths");
_LIT(KCmdCancelPrepare,					"CancelPrepare");
_LIT(KCmdPlay,							"Play");
_LIT(KCmdCancelPlay,					"CancelPlay");
_LIT(KCmdPrepareToPlayTone,				"PrepareToPlayTone");
_LIT(KCmdPrepareToPlayDualTone,			"PrepareToPlayDualTone");
_LIT(KCmdPrepareToPlayDTMFString,		"PrepareToPlayDTMFString");
_LIT(KCmdPrepareToPlayDesSequence,		"PrepareToPlayDesSequence");
_LIT(KCmdPrepareToPlayFileSequence,		"PrepareToPlayFileSequence");
_LIT(KCmdPrepareToPlayFixedSequence,	"PrepareToPlayFixedSequence");
_LIT(KCmdSetPriority,					"SetPriority");
_LIT(KCmdMaxVolume,						"MaxVolume");
_LIT(KCmdSetVolume,						"SetVolume");
_LIT(KCmdVolume,						"Volume");
_LIT(KCmdSetBalanceL,					"SetBalanceL");
_LIT(KCmdGetBalanceL,					"GetBalanceL");
_LIT(KCmdSetRepeats,					"SetRepeats");
_LIT(KCmdSetVolumeRamp,					"SetVolumeRamp");
_LIT(KCmdCustomInterface,				"CustomInterface");
_LIT(KCmdDestructor,					"~");

_LIT(KMdaAudioToneMaxVolume,			"MaxVolume");


_LIT(KMdaAudioToneUtilityNotReady,		"NotReady");
_LIT(KMdaAudioToneUtilityPrepared,		"Prepared");
_LIT(KMdaAudioToneUtilityPlaying,		"Playing");
_LIT(KMdaAudioToneUtilityUnknown,		"AudioToneUtilityUnknown");


_LIT(KMdaAudioToneUtilityMaxLeft,		"MaxLeft");
_LIT(KMdaAudioToneUtilityMaxRight,		"MaxRight");

_LIT(LMdaRepeatForever,					"RepeatForever");
/*@}*/

CT_MMDataAudioToneUtility* CT_MMDataAudioToneUtility::NewL()
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
	{
	CT_MMDataAudioToneUtility*  ret=new (ELeave) CT_MMDataAudioToneUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
CT_MMDataAudioToneUtility constructor.
@internalComponent
@return N/A
@pre None
@post CT_MMDataAudioToneUtility 1st stage constructor
*/
CT_MMDataAudioToneUtility::CT_MMDataAudioToneUtility()
:	iToneUtility(NULL)
,	iFileOpened(EFalse)
,	iAsyncErrorIndex(0)
,	iPlaying(EFalse)
	{
	}

void CT_MMDataAudioToneUtility::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	User::LeaveIfError(iFs.Connect());
	}


/**
CT_MMDataAudioToneUtility destructor
@internalComponent
@return N/A
@pre None
@post releases CT_MMDataAudioToneUtility heap memory
*/
CT_MMDataAudioToneUtility::~CT_MMDataAudioToneUtility()
	{
	DestroyData();

	iFs.Close();
	}

TAny* CT_MMDataAudioToneUtility::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iToneUtility;
	}

/**
Releases CT_MMDataAudioToneUtility dynamic memory.
@internalComponent
@return void
@pre None
@post All dynamic memory deallocated
*/
void CT_MMDataAudioToneUtility::DestroyData()
	{
	delete iToneUtility;
	iToneUtility = NULL;
	if (iFileOpened)
		{
		iFile.Close();
		iFileOpened = EFalse;
		}
	iAsyncErrorIndex = 0;
	}

TBool CT_MMDataAudioToneUtility::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
    {
    TBool	ret=ETrue;

    if (aCommand == KCmdNewL)
        {
        DoCmdNewL(aSection);
        }
    else if(aCommand == KCmdState)
        {
        DoCmdState(aSection);
        }
    else if(aCommand == KCmdFixedSequenceCount)
        {
        DoCmdFixedSequenceCount(aSection);
        }
    else if(aCommand == KCmdFixedSequenceName)
        {
        DoCmdFixedSequenceName(aSection);
        }
    else if(aCommand == KCmdSetDTMFLengths)
        {
        DoCmdSetDTMFLengths(aSection);
        }
    else if(aCommand == KCmdPlay)
        {
        DoCmdPlay(aAsyncErrorIndex);
        }
    else if(aCommand == KCmdCancelPlay)
        {
        DoCmdCancelPlay();
        }
    else if(aCommand == KCmdCancelPrepare)
        {
        DoCmdCancelPrepare();
        }
    else if(aCommand == KCmdPrepareToPlayTone)
        {
        DoCmdPrepareToPlayTone(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdPrepareToPlayDualTone)
        {
        DoCmdPrepareToPlayDualTone(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdPrepareToPlayFixedSequence)
        {
        DoCmdPrepareToPlayFixedSequence(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdPrepareToPlayDesSequence)
        {
        DoCmdPrepareToPlayDesSequenceL(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdPrepareToPlayDTMFString)
        {
        DoCmdPrepareToPlayDTMFString(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdPrepareToPlayFileSequence)
        {
        DoCmdPrepareToPlayFileSequenceL(aSection, aAsyncErrorIndex);
        }
    else if(aCommand == KCmdSetPriority)
        {
        DoCmdSetPriority(aSection);
        }
    else if(aCommand == KCmdMaxVolume)
        {
        DoCmdMaxVolume(aSection);
        }
    else if(aCommand == KCmdSetVolume)
        {
        DoCmdSetVolume(aSection);
        }
    else if(aCommand == KCmdVolume)
        {
        DoCmdVolume(aSection);
        }
    else if(aCommand == KCmdSetBalanceL)
        {
        DoCmdSetBalanceL(aSection);
        }
    else if(aCommand == KCmdGetBalanceL)
        {
        DoCmdGetBalanceL(aSection);
        }
    else if(aCommand == KCmdSetRepeats)
        {
        DoCmdSetRepeats(aSection);
        }
    else if(aCommand == KCmdSetVolumeRamp)
        {
        DoCmdSetVolumeRamp(aSection);
        }
	else if(aCommand == KCmdCustomInterface)
        {
        DoCmdCustomInterface(aSection);
		}
    else if(aCommand == KCmdDestructor)
        {
        DoCmdDestructor();
        }
    else
        {
        ret=EFalse;
        }
    return 	ret;
    }


void CT_MMDataAudioToneUtility::DoCmdNewL(const TDesC& aSection)
    {
	DestroyData();

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	err=KErrNone;
	TInt	priority;
    if ( CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		TRAP(err, iToneUtility = CMdaAudioToneUtility::NewL(*this, 0, priority, priorityPreference));
		}
	else // no priority specified use other overload
	    {
        TRAP(err, iToneUtility=CMdaAudioToneUtility::NewL(*this));
        }
	if(err!=KErrNone)
		{
	    ERR_PRINTF2(_L("CMdaAudioToneUtility::NewL failed with error %d"), err);
		SetError(err);
		}
    }


void CT_MMDataAudioToneUtility::DoCmdState(const TDesC& aSection)
    {
    TMdaAudioToneUtilityState   mdaState = iToneUtility->State();

    INFO_PRINTF2(_L("State. state=%S"), &GetStateString(mdaState));

    TMdaAudioToneUtilityState   expectedState;
    TPtrC                       state;

    if(GetStringFromConfig(aSection, KState(), state))
        {
        if ( !ConvertToState(state, expectedState) )
            {
            ERR_PRINTF2(_L("Illegal expected state: %S"), &state);
            SetBlockResult(EFail);
            }
        else if ( mdaState != expectedState )
            {
            SetBlockResult(EFail);
            ERR_PRINTF3(_L("state(%S) != expectedState(%S)"), &GetStateString(mdaState), &state);
            }
        }
    }


void CT_MMDataAudioToneUtility::DoCmdFixedSequenceCount(const TDesC& aSection)
    {
    TInt    fixedSequenceCount = iToneUtility->FixedSequenceCount();

    INFO_PRINTF2(_L("FixedSequenceCount. count=%d"), fixedSequenceCount);

    TInt	expectedValue;
    if (GetIntFromConfig(aSection, KSequenceCount(), expectedValue) )
	    {
	    if ( expectedValue != fixedSequenceCount )
		    {
			SetBlockResult(EFail);
	        ERR_PRINTF3(_L("FixedSequenceCount(%d) != ExpectedFixedSequenceCount(%d)"), fixedSequenceCount, expectedValue);
		    }
		}
    }


void CT_MMDataAudioToneUtility::DoCmdFixedSequenceName(const TDesC& aSection)
    {
	TPtrC	sequence;
	TInt	sequenceNumber = 0;

	GetIntFromConfig(aSection, KSequenceNumber(), sequenceNumber);
	INFO_PRINTF2(_L("sequenceNumber. =%d"), sequenceNumber);

	const TDesC&	fixedSequenceName = iToneUtility->FixedSequenceName(sequenceNumber);
	INFO_PRINTF2(_L("FixedSequenceName. name=%S"), &fixedSequenceName);

	TPtrC	expectedValue;
	if (GetStringFromConfig(aSection, KSequenceName(), expectedValue) )
		{
		if ( expectedValue != fixedSequenceName )
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("FixedSequenceName(%S) != ExpectedFixedSequenceName(%S)"), &fixedSequenceName, &expectedValue);
			}
		}
	}


void CT_MMDataAudioToneUtility::DoCmdSetDTMFLengths(const TDesC& aSection)
    {
    TInt    toneLength		= 0;
    TInt    toneOffLength	= 0;
    TInt    pauseLength		= 0;

    if (!GetIntFromConfig(aSection, KToneLength(), toneLength) )
        {
        ERR_PRINTF1(_L("ToneLength param not found"));
        SetBlockResult(EFail);
        }

    if (!GetIntFromConfig(aSection, KToneOffLength(), toneOffLength) )
        {
        ERR_PRINTF1(_L("ToneOffLength param not found"));
        SetBlockResult(EFail);
        }

    if (!GetIntFromConfig(aSection, KPauseLength(), pauseLength) )
        {
        ERR_PRINTF1(_L("PauseLength param not found"));
        SetBlockResult(EFail);
        }

    iToneUtility->SetDTMFLengths(
                    TTimeIntervalMicroSeconds32(toneLength),
                    TTimeIntervalMicroSeconds32(toneOffLength),
                    TTimeIntervalMicroSeconds32(pauseLength));
    }


void CT_MMDataAudioToneUtility::DoCmdPlay(const TInt aAsyncErrorIndex)
    {
	iAsyncErrorIndex = aAsyncErrorIndex;
    iToneUtility->Play();
	iPlaying=ETrue;
    IncOutstanding();
    }


void CT_MMDataAudioToneUtility::DoCmdCancelPlay()
    {
    iToneUtility->CancelPlay();
    }


void CT_MMDataAudioToneUtility::DoCmdCancelPrepare()
    {
    iToneUtility->CancelPrepare();
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayTone(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TInt    frequency	= 0;
    TInt    duration	= 0;
    iAsyncErrorIndex = aAsyncErrorIndex;

    if (!GetIntFromConfig(aSection, KFrequency(), frequency) )
        {
        ERR_PRINTF1(_L("Frequency param not found"));
        SetBlockResult(EFail);
        }

    if (!GetIntFromConfig(aSection, KDuration(), duration) )
        {
        ERR_PRINTF1(_L("Duration param not found"));
        SetBlockResult(EFail);
        }

    iToneUtility->PrepareToPlayTone(
                    frequency,
                    TTimeIntervalMicroSeconds(duration));
	if ( !iPlaying )
		{
		IncOutstanding();
		}
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayDualTone(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TInt    frequencyOne	= 0;
    TInt    frequencyTwo	= 0;
    TInt    duration		= 0;
    iAsyncErrorIndex = aAsyncErrorIndex;

    if (!GetIntFromConfig(aSection, KFrequency(), frequencyOne) )
        {
        ERR_PRINTF1(_L("FrequencyOne param not found"));
        SetBlockResult(EFail);
        }

    if (!GetIntFromConfig(aSection, KFrequencyTwo(), frequencyTwo) )
        {
        ERR_PRINTF1(_L("FrequencyTwo param not found"));
        SetBlockResult(EFail);
        }

    if (!GetIntFromConfig(aSection, KDuration(), duration) )
        {
        ERR_PRINTF1(_L("Duration param not found"));
        SetBlockResult(EFail);
        }

    iToneUtility->PrepareToPlayDualTone(
                    frequencyOne,
                    frequencyTwo,
                    TTimeIntervalMicroSeconds(duration));
	if ( !iPlaying )
		{
		IncOutstanding();
		}
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayFixedSequence(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TPtrC   sequence;
    TInt    sequenceNumber = 0;
    iAsyncErrorIndex = aAsyncErrorIndex;

    GetIntFromConfig(aSection, KSequenceNumber(), sequenceNumber);
    INFO_PRINTF2(_L("sequenceNumber. =%d"), sequenceNumber);

    iToneUtility->PrepareToPlayFixedSequence(sequenceNumber);
	if ( !iPlaying )
		{
		IncOutstanding();
		}
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayDesSequenceL(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
	iAsyncErrorIndex = aAsyncErrorIndex;

    TPtrC   seqfile;
    if (!GetStringFromConfig(aSection, KSequence(), seqfile) )
        {
        ERR_PRINTF1(_L("PrepareToPlayDesSequence: sequence param not found"));
        SetBlockResult(EFail);
        }
	else
        {
		RFile	file;
		User::LeaveIfError(file.Open(iFs, seqfile, EFileRead | EFileShareAny));
		CleanupClosePushL(file);

		TInt	size = 0;
		User::LeaveIfError(file.Size(size));

        HBufC8  *sequenceData = HBufC8::NewLC(size);
        TPtr8   sequence = sequenceData->Des();

		User::LeaveIfError(file.Read(sequence));
        iToneUtility->PrepareToPlayDesSequence(sequence);

        CleanupStack::PopAndDestroy(2, &file);  // sequenceData, file
		if ( !iPlaying )
			{
			IncOutstanding();
			}
	    }
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayDTMFString(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TPtrC   dtmf;
	iAsyncErrorIndex = aAsyncErrorIndex;

    if (!GetStringFromConfig(aSection, KDtmf(), dtmf) )
        {
        ERR_PRINTF1(_L("PrepareToPlayDTMFString: dtmf param not found"));
        SetBlockResult(EFail);
        }
    else
        {
 	    iToneUtility->PrepareToPlayDTMFString(dtmf);
		if ( !iPlaying )
			{
			IncOutstanding();
			}
        }
    }


void CT_MMDataAudioToneUtility::DoCmdPrepareToPlayFileSequenceL(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TPtrC   filename;
	iAsyncErrorIndex = aAsyncErrorIndex;

    if ( GetStringFromConfig(aSection, KFileName(), filename) )
        {
        iToneUtility->PrepareToPlayFileSequence(filename);
		if ( !iPlaying )
			{
			IncOutstanding();
			}
        }
    else
        {
        if ( GetStringFromConfig(aSection, KFileHandle(), filename) )
            {

			if (iFileOpened)
				{
				iFile.Close();
				iFileOpened = EFalse;
				}

            User::LeaveIfError(iFile.Open(iFs, filename, EFileRead | EFileShareAny));
            iFileOpened = ETrue;

            iToneUtility->PrepareToPlayFileSequence(iFile);
			if ( !iPlaying )
				{
				IncOutstanding();
				}
            }
        else
            {
            ERR_PRINTF1(_L("PrepareToPlayFileSequence: param not found"));
            SetBlockResult(EFail);
            }
        }
    }


void CT_MMDataAudioToneUtility::DoCmdSetPriority(const TDesC& aSection)
    {
	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
    if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
	    {
		priority=EMdaPriorityMin;
		}

    iToneUtility->SetPriority(priority, priorityPreference);
    }


void CT_MMDataAudioToneUtility::DoCmdMaxVolume(const TDesC& aSection)
    {
    TInt maxVolume = iToneUtility->MaxVolume();

    INFO_PRINTF2(_L("MaxVolume. volume=%d"), maxVolume);

    TInt    expectedVolume;

    if(GetIntFromConfig(aSection, KVolume(), expectedVolume))
		{
        if (maxVolume != expectedVolume )
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("maxVolume(%d) != expectedVolume(%d)"), maxVolume, expectedVolume);
			}
		}
    }


void CT_MMDataAudioToneUtility::DoCmdSetVolume(const TDesC& aSection)
    {
    TPtrC   volume;
    TInt    newVolume = 0;

	if ( GetStringFromConfig(aSection, KVolume(), volume) )
		{
        if (volume == KMdaAudioToneMaxVolume)
		    {
            newVolume = iToneUtility->MaxVolume();
            }
        else
            {
			if (!GetIntFromConfig(aSection, KVolume(), newVolume) )
				{
                ERR_PRINTF1(_L("volume param not found"));
				SetBlockResult(EFail);
			    }
			}
		}

    TInt    volumeAdd = 0;

    if (GetIntFromConfig(aSection, KVolumeAdd(), volumeAdd))
		{
        newVolume += volumeAdd;
        }

    iToneUtility->SetVolume(newVolume);
    }


void CT_MMDataAudioToneUtility::DoCmdVolume(const TDesC& aSection)
    {
    TInt    actualVolume = iToneUtility->Volume();

    INFO_PRINTF2(_L("Volume. volume=%d"), actualVolume);

    TInt    expectedVolume;
	TPtrC   volume;
	TBool	expectedVolumeGiven = ETrue;

    if(GetStringFromConfig(aSection, KVolume(), volume))
	    {
        if (volume == KMdaAudioToneMaxVolume)
		    {
            expectedVolume = iToneUtility->MaxVolume();
            }
        else
            {
			expectedVolumeGiven = GetIntFromConfig(aSection, KVolume(), expectedVolume);
			}
        if (expectedVolumeGiven && actualVolume != expectedVolume )
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("volume(%d) != expectedVolume(%d)"), actualVolume, expectedVolume);
			}
		}
    }


void CT_MMDataAudioToneUtility::DoCmdSetBalanceL(const TDesC& aSection)
    {
	TPtrC   balance;
    TInt    newBalance = 0;

	if ( GetStringFromConfig(aSection, KBalance(), balance) )
		{
		if ( !ConvertToBalance(balance, newBalance) )
		    {
			if (!GetIntFromConfig(aSection, KBalance(), newBalance) )
				{
				ERR_PRINTF2(_L("Illegal balance %S"), &newBalance);
				SetBlockResult(EFail);
			    }
			}
		}
    TRAPD(err, iToneUtility->SetBalanceL(newBalance));
    if(err != KErrNone)
		{
		ERR_PRINTF2(_L("SetBalanceL failed with error %d"), err);
		SetError(err);
	    }
    }


void CT_MMDataAudioToneUtility::DoCmdGetBalanceL(const TDesC& aSection)
    {
    TInt    actualBalance = 0;

    TRAPD(err, actualBalance = iToneUtility->GetBalanceL())
    if(err != KErrNone)
		{
		ERR_PRINTF2(_L("GetBalanceL failed with error %d"), err);
		SetError(err);
		}
    else
        {
        INFO_PRINTF2(_L("Balance. balance=%d"), actualBalance);

        TInt    expectedBalance;
		TPtrC   balance;
		TBool	expectedBalanceGiven = ETrue;

		if(GetStringFromConfig(aSection, KBalance(), balance))
			{
            if ( !ConvertToBalance(balance, expectedBalance) )
                {
                expectedBalanceGiven = GetIntFromConfig(aSection, KBalance(), expectedBalance);
                }
			if (expectedBalanceGiven && actualBalance != expectedBalance )
			    {
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("balance(%d) != expectedBalance(%d)"), actualBalance, expectedBalance);
				}
			}
        }
    }


void CT_MMDataAudioToneUtility::DoCmdSetRepeats(const TDesC& aSection)
    {
	TPtrC   repeat;
    TInt    newRepeat = 0;

	if (GetStringFromConfig(aSection, KRepeat(), repeat) )
		{
		if ( !ConvertToRepeat(repeat, newRepeat) )
		    {
			if (!GetIntFromConfig(aSection, KRepeat(), newRepeat) )
			    {
				ERR_PRINTF2(_L("Illegal repeat parameter %S"), &newRepeat);
				SetBlockResult(EFail);
				}
			}
		}
    else
		{
        ERR_PRINTF1(_L("repeat parameter not found"));
		SetBlockResult(EFail);
		}

    TInt    silence = 0;

    if (!GetIntFromConfig(aSection, KSilence(), silence))
		{
        ERR_PRINTF1(_L("Silence parameter not found"));
		SetBlockResult(EFail);
	    }

    iToneUtility->SetRepeats(newRepeat, TTimeIntervalMicroSeconds(silence));
    }


void CT_MMDataAudioToneUtility::DoCmdSetVolumeRamp(const TDesC& aSection)
    {
    TInt    newVolumeRamp = 0;

	if (!GetIntFromConfig(aSection, KDuration(), newVolumeRamp) )
		{
        ERR_PRINTF1(_L("Volume ramp param not found"));
		SetBlockResult(EFail);
	    }

    iToneUtility->SetVolumeRamp(TTimeIntervalMicroSeconds(newVolumeRamp));
    }


void CT_MMDataAudioToneUtility::DoCmdCustomInterface(const TDesC& aSection)
    {
    TInt    uidParam = 0;

    if (GetHexFromConfig(aSection, KCustomUid(), uidParam))
        {
        TUid	customUid = TUid::Uid(uidParam);
        TAny*	customInterface = iToneUtility->CustomInterface(customUid);
        TBool	customInterfaceFound = EFalse;
        if ( customInterface )
            {
            // cast the interface pointer to the desired type
            customInterfaceFound = ETrue;
            }

        TBool	customInterfaceExpected = ETrue;
        GetBoolFromConfig(aSection, KCustomInterfaceExpected(), customInterfaceExpected);

        if ( customInterfaceExpected && !customInterfaceFound )
            {
            ERR_PRINTF2(_L("CMdaAudioToneUtility::CustomInterface(0x%X) returned NULL"), customUid.iUid);
            SetBlockResult(EFail);
            }
        else if ( !customInterfaceExpected && customInterfaceFound )
            {
            ERR_PRINTF2(_L("CMdaAudioToneUtility::CustomInterface(0x%X) is not NULL"), customUid.iUid);
            SetBlockResult(EFail);
            }
        }
    else
        {
        ERR_PRINTF1(_L("CMdaAudioToneUtility::CustomInterface param not found"));
        SetBlockResult(EFail);
        }
    }


void CT_MMDataAudioToneUtility::DoCmdDestructor()
    {
    DestroyData();
    }


void CT_MMDataAudioToneUtility::MatoPrepareComplete(TInt aError)
    {
    if(aError == KErrNone )
        {
        INFO_PRINTF1(_L("MatoPrepareComplete OK"));
        }
    else
        {
        ERR_PRINTF2(_L("MatoPrepareComplete Error %d"), aError);
        SetAsyncError(iAsyncErrorIndex, aError);
        }
    INFO_PRINTF1(_L("MatoPrepareComplete (end)"));
    DecOutstanding();
    }


void CT_MMDataAudioToneUtility::MatoPlayComplete(TInt aError)
    {
    if(aError == KErrNone )
        {
        INFO_PRINTF1(_L("MatoPlayComplete OK"));
        }
    else
        {
        ERR_PRINTF2(_L("MatoPlayComplete Error %d"), aError);
        SetAsyncError(iAsyncErrorIndex, aError);
        }

    INFO_PRINTF1(_L("MatoPlayComplete"));

	if (iFileOpened)
		{
		iFile.Close();
		iFileOpened = EFalse;
		}
	iPlaying=EFalse;
	DecOutstanding();
    }


TBool CT_MMDataAudioToneUtility::ConvertToState(const TDesC& aDesc, TMdaAudioToneUtilityState& aMdaAudioToneUtilityState)
	{
	TBool	ret=ETrue;

	if ( aDesc==KMdaAudioToneUtilityNotReady )
		{
		aMdaAudioToneUtilityState=EMdaAudioToneUtilityNotReady;
		}
	else if ( aDesc==KMdaAudioToneUtilityPrepared )
		{
		aMdaAudioToneUtilityState=EMdaAudioToneUtilityPrepared;
		}
	else if ( aDesc==KMdaAudioToneUtilityPlaying )
		{
		aMdaAudioToneUtilityState=EMdaAudioToneUtilityPlaying;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


const TDesC& CT_MMDataAudioToneUtility::GetStateString(TMdaAudioToneUtilityState& aMdaAudioToneUtilityState)
	{
	switch(aMdaAudioToneUtilityState)
		{
	case EMdaAudioToneUtilityNotReady:
		return KMdaAudioToneUtilityNotReady;
	case EMdaAudioToneUtilityPrepared:
		return KMdaAudioToneUtilityPrepared;
	case EMdaAudioToneUtilityPlaying:
		return KMdaAudioToneUtilityPlaying;
	default:
		break;
		};

	return KMdaAudioToneUtilityUnknown;
	}


TBool CT_MMDataAudioToneUtility::ConvertToBalance(const TDesC& aDesc, TInt &aBalance)
	{
	TBool	ret=ETrue;

	if ( aDesc==KMdaAudioToneUtilityMaxLeft )
		{
		aBalance=KMMFBalanceMaxLeft;
		}
	else if ( aDesc==KMdaAudioToneUtilityMaxRight )
		{
		aBalance=KMMFBalanceMaxRight;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


TBool CT_MMDataAudioToneUtility::ConvertToRepeat(const TDesC& aDesc, TInt &aRepeat)
	{
	TBool	ret=ETrue;

	if ( aDesc==LMdaRepeatForever )
		{
		aRepeat=KMdaRepeatForever;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}
