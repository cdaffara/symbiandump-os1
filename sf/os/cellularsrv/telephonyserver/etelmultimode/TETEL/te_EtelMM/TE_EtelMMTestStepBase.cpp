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
// Te_EtelMMSuiteStepBase.cpp
// 
//

#include "TE_EtelMMTestStepBase.h"
#include "testdef.h"

TVerdict CEtelMMTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);	

	TInt ret = iTelServer.Connect();

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect phone"));
		User::Leave(ret);
		}

	ret = iTelServer.LoadPhoneModule(DMMTSY_MODULE_NAME);

	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CEtelMMTestStepBase::doTestStepPostambleL()
	{
	delete iTestScheduler;
	iTestScheduler = NULL;
	
	iTelServer.UnloadPhoneModule(DMMTSY_MODULE_NAME);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CEtelMMTestStepBase::ValidateDataCallParams(const RMobileCall::TMobileDataCallParamsV1 &aDataParams, TInt aCount)
	{
	TEST(aCount == 0 || aCount == 1);

	switch (aCount)
		{
	case 0:
		{
		TEST(aDataParams.iService==DMMTSY_DATA_CALL_SERVICE);
		TEST(aDataParams.iSpeed==DMMTSY_DATA_CALL_SPEED);
		TEST(aDataParams.iProtocol==DMMTSY_DATA_CALL_PROTOCOL);
		TEST(aDataParams.iQoS==DMMTSY_DATA_CALL_QOS);
		TEST(aDataParams.iRLPVersion==DMMTSY_DATA_CALL_RLP);
		TEST(aDataParams.iV42bisReq==DMMTSY_DATA_CALL_V42BIS);
		TEST(aDataParams.iUseEdge==DMMTSY_DATA_CALL_EGPRS_REQUIRED);
		break;
		}
	case 1:
		{
		TEST(aDataParams.iService==DMMTSY_DATA_CALL_SERVICE1);
		TEST(aDataParams.iSpeed==DMMTSY_DATA_CALL_SPEED1);
		TEST(aDataParams.iProtocol==DMMTSY_DATA_CALL_PROTOCOL1);
		TEST(aDataParams.iQoS==DMMTSY_DATA_CALL_QOS1);
		TEST(aDataParams.iRLPVersion==DMMTSY_DATA_CALL_RLP1);
		TEST(aDataParams.iV42bisReq==DMMTSY_DATA_CALL_V42BIS1);
		TEST(aDataParams.iUseEdge==DMMTSY_DATA_CALL_EGPRS_NOTREQUIRED);
		break;
		}
	default:
		break;
		}
	}

