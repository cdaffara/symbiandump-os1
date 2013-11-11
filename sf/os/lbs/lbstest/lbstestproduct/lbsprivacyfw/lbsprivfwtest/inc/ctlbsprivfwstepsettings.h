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
// @file ctlbsprivfwstepsettings.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//
 
#ifndef __CT_LBS_PRIVFW_STEP_SETTINGS_H__
#define __CT_LBS_PRIVFW_STEP_SETTINGS_H__


#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwnotifyrequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_Settings, "KLbsPrivFwStep_Settings");

class CT_LbsPrivFwStepSettings : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepSettings();
	static CT_LbsPrivFwStepSettings* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepSettings(CT_LbsPrivFwServer& aParent);



};

#endif // __CT_LBS_PRIVFW_STEP_SETTINGS_H__
