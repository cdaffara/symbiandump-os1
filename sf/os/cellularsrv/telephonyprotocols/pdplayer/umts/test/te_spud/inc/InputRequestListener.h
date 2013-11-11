// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Declaration for CInputRequestListener
// 
//

/**
 @file
 @internalComponent
*/

#ifndef INPUT_REQUEST_LISTENER_H
#define INPUT_REQUEST_LISTENER_H

#include <e32def.h>
#include <networking/umtsnifcontrolif.h>

#include "CompareContextParameters.h"
#include "reteldriverinput.h"
#include "meteldriverstrategy.h"
#include "PdpFsmInterfaceTest.h"
//#include "SpudManInterfaceTest.h"
#include "cpdpfsmfactory.h"

/**
 Storage for parameters that are passed to REtelDriverInput::Input
 */
class TEtelDriverInputRequestData
	{
	public:
	TContextId iPdpId;
	EtelDriver::TEtelInput iOperation;
	};

/**
 Storage for parameters that are passed to RPdpFsmInterfaceTest::Input by SpudTel
 */
class TEtelDriverInputResponseData
	{
	public:
	TContextId iPdpId;
	TInt iSignal;
	};
	
/**
 Storage for parameters that are passed during Etel driver notifications
 */
class TNotifyContextConfigData
	{
	public:
	TContextId iPdpId;
	RPacketContext::TContextConfigGPRS iContextConfigGPRS;
	RPacketContext::TContextConfigR99_R4 iContextConfigR99_R4;
	RPacketContext::TContextConfig_R5 iContextConfig_R5;
	};
	
class TNotifyPacketQosData
	{
	public:
	TContextId iPdpId;
	TNegotiatedProfileBuffer iNegotiated;
	};

class TNotifyContextStatusData
	{
	public:
	TContextId iPdpId;
	RPacketContext::TContextStatus iContextStatus;
	};

class TNotifyContextDataChannelData
	{
	public:
	TContextId iPdpId;
	RPacketContext::TDataChannelV2 iDataChannelV2;
	};

/**
 Storage for parameters that are passed to RPdpFsmInterface::Input by Spud man
 */
class TFsmInputRequestData
	{
	public:
	TContextId iPdpId;
	TInt iOperation;
	TInt iParam;
	};
	
/**
 Storage for parameters that are passed to RPdpFsmInterface::Input by Etel driver
 */
class TFsmInputResponseData
	{
	public:
	TContextId iPdpId;
	TInt iEvent;
	TInt iParam;
	};
	
/**
 Storage for parameters used for a context change notification by the FSM
 */
class TNotifyContextParametersChangeData
	{
	public:
	TContextId iPdpId;
	TNegotiatedProfileBuffer iNegotiated;
	RPacketContext::TContextConfigGPRS iContextConfig;
	RPacketContext::TContextConfigR99_R4 iContextConfigR99_R4;
	RPacketContext::TContextConfig_R5 iContextConfig_R5;
	};

class CInputRequestListener;

class CSpudUnitTestEventHandler: public MPdpFsmEventHandler
/**
Callback interface for PDPFsm to raise events.
Ignore Spud in the name. It's legacy artifact kept for
the old code to work.
*/
    {
public:
	virtual void Event(TInt aEvent, TInt aParam);
	static CSpudUnitTestEventHandler *NewL(CInputRequestListener& aListener, TInt aUmtsRelease);

	void SetContextId(TContextId aPdpId) { iPdpId = aPdpId; }
	
private:
	CSpudUnitTestEventHandler(CInputRequestListener& aListener, TInt aUmtsRelease);
	
private:
	CInputRequestListener& iListener;
	TContextId iPdpId;
	TInt iUmtsRelease;
    };

