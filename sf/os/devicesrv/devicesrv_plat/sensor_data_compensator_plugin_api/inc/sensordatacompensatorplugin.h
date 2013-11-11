/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor data compensation plugin API definition.
*
*/


#ifndef SENSORDATACOMPENSATIONPLUGIN_H
#define SENSORDATACOMPENSATIONPLUGIN_H

#include <e32std.h>

/**
 * Observer class for compensation value updates.
 *  
 * @since S60 5.0
 */
NONSHARABLE_CLASS(MSensorDataCompensatorObserver)
    {
    public:

    /**
    * Notification of compensation values update.
    *
    * @since S60 5.0
    */
    virtual void CompensationValuesUpdated() = 0;
    };

/**
 * Sensor data compensation plugin API definition.
 *  
 * @since S60 5.0
 */
NONSHARABLE_CLASS(CSensorDataCompensatorPlugin) : public CBase
    {
    public:
    
    /**
    * Retrieves item count for compensation values at current time.
    * Method is called prior fetching actual values with GetCompensationItem.
    * If plugin is not yet ready providing compensation values, it should
    * be indicated by returning KErrNotReady.
    *                            
    * @since S60 5.0
    * @return Count of compensation values.
    *         KErrNotReady if plugin is not ready providing compensation values.
    */
    virtual TInt GetCompensationItemCount() = 0;
    
    /**
    * Retrieves compensation values for requested data type. Values can
    * be configured for different device states per sensor data types
    * and there may be multiple values affecting compensation at current time.
    *                            
    * @since S60 5.0
    * @param aIndex Index for fetching compensation value from plugin.
    * @return The package of compensation value for example:
    *         TCompensationTypeAxisDataPckgBuf or
    *         TCompensationTypeAngleDataPckgBuf or
    *         TCompensationTypeDirectionDataPckgBuf
    */
    virtual TPtrC8 GetCompensationItem( TInt aIndex ) = 0;
    
    };

#endif  // SENSORDATACOMPENSATIONPLUGIN_H
