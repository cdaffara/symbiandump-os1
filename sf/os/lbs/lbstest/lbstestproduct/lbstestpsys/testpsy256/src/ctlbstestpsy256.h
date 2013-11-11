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



#ifndef CTESTPSY256_H
#define CTESTPSY256_H

//  INCLUDES
#include <lbs/epos_cpositioner.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsTestPsy256 : public CPositioner
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy256* NewLC(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy256* NewL(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsTestPsy256();

    protected:  // Functions from base classes

       /**
		* Requests position info asynchronously.
		*
		* @param aPosInfo A reference to a position info object. This object
		*                 must be in scope until the request has completed.
		* @param aStatus The request status
		*/
        void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
		/* OUT    */	TRequestStatus& aStatus
		);

		/**
        * From CPositioner. Cancels position info request.
        */
        void CancelNotifyPositionUpdate(); 

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsTestPsy256();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

    private: //data members

		TReal64		iLongitud;
        TReal64		iLatitud;
        TReal32		iAltitude;

    };

#endif      // CTESTPSY256_H

// End of File
