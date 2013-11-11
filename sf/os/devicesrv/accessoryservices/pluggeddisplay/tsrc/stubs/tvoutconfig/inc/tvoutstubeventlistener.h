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
* Description:  Declaration of CTVOutStubEventListener class.
*
*/


#ifndef __CTVOUTSTUBEVENTLISTENER_H__
#define __CTVOUTSTUBEVENTLISTENER_H__

#include <e32base.h>
#include <e32property.h>
#include "tvoutconfigstatuscontrol.h"

class MTVOutStubEventSender;

/**
 *  Observer for Starter module test events.
 *
 *  @lib None
 *  @since S60 9.2
 */
class CTVOutStubEventListener : public CActive
    {

public:

    /**
     * Second phase constructor.
     *
     * @param aSender Object that can send an event in response to an
     * indication received from P&S.
     * @param aCategory Property's category
     * @param aKey Property's key

     * @since S60 9.2
     */
    static CTVOutStubEventListener* NewL( 
            MTVOutStubEventSender& aSender,
            TUid aCategory, 
            TUint aKey );

    virtual ~CTVOutStubEventListener();

protected:

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     *
     * @since S60 9.2
     */
    virtual void DoCancel();

    /**
     * From CActive.
     * Handles an active object's request completion event.
     * Never leaves.
     *
     * @since S60 9.2
     */
    virtual void RunL();

private:

    /**
     * First phase constructor.
     *
     * @param aSender Object that can send an event in response to an
     * indication received from P&S.
     * @since S60 9.2
     */
    CTVOutStubEventListener( 
            MTVOutStubEventSender& aSender);

    void ConstructL(
            TUid aCategory, 
            TUint aKey );

private: // data

    /**
     * Handle to Publish&Subscribe key to observe.
     */
    RProperty iProperty;

    /**
     * Object that can send an event in response to an indication received from
     * P&S.
     */
    MTVOutStubEventSender& iSender;

    TUid iCategory;
    
    TUint iKey;
    
    TInt iPreviousValue;

    };

#endif // __CTVOUTSTUBEVENTLISTENER_H__
