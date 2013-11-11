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



#ifndef CMMPBLIST_H
#define CMMPBLIST_H

//  INCLUDES
#include "cmmtsyobjectlist.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmPhoneBookStoreTsy;

// CLASS DECLARATION

/**
 *  This class provides the means for the Phone object (CMmPhoneTsy class) to 
 *  keep track of opened phonebooks. When a phonebook object is opened it should
 *  be put on the Phone's PBList. 
 */
NONSHARABLE_CLASS( CMmPBList ) : public CMmTsyObjectList
    {
    public:     // Constructors and destructor
    
        /**
         * Two-phased constructor.
         * @param CMmPhoneTsy* aMmPhone pointer to the Phone
         * @return created CallList object 
         */        
        static CMmPBList* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor.
         */
        virtual ~CMmPBList();

    public: // New functions
    
        /**
         * Returns the PBStore that has the given index in container
         *
         * @param aIndex index of the PBStore which is to be returned.
         * @return PBStore object
         */
        virtual CMmPhoneBookStoreTsy* GetMmPBByIndex( const TInt aIndex );

    private:
    
        /**
         * C++ default constructor.
         */
        CMmPBList();

        /**
         * Class attributes are created in ConstructL
         */
        void ConstructL();
    };

#endif // CMMPBLIST_H

//  End of file
