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
 @file DataMonitoring.TestSteps.h
*/
#if (!defined DATAMONITORING_TESTSTEPS_H)
#define DATAMONITORING_TESTSTEPS_H


#include <comms-infras/datamonitormessages.h>
#include <comms-infras/datamonmessagesecom.h>

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

// General constants
//------------------
_LIT(KExpectTimeout, "ExpectTimeout");
_LIT(KDataMonitorName, "DataMonitorName");
_LIT(KDataMonitorResponderNameSuffix,"_Responder");

_LIT(KReceivedVolumeThreshold, "ReceivedVolume");
_LIT(KSentVolumeThreshold, "SentVolume");
_LIT(KTimeout, "Timeout");

_LIT(KCompareDataRequestResultsName, "CompareToSavedNamed");
_LIT(KSaveDataRequestResultsName, "SaveResultsName");


enum{EEqual, EGreaterThanEqual, ELessThan};
_LIT(KGreaterThanEqual, "GreaterThanEqual");
_LIT(KEqual, "Equal");
_LIT(KLessThan, "LessThan");

_LIT(KReceivedVolumeCondition, "ReceivedCondition");
_LIT(KSentVolumeCondition, "SentCondition");


/**
   Class implementing OpenRCommsDataMonitoringApiExtStep

   @internalComponent
*/
class COpenRCommsDataMonitoringApiExtStep : public CTe_EsockStepBase
	{
public:
   COpenRCommsDataMonitoringApiExtStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KOpenRCommsDataMonitoringApiExtStep, "OpenRCommsDataMonitoringApiExtStep");


/**
   Class implementing RequestDataReceivedNotificationStep

   @internalComponent
*/
class CRequestDataReceivedNotificationStep : public CTe_EsockStepBase
	{
public:
   CRequestDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KRequestDataReceivedNotificationStep, "RequestDataReceivedNotificationStep");


/**
   Class implementing RequestDataSentNotificationStep

   @internalComponent
*/
class CRequestDataSentNotificationStep : public CTe_EsockStepBase
	{
public:
   CRequestDataSentNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private: 
	TDataMonitorEventParams iParams;
   };

_LIT(KRequestDataSentNotificationStep, "RequestDataSentNotificationStep");


/**
   Class implementing RequestDataTransferredStep

   @internalComponent
*/
class CRequestDataTransferredStep : public CTe_EsockStepBase
	{
public:
	CRequestDataTransferredStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt iReceivedVolumeThreshold;
	TInt iSentVolumeThreshold;
	TDataMonitorEventParams iParams;
   };

_LIT(KRequestDataTransferredStep, "RequestDataTransferredStep");


/**
   Class implementing ReceiveDataReceivedNotificationStep

   @internalComponent
*/
class CReceiveDataReceivedNotificationStep : public CTe_EsockStepBase
	{
public:
   CReceiveDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KReceiveDataReceivedNotificationStep, "ReceiveDataReceivedNotificationStep");


/**
   Class implementing ReceiveDataSentNotificationStep

   @internalComponent
*/
class CReceiveDataSentNotificationStep : public CTe_EsockStepBase
	{
public:
   CReceiveDataSentNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KReceiveDataSentNotificationStep, "ReceiveDataSentNotificationStep");


/**
   Class implementing CloseRCommsDataMonitoringApiExtStep

   @internalComponent
*/
class CCloseRCommsDataMonitoringApiExtStep : public CTe_EsockStepBase
	{
public:
   CCloseRCommsDataMonitoringApiExtStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KCloseRCommsDataMonitoringApiExtStep, "CloseRCommsDataMonitoringApiExtStep");


/**
   Class implementing CancelDataReceivedNotificationStep

   @internalComponent
*/
class CCancelDataReceivedNotificationStep : public CTe_EsockStepBase
	{
public:
   CCancelDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KCancelDataReceivedNotificationStep, "CancelDataReceivedNotificationStep");


/**
   Class implementing CancelDataSentNotificationStep

   @internalComponent
*/
class CCancelDataSentNotificationStep : public CTe_EsockStepBase
	{
public:
   CCancelDataSentNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TDataMonitorEventParams iParams;
   };

_LIT(KCancelDataSentNotificationStep, "CancelDataSentNotificationStep");


#endif // DATAMONITORING_TESTSTEPS_H


