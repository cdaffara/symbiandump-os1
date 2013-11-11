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

#include "Te_GprsTestStepBase.h"
#include "Te_Gprscfgfile.h"

TVerdict CGprsTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iTestScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
//
//	This test requires a mobile phone to be connected to the serial port
//      specified in the .cfg file for the initialise below to work
//
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iSocketServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to connect to socket server (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	ret=iConnection.Open(iSocketServer,KAfInet);
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to create connection (%d)"), ret);
		iSocketServer.Close();
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	TRAP(ret,iTestConfigParam=CConfigParams::NewL(KTGPRSConfigFile()));
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to create config parameters (%d)"), ret);
		iConnection.Close();
		iSocketServer.Close();
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	return TestStepResult();
	}

TVerdict CGprsTestStep::doTestStepPostambleL()
	{
	delete iTestConfigParam;
	iConnection.Close();
	iSocketServer.Close();
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KtsyName);
	iTelServer.Close();
	delete iTestScheduler;

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CGprsTestStep::TestConfigParams(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2)
	{
	TEST(aArg1.iPdpType == aArg2.iPdpType);
    TEST(aArg1.iAccessPointName == aArg2.iAccessPointName );
    TEST(aArg1.iPdpCompression == aArg2.iPdpCompression);
	TEST(aArg1.iPdpAddress == aArg2.iPdpAddress);
	TEST(aArg1.iAnonymousAccessReqd == aArg2.iAnonymousAccessReqd);
	}
