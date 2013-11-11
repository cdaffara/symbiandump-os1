// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Agents for TechView
//

#include <mdaaudiosampleplayer.h>
#include <mvs/agentsutility.h>
#include <flogger.h>

#ifndef MVSAUDIOPLAYAGENT_H
#define MVSAUDIOPLAYAGENT_H

/**
@publishedPartner
@prototype
@test

Provides Audio playing functionality for the UI/Client

*/
class CMVSAudioPlayAgent : public CBase, MMdaAudioPlayerCallback, MMMFAudioResourceNotificationCallback
	{
	public:
		IMPORT_C static CMVSAudioPlayAgent* NewL(MMVSClientObserver& aObserver);
		IMPORT_C ~CMVSAudioPlayAgent();
		
		//opens an audio clip for playback
		IMPORT_C void OpenFileL(TDesC& aFile);
		//opens DRM protected source
		IMPORT_C void OpenFileL(TMMSource &aSource); //Added for future support
        IMPORT_C void OpenDesL(const TDesC8 &aDescriptor); //Added for future support
        
		IMPORT_C void Play();
		IMPORT_C void Stop();
        IMPORT_C TInt  Pause();
        IMPORT_C void Forward(); //Added for future support
        IMPORT_C void Rewind(); //Added for future support
        
      	//Setter Methods
      	
        IMPORT_C void SetVolume(TInt aVolume, TTimeIntervalMicroSeconds aRamp);
        IMPORT_C void SetPosition(TTimeIntervalMicroSeconds aPosition);
        IMPORT_C void SetRepeats(TInt aNoRepeats, TTimeIntervalMicroSeconds aDelay);
        IMPORT_C TInt SetBalance(TInt aBalance);
		IMPORT_C TInt SetPlayWindow(TTimeIntervalMicroSeconds aStart, TTimeIntervalMicroSeconds    aEnd);
        IMPORT_C TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
        IMPORT_C TInt ClearPlayWindow();
         
	    //closes the current audio clip
	   
        IMPORT_C void Reset();
        IMPORT_C void SetAutoPauseResume(TBool aEnable);
        
        //Getter Methods
        
        IMPORT_C TTimeIntervalMicroSeconds GetPosition(TTimeIntervalMicroSeconds& aPosition);
        IMPORT_C TInt GetVolume(TInt& aVolume);
        IMPORT_C TInt MaxVolume();
        IMPORT_C TTimeIntervalMicroSeconds Duration();
        IMPORT_C TInt GetBalance(TInt& aBalance);
        IMPORT_C void GetMetaArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaarray);
        IMPORT_C TInt GetBitRate(TUint& aBitRate);
        
        //Returns Implementation Info object of the controller is currently loaded
        
        IMPORT_C const CMMFControllerImplementationInformation& GetControllerInfoL();
        IMPORT_C TMVSState GetState();
    private:
		void ConstructL();//create playerutility here
    	CMVSAudioPlayAgent(MMVSClientObserver& aObserver);
    
		//from MMdaAudioPlayerCallback
		
		void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
		void MapcPlayComplete(TInt aError);
	
		//from MMMFAudioResourceNotificationCallback
		
		void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
	private:
		CMdaAudioPlayerUtility* iPlayer;
		MMVSClientObserver& iObserver;
		TMVSState iState;
    	RFileLogger iFileLogger;
	};

#endif MVSAUDIOPLAYAGENT_H
