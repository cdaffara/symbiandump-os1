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



#ifndef __CT_LBS_STUB_COMMUNICATOR_H__
#define __CT_LBS_STUB_COMMUNICATOR_H__

//  INCLUDES
#include <e32def.h>
#include <e32base.h>
#include <lbspositioninfo.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CPositionInfo;

// CLASS DECLARATION

class CT_LbsStubCommunicator : public CActive
    {
	public:  // Destructor

        /**
        * Used instead of destructor.
        */
		void Close();

    public: // New functions

        /**
        * Retreives a pointer to the object.
        *
        * @return a pointer to the singleton object
        */
		static CT_LbsStubCommunicator* GetInstanceL();

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
        * Removes a previously issued location request from the request
        * queue.
        * @param aObserver the positioner subsession that issued the 
        * request.
        */
        void CancelNotifyPositionUpdate();

    protected:  // Constructors
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsStubCommunicator* NewL();
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsStubCommunicator* NewLC();

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsStubCommunicator();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CT_LbsStubCommunicator();

        // Prohibit copy constructor
        CT_LbsStubCommunicator( const CT_LbsStubCommunicator& );
        // Prohibit assigment operator
        CT_LbsStubCommunicator& operator= ( const CT_LbsStubCommunicator& );

    private:  // New functions

        /**
        * Will be executed by the active scheduler when a comm port read has 
        * been completed.
        */
        void RunL();

        /**
        * Cancels any outstanding comm port read request
        */
        void DoCancel();



    private:    // Data
		TInt iReferenceCounter;
        RTimer				iTimer;

        TInt				iTimeout;
        TInt				iRequestCounter;

        // for single request psy:s
        TRequestStatus* iRequestStatus;
		TPositionInfo*		iPosition;

    };

#endif      // __CT_LBS_STUB_COMMUNICATOR_H__
            
// End of File
