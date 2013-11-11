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



#ifndef CPOSDATASIMULATIONCONTROLLER_H
#define CPOSDATASIMULATIONCONTROLLER_H

//  INCLUDES
#include <lbspositioninfo.h>
#include "EPos_TPosDataSimulator.h"
#include "EPos_SimPsyConstants.h"
#include "EPos_CPosControllerBase.h"
#include "EPos_CPosSimulationPositioner.h"

// CLASS DECLARATION

/**
 * A controller class that handles requests.
 *
 */
class CPosDataSimController : public CPosControllerBase
    {
    public:  // Constructors and destructors

        enum TPosSimulationState
        {
        EPosCheckIfFailRequest,
        EPosTransferAndComplete
        };

        /**
        * Two-phased constructor.
        *
        * @param aSimulationFile The name of the simulation file to read from.
        * @param aTimeRelativePlayback turns object to work in time relative playback mode
        */
        static CPosDataSimController* NewL(
        /* IN  */       const TDesC& aSimulationFile,
        /* IN  */       TBool aTimeRelativePlayback
        );

        /**
        * Destructor.
        */
        virtual ~CPosDataSimController();

    public: // From base classes

        /**
        * From CPosControllerBase. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        * @param aSimPos Reference to parent to call for request complete.
        */
        void NotifyPositionUpdate(
        /* IN/OUT */    TPositionInfoBase& aPosInfo,
        /* OUT    */    TRequestStatus& aStatus,
        /* IN     */	CPosSimulationPositioner& aSimPos
        );

        /**
        * From CPosControllerBase. Cancels position info request.
        * @param aSimPos Reference to parent who called this function
        */
        void CancelNotifyPositionUpdate(const CPosSimulationPositioner& aSimPos);

    private: // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aTimeRelativePlayback turns object to work in time relative playback mode
        */
        CPosDataSimController(TBool aTimeRelativePlayback);

        /**
        * EPOC constructor.
        *
        * @param aSimulationFile The name of the simulation file to read from.
        */
        void ConstructL(
        /* IN  */       const TDesC& aSimulationFile
        );

        // Prohibit copy constructor
        CPosDataSimController( const CPosDataSimController& );
        // Prohibit assigment operator
        CPosDataSimController& operator= ( const CPosDataSimController& );

    private:  // New functions

        /**
        * Completes all requests with the specified error code
        * @param aErrorCode The error code - KErrNone if everything went OK.
        * If error code is something else, the position is invalid.
        */
        void CompleteRequest(TInt aErrorCode);

        /**
        * Sets completion code and waits for "Time to fix".
        */
        void SetCompletionCodeAndWait();

    private: // Functions from base classes

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request
        */
        void DoCancel();

        /**
        * From CActive. Handles a leave occurring in the request
        * completion event handler RunL()
        * @param aError The leave code
        */
        TInt RunError(TInt aError);

    private:    // Data
        TRequestStatus*           iRequestStatus;
        TPosition				  iSimulatedPosition;
        TPosition				  iBasePosition;
        TPosDataSimulator         iDataSimulator;
        TBuf8<KMaxSentenceLength> iInputBuffer;
        CDesC8Array*              iSimDataArray;

        TPosSimulationState       iSimulationState;
        TBool                     iSimulationInProcess;
        TInt                      iCompleteCode;
        TInt                      iNumRequests;
        TTime                     iRequestStartTime;
        TTime                     iLastRequestTime;

        RTimer                    iTimer;
    };

#endif      // CPOSDATASIMCONTROLLER_H

// End of File
