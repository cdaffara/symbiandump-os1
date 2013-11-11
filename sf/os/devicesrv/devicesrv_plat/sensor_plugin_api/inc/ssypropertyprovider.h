/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property provider interface for SSY plug-ins.
*
*/



#ifndef SSYPROPERTYPROVIDER_H
#define SSYPROPERTYPROVIDER_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h>

/** 
The UID of the ssy property provider API.
*/
static const TUid KSsyPropertyProviderInterface1 = {0x200159F0};

// CLASS DECLARATION

/**
* Channel property provider implementation
 */

class MSsyPropertyProvider
    {
public:
    /**
    * Check if property value affects other sensor channels already open.
    * If the new property value is legal but affects somehow other channels' 
    * properties, SSY must return list of the affected channels so that the 
    * sensor server can check if the client is allowed to set this property. 
    * If the SSY value is not legal SSY must leave with KErrArgument-value.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this operation
    * @param[in] aProperty Property to be checked. 
    * @param[out] aAffectedChannels Return list of the channel which will be 
    *             affected if the property value will be set.
    * @leave KErrNotFound If the property is not found.
    */  
    virtual void CheckPropertyDependenciesL
                              ( const TSensrvChannelId aChannelId,
                                const TSensrvProperty& aProperty,
                                RSensrvChannelList& aAffectedChannels ) = 0;   

    /**
    * Set property for the channel. SSY is responsible for checking that the
    * property setting is legal. 
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this set property 
    *            operation
    * @param[in] aProperty Rereference to a property object to be set.
    *            iSecurityInfo of the property contains client capabilities.
    * @leave KErrNotFound If the property is not found.
    * @leave KErrArgument If the property value is invalid.
    * @leave KErrPermissionDenied If client capabilities are not enough to 
    *        set the property.
    */  
    virtual void SetPropertyL(  const TSensrvChannelId aChannelId,
                                const TSensrvProperty& aProperty ) = 0;    
      
    /**
    * Get channel property value. The property parameter contains channel id and
    * item index. 
    *
    * In case of array property, the required array index is also included.
    * If array index is not defined and the property is an array property,
    * then array info property object must be returned (ESensrvArrayPropertyInfo).
    *
    * SSY fills values and attributes to the property object.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this get property operation
    * @param[in, out] aProperty Reference to a property object to be filled with 
    * property values and attributes.
    * @leave KErrNotFound If the property is not found.
    */  
    virtual void GetPropertyL( const TSensrvChannelId aChannelId, 
                               TSensrvProperty& aProperty ) = 0;    

    /**
    * Get all channel properties. Returns all properties which are related to this 
    * channel.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this get all properties 
    *            operation
    * @param[out] aChannelPropertyList List of the all properties of the channel.
    */  
    virtual void GetAllPropertiesL( const TSensrvChannelId aChannelId, 
                                    RSensrvPropertyList& aChannelPropertyList ) = 0;    

    /** 
    * Returns a pointer to a specified interface - to allow future extension
    * of this class without breaking binary compatibility
    *
    * @since S60 5.0
    * @param aInterfaceUid Identifier of the interface to be retrieved
    * @param aInterface A reference to a pointer that retrieves the specified interface.
    */
    virtual void GetPropertyProviderInterfaceL( TUid aInterfaceUid, 
                                                TAny*& aInterface ) = 0;
    };


#endif //SSYPROPERTYPROVIDER_H

// End of File


