
// MmfDevSoundCustomInterfaceSupport.h

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
//

/**
 @publishedAll
 @released
 @file
*/

#ifndef MMFDEVSOUNDCUSTOMINTERFACESUPPORT_H
#define MMFDEVSOUNDCUSTOMINTERFACESUPPORT_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfdevsoundcustominterfaceobserver.h>
#include <mmf/server/mmfdevsoundcustominterfacesupportclasses.h>
#endif

/**
@publishedAll
@released

Custom Interface providing support for AutoPauseResume Register, Cancel, GetResourceNotificationData and WillResumePlay.
*/

class MAutoPauseResumeSupport
	{
public:
	/**
	Registers the event for notification when resource is avaliable.

	@param aEventType
		The event which the client is notified of.
		
	@param aNotificationRegistrationData
		Notification registration specific data, which has been reserved for future use.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8)=0;
	
	/**
	Cancels the registered notification event.

	@param  aEventType
		The event to notify the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt CancelRegisterAsClient(TUid aEventType)=0;
	/**
	Gets the notification data for the event.
	
	@param  aEventType
		The event which the client is notified of.
		
	@param aNotificationData
		The notification data for the client to resume playing. The actual data depends on the event type.
		Note that for the event type 'KMMFEventCategoryAudioResourceAvailable' the package buffer returned
		is TMMFTimeIntervalMicroSecondsPckg, but the contents should be converted to an integer and
		interpreted as the data returned is samples played, but not as a microsecond value.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData)=0;
	/**
	Waits for the client to resume the play even after the default timer expires.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt WillResumePlay()=0;
	};

#endif // MMFDEVSOUNDCUSTOMINTERFACESUPPORT_H
