/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file Te_lbsnetgatewaySuiteStepBase.h
*/

#if (!defined __TE_LBSNETGATEWAY_STEP_BASE__)
#define __TE_LBSNETGATEWAY_STEP_BASE__
#include <test/testexecutestepbase.h>
// Please add your include here if you have 
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsadmin.h>
#include "lbsnetinternalapi.h"
#include "lbsngnlmint.h"
#include "lbsnrhngmsgs.h"
#include "lbspsyngmsgs.h"
#include "lbsreffnpint.h"
#include "lbsassistancedatacacheapi.h"
#include "lbsnetgatewayprotocoltestapi.h"
#include "lbsprocesssupervisor.h"


/* Observer class that notifies of incoming Network Location messages
*/
class MNetworkLocationChannelObserver
	{
public:
	virtual void ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage) = 0;
	};
	
/* Active Object to handle sending and receiving messages
   across the Network Location Request/Response interfaces
*/
class CNetworkLocationChannel : public CActive
	{
public:
	static CNetworkLocationChannel* NewL(MNetworkLocationChannelObserver& aObserver);
	~CNetworkLocationChannel();
	
	void RunL();
	void DoCancel();
	
	TInt SendNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage);
	
private:
	CNetworkLocationChannel(MNetworkLocationChannelObserver& aObserver);
	void ConstructL();
	
	void NotifyNetworkLocationMessage();

private:
	MNetworkLocationChannelObserver& iObserver;
	RLbsNetworkLocationRequests iRequests;
	RLbsNetworkLocationResponses iResponses;
	};
		

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_lbsnetgatewaySuiteStepBase : public CTestStep,
									   public MLbsNetChannelObserver,
//									   public MLbsAgpsAssistanceDataObserver,
									   public MNetGatewayProtocolTestObserver,
									   public MNetworkLocationChannelObserver
	{
public:
    /* Class that handles sending messages to the Network Gateway
     * over the PSY interface
     * 
     * AO handles the ACK from the NG
     */
    
    class CPsyToNgChannel : public CActive
        {
        public:
            static CPsyToNgChannel* NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
             ~CPsyToNgChannel();
             
             void RunL();
             void DoCancel();
             
             void SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage);

         private:
             CPsyToNgChannel();
             void ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
         
         private:
             RLbsNetChannel iPsyGatewayChannel;
             CActiveSchedulerWait iPsyActiveWait;
        };
    
	/* Class that handles sending messages to the Network Gateway
	   over the NRH interface.
	   
	   It is an active object because it needs to wait for the
	   acknowledgement signal to ensure the message has been
	   read by the Network Gateway.
	*/
	
	class CNetGatewayChannel : public CActive
		{
	public:
		static CNetGatewayChannel* NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
		~CNetGatewayChannel();
		
		void RunL();
		void DoCancel();
		
		void SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage);

	private:
		CNetGatewayChannel();
		void ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId);
	
	private:
		RLbsNetChannel iNetGatewayChannel;
		CActiveSchedulerWait iActiveWait;
		
		};
		
public:
	virtual ~CTe_lbsnetgatewaySuiteStepBase();
	CTe_lbsnetgatewaySuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
    
	// Needed by all tests for setting up arrays and comms channels
     void InitialiseStubPMChannels();
     void CloseStubPMChannels();
     void InitialiseUidArray();
     
	// from MLbsNetChannelObserver - receives messages from NG
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	// from MLbsAgpsAssistanceDataObserver
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aMask);
	
	// from MNetGatewayProtocolTestObserver
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	
	// from MNetworkLocationChannelObserver
	void ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage);
	
	// start/stop the lbsnetgateway.exe
	TInt StartNetGateway();
	TInt StopNetGateway();
	
	// Used to check data sent in to NRH is matched by the data it sends
	// back out.
	void TestMessagesAreEquivalentL(const TLbsNetInternalMsgBase& aNrhMsg,
									const TNetGatewayMsg& aPmMsg);

	void TestMessagesAreEquivalentL(const TLbsNetLocMsgBase& aNetLocMsg,
								    const TNetGatewayMsg& aPmMsg);
								    
	TBool TestPositionInfosAreEqual(const TPositionInfoBase& aPosInfo1,
									const TPositionInfoBase& aPosInfo2);
									
	TBool TestTReal32Equal(TReal32 aReal1, TReal32 aReal2);
	TBool TestTReal64Equal(TReal64 aReal1, TReal64 aReal2);
	
	void ParseAdminSettingsFromIniL();
	

// Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
	CActiveScheduler* iScheduler;
	CActiveSchedulerWait* iSchedWait;
	CNetGatewayChannel* iNetGatewayChannel;
	CPsyToNgChannel* iPsyGatewayChannel;
	CNetGatewayChannel* iAgpsChannel;
	RAssistanceDataCache	  iAssDataCache;

	CNetworkLocationChannel* iNetworkLocationChannel;
	RLbsNetworkPositionUpdates iReferencePositionUpdates;
	RLbsNetworkPositionUpdates iFinalPositionUpdates;
	RLbsProcessSupervisor iProcessSupervisor;
	
	// Member variables for multiple protocol testing
	RNetGatewayProtocolTestChannel* iStubPMChannelArray;

	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray iUidHomeArray;
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray iUidRoamingArray;
	    
	TInt iCurrentNetworkStatus;
    TInt iLoopCounter;
    TInt iMaxLoopCounter;
    TInt iPmUidIndex;
	
	// Lbs Admin to read and store the settings
	CLbsAdmin* iAdmin;
	};

#endif

