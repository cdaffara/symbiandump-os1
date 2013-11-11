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



#ifndef CPOSPSYECOMHANDLER_H
#define CPOSPSYECOMHANDLER_H

//  INCLUDES
#include "epos_tpospsyinfo.h"

// CONSTANTS
const TUid KPosPsyInterfaceUid = {0x101f7a7c};
//const TInt KVisibilityPos = 60;
const TInt KVisibilityPos = 1;

// CLASS DECLARATION

/**
*  Class handles interaction with ECom to get information about installed PSYs
*
*/
class CPosPSYEComHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYEComHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYEComHandler();

    public: // New functions

		/**
        * void GetPsyInfoList()
        * @param RArray aInfoList, reference to array that should be filled with 
        *        TPosPSYBaseInfo objects     
        */
        void GetPsyInfoListL(
        /* OUT */    RArray<TPosPSYBaseInfo>& aPSYInfoArray
        );
		
        /**
        * void GetSpecificPSYInformationL()
        * @param TPosPSYInfo& aPSYInfo, object to be filled in 
        *		 with PSY specific information 
        */
		void GetSpecificPSYInformationL(
        /* IN/OUT */    TPosPSYInfo& aPSYInfo
        );       
   
    private:

        /**
        * C++ default constructor.
        */
        CPosPSYEComHandler();

        // By default, prohibit copy constructor
        CPosPSYEComHandler( const CPosPSYEComHandler& );
        // Prohibit assigment operator
        CPosPSYEComHandler& operator= ( const CPosPSYEComHandler& );

    };

#endif      // CPOSPSYECOMHANDLER_H

// End of File
