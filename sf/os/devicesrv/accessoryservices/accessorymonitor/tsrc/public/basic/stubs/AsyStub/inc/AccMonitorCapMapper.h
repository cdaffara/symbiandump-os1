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
* Description:  AccMonitorCapMapper functionality.
*
*/


#ifndef ACCMONITORCAPMAPPER_H
#define ACCMONITORCAPMAPPER_H

// INCLUDES
#include <e32base.h>
#include <AccPolSubblockNameArray.h>
#include <AccMonitorInfo.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION
/** 
* This class is a maps the accessory capabilities to capability definitions
* in this API.
*
* The CAccMonitorCapMapper class provides fuctions to map generic id
* definitions to Accessory Monitoring API capability definitions. The Accessory
* Monitoring API definitions are defined in AccMonitorCapabilities.h file. The
* API definitions can also be mapped to generic id definitions.
*/ 
NONSHARABLE_CLASS( CAccMonitorCapMapper ): public CBase 
    {
public:     // Constructors & destructors
  
    /**
    * Symbian two phased constructor.
    */
    static CAccMonitorCapMapper* NewL();

    /**
    * Destructor 
    */
    ~CAccMonitorCapMapper();


  
    /**
    * Get's all the capabilities from the GID Header that are defined for
    * this API and maps those to this API's definitions. 
    * @param    aGenericIDHeader is the generic id header that needs to be
    *     resolved.
    * @param    aCapabilityArray is a reference to the array that is filled
    *           with the capabilities.
    * @return   KErrNone if query succeeded, otherwise standard Symbian
    *           error codes.
    */
    TInt GetCapabilitiesFromGIDHeader( 
      const TAccPolGenericID& aGenericIDHeader,
      RAccMonCapabilityArray& aCapabilityArray );

public:     // Methods with functionality

    /**
    * Get's all the capabilities from a subblocknamearray that are defined
    * for this API and maps those to this API's definitions. 
    * @param    aNameArray is the name array that needs to be resolved.
    * @param    aCapabilityArray is a reference to the array that is filled
    *           with the capabilities.
    * @return   KErrNone if query succeeded, otherwise standard symbian
    *           error codes.
    */
    TInt GetCapabilitiesFromSubblockNameArray( 
      const CAccPolSubblockNameArray* aNameArray,
      RAccMonCapabilityArray& aCapabilityArray );

    /**
    * Get's all the capabilities defined in the array and constructs a
    * generic id of those definitions.
    * @param    aGenericIDHeader is the generic id header that needs to be
    *           constructed.
    * @param    aCapabilityArray is the array where the capabilities exist.
    * @return   KErrNone if query succeeded otherwise standard symbian
    *           error codes.
    */
    /*TInt GetGIDHeaderFromCapabilities( 
      TAccPolGenericID& aGenericIDHeader,
      const RAccMonCapabilityArray& aCapabilityArray );*/
    
    /**
    * Get's the deivce address for some generic ID.
    * @param    aGenericIDHeader is the generic id header that needs to be
    *           resolved.
    * @return   The device address or standard Symbian error codes.
    */
    TUint64 GetDeviceAddress( 
      const TAccPolGenericID& aGenericIDHeader ) const;
    
    /**
    * Get's a generic ID capability definition from an AccMonitoring
    * capability definition.
    * @param    aCapabilityDefinition is the capability that needs to be
    *     resolved.
    * @return   KErrNone if query succeeded, KErrNotFound if no capabilities
    *           exist for this accessory. Otherwise standard symbian error
    *     codes.
    */
    TUint32 GetCapabilityFromDefinitionL( 
      const TAccMonCapability aCapabilityDefinition );
    
    /**
    * Get's a generic ID header capability definition from an AccMonitoring
    * capability definition. 
    * @param    aCapabilityDefinition is the gid header capability that
    *     needs to be resolved.
    * @return   KErrNone if query succeeded, KErrNotFound if no capabilities
    *           exist for this accessory. Otherwise standard symbian error
    *     codes.
    */
    TUint32 GetHeaderCapabilityFromDefinitionL( 
      const TAccMonCapability aCapabilityDefinition );
    
    /**
    * Default Constructor 
    */
    CAccMonitorCapMapper();

private:
    
    /**
    * Symbian OS 2nd phase constructor 
    */
    void ConstructL();
        
  };
  
#endif // ACCMONITORCAPMAPPER_H

// End of File
