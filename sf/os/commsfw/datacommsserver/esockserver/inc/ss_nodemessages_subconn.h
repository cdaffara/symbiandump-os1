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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalAll
*/

#ifndef SYMBIAN_NODEMESSAGES_SUBCONN_H
#define SYMBIAN_NODEMESSAGES_SUBCONN_H

#include <comms-infras/ss_nodemessages_internal.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>

namespace ESock
{

class TCFSubConnControlClient : public TCFControlClient
	{
private:
  	enum
		{
		ECFSubConnNotification     	    = 1,
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		ECFPlaneNotification     	    = 2,		// PJTODO, move to another realm, not subconnection specific notification
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		};

public:
    enum { ERealmId = 0x10285F50 }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
    
    /*==========================================================================
    [Name]:      TSubConnNotification
    [Semantics]: Sent by a subconnection provider to its control clients raising a CSubConNotificationEvent.
    			 The message has to be consumed as the CSubConNotificationEvent is refcounted. I.e.: the
    			 refcount is increased with each post and hence its dispatch has to consume the message
    		     by calling iRefCountOwnedSubConNotification.Close() (decrease the refcount).

    [Type]:      Notification

    [Structure]:
    param:       iRefCountOwnedNotification - refcounting wrapper that holds a reference to the
    			 XPlaneEvent.
    ===========================================================================*/
    typedef TMessageSigSubConNotification<ECFSubConnNotification, TCFSubConnControlClient::ERealmId> TSubConnNotification;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    typedef TMessageSigPlaneNotification<ECFPlaneNotification, TCFSubConnControlClient::ERealmId> TPlaneNotification;
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	};

} //namespace esock

#endif //SYMBIAN_NODEMESSAGES_SUBCONN_H

