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



#ifndef __CT_LBS_PSY_TEST_POSITIONER_H__
#define __CT_LBS_PSY_TEST_POSITIONER_H__

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cpositioner.h>

// FORWARD DECLARATIONS
class CPositionInfo;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsPsyTtfSimulator : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsPsyTtfSimulator* NewL(TInt aTimeout);

        /**
        * Two-phased constructor.
        */
        static CT_LbsPsyTtfSimulator* NewLC(TInt aTimeout);

        /**
        * Destructor.
        */
        virtual ~CT_LbsPsyTtfSimulator();

    public: // New functions

        /**
        * Starts a simulated position request.
        * @param aStatus the request status flag
        * @param aPosition the position that will be returned
        */
        void Start(TRequestStatus* aStatus,
                   TPositionInfo* aPosition,
                   TUid aImplUid);

    private: // Functions from base classes

        /**
        * From CActive
        * Called when an event has occured.
        */
        void RunL();

        /**
        * From CActive
        * Called by the Cancel method in the base class.
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsPsyTtfSimulator();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TInt aTimeout);

    protected:  // Data
        RTimer              iTimer;
        TRequestStatus*     iPositionStatus;
        TInt                iTimeout;
    };

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class CT_LbsPsyTestPositioner : public CPositioner
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsPsyTestPositioner* NewL(TAny* aConstructionParameters);
		
		/**
        * Two-phased constructor.
        */
        static CT_LbsPsyTestPositioner* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsPsyTestPositioner();

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
        * From CPositioner. It does nothing in this stub implementation
        */
        void CancelNotifyPositionUpdate();

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsPsyTestPositioner();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

    private: // Data
        CT_LbsPsyTtfSimulator*	iTtfSimulator;
    };

#endif      // __CT_LBS_PSY_TEST_POSITIONER_H__

// End of File
