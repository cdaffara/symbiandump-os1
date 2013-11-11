/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TDESTOKENISER_H
#define TDESTOKENISER_H

//  INCLUDES
#include <e32base.h>

// CONSTANT DECLARATIONS
const TUint8 KPosDefaultSeparator = ',';

// CLASS DECLARATION

/**
 *  A helper class used to simplify parsing.
 *
 *  Creates tokens from a descriptor with values separated by a single character
 */
class TDesTokeniser
    {
    public:  // Constructors and destructor

        /**
         * C++ constructor.
         *
         * @param aTokenString the string to parse
         * @param aSeparator   the separator character
         */
        TDesTokeniser(const TDesC8& aTokenString);

    public:  // New functions

        /**
         * Checks whether the string contains more unseen tokens.
         *
         * @return true if more tokens, otherwise false
         */
        TBool HasMoreTokens();

        /**
         * Skips the next token.
         *
         * @param  aSeparator specific separator
         */
        void SkipToken(
        /* IN */    const TChar& aSeparator = KPosDefaultSeparator
        );

         /**
         * Skips the next token.
         *
         * @param  aNrOfTimes Number of times tokeniser should skip set token.
         */
        void SkipToken(
        /* IN */    const TInt aNrOfTimes
        );

        /**
         * Retreives the next token.
         *
         * @param  aSeparator specific separator
         * @return the next token
         */
        TPtrC8 NextToken(const TChar& aSeparator = KPosDefaultSeparator);

    private:  // Data
        TLex8   iLexer;
    };

#endif      // TDESTOKENISER_H

// End of File
