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



#ifndef __CT_LBS_CLIENT_REQUESTOR_H__ 
#define __CT_LBS_CLIENT_REQUESTOR_H__

//  INCLUDES
#include <e32base.h>
#include <lbs.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <test/testexecutelogger.h>


// CONSTANTS
const TInt KNoEvent = 42;

// FORWARD DECLARATIONS
class MCT_LbsRequestObserver;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CT_LbsClientRequestor : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return A new instance of this class.
        */
        static CT_LbsClientRequestor* NewL(
            /* IN  */   TUid                aPsyUid,
			/* IN  */   MCT_LbsRequestObserver*   aObserver,
			/* IN  */	const TDesC& aClientName,
		    CTestExecuteLogger&						aLogger
		    );
        
        /**
        * Destructor.
        */
        virtual ~CT_LbsClientRequestor();

    protected:

        /**
        * C++ default constructor.
        */
        CT_LbsClientRequestor(
			/* IN  */   MCT_LbsRequestObserver*   aObserver,
						TUid				aPsyUid
			);

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(const TDesC& aClientName, CTestExecuteLogger& aLogger);

    public: // New functions
        
        const TTime& CompletionTimeUTC() const;
       
		TInt CompletionCode() const;

		void ResetCompletionCode();

        /**
        * Performs a request to iPsy. Notifies iObserver when request is complete
        */
        void StartRequestL(TBool aDefaultPsy);

		void RequestModuleStatusEvent();

		TInt ReadModuleStatusEvent();

		void ContinueRequesting();

		void ClosePositioner();

		TBool ReadModuleStatusModulesApi(CPosModules* aDb, TPositionModuleId aModuleId);
		
		TInt ReadModuleStatusClientApi(TPositionModuleId aModuleId);

		void CancelModuleStatusEvent();

    public: // Functions from base classes

        void RunL();

        void DoCancel();

    protected: // New functions

        void RequestComplete();

    private:

        // By default, prohibit copy constructor
        CT_LbsClientRequestor( const CT_LbsClientRequestor& );
        // Prohibit assigment operator
        CT_LbsClientRequestor& operator= ( const CT_LbsClientRequestor& );
        
    	// Logger interface used for constructing result strings to be fed for the logger instance
    	inline CTestExecuteLogger& Logger() {return iLogger;};

    private:    // Data

        RPositionServer     iPosServer;
        RPositioner         iPositioner;
		TBuf<100>			iName;

		MCT_LbsRequestObserver*   iObserver;
        TTime               iCompletionTimeUTC;
        TInt                iCompletionCode;
        TRequestStatus      iRequestStatus;
        TPositionInfo       iModuleInfo;
		TUid				iPsyUid;
		TPositionModuleStatusEvent istatusEvent;
		TBool iContinueEventRequesting;
		
        CTestExecuteLogger	iLogger;
    };

#endif      // __CT_LBS_CLIENT_REQUESTOR_H__  
            
// End of File