class CInputRequestListener : public CActive
	{
public:
	/**
	 Enum of the types of requests supported by CInputRequestListener. When the iStatus is completed with
	 these results, the associated method is called with the corresponding parameters.
	 */
	enum TRequestType
		{
		EEtelDriverInputRequest,
		EEtelDriverCancelPdpRequest,
		EFsmInputRequest,
		ECleanup,
		EFsmObjectCreate
		};

public:
static CInputRequestListener* NewL(TBool aUseTestPdpFsmInterface, TThreadId aParentThreadId, TInt aUmtsRelease);
	virtual ~CInputRequestListener();
	void Activate();
	void RunL();
	void DoCancel();
	
	void CreateUnitUnderTestL();
	void DeleteUnitUnderTest();

private:
	CInputRequestListener(TBool aUseTestPdpFsmInterface, TInt aUmtsRelease);	
	void ConstructL(TThreadId aParentThreadId);
	void CreateEtelDriverL();
	void CreatePdpFsmL();

public:
	/** Set to true when testing Etel driver, and as such the test FSM interface it used to interact with Etel driver
		and set to false when the real FSM is used */
	TBool iUseTestPdpFsmInterface;
	/** Contains the Etel driver under test */
	REtelDriverInput* iEtelDriver;
	/** Contains the RPdpFsmInterface used for Etel driver tests */
	CPdpFsmInterfaceTest *iPdpFsmInterfaceTest;

	/** Contains the RPdpFsmInterface under test */
	CPdpFsmInterface *iPdpFsmInterface;

	/** The thread that test execute is running in */
	RThread iParentThread;
	/** This active scheduler will run the SPUD TEL, SPUD FSM and this CActive object */
	CActiveScheduler* iScheduler;

	// contains the data that is passed to REtelDriverInput interface
	TEtelDriverInputRequestData iEtelDriverInputRequestData;
	TEtelDriverInputResponseData iEtelDriverInputResponseData;
	TContextId iEtelDriverCancelRequestPdpId;
		
	// contains the data that is passed to RPdpFsmInterface::Set
	TNotifyContextConfigData iNotifyContextConfigData;
	TNotifyContextStatusData iNotifyContextStatusData;
	TNotifyPacketQosData iNotifyQosNegotiatedData;
	TNotifyContextDataChannelData iNotifyContextDataChannelData;
	RPacketService::TStatus iPacketServiceStatus;

	// Data stored for FSM input requests
	TFsmInputRequestData iFsmInputRequestData;
	TFsmInputResponseData iFsmInputResponseData;
	TNotifyContextParametersChangeData iContextParametersEventData;
	TContextId iContextBlockedEventPdpId;
	TContextId iContextUnblockedEventPdpId;

	/** Completed when the Active Scheduler thread is initialized and ready to handle requests */
	TRequestStatus iThreadInitialized;
	/** Completed when the Active Scheduler thread has cleaned up any resources and can be destroyed */
	TRequestStatus iThreadDestructed;

	/** Must be pending before any Input requests are completed */
	TRequestStatus iReadyForInputRequest;
	
	/** Completed when RPdpFsmInterfaceTest::Input is called (with a non-notification aOperation parameter) */
	TRequestStatus iEtelDriverInputResponseStatus;
	/** Completed when RPdpFsmInterfaceTest::Input is called with a QoS notification as the aOperation parameter */
	TRequestStatus iEtelDriverQosNotificationStatus;
	/** Completed when RPdpFsmInterfaceTest::Input is called with a GPRS config notification as the aOperation parameter */
	TRequestStatus iEtelDriverConfigGPRSNotificationStatus;
	/** Completed when RPdpFsmInterfaceTest::Input is called with a context status change notification as the aOperation parameter */
	TRequestStatus iEtelDriverContextStatusNotificationStatus;
	/** Completed when RPdpFsmInterfaceTest::Input is called with a service status change as the aOperation parameter */
	TRequestStatus iEtelDriverServiceNotificationStatus;

	/** Completed when RSpudManInterfaceTest::Input is called (with a non-notification aOperation parameter) */
	TRequestStatus iFsmInputResponseStatus;
	/** Completed when RSpudManInterfaceTest::Input is called with a service status change as the aOperation parameter */
	TRequestStatus iFsmNetworkStatusStatus;
	/** Completed when RSpudManInterfaceTest::Input is called with a context status change as the aOperation parameter */
	TRequestStatus iFsmContextParameterChangeStatus;
	/** Completed when RSpudManInterfaceTest::Input is called with a context blocked event as the aOperation parameter */
	TRequestStatus iFsmContextSuspendedStatus;
	/** Completed when RSpudManInterfaceTest::Input is called with a context unblocked event as the aOperation parameter */
	TRequestStatus iFsmContextResumedStatus;
	
	/** Completed by RunL() */
    TRequestStatus iFsmObjectCreateResponse;	

	RPointerArray<CSpudUnitTestEventHandler> iEventHandlers;
	
    TInt iUmtsRelease;
	};
	
/** Contains the data passed from the test execute thread to the active object thread */
class TActiveSchedulerThreadParams
	{
	public:
	CInputRequestListener* iListener;
	};

/** The start-up method for the thread running the active objects */
TInt ActiveSchedulerThread(TAny* aActiveSchedulerParams);

#endif
// INPUT_REQUEST_LISTENER_H

