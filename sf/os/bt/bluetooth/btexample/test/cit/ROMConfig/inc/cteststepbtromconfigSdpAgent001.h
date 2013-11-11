// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CTESTSTEPBTROMCONFIGSDPAGENT001_H
#define CTESTSTEPBTROMCONFIGSDPAGENT001_H

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include "cteststepbtromconfigbase.h"
#include <btsdp.h>

class CTestStepBtRomConfigSdpAgent001 : public CTestStepBtRomConfigBase, 
										public MSdpAgentNotifier
	{
public:
	static CTestStepBtRomConfigSdpAgent001* New(CTestServer& aParent);
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigSdpAgent001(CTestServer& aParent);
	void TestSdpAgentL(const TInt);
	
	virtual void NextRecordRequestComplete(TInt aError, 
										   TSdpServRecordHandle aHandle, 
										   TInt aTotalRecordsCount);

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, 
										TSdpAttributeID aAttrID, 
										CSdpAttrValue *aAttrValue);
	
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);
	};

_LIT(KTestNameSdpAgent001, "BT_ROMCONFIG_SDP_AGENT_001"); 
#endif //  CTESTSTEPBTROMCONFIGSDPAGENT001_H

// EOF
