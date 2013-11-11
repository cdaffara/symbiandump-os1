/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the class declarations for the power state plugin and power state callback APIs.
*
*/




/**
 @file
 @publishedPartner
 @released
*/


#ifndef HWRMPOWERSTATEPLUGIN_H
#define HWRMPOWERSTATEPLUGIN_H

#include <e32base.h>
// included so only this header is necessary to use power state plugin API
#include <hwrmpowerstatesdkpskeys.h>

/** ECOM Power state plugin interface UID 
@publishedPartner
@released
*/
const TUid KCHWRMPowerStatePluginInterfaceUid = { 0x10205042 };


/**
*  Callback interface for the HWRM Power state plugin.
*  Used to indicate state changes for battery and charging state.
*  Implemented by the HWRM server handler.
*
@publishedPartner
@released
*/
class MHWRMPowerStatePluginCallback
    {
    public:
        /**
        * Callback method used to notify power state changes.
        * This includes charging and battery state information.
        *
        * @param aKey       Key type that is to be updated, can be one of:
        *                       EPSHWRMBatteryLevel
        *                       EPSHWRMBatteryStatus
        *                       EPSHWRMChargingStatus
        * @param aValue     Key value, see the associated enum for the key
        *                   in HWRMPowerStateSDKPSKeys.h
        */
        virtual void NotifyStateChange( const TUint32 aKey,
                                        TInt aValue ) = 0;
    };


/**
*  Plugin interface class to be implemented by adaptation.
*  Responsible for invoking the adaptation implementation.
*
@publishedPartner
@released
*/
class CHWRMPowerStatePlugin : public CBase
    {
    public:  // construction and destruction
    
        /**
        * Constructor method for instance.
        * Uses ECom to find correct instance.
        *
        * @param aNotificationCallback Pointer to plugin callback handler.
        */
        inline static CHWRMPowerStatePlugin* NewL(
                        MHWRMPowerStatePluginCallback* aNotificationCallback);

        /**
        * Destructor
        */
        inline virtual ~CHWRMPowerStatePlugin();       

   protected:

        /**
        * Destructor identifier to be used with ECom framework.
        */
        TUid iDestructorIDKey;

        /**
        * Callback pointer to be used for notifications.
        * This pointer is not owned by this class.
        */
        MHWRMPowerStatePluginCallback* iNotificationCallback;  // not owned
    };

#include <hwrm/hwrmpowerstateplugin.inl>


#endif  // HWRMPOWERSTATEPLUGIN_H   
            
// End of File
