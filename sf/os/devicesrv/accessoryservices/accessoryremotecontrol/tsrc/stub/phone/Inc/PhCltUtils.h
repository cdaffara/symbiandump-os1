/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It provides functionality to check if character is valid and 
*                remove invalid characters.
*
*/


#ifndef PHCLTUTILS_H
#define PHCLTUTILS_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  It contains utility methods.
*
*  This class can be used to remove illegal characters from descriptors
*  before dialing. However, it shouldn't be done before you have ensured
*  that string is acceptable by application specific grammar.
*  
*  Thus, you should process as follows:
*       1. Check that string is acceptable. (application specific)
*       2. If not, stop. Otherwise continue.
*       3. Convert to string acceptable by phoneclient -
*          typically, you just have to remove invalid characters.
*       4. Do whatever you wish with the string, e.g. dial.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class PhCltUtils
    {
    public:  // New functions

        /**
        * Checks if character is acceptable.
        *
        * @param aChar character.
        * @return ETrue iff characters is ok.
        */
        IMPORT_C static TBool IsValidChar( TChar aChar );

        /**
        * Removes all illegal characters from descriptor.
        *
        * @param aString string from which those characters are removed.
        */
        IMPORT_C static void RemoveInvalidChars( TDes& aString );

        /**
        * Removes SIP prefixses and domain part from descriptor.
        *
        * @since 3.0
        * @param aString string from which those characters are removed.
        */        
        IMPORT_C static void RemoveURIPrefixdAndDomainChars( TDes& aString );
    };
        
#endif      // PHCLTUTILS_H   
            
// End of File
