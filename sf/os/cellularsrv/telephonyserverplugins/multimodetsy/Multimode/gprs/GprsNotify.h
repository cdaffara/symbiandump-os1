// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Gprs Notifications
// 
//

/**
 @file
 @internalAll 
*/

#ifndef _GPRSNOTIFY_H
#define _GPRSNOTIFY_H

#include "NOTIFY.H"
#include "etelpckt.h"




class CGprsContextAddedNotify : public CNotifyBase
/**
This is a Notification class. Handles a GPRS context added notification.
CGprsContextAddedNotify inherits from the CNotifyBase class defined in
notify.h.
@internalComponent
*/
{
public:
	static CGprsContextAddedNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes* aContextId);
	~CGprsContextAddedNotify();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	CGprsContextAddedNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes* aContextId);
	TDes* iContextId;									//< Pointer to clients notification structure/class.
};		


class CGprsStatusNotify : public CNotifyBase
/**
This is a Notification class. Handles a GPRS status notification.
CGprsStatusNotify inherits from the CNotifyBase class defined in
notify.h.
@internalComponent
*/
{
public:
	static CGprsStatusNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TStatus* aGprsStatus);
protected:
	CGprsStatusNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TStatus* aGprsStatus);
	~CGprsStatusNotify();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	RPacketService::TStatus* iGprsStatus;						//< Pointer to clients notification structure/class.
};



class CGprsNtwkRegStatusChangeNotify : public CNotifyBase
/**
Notification class for the NtwkRegStatusChange.
This notification is completed when the phone
@internalComponent
*/
{
public:
	static CGprsNtwkRegStatusChangeNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TRegistrationStatus* aRegStatus);
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject, TNotifications aNotification);
private:
	CGprsNtwkRegStatusChangeNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TRegistrationStatus* aRegStatus);
private:
	RPacketService::TRegistrationStatus* iRegStatus;	//< Pointer to clients notification structure
};		





class CGprsContextConfigNotify: public CNotifyBase
/**
This is a Notification class. Handles a context configuration change notification.
CGprsContextConfigNotify inherits from the CNotifyBase class defined in
notify.h.
@internalComponent
*/
{
public:
	static CGprsContextConfigNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aConfig);
protected:
	CGprsContextConfigNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aConfig);
	~CGprsContextConfigNotify();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);

private:
	TDes8* iConfig;										//< Pointer to clients notification structure/class.
};



class CGprsContextStatusNotify: public CNotifyBase
/**
This is a Notification class. Handles a GPRS context status change notification.
CGprsContextStatusNotify inherits from the CNotifyBase class defined in
notify.h.
@internalComponent
*/
{
public:
	static CGprsContextStatusNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketContext::TContextStatus* aContextStatus);
protected:
	CGprsContextStatusNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketContext::TContextStatus* aContextStatus);
	~CGprsContextStatusNotify();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	RPacketContext::TContextStatus* iContextStatus;		//< Pointer to clients notification structure/class.
};



class CGprsQoSProfileNotify: public CNotifyBase
/**
This is a Notification class. Handles a qos profile configuration change notification.
CGprsQoSProfileNotify inherits from the CNotifyBase class defined in
notify.h.
@internalComponent
*/
{
public:
	static CGprsQoSProfileNotify* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aProfile);
protected:
	CGprsQoSProfileNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aProfile);
	~CGprsQoSProfileNotify();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	TDes8* iProfile;										//< Pointer to clients notification structure/class.
};

#endif
