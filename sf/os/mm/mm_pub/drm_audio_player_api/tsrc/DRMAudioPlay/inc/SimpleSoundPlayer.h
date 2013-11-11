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
* Description: DRM Player
*
*/

#ifndef __SOUND_PLAYER_H
#define  __SOUND_PLAYER_H

#include <DrmAudioSamplePlayer.h>
#include <AudioPreference.h>
#include <StifTestModule.h>
#include "EventTarget.h"
#include "Parameters.h"

#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include <mmfcontrollerpluginresolver.h>
#include "FileNameParameters.h"
#include "Des8Parameters.h"
#include "UrlParameters.h"
#include "ObjectCountDown.h"

const TUid KUidInterfaceMMFController = {KMmfUidPluginInterfaceController};

class CSimpleSoundPlayer: public CBase , public MEventTarget , public MDrmAudioPlayerCallback
	{
public:
	static CSimpleSoundPlayer* NewUrlL(const TDesC &aDescriptor, TInt aIapId, const TDesC8 &aMime, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate=ETrue);
	static CSimpleSoundPlayer* NewUrlLC(const TDesC &aDescriptor, TInt aIapId, const TDesC8 &aMime, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate=ETrue);

	static CSimpleSoundPlayer* NewL(const TDesC8 &aDescriptor, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate=ETrue, TBool aDesReadOnly=EFalse);
	static CSimpleSoundPlayer* NewLC(const TDesC8 &aDescriptor, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate=ETrue, TBool aDesReadOnly=EFalse);

	static CSimpleSoundPlayer* NewL(const TFileName &aFile, CTestModuleIf &aConsole, CStifLogger &iLogger, TBool aImmediate=ETrue, TBool aFileType=EFalse);
	static CSimpleSoundPlayer* NewLC(const TFileName &aFile, CTestModuleIf &aConsole, CStifLogger &iLogger, TBool aImmediate=ETrue, TBool aFileType=EFalse);

	static CSimpleSoundPlayer* NewL(const RFile &aFile, CTestModuleIf &aConsole, CStifLogger &iLogger, TBool aImmediate=ETrue);
	static CSimpleSoundPlayer* NewLC(const RFile &aFile, CTestModuleIf &aConsole, CStifLogger &iLogger, TBool aImmediate=ETrue);
	~CSimpleSoundPlayer();

	// from MMdaAudioPlayerCallback
//	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
//	void MapcPlayComplete(TInt aError);

	//MDrmAudioPlayerCallBack
    virtual void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MdapcPlayComplete(TInt aError);

	void SetVolumeRamp(const TTimeIntervalMicroSeconds &aRampDuration);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds &aTrailingSilence);
	void SetPosition(const TTimeIntervalMicroSeconds &aPosition);
	TInt SetPlayWindow(const TTimeIntervalMicroSeconds &aStart, const TTimeIntervalMicroSeconds &aEnd);
	TTimeIntervalMicroSeconds GetPlayDuration();
	TTime GetPlayStartTime();
	TTime GetPlayStopTime();
	TInt GetBitRate(TUint &aBitRate);
	TInt GetBalance(TInt &aBalance);
	TInt SetBalance(TInt aBalance);
	TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
	TTimeIntervalMicroSeconds GetDuration();
	TInt ClearPlayWindow();
	TInt Play();
	TInt OpenDesL(const TDesC8 &aSoundDes , const TBool &aImmediate=ETrue) ;
	TInt GetPosition(TTimeIntervalMicroSeconds &aPosition);

	void SetSupervisor(TObjectCountDown *aCounter) ;
	void SetPlayCompleteTarget(MEventTarget *aTarget, CParameters *aParameters) ;

	void SetVolume(TInt aNewVolume);
	TInt GetVolume(TInt &aVolume);
	TInt MaxVolume();

	void RegisterForAudioLoadingNotification(MAudioLoadingObserver &aLoadingObserver);
	void GetAudioLoadingProgressL(TInt &aPercentageProgress);
//	CMMFControllerImplementationInformation *ControllerImplementationInformationL();
	void ControllerImplementationInformationL();

	TInt GetNumberOfMetaDataEntries(TInt &aNumEntries);
	CMMFMetaDataEntry *GetMetaDataEntryL(TInt aMetaDataIndex);
	TInt OpenL(const TFileName &aFileName , const TBool &aImmediate);

private:
	TInt OpenUrlL(const TDesC &aUrl, TInt aIapId, const TDesC8 &aMime) ;
	TInt Pause();
//	TInt OpenL(const TFileName &aFileName , const TBool &aImmediate);
	TInt OpenFileHandlerL(const TFileName &aFileName , const TBool &aImmediate);
	TInt Stop(const TBool &aAlsoClose=ETrue);

	CSimpleSoundPlayer(CTestModuleIf &aConsole, CStifLogger &iLogger, TBool aImmediate=ETrue, TBool aFileType=EFalse);
	void ConstructUrlL(const TDesC &aDescriptor, TInt aIapId, const TDesC8 &aMime);
	void ConstructL(const TDesC8 &aDescriptor);
	void ConstructL(const TFileName &aFile);
	void ConstructL(const RFile &aFile);

	void Exit(TInt aExitCode);

private:
	MEventTarget *iEventTarget;
	CParameters *iParameters;

	TObjectCountDown *iSupervisor;
	CDrmPlayerUtility* iMdaPlayer;
	CTestModuleIf &iConsole;
	TBool iImmediate;
	CStifLogger &iLogger;

	TTime iLastPlayTime;
	TTime iLastStopTime;


public:
	static const TInt KPlayerActionPlay;
	static const TInt KPlayerActionPause;
	static const TInt KPlayerActionStopPlayFile;
	static const TInt KPlayerActionReportPosition;
	static const TInt KPlayerActionReportDuration;
    static const TInt KplayerActionMaxVolume;
    static const TInt KplayerActionSetVolume;
    static const TInt KplayerActionSetBalance;
    static const TInt KplayerActionGetBalance;
    static const TInt KplayerActionGetBitRate;
    static const TInt KplayerActionSetPriority;
    static const TInt KplayerActionSetRepeats;
	static const TInt KPlayerActionStopPlayDes;
	static const TInt KPlayerActionReportVolume;
	static const TInt KPlayerActionStopPlayUrl;
	static const TInt KPlayerActionStopPlayFileHandler;
	static const TInt KPlayerActionOpen;
	static const TInt KPlayerActionOpenFileHandler;
	static const TInt KPlayerActionGetMetaDataEntry;
	static const TInt KPlayerActionGetNumberOfMetaDataEntries;

	static const TInt KplayerActionGetAudioLoadingProgress;
	static const TInt KplayerActionRegisterForAudioLoadingNotification;
	static const TInt KplayerActionControllerImplementationInformation;

	TInt ExecuteL(CParameters *aParams) ;

	enum TState
	{
		ENotReady,
		EReady,
		EPlaying,
		EPaused
	};

	RFs iFs;
	RFile iFileHandler;

	TInt iPlayerId;
	TState iState;
	TBool iSoundOpen;
	/////TBool iAutoClose;
	TInt iFinalError;
	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iStartPosition;
	TTimeIntervalMicroSeconds iEndPosition;
	TInt  iBalance;
	TBool iPlayVolume;
	TBool iMetaInfo;
	TBool iMetaInfoTest;
	TBool iPlayWindow;
	TBool iClearWindow;
	TBool iSetBalance;
	TBool iFsOpened;
	TBool iFileType;

	TBool iDontPlayAudioClip;

private:
	TBool iIsLoopPlayEnabled;
	};

#endif //__SOUND_PLAYER_H

