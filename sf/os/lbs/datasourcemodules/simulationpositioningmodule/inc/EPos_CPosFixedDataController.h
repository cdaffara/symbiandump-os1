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



#ifndef CPOSFIXEDDATACONTROLLER_H
#define CPOSFIXEDDATACONTROLLER_H

// INCLUDES
// #include <e32base.h>
#include <lbspositioninfo.h>
#include "EPos_CPosControllerBase.h"
#include "EPos_CPosSimulationPositioner.h"


/**
 *
 * A controller class that returns fixed location information.
 *
 */
class CPosFixedDataController : public CPosControllerBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosFixedDataController* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosFixedDataController();

    public: // From base classes

        /**
        * From CPosControllerBase. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status.
        */
        void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
        /* OUT    */    TRequestStatus& aStatus,
        /* IN */    	CPosSimulationPositioner& aSimPos
        );

        /**
        * From CPosControllerBase. Cancels position info request.
        */
        void CancelNotifyPositionUpdate();

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPosFixedDataController();

        /**
        * EPOC constructor.
        *
        */
        void ConstructL();

        // Prohibit copy constructor
        CPosFixedDataController( const CPosFixedDataController& );
        // Prohibit assignment operator
        CPosFixedDataController& operator= ( const CPosFixedDataController& );

    private:  // New functions

        /**
        * Completes all requests with the specified error code
        * @param aErrorCode The error code - KErrNone if everything went OK.
        * If error code is something else, the position is invalid.
        */
        void CompleteRequest(TInt aErrorCode);

    private: // Functions from base classes

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request.
        */
        void DoCancel();

    };

#endif      // CPOSFIXEDDATACONTROLLER_H

// End of File
