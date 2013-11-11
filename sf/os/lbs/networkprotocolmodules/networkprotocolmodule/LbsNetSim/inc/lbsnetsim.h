// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
 @test
*/


#ifndef LBSNETSIM_H
#define LBSNETSIM_H

#include <e32base.h>

#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsloccommon.h>

// The name of the server
_LIT(KLbsNetSimServerName, "LbsNetworkSimulatorServer");
_LIT(KLBSNetSimServerBinary, "lbsnetsim.exe");

// The server UID
const TInt32 KLbsNetSimServerUid = 0x1028224A;

// Session type
enum ESessionType
	{
	ETestHandler = 0, // Session is Test API
	EGatewayHandler = 1, // Session is Gateway API
	};
	
// Publish & Subscribe information for StepMode
// Secure Id of client
const TInt32 KLbsNetSimClient = 0x101F7784;
// The publish and subscribe key
const TInt32 KLbsStepModeKey = 0x01;
	
// Callback types
enum ECallbackTypes
	{
	ENotificationConnect = 0,
	ENotificationConnectCancel,
	ENotificationDisconnect,
	ENotificationDisconnectCancel,
	ENotificationRegisterLcsMoLr,
	ENotificationRegisterLcsMoLrCancel,
	ENotificationMeasurementControlLocation,
	ENotificationMeasurementControlLocationCancel,
	ENotificationReleaseLcsMoLr,
	ENotificationReleaseLcsMoLrCancel,
	ENotificationMeasurementReport,
	ENotificationMeasurementReportCancel,
	ENotificationMeasurementReportRequestMoreAssitanceData,
	ENotificationMeasurementReportRequestMoreAssitanceDataCancel,
	ENotificationMeasurementReportControlFailure,
	ENotificationMeasurementReportControlFailureCancel,	
	ENotificationFacilityLcsMoLrResult,
	ENotificationFacilityLcsMoLrResultCancel,
	};

// Messages
enum ESessionMessages
	{
		// Standard Messages
		EInitialise = 0,
		ECallbacks,
		
		// Gatweay messages
		EGWRegisterLcsMoLr = 1000,
		EGWMeasurementReportLocation = 1001,
		EGWMeasurementReportLocationCancel = 1002,
		EGWReleaseLcsMoLr = 1003,
		EGWMeasurementControlFailure = 1004,
		EGWRequestMoreAssistanceData = 1005,
		EGWNotificationMeasurementControlLocation = 1006,
		EGWNotificationMeasurementControlLocationCancel = 1007,
		EGWNotificationRegisterLcsLocation = 1008,
		EGWNotificationRegisterLcsLocationCancel = 1009,
		EGWReleaseLcsLocationNotification = 1010,
		EGWNotificationCancelPrivacy = 1011,
		EGWNotificationCancelPrivacyCancel = 1012,
		EGWNotificationNetworkGone = 1013,
		EGWNotificationNetworkGoneCancel = 1014,
		EGWNotificationResetAssistanceData = 1015,
		EGWNotificationResetAssistanceDataCancel = 1016,
		
		// Test Messages
		ETSetReferenceLocation = 2000,
		ETSetReferenceLocationData = 2002,
		ETSetResponseTime = 2003,
		ETSetResponseError = 2004,
		ETSetDefaultAssitanceDataFilter = 2005,
		ETSetMoreDefaultAssitanceDataFilter = 2006,
		ETClearAssitanceDataFilters = 2007,
		ETSetAssistanceDataProvider = 2008,
		ETSetEmergenyStatus = 2009,
		ETSetRoamingStatus = 2010,
		ETSetRequestQuality = 2011,
		ETSetStepMode = 2012,
		ETReleaseLcsLocationNotification = 2013,
		ETReleaseLcsLocationNotificationCancel = 2014,
		ETStartNetworkLocationRequest = 2015,
		ETStartNetworkLocationRequestCancel = 2016,
		ETStepMode = 2017,
		ETStepModeCancel = 2018,
		ETSendResetAssistanceData = 2019,
	};

#endif // LBSNETSIM_H
