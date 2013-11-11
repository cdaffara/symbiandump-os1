/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_TEST_PSY_SIMULATEISA_H__
#define __CT_LBS_TEST_PSY_SIMULATEISA_H__

//  INCLUDES
#include <lbs/epos_cpositioner.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>
// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  PSY for simulate ISA position server
*/
class CT_LbsTestPsySimulateIsa :public CPositioner
    {
    public:  // Constructors and destructor

		/**
        * Two-phased constructor.
        */
        static CT_LbsTestPsySimulateIsa* NewLC(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsySimulateIsa* NewL(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        virtual ~CT_LbsTestPsySimulateIsa();

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


    protected:

        /**
        * C++ default constructor.
        */
        CT_LbsTestPsySimulateIsa();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

    private:    // Data
		TReal64		iLongitud;
        TReal64		iLatitud;
        TReal32		iAltitude;
    };

#endif      // __CT_LBS_TEST_PSY_SIMULATEISA_H__

// End of File
