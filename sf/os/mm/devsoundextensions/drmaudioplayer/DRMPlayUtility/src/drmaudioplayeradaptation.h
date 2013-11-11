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
* Description:   DRM audio player adapatation class
*
*/

#ifndef __DRM_AUDIOPLAYER_ADAPTATION_H
#define __DRM_AUDIOPLAYER_ADAPTATION_H

#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>

#include <DrmAudioSamplePlayer.h>
#include <mdaaudiosampleplayer.h>


class CDrmAudioPlayerAdaptation :   public CBase,
                                    public MMdaAudioPlayerCallback
	{
	public: // construct/destruct
	    CDrmAudioPlayerAdaptation( MDrmAudioPlayerCallback& aCallback );

	    static CDrmAudioPlayerAdaptation* NewL( MDrmAudioPlayerCallback& aCallback,
    		                                      TInt aPriority,
    		                                      TMdaPriorityPreference aPref );
		~CDrmAudioPlayerAdaptation();

		//Services provided:
		static CDrmAudioPlayerAdaptation* NewFilePlayerL(const TDesC& aFileName,
		                    MDrmAudioPlayerCallback& aCallback,
		                    TInt aPriority,
		                    TMdaPriorityPreference aPref);

		static CDrmAudioPlayerAdaptation* NewDesPlayerL(const TDesC8& aData,
                            MDrmAudioPlayerCallback& aCallback,
                            TInt aPriority,
                            TMdaPriorityPreference aPref);

		static CDrmAudioPlayerAdaptation* NewDesPlayerReadOnlyL(const TDesC8& aData,
                                                                MDrmAudioPlayerCallback& aCallback,
                                                                TInt aPriority,
                                                                TMdaPriorityPreference aPref);

		void OpenFileL(const TDesC& aFileName);
		void OpenFileL(const RFile& aFile);
		void OpenFileL(const TMMSource& aSource);
		void OpenDesL(const TDesC8& aDescriptor);
		void OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType);
		void Play();
		void Stop();
		void SetVolume(const TInt aVolume );
		void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
		void SetVolumeRamp( const TTimeIntervalMicroSeconds& aRampDuration );
		const TTimeIntervalMicroSeconds& Duration();
		TInt MaxVolume();

		// API Additions since version 7.0
		TInt Pause();
		void Close();
		TInt GetPosition( TTimeIntervalMicroSeconds& aPosition );
		void SetPosition( const TTimeIntervalMicroSeconds& aPosition );

		TInt SetPriority( TInt aPriority, TMdaPriorityPreference aPref );
		TInt GetVolume(TInt& aVolume);
		TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
		CMMFMetaDataEntry* GetMetaDataEntryL( TInt aMetaDataIndex );
		TInt SetPlayWindow(   const TTimeIntervalMicroSeconds& aStart,
                            const TTimeIntervalMicroSeconds& aEnd);
		TInt ClearPlayWindow();
		TInt SetBalance(const TInt aBalance);
		TInt GetBalance(TInt& aBalance);
		TInt GetBitRate(TUint& aBitRate);

		void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);
		void GetAudioLoadingProgressL( TInt& aPercentageProgress );
		const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

		TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
		                        TInt aFunction,
		                        const TDesC8& aDataTo1,
		                        const TDesC8& aDataTo2,
		                        TDes8& aDataFrom);

		TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
	                                TInt aFunction,
	                                const TDesC8& aDataTo1,
	                                const TDesC8& aDataTo2 );

		void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
		                                                   TInt aFunction,
		                                                   const TDesC8& aDataTo1,
		                                                   const TDesC8& aDataTo2,
		                                                   TDes8& aDataFrom,
		                                                   TRequestStatus& aStatus);

		void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
		                                                   TInt aFunction,
		                                                   const TDesC8& aDataTo1,
		                                                   const TDesC8& aDataTo2,
		                                                   TRequestStatus& aStatus);

	private:
    		void ConstructL( TInt aPriority, TMdaPriorityPreference aPref );

    		void ConstructNewFilePlayerL( const TDesC& aFileName,
    		                              TInt aPriority,
    		                              TMdaPriorityPreference aPref );

    		void ConstructNewDesPlayerL( const TDesC8& aData,
    		                             TInt aPriority,
    		                             TMdaPriorityPreference aPref );

    		void ConstructNewDesPlayerReadOnlyL( const TDesC8& aData,
                                                TInt aPriority,
                                                TMdaPriorityPreference aPref );


    		//from MMdaAudioPlayerCallback
    		void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    		void MapcPlayComplete(TInt aError);

    		//Returns False if Destination CustomCommand needs to be blocked
    		TBool IsValidCustomCommandDestination(TUid aDestinationUid, TDesC8& aParam);
    		TBool CheckCustomInterfaceBuilderImplL(const TDesC8& aParam);


	private:
		CMdaAudioPlayerUtility* iMdaPlayUtil;

		// NOT OWNED !
		MDrmAudioPlayerCallback* iCallback;
	};

#endif //__DRM_AUDIOPLAYER_ADAPTATION_H

// End of file
