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

#include "GprsNotify.h"
#include "mSLOGGER.H"
#include "Matstd.h"
#include "gprs.h"
#include "Gprscontext.h"
#include "gprsqos.h"

/**
 * @file
 * This file implements all CGprs and CGprsContext notifications
 */

CGprsContextAddedNotify* CGprsContextAddedNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes* aContextId)
/**
 * Leavable constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{
	return new(ELeave) CGprsContextAddedNotify(aReqHandle,aTelObject,aContextId);
	}

CGprsContextAddedNotify::CGprsContextAddedNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes* aContextId)
: CNotifyBase(aReqHandle,aTelObject), iContextId(aContextId)
/**
 * Constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{}


CGprsContextAddedNotify::~CGprsContextAddedNotify()
/**
 * Destructor.
 */
	{}


TBool CGprsContextAddedNotify::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a context added Notification.
 * @param aTelObject Owner pointer.
 * @param aEvent The notification event.
 * @param aLastEvent previous event.
 */
	{
	if (aEvent==EPacketContextAddedChanged && aTelObject == iTelObject)
		{
		LOGTEXT2(_L8("CGprsContextAddedNotify Notification completed"),aEvent);
		iContextId->Copy(*((CGprs*)iTelObject)->NameOfLastAddedContext());
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}


// class  CGprsStatusNotify definitions

CGprsStatusNotify* CGprsStatusNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TStatus* aGprsStatus)
/**
 * Leavable constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{
	return new(ELeave) CGprsStatusNotify(aReqHandle,aTelObject,aGprsStatus);
	}


CGprsStatusNotify::CGprsStatusNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TStatus* aGprsStatus)
: CNotifyBase(aReqHandle,aTelObject), iGprsStatus(aGprsStatus)
/**
 * Constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{}


CGprsStatusNotify::~CGprsStatusNotify()
/**
 * Destructor.
 */
	{}


TBool CGprsStatusNotify::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a GPRS status changed Notification.
 * @param aTelObject Owner pointer.
 * @param aEvent The notification event.
 * @param aLastEvent previous event.
 */
	{
	if (aEvent==EPacketStatusChanged && aTelObject == iTelObject)
		{
		LOGTEXT2(_L8("CGprsStatusNotify Notification completed"),aEvent);
		*iGprsStatus = ((CGprs*)iTelObject)->Status();
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}



CGprsContextConfigNotify* CGprsContextConfigNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aConfig)
/**
 * Leavable constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{
	return new(ELeave) CGprsContextConfigNotify(aReqHandle,aTelObject,aConfig);
	}


CGprsContextConfigNotify::CGprsContextConfigNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aConfig)
: CNotifyBase(aReqHandle,aTelObject), iConfig(aConfig)
/**
 * Constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{}


CGprsContextConfigNotify::~CGprsContextConfigNotify()
/**
 * Destructor.
 */
	{}


TBool CGprsContextConfigNotify::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a context configuration changed Notification.
 * @param aTelObject Owner pointer.
 * @param aEvent The notification event.
 * @param aLastEvent previous event.
 */
	{
	if (aEvent==EPacketContextConfigChanged && aTelObject == iTelObject)
		{
		LOGTEXT2(_L8("CGprsContextConfigNotify Notification completed"),aEvent);
		TPckg<RPacketContext::TContextConfigGPRS>* contextConfigV1Pckg = (TPckg<RPacketContext::TContextConfigGPRS>*)iConfig;
		RPacketContext::TContextConfigGPRS& iContextConfigGPRS = (*contextConfigV1Pckg)();
		iContextConfigGPRS =  *((CGprsContext*)iTelObject)->Config();
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}



// class  CGprsContextStatusNotify definitions

CGprsContextStatusNotify* CGprsContextStatusNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketContext::TContextStatus* aContextStatus)
/**
 * Leavable constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.
 */
	{
	return new(ELeave) CGprsContextStatusNotify(aReqHandle,aTelObject,aContextStatus);
	}


