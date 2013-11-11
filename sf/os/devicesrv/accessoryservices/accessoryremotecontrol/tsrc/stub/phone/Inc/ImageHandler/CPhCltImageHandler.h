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
* Description:  Image storing and loading interface.
*
*/


#ifndef CPhCltImageHandler_H
#define CPhCltImageHandler_H

#include <e32base.h>
#include "CPhCltImageParams.h"

// FORWARD DECLARATIONS
class RPhCltServer;
class CPhCltImageParams;


/**
*  Interface used in image storing to PhoneServer.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
class CPhCltImageHandler : public CBase
    {
    
    public:  // Constructors 
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltImageHandler* NewL();
        
    public: // New functions

        /**
        * Save image(s)
        * @param aParam parameters passed 
        * @return KErrNone if successful, Symbian error code otherwise.
        * NOTE: can be used to delete operator logos,
        * @see CPhCltExtOperatorLogoParams::SetCodesL
        * @see CPhCltExtOperatorLogoParams::AddImageL        
        */
        IMPORT_C virtual TInt SaveImages( CPhCltImageParams& aParams ) = 0;
            
        /**
        * Load image(s)
        * @param aParam parameters in return passed 
        * @return KErrNone if successful, Symbian error code otherwise.
        */
        IMPORT_C virtual TInt LoadImages( CPhCltImageParams*& aParams ) = 0;

        /**
        * Creates image parameter class.
        * @param aType type of the implementation class.
        * @return CPhCltBaseImageParams implementation class. 
        *         NULL if not supported.
        *         Ownership transferred.
        */
        IMPORT_C virtual CPhCltImageParams* 
            CPhCltBaseImageParamsL( const TPhCltImageType aType ) = 0; 
            
      protected:

        /**
        * C++ constructor.
        */
       CPhCltImageHandler();
    };


#endif      // CPhCltImageHandler_H 

// End of File
