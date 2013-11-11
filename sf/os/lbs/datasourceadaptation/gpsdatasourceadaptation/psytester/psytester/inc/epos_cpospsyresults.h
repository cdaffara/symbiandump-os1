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



#ifndef CPOSPSYRESULTS_H
#define CPOSPSYRESULTS_H

//  INCLUDES
#include "epos_cpospsyresult.h"

// CLASS DECLARATION
/**
*  Holder of PSY test results
*/
class CPosPSYResults : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYResults* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYResults();

    public: // New functions

        /**
        * AddResult. Adds a new result to the result array
        * @param CPosPSYResult aResult container for test information
        */
        void AddResultL(
        /* IN */ const CPosPSYResult& aResult
        );

        /**
        * NumberOfResults. Returns the number of results in the results array
        * @param CPosPSYResult aResult container for test information
        * @return TInt, number of stored test results.
        */
        TInt NumberOfResults() const;
        
        /**
        * GetResult. Gets a pointer to a result in the result array
        * @param TInt aIndex index to retrieve the result from in the array
        * @return CPosPSYResult*, the result on the specified position
        */
        CPosPSYResult* GetResult(
        /* IN */ const TInt aIndex
        );
        
        /**
        * ClearResults. Clears the result array
        */
        void ClearResults();
        
    private:

        /**
        * C++ default constructor.
        */
        CPosPSYResults();    
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPosPSYResults( const CPosPSYResults& );
        
        // Prohibit assigment operator
        CPosPSYResults& operator= ( const CPosPSYResults& );

    private:    // Data
        
        RPointerArray<CPosPSYResult> iResultArray;

    };

#endif      // CPOSPSYRESULTS_H

// End of File
