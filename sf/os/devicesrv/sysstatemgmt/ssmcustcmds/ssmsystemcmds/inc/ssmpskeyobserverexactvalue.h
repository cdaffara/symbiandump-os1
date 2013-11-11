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
* Declaration of CSsmPsKeyObserverExactValue class.
*
*/

#ifndef SSMPSKEYOBSERVEREXACTVALUE_H
#define SSMPSKEYOBSERVEREXACTVALUE_H

#include "ssmpskeyobserver.h"

/**
*  An active object which waits until a P&S key value changes to a predefined
*  value and then signals a request status.
*/
NONSHARABLE_CLASS( CSsmPsKeyObserverExactValue ) : public CSsmPsKeyObserver
    {

public:

    /**
    * First phase constructor.
    *
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the
    *             specific property within the category.
    * @param aTargetValue The target value which the property should reach.
    */
    CSsmPsKeyObserverExactValue(
        TUid aCategory,
        TUint aKey,
        TInt aTargetValue );

    /**
    * Destructor.
    */
    virtual ~CSsmPsKeyObserverExactValue();

private:

    virtual TBool IsMatch( const TInt aKeyValue ) const;

private: // data

    /** The target value which the property should reach. */
    TInt iTargetValue;

    };

#endif // SSMPSKEYOBSERVEREXACTVALUE_H
