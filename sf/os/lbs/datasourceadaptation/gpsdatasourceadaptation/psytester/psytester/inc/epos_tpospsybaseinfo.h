/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TPOSPSYBASEINFO_H
#define TPOSPSYBASEINFO_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KMaximumNameLength = 25;

// CLASS DECLARATION
/**
*  Class containing base information about a PSY
*  The information includes Uid, implementation name 
*  and version information 
*/
class TPosPSYBaseInfo
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TPosPSYBaseInfo();

        /**
        * C++ constructor.
        * @param aModuleId the PSY Uid
        * @param aDisplayName the PSY Name
        * @param aVersion the PSY Version
        */
        TPosPSYBaseInfo(
        /* IN */ const TUid aModuleId, 
        /* IN */ const TDesC& aDisplayName, 
        /* IN */ const TInt aVersion
        );

    public: // New functions

        /**
        * Getter for version information
        * @return version number
        */
       TInt Version() const;

        /**
        * Getter for psy implementation name
        * @return psy information name.
        */
       TDesC& Name();

        /**
        * Getter for psy implementation uid
        * @return psy implementation uid.
        */
       TUid Uid() const;

    private: // Data
        
        TBuf<KMaximumNameLength> iName;
        TInt iVersion;
        TUid iUid;

    };

#endif      // TPOSPSYBASEINFO_H

// End of File