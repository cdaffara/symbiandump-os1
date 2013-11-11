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
*
*/



#ifndef _CMMENSTOREEXTINTERFACE_H_
#define _CMMENSTOREEXTINTERFACE_H_

// INCLUDE FILES
#include <e32def.h>
#include <et_tsy.h>

//  FORWARD DECLARATIONS
class TDataPackage;

// CLASS DECLARATION

/**
*  CMmENStoreExtInterface is used to provide an extension interface 
*  for system specific phone object extensions.
*/
class CMmENStoreExtInterface : public CBase
    {
    public: // Constructors and destructor
    
        /**
        * C++ default constructor.
        */
        CMmENStoreExtInterface(){}

        /**
        * Destructor
        */
        virtual ~CMmENStoreExtInterface(){}

        /**
        * Read function for reading individual entry or to get EN store info
        *         
        *
        * @param aIPC
        * @param aIndex index of thee EN number entry
        * @return KErrNotSupported
        */
        virtual TInt Read( TInt /*aIpc*/, TInt /*aIndex*/ ) = 0;
    };

#endif /* _CMMENSTOREEXTINTERFACE_H_ */

// End of file
