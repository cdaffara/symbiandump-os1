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

#include <mdaaudiosampleeditor.h>
#include <mdaaudiosampleplayer.h>
#include <mda/client/utility.h>
#include <f32file.h>
#include <badesca.h>
#include <flogger.h>
#include <mvs/agentsutility.h>

#ifndef MVSAUDIORECORDAGENT_H
#define MVSAUDIORECORDAGENT_H


/**
@publishedPartner
@prototype
@test

Provides Audio recording functionality for the UI/Client.

*/
class CMVSAudioRecordAgent : public CBase, MMdaObjectStateChangeObserver

{
public:
	IMPORT_C static CMVSAudioRecordAgent* NewL(MMVSClientObserver& aObserver);
	IMPORT_C ~CMVSAudioRecordAgent();
	
	//creates new file for recording
	IMPORT_C void CreateNewFileL(TDesC& aFile,TUid aRecordControllerUid,TUid aPlayControllerUid);
	
	// supply controller uid
	IMPORT_C void OpenFileL(const TDesC& aFile, TUid aRecordControllerUid,TUid aPlayControllerUid); 
	// allow MMF to choose controller
	IMPORT_C void OpenFileL(const TDesC& aFile); 
	
	IMPORT_C  void OpenDesL(const TDesC8 &aDescriptor); //for future support
	
	IMPORT_C void RecordL();
	IMPORT_C void Stop();
	IMPORT_C void PlayL();
    
    //Setter methods - General
    IMPORT_C void SetGain(TInt aGain);
    IMPORT_C void CropClipL(TTimeIntervalMicroSeconds& aCropStart, 
                   TBool aflag);
    
    IMPORT_C void SetMetaDataL(RPointerArray<CMMFMetaDataEntry> aMetaEntry);
    IMPORT_C TInt SetPlayBalance(TInt aBalance);
    IMPORT_C TInt SetRecordBalance(TInt aBalance);
    IMPORT_C void SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
    IMPORT_C void SetRepeats(TInt aNoRepeats, TTimeIntervalMicroSeconds aDelay);
    IMPORT_C void SetPosition(TTimeIntervalMicroSeconds& aPosition);
    IMPORT_C void SetVolume(TInt aVolume, TTimeIntervalMicroSeconds aRamp);
    IMPORT_C TInt SetBalance(TInt aBalance);
    IMPORT_C void SetMaxWriteLength(TInt aMaxWriteLength);    
    //closes the current audio clip
    IMPORT_C void Reset();
	
	//Setter methods - Format configuration
	IMPORT_C void SetDestinationBitRateL(TUint aBitRate);
	IMPORT_C void SetDestinationSampleRateL(TUint aSampleRate);
	IMPORT_C void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);
	IMPORT_C void SetDestinationDataTypeL(TFourCC aDataType);
		
	//Getter methods - General
	IMPORT_C TTimeIntervalMicroSeconds& Position() const;
	IMPORT_C TInt Gain(TInt& aGain);
	IMPORT_C TInt MaxGain() const;
	IMPORT_C void GetMetaDataArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaData);
	IMPORT_C TInt GetVolume(TInt& aVolume);
	IMPORT_C TInt GetPlayBalance(TInt& aBalance);
	IMPORT_C TInt GetRecordBalance(TInt& aBalance);
	IMPORT_C TTimeIntervalMicroSeconds Duration();
	IMPORT_C TUint SourceBitRateL();
	IMPORT_C TInt MaxVolume();
	IMPORT_C const TTimeIntervalMicroSeconds RecordTimeAvailable();
	
	//Getter methods - Format configuration
	IMPORT_C void GetSupportedNoChannelsArrayL(RArray<TUint>& aNoChannelsArray);
    IMPORT_C void GetSupportedBitRatesArrayL(RArray<TUint>& aBitRatesArray);
    IMPORT_C void GetSupportedSampleRatesArrayL(RArray<TUint>& aSampleRatesArray);
    IMPORT_C void GetSupportedCodecsArrayL(RArray<TFourCC>& aCodecsArray);
    
    /**
	Returns the current state of Client. Provided for the UI to enable/disable the 
	menu items depending on the current state of the Client
	*/
    IMPORT_C TMVSState State();
    	
private:
	void ConstructL();// create recorder utility here
    CMVSAudioRecordAgent(MMVSClientObserver& aObserver);
	
	//from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/,
                                TInt /*aPreviousState*/, 
                                TInt /*aCurrentState*/, 
                                TInt /*aErrorCode*/);
    TBool EstablishFsSessionL();
    void CloseFsSession();
    void CloseFileSession();
    TBool EstablishFileSessionL(TDesC& aPath);
    

private:
	CMdaAudioRecorderUtility* iRecorder;
	MMVSClientObserver& iObserver;
	
	TMVSState iState;
	    
    //used to create the file to be recorded
    RFs iFs;
    RFile iFile;
    TBool iFsSessionEstablished;
    TBool iFileSessionEstablished;
    RFileLogger iFileLogger;        
}; 
#endif __MVSAUDIORECORDAGENT_H__