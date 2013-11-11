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




#ifndef __CT_LBS_TEST_PSY_PARTIAL_UPDATE2_H__
#define __CT_LBS_TEST_PSY_PARTIAL_UPDATE2_H__

//  INCLUDES
#include <e32def.h>
#include <e32base.h>	
#include <lbs/epos_cpositioner.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>
#include <f32file.h>


// CONSTANTS

// FORWARD DECLARATIONS
class CT_LbsTestProxyPsyBase;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsTestPsyPartialUpdate2 : public CPositioner
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsyPartialUpdate2* NewLC(TAny* aConstructionParameters);
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsyPartialUpdate2* NewL(TAny* aConstructionParameters);
        
        /**
        * Destructor.
        */
        ~CT_LbsTestPsyPartialUpdate2();

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
        CT_LbsTestPsyPartialUpdate2();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);
        
        TInt OpenFileForAppend( );

    private:
           	CT_LbsTestProxyPsyBase*		    iRequestHandler;
           	RFs                         iFs;
            RFile                       iFile;
            TFileText                   iFileText; 

    };

#endif      // __CT_LBS_TEST_PSY_PARTIAL_UPDATE2_H__   
            
// End of File
