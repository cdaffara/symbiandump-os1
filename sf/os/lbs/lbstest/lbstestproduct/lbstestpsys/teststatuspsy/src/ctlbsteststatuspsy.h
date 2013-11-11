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



#ifndef __CT_LBS_TEST_STATUS_PSY_H__
#define __CT_LBS_TEST_STATUS_PSY_H__

//  INCLUDES
#include <f32file.h>
#include "epos_cpositioner.h"

// CONSTANTS
//File name. Beware, also declared in FT_CPosTp213.h !!!!
_LIT(KDummyFileName, "c:\\teststatuspsyisnotunloaded.txt");

// FORWARD DECLARATIONS
class CPositionInfo;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsTestStatusPsy : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestStatusPsy* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestStatusPsy* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsTestStatusPsy();

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
        CT_LbsTestStatusPsy();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

		static TInt SetDeviceStatus(
        /* IN */ TAny* aThis
		);

		void ReportStatus(
		/* IN */ const TInt aDeviceStatus, 
		/* IN */ const TInt aDataQuality
		);

		void ChangeDeviceStatus();
        
		// By default, prohibit copy constructor
        CT_LbsTestStatusPsy( const CT_LbsTestStatusPsy& );
        // Prohibit assigment operator
        CT_LbsTestStatusPsy& operator= ( const CT_LbsTestStatusPsy& );
	
	private: //Data
		MPositionerStatus* iPositioner;
		CPeriodic* iPeriodicTimer;
		TInt iReportStatusNr;

		enum TReportStatus
		{
		EReportReadyFirst,
        EReportDisabled,
		EReportErroneous,
		EReportInitialising,
		EReportStandby,
		EReportActive,
		EReportError,
		EReportUnknown,
		EReportInactive,
		EReportReadyLast
		};

		//file related
		RFs ifileserver;
		RFile idummyfile;
		CFileMan* ifileMan;


    };

#endif      // __CT_LBS_TEST_STATUS_PSY_H__   
            
// End of File
