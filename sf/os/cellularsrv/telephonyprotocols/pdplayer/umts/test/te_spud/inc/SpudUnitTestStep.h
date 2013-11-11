// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration of test classes for SPUD TEL and SPUD FSM unit tests
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SPUD_UNIT_TEST_STEP_H
#define SPUD_UNIT_TEST_STEP_H

#include <c32comm.h>
#include <networking/umtsnifcontrolif.h>
#include <test/testexecutestepbase.h>

#include "CompareContextParameters.h"
#include "meteldriverstrategy.h"
#include "eteldrivernmspace.h"
#include "pdpfsmnmspace.h"
#include "InputRequestListener.h"

#define TEST_SETRESULT(cond,result) if (!(cond)) SetTestStepResult((result))

const TInt KSimTsyTestNumberR5OrR99R4 = 109;

/** Identifiers for each of the simtsy parameters that can be changed using publish and subscribe */
enum TEtelRequestType
	{
	/** Change QoS parameters */
	ENetworkQoSChange,
	/** Change network status */
	ENetworkChangeRegStatus,
	/** Change context status */
	EContextStatusChange,
	/** Change context config parameters */
	EContextConfigChange
	};
	
/** Contains methods common to both SPUD FSM and SPUD TEL tests */
class CSpudUnitTestStepBase : public CTestStep
	{
public:
	CSpudUnitTestStepBase(TInt aTestNumber, TBool aUseTestPdpFsmInterface);
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	static void SetSimTsyTestNumberL(TInt aTestNumber);

protected:
	void InitPhoneAndPacketServiceL();
	void DestroyPhoneAndPacketService();
	void EtelRequest(TEtelRequestType aCommand, TInt aNewValue);
	TInt iTestNumber;

	/** Used as the interface between the thread running the active objects and the test execute thread.
		Contains the data passed to/from Input requests */
	CInputRequestListener *iListener;
	/** Set to true when testing Etel driver, and as such the test FSM interface it used to interact with Etel driver
		and set to false when the real FSM is used */
	TBool iUseTestPdpFsmInterface;
	/** The thread used to run the SPUD FSM and SPUD TEL active objects */
	RThread iActiveSchedulerThread;
	
	/** Etel parameters that can be used in test steps */
	RTelServer iTelServer;
	RPhone iPhone;
	RPacketService iPacketService;
	RPacketContext iPrimaryPacketContext, iSecondaryPacketContext;
	};

/** Contains methods for SPUD TEL tests */
class CSpudTelUnitTestStepBase : public CSpudUnitTestStepBase
	{
public:
	CSpudTelUnitTestStepBase(TInt aTestNumber);

protected:
	void EtelDriverInputRequest(TContextId aPdpId, EtelDriver::TEtelInput aOperation);
	void CancelPdpRequest(TContextId aPdpId);
	void WaitForEtelDriverInputResponse(TContextId aPdpId, PdpFsm::TEtelSignal aSignal, TInt aError=KErrNone);
	
	void WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigGPRS& aContextConfigGPRS);
	void WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfig_R5& aContextConfigR5);
	void WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigR99_R4& aContextConfigR99R4);

	void WaitForGivenEtelContextStatusNotification(TContextId aPdpId, RPacketContext::TContextStatus aContextStatus);

	void WaitForNextQosNotification(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated);

	void WaitForNextServiceNotificationStatus(RPacketService::TStatus aServiceStatus);
	void WaitForEtelDriverSetDataChannel(TContextId aPdpId, RPacketContext::TDataChannelV2 aDataChannelV2);
	};

/** Contains methods for SPUD FSM tests */
class CSpudFsmUnitTestStepBase : public CSpudUnitTestStepBase
	{
public:
	CSpudFsmUnitTestStepBase(TInt aTestNumber);

protected:
	void FsmInputRequest(TContextId aPdpId, TInt aOperation, TInt aParam);
	void WaitForFsmInputResponse(TInt aPdpId, TInt aEvent, TInt aParam);
	
    TContextId FsmObjectCreate();
    TInt WaitForFsmObjectCreateResponse(TInt aPdpId);

	void WaitForFsmSuspendedNotification(TContextId aPdpId);
	void WaitForFsmResumedNotification(TContextId aPdpId);
	void WaitForFsmServiceNotificationStatus();
	void WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigGPRS& aContextConfig);
	void WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfig_R5& aContextConfig);
	void WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigR99_R4& aContextConfig);

	void WaitForFsmQosChangeNotification(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aQosParams);

	void WaitForGivenContextStatus(RPacketContext& aPacketContext, RPacketContext::TContextStatus aStatus);
	};

