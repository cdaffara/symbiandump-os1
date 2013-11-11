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
* Declaration of CSsmPsKeyObserver class.
*
*/

#ifndef SSMPSKEYOBSERVER_H
#define SSMPSKEYOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/**
*  An active object which waits until a P&S key value changes to a predefined
*  value and then signals a request status.
*/
NONSHARABLE_CLASS( CSsmPsKeyObserver ) : public CActive
    {

public:

    /**
    * First phase constructor.
    *
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the
    *             specific property within the category.
    */
    CSsmPsKeyObserver( TUid aCategory, TUint aKey );

    /**
    * Destructor.
    */
    virtual ~CSsmPsKeyObserver();

    /**
    * Start observing the key value.
    *
    * @param aStatus Request status to be completed when the property reaches
    *        the target value.
    */
    void StartObserving( TRequestStatus& aStatus );

private:

    /**
    * From CActive.
    * Implements cancellation of an outstanding request.
    */
    virtual void DoCancel();

    /**
    * From CActive.
    * Handles an active object’s request completion event.
    */
    virtual void RunL();

protected:

    virtual TBool IsMatch( const TInt aKeyValue ) const = 0;

private:

    /**
    * Check the key value fron P&S and subscribe to listen to changes
    * in the P&S key if the value is not the same as the target value.
    */
    void CheckKeyValue();

    /**
    * Complete client request with the given error code.
    *
    * @param aErrorCode The error code to complete the request with.
    */
    void Complete( const TInt aErrorCode );

private: // data

    /** P&S variable to listen to. */
    RProperty iProperty;

    /** The UID that identifies the property category. */
    TUid iCategory;

    /**
    * The property sub-key, i.e. the key that identifies the specific property
    * within the category.
    */
    TUint iKey;

    /**
    * Request status to be completed when the property reaches the target value.
    */
    TRequestStatus* iClientStatus;

    };

#endif // SSMPSKEYOBSERVER_H
