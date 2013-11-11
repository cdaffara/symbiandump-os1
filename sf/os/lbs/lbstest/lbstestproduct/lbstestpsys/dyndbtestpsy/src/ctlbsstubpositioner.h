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



#ifndef __CT_LBS_DYN_STUB_POSITIONER_H__
#define __CT_LBS_DYN_STUB_POSITIONER_H__

//  INCLUDES
#include <lbspositioninfo.h>
#include <lbs/epos_cpositioner.h>

// CONSTANTS

// FORWARD DECLARATIONS
class TPositionInfo;
class CT_LbsStubCommunicator;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsDynStubPositioner : public CPositioner
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsDynStubPositioner* NewL(TAny* aConstructionParameters);

		/**
        * Two-phased constructor.
        */
        static CT_LbsDynStubPositioner* NewLC(TAny* aConstructionParameters);
        
        /**
        * Destructor.
        */
        ~CT_LbsDynStubPositioner();

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
        * Removes a previously issued location request from the request
        * queue.
        * @param aObserver the positioner subsession that issued the 
        * request.
        */
        void CancelNotifyPositionUpdate(); 

    private:

        /**
        * C++ default constructor.
        */
        CT_LbsDynStubPositioner();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

    private: // Data 

        CT_LbsStubCommunicator* iStubCommunicator;

    };

#endif      // __CT_LBS_DYN_STUB_POSITIONER_H__   
            
// End of File
