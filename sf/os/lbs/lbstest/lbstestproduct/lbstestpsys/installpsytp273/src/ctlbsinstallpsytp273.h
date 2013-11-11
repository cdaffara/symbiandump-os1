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



#ifndef __CT_LBS_INSTALL_PSYTP273_H__
#define __CT_LBS_INSTALL_PSYTP273_H__

//  INCLUDES
#include "epos_cpositioner.h"

// CONSTANTS

// FORWARD DECLARATIONS
class TPositionInfo;

// CLASS DECLARATION

/**
* Used to "crash" Epos, this is done when this PSY panics
*
*/
class CT_LbsInstallPsyTp273 : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsInstallPsyTp273* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsInstallPsyTp273* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsInstallPsyTp273();

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
        CT_LbsInstallPsyTp273();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

        // By default, prohibit copy constructor
        CT_LbsInstallPsyTp273( const CT_LbsInstallPsyTp273& );
        // Prohibit assigment operator
        CT_LbsInstallPsyTp273& operator= ( const CT_LbsInstallPsyTp273& );




    };

#endif      // __CT_LBS_INSTALL_PSYTP273_H__   
            
// End of File
