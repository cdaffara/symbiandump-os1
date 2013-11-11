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



#ifndef CPOSPSYCRTESTHANDLER_H
#define CPOSPSYCRTESTHANDLER_H

//  INCLUDES
#include <barsread.h>
#include <barsc.h>


#include "epos_cpospsycrlog.h"

// FORWARD DECLARATION


// CONSTANTS


// CLASS DECLARATION
/**
*  Driver for testing the CR capabilities of a PSY
*/
class CPosPSYCRTestHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYCRTestHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYCRTestHandler();
        

    public: // New functions
		
		/**
        * Starts the PSY tester to test 
        * with the PSY at the specified index
        *
		* @param TInt aIndex, The index for the PSY to be tested
        */
        void StartPSYCRTesterL(
        /* IN */ TUid aUid
        );

    
    private:
    	CPosPSYCRTestHandler();  
    	void ConstructL();  
    	void TestCRValueCapabilitiesL(TUid aUid);
    	
    private:
    
    
		CPosPSYCRLog* iLog;
    };

#endif      // CPOSPSYTESTHANDLER_H

// End of File
