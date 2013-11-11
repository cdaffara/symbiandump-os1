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
* Description:  Notify service class about property changes.
*
*/

#ifndef MAUTOAUDIOASYPROPERTYLISTENER_H
#define MAUTOAUDIOASYPROPERTYLISTENER_H

#include <e32property.h>

/**
 * Methods needed to handle Publish & Subscribe property changes.
 */
class MAutoAudioAsyPropertyListener
    {
public:
    /**
     *
     * @param aProperty Handle to the property
     * @param aCategory The Uid of the property
     * @param aKey The property key
     * @param aStatus The property change status
     * @return void
     */
    virtual void PropertyChange ( RProperty& aProperty, TUid aCategory,
        TUint aKey, TInt aStatus ) = 0;
    };

#endif // MAUTOAUDIOASYPROPERTYLISTENER_H
