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



#ifndef CMMGSMPHONESTORAGEPRIVATEUTILITY_H
#define CMMGSMPHONESTORAGEPRIVATEUTILITY_H

//  INCLUDES
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>

// CONSTANTS
const TInt KMaxTextLength = 20;
const TInt KNoInfoAvail = -1;
const TInt KPBMaxLen = 255;

/** Defined values for phonebook types */
const TUint8 KADNPhoneBook  = 1;
const TUint8 KFDNPhoneBook  = 2;
const TUint8 KSDNPhoneBook  = 3;
const TUint8 KVMBXPhoneBook = 4;
const TUint8 KBDNPhoneBook  = 5;
const TUint8 KMBDNPhoneBook = 6;


// CLASS DECLARATION

/**
 *  Derived class from TKeyArrayFix, used for
 *  sort PB Store cache array
 */
class TKeyArrayPtr : public TKeyArrayFix
    {
    public:
        inline TKeyArrayPtr( TInt aOffset, TKeyCmpNumeric aType )
            :TKeyArrayFix( aOffset, aType ) {}

        /**
         * Returns pointer to specified array object
         *
         *
         * @param aIndex index of wanted object
         * @return pointer to object in array
         */
        virtual TAny* At( TInt aIndex ) const
            {
            if ( aIndex == KIndexPtr )
                {
                return *( TUint8** )iPtr + iKeyOffset;
                }
            return *( TUint8** )iBase->
                Ptr( aIndex* sizeof( TUint8** ) ).Ptr() + iKeyOffset;
            };
    };

#endif      //  CMMGSMPHONESTORAGEPRIVATEUTILITY_H


// End of File
