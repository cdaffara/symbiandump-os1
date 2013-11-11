/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of MTVOutStubEventSender interface class.
*
*/

#ifndef __TVOUTSTUBEVENTSENDER_H__
#define __TVOUTSTUBEVENTSENDER_H__

#include <e32def.h>

/**
 *  Interface representing object capable of sending Startup Adaptation events.
 *
 *  @lib None
 *  @since S60 9.2
 */
class MTVOutStubEventSender
    {

public:

    /**
     * Send an event.
     *
     * @since S60 9.2
     * @param aCategory Identifies the category of the event to send.
     * @param aEvent Identifies the event to send.
     * @param aArg Optional parameter for the event.
     */
    virtual void SendEvent( const TUid aCategory, const TInt aEvent, const TInt aArg ) = 0;

    };

#endif // __TVOUTSTUBEVENTSENDER_H__