CGprsContextStatusNotify::CGprsContextStatusNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketContext::TContextStatus* aContextStatus)
: CNotifyBase(aReqHandle,aTelObject), iContextStatus(aContextStatus)
/**
 * Constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aContextId Client pointer to notification information.

 */
	{}


CGprsContextStatusNotify::~CGprsContextStatusNotify()
/**
 * Destructor.
 */

	{}


TBool CGprsContextStatusNotify::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a context status changed Notification.
 * @param aTelObject Owner pointer.
 * @param aEvent The notification event.
 * @param aLastEvent previous event.
 */
	{
	if (aEvent==EPacketContextStatusChanged  && aTelObject == iTelObject)
		{
		LOGTEXT2(_L8("CGprsContextStatusNotify Notification completed"),aEvent);
		*iContextStatus = ((CGprsContext*)iTelObject)->ContextInfo()->iStatus;
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}


CGprsQoSProfileNotify* CGprsQoSProfileNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aProfile)
/**
 * Leavable constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aProfile Client pointer to notification information.
 */
	{
	return new(ELeave) CGprsQoSProfileNotify(aReqHandle,aTelObject,aProfile);
	}


CGprsQoSProfileNotify::CGprsQoSProfileNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TDes8* aProfile)
: CNotifyBase(aReqHandle,aTelObject), iProfile(aProfile)
/**
 * Constructor.
 * @param aReqHandle Handle to client.
 * @param aTelObject Owner object.
 * @param aProfile Client pointer to notification information.
 */
	{}


CGprsQoSProfileNotify::~CGprsQoSProfileNotify()
/**
 * Destructor.
 */
	{}


TBool CGprsQoSProfileNotify::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a context configuration changed Notification.
 * @param aTelObject Owner pointer.
 * @param aEvent The notification event.
 * @param aLastEvent previous event.
 */
	{
	if (aEvent==EPacketQoSProfileChanged && aTelObject == iTelObject)
		{
		LOGTEXT2(_L8("CGprsQoSProfileNotify Notification completed"),aEvent);

		TPckg<RPacketQoS::TQoSGPRSRequested>* qosProfilePckg = (TPckg<RPacketQoS::TQoSGPRSRequested>*) iProfile;
		RPacketQoS::TQoSGPRSRequested& iQoSProfileGPRS = (*qosProfilePckg)();
		iQoSProfileGPRS =	*((CGprsQoS*)iTelObject)->GetReqQoSProfile();

		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}


//
// Implementation of CGprsNtwkRegStatusChangeNotify is below
//

CGprsNtwkRegStatusChangeNotify* CGprsNtwkRegStatusChangeNotify::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TRegistrationStatus* aRegStatus)
/** Factory function */
 	{
	return new(ELeave) CGprsNtwkRegStatusChangeNotify(aReqHandle,aTelObject,aRegStatus);
	}

CGprsNtwkRegStatusChangeNotify::CGprsNtwkRegStatusChangeNotify(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RPacketService::TRegistrationStatus* aRegStatus)	
	: CNotifyBase(aReqHandle,aTelObject),
	  iRegStatus(aRegStatus)
/** C++ constructor */
 	{}

TBool CGprsNtwkRegStatusChangeNotify::CheckAndCompleteNotification(CTelObject* aTelObject, TNotifications aNotification)
/**
 * This function completes a context configuration changed Notification.
 */
 	{
	if (aNotification==EPacketNtwkRegStatusChange && aTelObject==iTelObject)
		{
		LOGTEXT2(_L8("CGprsNtwkRegStatusChangeNotify Notification completed"),aNotification);
		*iRegStatus = ((CGprs*)iTelObject)->RegistrationStatus();
		iTelObject->ReqCompleted(iReqHandle,KErrNone);
		return ETrue;
		}
	return EFalse;
	}






