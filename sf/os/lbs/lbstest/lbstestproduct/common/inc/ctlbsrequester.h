/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_REQUESTER_H__
#define __CT_LBS_REQUESTER_H__

//  INCLUDES
#include <e32base.h>
#include <lbs.h>
#include <test/testexecutelogger.h>

// CONSTANTS

// FORWARD DECLARATIONS
class MCT_LbsRequestObserver;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CT_LbsRequester : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return A new instance of this class.
        */
        static CT_LbsRequester* NewL(
            /* IN  */   TUid                	aPsyUid,
            /* IN  */   MCT_LbsRequestObserver*	aObserver,
            CTestExecuteLogger&					aLogger
            );
        
        /**
		* Two-phased constructor.
		* @return A new instance of this class.
		*/
		static CT_LbsRequester* NewL(
		    /* IN  */   MCT_LbsRequestObserver*		aObserver,
		    /* IN  */   TTimeIntervalMicroSeconds	aInterval,
		    CTestExecuteLogger&						aLogger
		    );
        
        /**
        * Destructor.
        */
        virtual ~CT_LbsRequester();

    protected:

        /**
        * C++ default constructor.
        */
        CT_LbsRequester(
            /* IN  */   MCT_LbsRequestObserver*   aObserver
            );

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TUid aPsyUid, CTestExecuteLogger& aLogger);

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TTimeIntervalMicroSeconds aInterval, CTestExecuteLogger& aLogger);

    public: // New functions
        
        const TTime& CompletionTimeUTC() const;
        TInt CompletionCode() const;
        void GetPositionInfo(
            TPositionInfo& aModuleInfo
                        );

		/**
        * Set Requestorstack
        */
        TInt SetRequestorStackL(const TDesC& aService, const TDesC& aContact);

        /**
        * Performs a request to iPsy. Notifies iObserver when request is complete
        */
        void StartRequest();


        /**
        * Performs a request to iPsy. Notifies iObserver when request is complete
        */
        void SetUpdateIntervalL(TTimeIntervalMicroSeconds aInterval);

        void SetUpdateType(const TInt& aUpdateType);

    public: // Functions from base classes

        void RunL();

        void DoCancel();

    protected: // New functions

        void RequestComplete();

    private:

        // By default, prohibit copy constructor
        CT_LbsRequester( const CT_LbsRequester& );
        // Prohibit assigment operator
        CT_LbsRequester& operator= ( const CT_LbsRequester& );
        
    	// Logger interface used for constructing result strings to be fed for the logger instance
    	inline CTestExecuteLogger& Logger() {return iLogger;};

    private:    // Data

        RPositionServer     iPosServer;
        RPositioner         iPositioner;

        MCT_LbsRequestObserver*   iObserver;
        TTime               iCompletionTimeUTC;
        TInt                iCompletionCode;
        TRequestStatus      iRequestStatus;
        TPositionInfo       iModuleInfo;
        
        CTestExecuteLogger	iLogger;
    };

#endif      // __CT_LBS_REQUESTER_H__  
            
// End of File
