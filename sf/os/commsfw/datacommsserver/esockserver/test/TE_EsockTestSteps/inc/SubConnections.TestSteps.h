/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SubConnections.TestSteps.h
*/

#if (!defined SUBCONNECTIONS_TESTSTEPS_H)
#define SUBCONNECTIONS_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

//subconnection generic
_LIT(KTe_SubConnectionName,"SubConnName");
_LIT(KTe_SubConnectionParamsName,"SubConnParamsName");
_LIT(KTe_SubConnectionTypeName,"SubConnType");

_LIT(KTe_SubConnectionTypeAttach,"EAttachToDefault");
_LIT(KTe_SubConnectionTypeNew,"ECreateNew");

//subcon parameters


_LIT(KTe_ParamsTypeIdUidName,"Type_UID");
_LIT(KTe_ParamsTypeIdIntName,"Type_INT");
_LIT(KTe_ParamsTypeRequested,"Requested");
_LIT(KTe_ParamsTypeAcceptable,"Acceptable");
_LIT(KTe_ParamsTypeGranted,"Granted");

_LIT(KTe_HeaderMode,"HeaderMode");
_LIT(KTe_DownlinkBandwidth,"DownlinkBandwidth");
_LIT(KTe_UplinkBandwidth,"UplinkBandwidth");
_LIT(KTe_DownLinkMaximumBurstSize,"DownLinkMaxBurstSize");
_LIT(KTe_UpLinkMaximumBurstSize,"UpLinkMaxBurstSize");
_LIT(KTe_DownLinkAveragePacketSize,"DownLinkAvPacketSize");
_LIT(KTe_UpLinkAveragePacketSize,"UpLinkAvPacketSize");
_LIT(KTe_DownLinkMaximumPacketSize,"DownLinkMaxPacketSize");
_LIT(KTe_UpLinkMaximumPacketSize,"UpLinkMaxPacketSize");
_LIT(KTe_DownLinkDelay,"DownLinkDelay");
_LIT(KTe_UpLinkDelay,"UpLinkDelay");
_LIT(KTe_DownLinkDelayVariation,"DownLinkDelayVariation");
_LIT(KTe_UpLinkDelayVariation,"UpLinkDelayVariation");
_LIT(KTe_DownLinkPriority,"DownLinkPriority");
_LIT(KTe_UpLinkPriority,"UpLinkPriority");
_LIT(KTe_AccessBearer,"AccessBearer");
_LIT(KTe_ServiceMode,"ServiceMode");
_LIT(KTe_MultiClient,"MultiClients");
_LIT(KTe_MultiConnection,"MultiConnection");
_LIT(KTe_NumOfTmgis,"NumOfTMGIs");
_LIT(KTe_MCC,"MCCID");
_LIT(KTe_MNC,"MNCID");
_LIT(KTe_ServiceId,"ServiceId");
_LIT(KTe_ServicePrty,"ServicePriority");
_LIT(KTe_SessionId,"SessionId");
_LIT(KTe_SessionId2,"SessionId2");
_LIT(KTe_DeactivationReqd,"DeactivationReqd");
_LIT(KTe_ActivationSuccess,"ActivationSuccess");
_LIT(KTe_SessionOperationType,"OperationType");

_LIT(KTe_ParamsTypeInvite,"Invite");
_LIT(KTe_ParamsTypeSubscribe,"Subscribe");
//subcon events
_LIT(KTe_SubConnectionEventsName,"SubConnEventsName");
_LIT(KTe_EventTypeParamsGranted,"ParamsGranted");
_LIT(KTe_EventTypeDataClientJoined,"DataClientJoined");
_LIT(KTe_EventTypeDataClientLeft,"DataClientLeft");
_LIT(KTe_EventTypeSubConDown,"SubConDown");
_LIT(KTe_EventTypeParamsChanged,"ParamsChanged");
_LIT(KTe_EventTypeParamsRejected,"ParamsRejected");


_LIT(KTe_SubConGenericParamsSetName,"GSet");
_LIT(KTe_SubConParamsSetName,"Set");

_LIT(KTe_CompareFamily1Type,"FamilyType1");
_LIT(KTe_CompareFamily2Type,"FamilyType2");
_LIT(KTe_SubConnectionParams1Name,"SubConnParamsName1");
_LIT(KTe_SubConnectionParams2Name,"SubConnParamsName2");
_LIT(KTe_SubConnectionParams1Type,"SubConnParamsType1");
_LIT(KTe_SubConnectionParams2Type,"SubConnParamsType2");


/**
Class implementing CreateRSubConnectionStep

@internalComponent
*/
class CCreateRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CCreateRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCreateRSubConnectionStep,"CreateRSubConnectionStep");


/**
Class implementing OpenRSubConnectionOOMStep

@internalComponent
*/
class COpenRSubConnectionOOMStep : public CTe_EsockStepBase
	{
public:
	COpenRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
private:
    TInt OpenSubConnectionOOM();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KOpenRSubConnectionOOMStep,"OpenRSubConnectionOOMStep");


/**
Class implementing OpenRSubConnectionStep

@internalComponent
*/
class COpenRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	COpenRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KOpenRSubConnectionStep,"OpenRSubConnectionStep");


/**
Class implementing CloseRSubConnectionStep

@internalComponent
*/
class CCloseRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CCloseRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCloseRSubConnectionStep,"CloseRSubConnectionStep");


/**
Class implementing checkrsubconnectioneventsStep

@internalComponent
*/
class CCheckRSubConnectionEventsStep : public CTe_EsockStepBase
	{
public:
	CCheckRSubConnectionEventsStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TEventContainerParams iParams; //current params (.ini)
	};

_LIT(KCheckRSubConnectionEventsStep,"CheckRSubConnectionEventsStep");


/**
Class implementing controlrsubconnectionStep

@internalComponent
*/
class CControlRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CControlRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KControlRSubConnectionStep,"ControlRSubConnectionStep");


/**
Class implementing multipleopenrsubconnectionStep

@internalComponent
*/
class CMultipleOpenRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CMultipleOpenRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KMultipleOpenRSubConnectionStep,"MultipleOpenRSubConnectionStep");


/**
Class implementing MultipleRemoveRSubConnectionStep

@internalComponent
*/
class CMultipleRemoveRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CMultipleRemoveRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KMultipleRemoveRSubConnectionStep,"MultipleRemoveRSubConnectionStep");


/**
Class implementing RegisterForRSubConnectionEventsStep

@internalComponent
*/
class CRegisterForRSubConnectionEventsStep : public CTe_EsockStepBase
	{
public:
	CRegisterForRSubConnectionEventsStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TEventContainerParams iParams; //current params (.ini)
	};

_LIT(KRegisterForRSubConnectionEventsStep,"RegisterForRSubConnectionEventsStep");


// Enumerate subconnections
//-------------------------

class TEnumerateSubConnectionsParams
	{
public:
	TPtrC iConnectionName;
	TInt iExpectedSubConnCount;
	};


class CEnumerateSubConnectionsStep : public CTe_EsockStepBase
	{
public:
	CEnumerateSubConnectionsStep(CCEsockTestBase*& aEsockTest);
	TInt ConfigureFromIni();
	TVerdict doSingleTestStep();

private:
    TEnumerateSubConnectionsParams iParams;
	};

_LIT(KEnumerateSubConnectionsStep,"EnumerateSubConnectionsStep");


_LIT(KExpectedConnectionCount, "ExpectedConnCount");
_LIT(KExpectedSubConnectionCount, "ExpectedSubConnCount");



#endif // SUBCONNECTIONS_TESTSTEPS_H

