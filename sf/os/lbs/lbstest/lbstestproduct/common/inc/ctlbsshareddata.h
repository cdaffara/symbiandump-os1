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
// @file CT_LbsSharedData.h
// This contains CT_LbsSharedData
// 
//


#ifndef __T_LBS_SHARED_DATA_H__
#define __T_LBS_SHARED_DATA_H__

// system includes
#include <e32cmn.h>

// Lbs includes
#include <lbspositioninfo.h>
#include <lbscommon.h>

#include <lbs/lbslogevent.h>
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>

class CT_LbsSharedData : public CBase
{
public:
	static CT_LbsSharedData* NewL();
	CT_LbsSharedData();
	virtual ~CT_LbsSharedData();

protected:
	virtual void ConstructL();

public:
	RPointerArray<TAny>	iVerifyPosInfoArr;
	RPointerArray<TAny>	iCurrentPosInfoArr;
	
	// For logging, different type arrays to store multiple events
	RPointerArray<CLbsSelfLocateLogEvent>		iSelfLocateLogInfoArr;
	RPointerArray<CLbsNetworkLocateLogEvent>	iNetworkLocateLogInfoArr;
	RPointerArray<CLbsExternalLocateLogEvent>	iExternalLocateLogInfoArr;
	RPointerArray<CLbsTransmitLocationLogEvent>	iX3PLocateLogInfoArr;
	RPointerArray<CLbsAssistanceDataLogEvent>	iAssistanceDataLogInfoArr;
		
	TPositionModuleInfo iVerifyModuleInfo;
	TPositionModuleInfo iCurrentModuleInfo;

	TPositionModuleStatus iVerifyModuleStatus;
	TPositionModuleStatus iCurrentModuleStatus;

	TPositionModuleStatusEvent iVerifyModuleStatusEvent;
	TPositionModuleStatusEvent iCurrentModuleStatusEvent;

	TPositionUpdateOptions iVerifyUpdateOpts;
	TPositionUpdateOptions iCurrentUpdateOpts;
	
	/** Flag used to indicate if the A-GPS integration test module is being used during testing.
	*/
	TBool iTestModuleInUse;
};

#endif // __T_LBS_SHARED_DATA_H__
