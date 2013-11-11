/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __LBSQNNNOTIFIER_H_
#define __LBSQNNNOTIFIER_H_

//  INCLUDES
#include <e32base.h>
#include <techview/eikdialg.h>
#include <lbs/epos_cposprivacynotifier.h>
#include <lbs/lbsloccommon.h>
#include <lbs/epos_cposrequestor.h>

#include <lbs/test/tlbstestrequestchannel.h>
#include <lbs/test/lbsprivfwdefs.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CPeriodic;
// CLASS DECLARATION

/**
*  This class controls the notification dialog
*
*/
class CQnNPlugin : public CPosPrivacyNotifier, public MLbsTestChannelRequestObserver
    {
	private: // Constructors

    CQnNPlugin();
	void ConstructL();

    public:  // Constructors and destructor

	/*IMPORT_C*/ static CQnNPlugin* NewL();
    ~CQnNPlugin();

    public: // Functions from base classes

        /** 
        * From EPos_CPosPrivacyNotifier.h
        * This method is called when a new privacy query or notification has
        * been requested.
        *
        * @param aRequestId The id of the new query or notification request.
        */
        void HandleNewRequestL(/*IN*/ TPosQNRequestId aRequestId);


        /**
        * From EPos_CPosPrivacyNotifier.h
        * This method is called when a privacy query or notification request is
        * cancelled.
        *
        * It is not necessary to call @ref CompleteRequest() for a cancelled
        * request.
        *
        * @param aRequestId The id of the cancelled query or notification
        *   request.
        */
        void HandleRequestCancelled(/*IN*/ TPosQNRequestId aRequestId);

        /**
        * From EPos_CPosPrivacyNotifier.h
        * This method is called to cancel all outstanding requests.
        *
        * It is not necessary to call @ref CompleteRequest() for a cancelled
        * request.
        */
        void HandleAllRequestCancelled();

    protected: // Functions from base classes


	private:

        // By default, prohibit copy constructor
        CQnNPlugin( const CQnNPlugin& );
        // Prohibit assigment operator
        CQnNPlugin& operator= ( const CQnNPlugin& );

        static TInt TimerCallBack(TAny* aPtr);
        static TInt CompletionTimerCallBack(TAny* aPtr);
		void DismissDialog();
		void SendRequestDetails(TPosQNRequestId aRequestId, TLbsTestChannelMsgBase::TLbsTestChannelMsgType aMsgType);
		void ResetResponseTimer();
		void GetTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo);

		TLbsExternalRequestInfo::TRequestType ConvertTRequestType(CPosRequestor::TRequestType aRequestType);
		TLbsExternalRequestInfo::TNetworkType ConvertTNetworkType(CPosRequestor::TNetworkType aNetworkType);
		TLbsExternalRequestInfo::TFormatIndicator ConvertTRequestorIdFormat(CPosRequestor::TRequestorIdFormat aRequestorIdFormat);
		
		//From MReadPropPipeObserver
		//void OnReadyToReadL(CPrivFwReadPropPipe& aPipe);

		//From MLbsTestChannelRequestObserver
		virtual void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

	private: // Data

        CPeriodic*			iTimer;
        CPeriodic*			iCompletionTimer;
        TPosQNRequestId		iCompletionRequestId;
        TResponseType		iCompletionResponse;
        TTimeIntervalMicroSeconds* iCompletionDelay;

	 	//CPrivFwWritePropPipe* 		iResponseWriter;
		//CPrivFwNotifyReadPropPipe* 	iMsgReader;
		CTestRequestChannel* 		iHandlerChannel;
		CTestRequestChannel* 		iProxyChannel;
		//TBool						iPipesCreated;
		TBool						iChannelCreated;
		TBool						iRequestCancelled;
   };

#endif      //__LBSQNNNOTIFIER_H_
            
// End of File
