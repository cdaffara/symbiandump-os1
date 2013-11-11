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

#ifndef MVSVIDEORECORDAGENT_H
#define MVSVIDEORECORDAGENT_H

#include <videorecorder.h>
#include <mvs/agentsutility.h>
#include <f32file.h>
#include <mmf/common/mmffourcc.h>
#include <ecam.h>
#include <badesca.h>
#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfcontroller.h>

class CCamera;

/**
@publishedPartner
@prototype
@test

Provides Video recording functionality for the UI

*/
class CMVSVideoRecordAgent : public CBase, MVideoRecorderUtilityObserver,MCameraObserver2
{
public:
	IMPORT_C static CMVSVideoRecordAgent* NewL(MMVSClientObserver& aObserver);
	IMPORT_C ~CMVSVideoRecordAgent();
	
	IMPORT_C TInt OpenFileL(TDesC &aFileName, TUid aControllerUid);
    IMPORT_C void OpenDesL(TDes8 &aDescriptor, TInt aCameraHandle, TUid   aControllerUid, TUid aVideoFormat, const TDesC8 &aVideoType=KNullDesC8, TFourCC aAudioType=KMMFFourCCCodeNULL);
	
	IMPORT_C void Close();
	IMPORT_C void Prepare();
	IMPORT_C void Record();
	IMPORT_C TInt Stop();
	IMPORT_C void PauseL();
	
	IMPORT_C void SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref);
	IMPORT_C void SetVideoFrameRateL(TReal32 aFrameRate);
	IMPORT_C void SetVideoFrameSizeL(const TSize &aSize);
	IMPORT_C void SetVideoBitRateL(TInt aBitRate);
	IMPORT_C void SetAudioBitRateL(TInt aBitRate);
	IMPORT_C void SetAudioEnabledL(TBool aEnabled);
	IMPORT_C void SetGainL(TInt aGain);
	IMPORT_C void SetVideoTypeL(const TDesC8 &aType);
	IMPORT_C void SetAudioTypeL(TFourCC aType);
	
	
	IMPORT_C void GetPriorityL(TInt &aPriority, TMdaPriorityPreference &aPref);
	IMPORT_C TReal32 VideoFrameRateL();
	IMPORT_C void GetSupportedFrameRatesArrayL(RArray<TReal32>& aFrameRatesArray);
	IMPORT_C void GetSupportedFrameSizesArrayL(RArray<TSize>& aFrameSizesArray);
	IMPORT_C void GetVideoFrameSizeL(TSize &aSize);
	IMPORT_C TInt VideoBitRateL();
	IMPORT_C TInt AudioBitRateL();
	IMPORT_C TBool AudioEnabledL();
	IMPORT_C TTimeIntervalMicroSeconds DurationL();
	IMPORT_C TInt GainL() ;
	IMPORT_C TInt MaxGainL() ;
	
	IMPORT_C void SetMetaDataL(RPointerArray<CMMFMetaDataEntry> aMetaArray);
	IMPORT_C TFourCC AudioTypeL() ;
	IMPORT_C void GetSupportedVideoTypesL(CDesC8ArrayFlat &aVideoTypes) ;
	IMPORT_C void GetSupportedAudioTypesL(RArray< TFourCC > &aAudioTypes) ;
	IMPORT_C const TDesC8 &VideoFormatMimeType() ;
	IMPORT_C const CMMFControllerImplementationInformation &ControllerImplementationInformationL();
    
    /**
	Returns the current state of Client. Provided for the UI to enable/disable the 
	menu items depending on the current state of the Client
	*/
    IMPORT_C TMVSState State();
    
	CCamera* Camera() {return iCamera;}
	
private:
	void ConstructL();// create recorder utility here
    CMVSVideoRecordAgent(MMVSClientObserver& aObserver);
	
	//from MVideoRecorderUtilityObserver
	void MvruoOpenComplete(int);
	void MvruoPrepareComplete(int);
	void MvruoRecordComplete(int);
	void MvruoEvent(const TMMFEvent& aEvent);
		
	// from MCameraObserver2
	void HandleEvent(const TECAMEvent &aEvent);
	void ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);
	void ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);	
	void VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
	TBool EstablishFsSessionL();
    void CloseFsSession();
    void CloseFileSession();
    TBool EstablishFileSessionL(TDesC& aPath);
    void CreateDependencyFileL();
    void RemoveDependencyFile();

private:
	CVideoRecorderUtility* iVideoRecorder;
	MMVSClientObserver& iObserver;
	CCamera* iCamera;
	TMVSState iState;
	//used to create the file to be recorded
    RFs iFs;
    RFs ifsSession;
    RFile iFile;
    TBool iFsSessionEstablished;
    TBool iFileSessionEstablished;
    RFileLogger iFileLogger;
    TCameraInfo iInfo;
};


#endif MVSVIDEORECORDAGENT_H