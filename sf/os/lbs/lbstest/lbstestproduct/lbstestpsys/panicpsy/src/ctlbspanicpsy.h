/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_PANIC_PSY_H__
#define __CT_LBS_PANIC_PSY_H__

//  INCLUDES
#include <lbspositioninfo.h>
#include "epos_cpositioner.h"

// CONSTANTS

// FORWARD DECLARATIONS
class TPositionInfo;

// CLASS DECLARATION

/**
* Used to "crash" Epos, this is done when this PSY panics
*
*/
class CT_LbsPanicPsy : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsPanicPsy* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsPanicPsy* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsPanicPsy();

    protected:  // Functions from base classes

		/**
        * From CPositioner. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        */
		void NotifyPositionUpdate(
		/* IN/OUT */	TPositionInfoBase& aPosInfo,
		/* OUT	  */	TRequestStatus& aStatus
		);

		/**
        * From CPositioner. Cancels position info request.
        */
        void CancelNotifyPositionUpdate();
        

    private:

         /**
        * C++ default constructor.
        */
        CT_LbsPanicPsy();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

        // By default, prohibit copy constructor
        CT_LbsPanicPsy( const CT_LbsPanicPsy& );
        // Prohibit assigment operator
        CT_LbsPanicPsy& operator= ( const CT_LbsPanicPsy& );




    };

#endif      // __CT_LBS_PANIC_PSY_H__   
            
// End of File
