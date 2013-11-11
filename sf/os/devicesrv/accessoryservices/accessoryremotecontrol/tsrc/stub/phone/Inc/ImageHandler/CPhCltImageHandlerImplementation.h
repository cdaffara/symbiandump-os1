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
* Description:  Implementation of the PhCltExtension Image handler interface.
*
*/



#ifndef CPHCLTIMAGEHANDLERIMPLEMENTATION_H
#define CPHCLTIMAGEHANDLERIMPLEMENTATION_H


//  INCLUDES
#include    <CPhCltImageHandler.h>
#include    "RPhCltImageHandler.h"
#include    "RPhCltServer.h"

/**
*  Implementation of image handler interface.
*
*  @lib PhoneClient.lib
*  @since Series60 3.2
*/
NONSHARABLE_CLASS ( CPhCltImageHandlerImplementation )
:   public CPhCltImageHandler
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPhCltImageHandlerImplementation* NewL();

        /**
        * Destructor.
        */
        ~CPhCltImageHandlerImplementation();

    public: // Functions from base classes

        /**
        * @see CPhCltImageHandler.
        */        
        TInt SaveImages( CPhCltImageParams& aParams );
            
        /**
        * @see CPhCltImageHandler.
        */
        TInt LoadImages( CPhCltImageParams*& aParams );

        /**
        * @see CPhCltImageHandler::CPhCltBaseImageParamsL().
        */
        virtual CPhCltImageParams* 
            CPhCltBaseImageParamsL( const TPhCltImageType aType ); 

    private:

        /**
        * C++ constructor.
        */
        CPhCltImageHandlerImplementation();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data
        RPhCltImageHandler iStillHandler;
        
        //RPhCltServer class instance creates phonesession
        RPhCltServer iServer;

    };

#endif      // CPHCLTIMAGEHANDLERIMPLEMENTATION_H

  
// End of File
