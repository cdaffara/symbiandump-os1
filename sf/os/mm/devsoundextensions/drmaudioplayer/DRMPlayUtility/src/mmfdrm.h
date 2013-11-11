/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   DRM Audio player 
*
*/



#ifndef __CDRM_H
#define __CDRM_H


#include <e32base.h>
#include <e32cons.h>
#include <mmfbase.h>
#include <mmfcontrollerframeworkbase.h>
#include <mmfaudio.h>
#include <mmfcontrollerpluginresolver.h>
#include <mmfcontrollerframework.h>
#include <mmfstandardcustomcommands.h>
#include "mmfdrmSession.h"

class CDrm: public CActive
{
public:

     /**
     * Two-phased constructor.
    */
   	static CDrm* NewL(TInt aPriority);
    /**
    * Two-phased constructor.
    */
	static CDrm* NewLC(TInt aPriority);
	/**
	* Destructor
	*/
	~CDrm();
	TInt Connect(); 
	void Play();
	void Stop();
	void SetVolume(TInt aVolume);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	const TTimeIntervalMicroSeconds& Duration();
	TInt MaxVolume();
	void OpenFileL(const TDesC& aFileName);
	void OpenFileL(const RFile& aFile);
	void OpenFileL(const TMMSource& aSource);
	void OpenDesL(const TDesC8& aDescriptor);
	void OpenUrlL(const TDesC& aUrl, TInt aIapId = KUseDefaultIap, const TDesC8& aMimeType=KNullDesC8);
	TInt Pause();
	void Close();
	TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);
	TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
	TInt GetVolume(TInt& aVolume);
	TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);
	TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
							const TTimeIntervalMicroSeconds& aEnd);
	TInt ClearPlayWindow();
	TInt SetBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalance(TInt& aBalance);
	void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);
	void GetAudioLoadingProgressL(TInt& aPercentageComplete);
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	TInt GetBitRate(TUint& aBitRate);
protected:
	//inherited from CActive
	void DoCancel();
    void RunL();


private:

	//C++ constructor
   CDrm(TInt aPriority);
   // 2nd phase construction.
   void ConstructL (void);

   // Session to server   
   RDrmSession*	         iSess;
   //Current state
   TInt		         iState;

	enum TState  //events
		{
		EStatePlay = 0,
		EStateStop,
		EStateSetVolume,
		EStateSetRepeats,
		EStateSetVolumeRamp,
		EStateDuration,
		EStateMaxVolume,
		EStateOpenFile,
		EStateOpenDes,
		EStateOpenUrl,
		EStatePause,
		EStateClose,
		EStateGetPosition,
		EStateSetPosition,
		EStateSetPriority,
		EStateGetVolume,
		EStateGetNumberOfMetaDataEntries,
		EStateGetMetaDataEntry,
		EStateSetPlayWindow,
		EStateClearPlayWindow,
		EStateSetBalance,
		EStateGetBalance,
		EStateRegisterForAudioLoadingNotification,
		EStateGetAudioLoadingProgress,
		EStateControllerImplementationInformation,
		EStateCustomCommandSync,
		EStateCustomCommandSync2,
		EStateCustomCommandAsync,
		EStateCustomCommandAsync2,
		EStateGetBitRate
		};

};




#endif
