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
// @file CT_LbsClientSharedData.h
// This contains the header file for CT_LbsClientSharedData
// 
//


#ifndef __CT_LBS_CLIENT_SHARED_DATA_H__
#define __CT_LBS_CLIENT_SHARED_DATA_H__

// system includes
#include <e32cmn.h>

// Lbs includes
#include <lbspositioninfo.h>
#include <lbscommon.h>


class CT_LbsClientSharedData : public CBase
{
public:
	static CT_LbsClientSharedData* NewL();
	CT_LbsClientSharedData();
	~CT_LbsClientSharedData();

private:
	void ConstructL();

public:
	RPointerArray<TAny>	iVerifyPosInfoArr;
	RPointerArray<TAny>	iCurrentPosInfoArr;
	
	TPositionModuleInfo iVerifyModuleInfo;
	TPositionModuleInfo iCurrentModuleInfo;

	TPositionModuleStatus iVerifyModuleStatus;
	TPositionModuleStatus iCurrentModuleStatus;

	TPositionModuleStatusEvent iVerifyModuleStatusEvent;
	TPositionModuleStatusEvent iCurrentModuleStatusEvent;

	TPositionUpdateOptions iVerifyUpdateOpts;
	TPositionUpdateOptions iCurrentUpdateOpts;
};

#endif // __CT_LBS_CLIENT_SHARED_DATA_H__
