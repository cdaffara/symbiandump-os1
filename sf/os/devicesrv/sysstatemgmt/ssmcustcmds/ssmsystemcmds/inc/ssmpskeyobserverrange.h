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
* Description:
* Declaration of CSsmPsKeyObserverRange class.
*
*/

#ifndef SSMPSKEYOBSERVERRANGE_H
#define SSMPSKEYOBSERVERRANGE_H

#include "ssmpskeyobserver.h"

/**
*  An active object which waits until a P&S key value changes to a predefined
*  value and then signals a request status.
*/
NONSHARABLE_CLASS( CSsmPsKeyObserverRange ) : public CSsmPsKeyObserver
    {

public:

    /**
    * First phase constructor.
    *
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the
    *             specific property within the category.
    */
    CSsmPsKeyObserverRange(
        TUid aCategory,
        TUint aKey,
        TInt aTargetLow,
        TInt aTargetHigh );

    /**
    * Destructor.
    */
    virtual ~CSsmPsKeyObserverRange();

private:

    virtual TBool IsMatch( const TInt aKeyValue ) const;

private: // data

    TInt iTargetLow;
    TInt iTargetHigh;

    };

#endif // STRTPSKEYOBSERVERRANGE_H
