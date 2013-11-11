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



#if (!defined __T_MM_DATA_AUDIO_TONE_UTILITY_H__)
#define __T_MM_DATA_AUDIO_TONE_UTILITY_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <mdaaudiotoneplayer.h>
#include <imageconversion.h>

/**
CMdaAudioToneUtility API test commands
@internalComponent
*/
class CT_MMDataAudioToneUtility : public CDataWrapperBase, private MMdaAudioToneObserver
	{
public:
	~CT_MMDataAudioToneUtility();

	static	CT_MMDataAudioToneUtility*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_MMDataAudioToneUtility();

	void	ConstructL();

private:
	// Class commands
    inline void			DoCmdNewL(const TDesC& aSection);
    inline void			DoCmdState(const TDesC& aSection);
    inline void			DoCmdFixedSequenceCount(const TDesC& aSection);
    inline void			DoCmdFixedSequenceName(const TDesC& aSection);
    inline void			DoCmdSetDTMFLengths(const TDesC& aSection);
    inline void			DoCmdCancelPrepare();
    inline void			DoCmdPlay(const TInt aAsyncErrorIndex);
    inline void			DoCmdCancelPlay();
    inline void			DoCmdPrepareToPlayTone(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdPrepareToPlayDualTone(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdPrepareToPlayDTMFString(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdPrepareToPlayDesSequenceL(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdPrepareToPlayFileSequenceL(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdPrepareToPlayFixedSequence(const TDesC& aSection, const TInt aAsyncErrorIndex);
    inline void			DoCmdSetPriority(const TDesC& aSection);
    inline void			DoCmdMaxVolume(const TDesC& aSection);
    inline void			DoCmdSetVolume(const TDesC& aSection);
    inline void			DoCmdVolume(const TDesC& aSection);
    inline void			DoCmdSetBalanceL(const TDesC& aSection);
    inline void			DoCmdGetBalanceL(const TDesC& aSection);
    inline void			DoCmdSetRepeats(const TDesC& aSection);
    inline void			DoCmdSetVolumeRamp(const TDesC& aSection);
    inline void			DoCmdCustomInterface(const TDesC& aSection);

    // helper commands
    inline void			DoCmdDestructor();

    //  Helper methods
	void				DestroyData();
	static TBool		ConvertToState(const TDesC& aDesc, TMdaAudioToneUtilityState& aMdaAudioToneUtilityState);
	static const TDesC&	GetStateString(TMdaAudioToneUtilityState& aMdaAudioToneUtilityState);
	static TBool		ConvertToBalance(const TDesC& aDesc, TInt &aBalance);
	static TBool		ConvertToRepeat(const TDesC& aDesc, TInt &aRepeat);

	// from the MMdaAudioToneObserver interface
    inline void			MatoPrepareComplete(TInt aError);
    inline void			MatoPlayComplete(TInt aError);

private:
    CMdaAudioToneUtility*   iToneUtility;
    RFs                     iFs;

    TBool					iFileOpened;
    RFile					iFile;
   	TInt					iAsyncErrorIndex;
	TBool					iPlaying;
    };

#endif /* __T_MM_DATA_AUDIO_TONE_UTILITY_H__ */
