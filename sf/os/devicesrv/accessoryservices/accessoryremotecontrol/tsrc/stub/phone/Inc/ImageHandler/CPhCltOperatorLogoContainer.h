/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Operator logo container.
*
*/

#ifndef CPHCLTOPERATORLOGOCONTAINER_H
#define CPHCLTOPERATORLOGOCONTAINER_H

// INCLUDES
#include   <CPhCltBaseImageParams.h>
#include    <fbs.h>

// CLASS DECLARATION

/**
*  Image handler Subsession.
*
*  @since Series60 3.0
*/
NONSHARABLE_CLASS( CPhCltOperatorLogoContainer ) : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Destructor.
        */
        ~CPhCltOperatorLogoContainer();
        
        /**
        * Constructor.
        * @param aCountryCode country code for operator logo
        * @param aNetworkCode network code for operator logo
        * @param aBitmap bitmap to store.
        */
        CPhCltOperatorLogoContainer( 
            const TPhCltCountryCode aCountryCode,
            const TPhCltNetworkCode aNetworkCode,
            const TPhCltExtOperatorLogoType aLogoType, 
            CFbsBitmap* aBitmap );
            
        // Stored image.
        CFbsBitmap* iBitmap;
        
        // Operator logo country code.
        TPhCltCountryCode iCountryCode;
        
        // Operator logo network code.
        TPhCltNetworkCode iNetworkCode;
        
        // Operator logo type.
        TPhCltExtOperatorLogoType iLogoType;        
    };

#endif // CPHCLTOPERATORLOGOCONTAINER_H

// End of file.