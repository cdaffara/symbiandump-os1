// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// rtcadaptation.h
// 
//

#ifndef __RTCADAPTATION_H__
#define __RTCADAPTATION_H__

#include <ssm/ssmadaptation.h>

// C Class
class CRtcAdaptationRef : public CBase, public MRtcAdaptation
	{
public:
	static CRtcAdaptationRef* NewL();
	~CRtcAdaptationRef();

	//from MRtcAdaptation
	void Release();

	void ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus);
	void SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus);
	void UnsetWakeupAlarm(TRequestStatus& aStatus);
	void Cancel();

private:
	CRtcAdaptationRef();
	void ConstructL();
	//Helper function to check for P&S Key
    TBool IsTestPsKeyDefined();
    
private:	
	MRtcAdaptation* iSaaRtcAdaptationDll;
	RLibrary iSaaRtcAdaptationLib;
	};

#endif // __RTCADAPTATION_H__