/*
 Declarations for SPUD TEL unit tests - they all implement constructors, destructors and override doTestStep()
 */

#define DEFINE_SPUD_TEL_UNIT_TEST_CLASS(state) class CSpudTelUnit##state : public CSpudTelUnitTestStepBase \
	{ \
public: \
	CSpudTelUnit##state(); \
	~CSpudTelUnit##state(); \
	virtual TVerdict doTestStepL(); \
	};
 
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(NormalOperation)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(Notifications)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(CreatePrimaryError)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(SetQosError)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(SetTftError)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(ActivateError1)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(ActivateError2)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(DeleteError1)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(DeleteError2)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(MultipleContexts)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(Cancel)
DEFINE_SPUD_TEL_UNIT_TEST_CLASS(NotificationsR5OrR99R4)


/*
 Declarations for SPUD FSM unit tests - they all implement constructors, destructors and override doTestStep()
 */
 
#define DEFINE_SPUD_FSM_UNIT_TEST_CLASS(state) class CSpudFsmUnit##state : public CSpudFsmUnitTestStepBase \
{ \
public: \
	CSpudFsmUnit##state(); \
	~CSpudFsmUnit##state(); \
	virtual TVerdict doTestStepL(); \
};	

DEFINE_SPUD_FSM_UNIT_TEST_CLASS(NormalOperation)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(CreatePrimaryError1)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(CreatePrimaryError2)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(CreatePrimaryError3)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(SetQosAndTftError)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(ChangeQosAndTftError)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(Notifications)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(NetworkDelete)
DEFINE_SPUD_FSM_UNIT_TEST_CLASS(NotificationsR5OrR99R4)


/*
 Declarations for test names used by test execute script
 */ 
_LIT(KSpudTelUnitNormalOperation,"SpudTelUnitNormalOperation");
_LIT(KSpudTelUnitNotifications,"SpudTelUnitNotifications");
_LIT(KSpudTelUnitCreatePrimaryError,"SpudTelUnitCreatePrimaryError");
_LIT(KSpudTelUnitSetQosError,"SpudTelUnitSetQosError");
_LIT(KSpudTelUnitSetTftError,"SpudTelUnitSetTftError");
_LIT(KSpudTelUnitActivateError1,"SpudTelUnitActivateError1");
_LIT(KSpudTelUnitActivateError2,"SpudTelUnitActivateError2");
_LIT(KSpudTelUnitDeleteError1,"SpudTelUnitDeleteError1");
_LIT(KSpudTelUnitDeleteError2,"SpudTelUnitDeleteError2");
_LIT(KSpudTelUnitMultipleContexts,"SpudTelUnitMultipleContexts");
_LIT(KSpudTelUnitNotificationsR5OrR99R4,"SpudTelUnitNotificationsR5OrR99R4");
_LIT(KSpudTelUnitCancel,"SpudTelUnitCancel");

_LIT(KSpudFsmUnitNormalOperation,"SpudFsmUnitNormalOperation");
_LIT(KSpudFsmCreatePrimaryError1,"SpudFsmCreatePrimaryError1");
_LIT(KSpudFsmCreatePrimaryError2,"SpudFsmCreatePrimaryError2");
_LIT(KSpudFsmCreatePrimaryError3,"SpudFsmCreatePrimaryError3");
_LIT(KSpudFsmSetQosAndTftError,"SpudFsmSetQosAndTftError");
_LIT(KSpudFsmChangeQosAndTftError,"SpudFsmChangeQosAndTftError");
_LIT(KSpudFsmTestNotifications,"SpudFsmTestNotifications");
_LIT(KSpudFsmTestNotificationsR5OrR99R4,"SpudFsmTestNotificationsR5OrR99R4");
_LIT(KSpudFsmNetworkDelete,"SpudFsmNetworkDelete");

/*
 Declarations for names generated by SIMTSY for different contexts
 */ 
_LIT(KFirstPrimaryContextName, "PRIMARY1");
_LIT(KSecondPrimaryContextName, "PRIMARY2");
_LIT(KThirdPrimaryContextName, "PRIMARY3");
_LIT(KFirstSecondaryContextName, "SECONDARY1");
_LIT(KSecondSecondaryContextName, "SECONDARY2");
_LIT(KFirstQosName, "PACKET_QOS1");

const TInt KContextId0 = 0;
const TInt KContextId1 = 1;
const TInt KContextId2 = 2;
const TInt KContextId3 = 3;

#endif // SPUD_UNIT_TEST_STEP_H
