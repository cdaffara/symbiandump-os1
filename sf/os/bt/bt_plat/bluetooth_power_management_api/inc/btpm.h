/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BT Server power management API definition.
*
*/


#ifndef __BTPM_H
#define __BTPM_H


#include <e32std.h>
#include <es_sock.h>


/**
 *  Class MBTPowerManagerObserver
 *
 *  The observer class that is interested in power mode change
 *
 *  @lib btpm.lib
 *  @since S60 v2.1
 */
class MBTPowerManagerObserver
    {

public:

    /**
     * Callback to notify of the new Bluetooth power mode.
     *
     * @since S60 v2.1
     * @param aState The new power mode; 
     *               EFalse if Bluetooth is off, ETrue if it is on
     * @return none
     */
    virtual void HandlePowerChange(TBool aNewState) = 0;

    };


/**
 *  Class CBTPowerManagerBase
 *
 *  The base class of BT power management.
 *
 *  @lib btmp.lib
 *  @since S60 v2.1
 */
class CBTPowerManagerBase : public CBase
    {

public:

    /**
     * Get the current power state
     *
     * @since S60 v2.1
     * @param aState Reference where the current power state to be saved to
     * @return KErrNone if successful otherwise one of the system wide error codes
     */
    virtual TInt GetPowerState(TBool& aState) = 0;

    /**
     * Set the current power state
     *
     * @since S60 v2.1
     * @param aState the current power state
     * @return KErrNone if successful otherwise one of the system wide error codes
     */
    virtual TInt SetPowerState(TBool aState) = 0;

    /**
     * Notify observers that the power state has been changed
     *
     * @since S60 v2.1
     * @param none
     * @return none
     */
    virtual void NotifyPowerChange() = 0;

    /**
     * Notify observers that the power change has been cancelled
     *
     * @since S60 v2.1
     * @param none
     * @return none
     */
    virtual void NotifyPowerChangeCancel() = 0;

    /**
     * Set the device into test mode
     *
     * @since S60 v2.1
     * @param none
     * @return KErrNone if successful otherwise one of the system wide error codes
     */
    virtual TInt SetDUTMode() = 0;

    };


typedef CBTPowerManagerBase* (*TNewBTPowerManagerL)(MBTPowerManagerObserver* aObserver);


#endif  // __BTPM_H
