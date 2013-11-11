/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for property observer
*
*/
#ifndef CAUTOAUDIOASYPROPERTYOBSERVER_H__
#define CAUTOAUDIOASYPROPERTYOBSERVER_H__

#include <e32base.h>
#include <e32property.h>

class MAutoAudioAsyPropertyListener;

/**
 * This class follows PS key (informs when RTP streaming is enabled or disabled).
 */
NONSHARABLE_CLASS(CAutoAudioAsyPropertyObserver): public CActive
    {
public:
    /**
     * Two-phase constructor
     */
    static CAutoAudioAsyPropertyObserver* NewL ( TUid aCategory, TUint aKey, MAutoAudioAsyPropertyListener& aListener ); 
    virtual ~CAutoAudioAsyPropertyObserver ();

protected:
    // CActive
    void DoCancel ();
    void RunL ();
    TInt RunError ( TInt aError );

private:
    CAutoAudioAsyPropertyObserver ( TUid aCategory, TUint aKey, MAutoAudioAsyPropertyListener& aListener );

private:
    /**
     * Handle to P&S property.
     * Own.
     */
    RProperty iProperty;

    /**
     * The UID of the property we are listening for
     */
    TUid iCategory;

    /**
     * The key we are listening for
     */
    TUint iKey;

    /**
     * Reference to listener class.
     * Now own.
     */
    MAutoAudioAsyPropertyListener& iListener;
    };

#endif // CAUTOAUDIOASYPROPERTYOBSERVER_H__
