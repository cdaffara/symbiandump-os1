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
* This class maps the accessory API capabilities to capability definitions
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
    IMPORT_C static CAccMonitorCapMapper* NewL();

    /**
    * Destructor 
    */
    virtual ~CAccMonitorCapMapper();

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
      RAccMonCapabilityArray& aCapabilityArray ) const;

    /**
    * Get's all the capabilities from a subblocknamearray that are defined
    * for this API and maps those to this API's definitions. 
    * @param    aNameArray is the name array that needs to be resolved.
    * @param    aCapabilityArray is a reference to the array that is filled
    *           with the capabilities.
    */
    void GetCapabilitiesFromSubblockNameArray( 
      const CAccPolSubblockNameArray* aNameArray,
      RAccMonCapabilityArray& aCapabilityArray ) const;
    
    /**
    * Get's a generic ID capability definition from an AccMonitoring
    * capability definition.
    * @param    aCapabilityDefinition is the capability that needs to be
    *     resolved.
    * @return   Capability for the definition.
    */
    TUint32 GetCapabilityFromDefinitionL( 
      const TAccMonCapability aCapabilityDefinition ) const;
    
private:
        
    /**
    * Default Constructor 
    */
    CAccMonitorCapMapper();
        
  };
  
#endif // ACCMONITORCAPMAPPER_H

// End of File
