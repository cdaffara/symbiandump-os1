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

#ifndef AGENTSUTILITY_H
#define AGENTSUTILITY_H

#include <badesca.h>
#include <mmf/common/mmfcontroller.h>
#include <flogger.h>


/**
States of the Client
*/
enum TMVSState
{
	ENotReady	= 0,
	EAudioOpening,
	EAudioOpened,
	EAudioPlaying,
	EAudioRecording,
	EAudioStopped,
	EVideoOpening,
	EVideoOpened,
	EVideoPlaying,
	EVideoRecording,
	EVideoStopped,
	EVideoPaused,
	EAudioPaused
};

/**
Modes of the Client
*/
enum TMVSMode
{
	EAudioRecord,
	EAudioPlay,
	EVideoRecord,
	EVideoPlay,
	EIdle
};	
 

/**
@publishedPartner
@prototype
@test

An interface to Client callback function
This serves as the method of communication between the Client and the UI

*/
class MMVSClientObserver
{
public:
	
	/**
	
	Called when a multimedia controller plugin changes state e.g. when audio starts to play, 
	the state changes from EAudioOpened to EAudioPlaying
	
	@param  aState
	        Current State of the Client
	@param  aError
	        Error code. The status of the current operation
	*/
	virtual void UpdateStateChange(TMVSState aState, TInt aError) = 0;
	
	/**
	Called when the audio resource becomes available but only if the resource was previously unavailable (e.g. if 
	the audio resource is available when the client registers, then this callback is not received).

	@param  aNotificationData
			The data that corresponds to the notification event
	*/
	virtual void MvsResourceNotification(const TDesC8& aNotificationData) = 0;

};


/**
@publishedPartner
@prototype
@test

Utility class that does fetching the existing controller plugins,the extensions
supported by them and finding the type of the media file for the UI.
*/
class CMVSControllerPluginInfo: public CBase
{
public:
	IMPORT_C static CMVSControllerPluginInfo* NewL();
	IMPORT_C ~CMVSControllerPluginInfo();

	IMPORT_C  TInt GetPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray);
	IMPORT_C  TInt GetAudioPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray,RArray<TUid>& aUidPlayArray, TDesC* aExt);
	IMPORT_C  TInt GetVideoPluginListL(CDesCArrayFlat* aDisplayNames, RArray<TUid>& aUidArray, TDesC* aExt);	
	IMPORT_C TInt GetExtensionListL(TBool aControllerUid, CDesCArrayFlat* aExtArray);
	IMPORT_C TUid GetMediaTypeL(TDesC& aFileName);
	IMPORT_C CMMFControllerImplementationInformation& GetControllerInfo(TUid aControllerUid);
private:
	void CollectAudioControllersL(TDesC* aExt);
	void CollectAudioPlayControllersL(TDesC* aExt);
	void CollectVideoControllersL(TDesC* aExt);
	void ConstructL(); //populate iControllers
	CMVSControllerPluginInfo();
	void GetHeaderL(TDesC& aFileName, TDes8& aHeaderData);
private:
	//list of controller info objects
	RMMFControllerImplInfoArray iControllers;
	RMMFControllerImplInfoArray iSupportedControllers;
	RArray<TUid> iUidArray;
	RMMFControllerImplInfoArray iAudioControllers;
	RMMFControllerImplInfoArray iVideoControllers;
	RMMFControllerImplInfoArray iAudioPlayControllers;
	RArray<TUid> iAudioUidArray;
	RArray<TUid> iAudioPlayUidArray;
	RArray<TUid> iVideoUidArray;
	RFileLogger iFileLogger;
};


#endif AGENTSUTILITY_H
