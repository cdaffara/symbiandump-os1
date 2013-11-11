/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main control interface for SSY plug-ins.
*
*/


#ifndef SSYCONTROL_H
#define SSYCONTROL_H

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

#include <e32base.h>
#include <sensrvtypes.h>
#include <sensrvresource.hrh>

class MSsyCallback;

/**
* Main control class for SSY. Controls sensor basic functionality and provides 
* mandatory ECOM interface specific things.
* @since S60 5.0
*/
class CSsyControl : public CBase
    {
public:
    /**
    * Two-phase constructor
    *
    * @since S60 5.0
    * @param[in] aSsyCallback Reference to SSY callback instance.
    * @param[in] aImplementationUid UID of the implementation to create.
    * @return CSsyControl* Pointer to created CSsyControl object
    */  
    inline static CSsyControl* NewL( MSsyCallback& aSsyCallback,
                                     TUid aImplementationUid );

    /**
    * Virtual destructor
    *
    * @since S60 5.0
    */  
    inline virtual ~CSsyControl();

public:
    /**
    * Request for SSY to open a sensor channel asynchronously. 
    * Response to the request is delivered through MSsyCallback::ChannelOpened().
    * Initilizes SSY (and the sensor) to be ready for other control commands via
    * data and property providers. Multiple OpenChannel()-requests can be
    * active for different channels at the same time.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel that is requested to be opened
    */  
    virtual void OpenChannelL( TSensrvChannelId aChannelId ) = 0;

    /**
    * Request to close a sensor channel asynchronously. 
    * Response to the request is delivered through MSsyCallback::ChannelClosed().
    * Multiple CloseChannel()-requests can be active for different channels 
    * at the same time.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel that is reqeusted to be closed
    */  
    virtual void CloseChannelL( TSensrvChannelId aChannelId ) = 0;


private:
    
    /**
    * Destructor key for a ECOM component
    */
    TUid iDestructorIdKey;
    };

#include <ssycontrol.inl>

#endif //SSYCONTROL_H

// End of File


