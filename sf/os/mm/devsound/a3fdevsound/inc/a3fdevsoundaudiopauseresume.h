// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef A3FDEVSOUNDAUDIOPAUSERESUME_H
#define A3FDEVSOUNDAUDIOPAUSERESUME_H

/**
* @publishedPartner 
* @released
* @file
*/

#include <e32base.h>

/**
* KUIdAudioResourceNotification – To get the interface of MAutoPauseResume from context.
*/
const TInt KUIdAudioResourceNotificationValue = 0x10285D3B;
const TUid KUIdAudioResourceNotification = {KUIdAudioResourceNotificationValue};


/**
Defines the interface for observing auto pause and resume events
*/
class MA3FDevSoundAutoPauseResumeObserver
	{
public:
	/*
	Notifies the DevSound Adaptor that the resource is available.
	*/
	virtual void NotifyResume() = 0;
	};

/**
Interface providing support for AutoPauseResume Register, Cancel and WillResumePlay.
*/
class MA3FDevSoundAutoPauseResume
	{
public:
	/*
	Registers the Event for Notification when resource is avaliable.
	@param TUid aEventType The event which the client is notified of.
	@param TDesC8 &aNotificationRegistrationData Notification registration specific data.
	@param MA3FDevSoundAutoPauseResumeObserver* aObserverPtr An Observer to receive the notification.
	@return An error code indicating if the registration was successful. KErrNone on success, otherwise another of the system-wide error codes.
	*/
	virtual TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData, MA3FDevSoundAutoPauseResumeObserver* aObserverPtr) = 0;

	/*
	Cancels the registered notification event.
	@param TUid aEventType The Event to notify the client.
	@return An error code indicating if the cancel registration was successful. KErrNone on success, otherwise another of the system-wide error codes.
	*/
	virtual TInt CancelRegisterAsClient(TUid aEventType) = 0;

	/*
	Waits for the client to resume the play even after the default timer expires.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise another of the system-wide error codes.
	*/
	virtual TInt WillResumePlay() = 0;
	};

#endif //A3FDEVSOUNDAUDIOPAUSERESUME_H
