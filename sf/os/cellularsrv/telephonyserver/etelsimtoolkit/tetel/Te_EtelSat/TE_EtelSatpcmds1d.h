// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_ETELSATPCMDS1D_H_
#define _TE_ETELSATPCMDS1D_H_

#include <etel.h>

class CTestSatPCmds1d : public CRSatTestStep
	{
public:
	CTestSatPCmds1d() ;
	~CTestSatPCmds1d(){} ;
	virtual enum TVerdict doTestStepL();


private:
	RSat::TOpenCsChannelV2 iOpenCsChannel;
	RSat::TOpenCsChannelV5 iOpenCsChannelV5;
	RSat::TOpenGprsChannelV4 iOpenGprsChannel;
	RSat::TOpenGprsChannelV5 iOpenGprsChannelV5;
	RSat::TOpenChannelBaseV2 iOpenBaseChannel;
	RSat::TOpenChannelRspV2 iOpenChannelRsp;
	RSat::TCloseChannelV2 iCloseChannel;
	RSat::TCloseChannelV5 iCloseChannelV5;
	RSat::TReceiveDataV2 iReceiveData;
	RSat::TReceiveDataV5 iReceiveDataV5;
	RSat::TReceiveDataRspV2 iReceiveDataRsp;
	RSat::TSendDataV2 iSendData;
	RSat::TSendDataV5 iSendDataV5;
	RSat::TSendDataRspV2 iSendDataRsp;
	RSat::TGetChannelStatusV2 iGetChannelStatus;
	RSat::TGetChannelStatusRspV2 iGetChannelStatusRsp;
	RSat::TDeclareServiceV2 iDeclareService;
	RSat::TServiceSearchV2 iServiceSearch;
	RSat::TServiceSearchV5 iServiceSearchV5;
	RSat::TServiceSearchRspV2 iServiceSearchRsp;
	RSat::TGetServiceInfoV2 iGetServiceInfo;
	RSat::TGetServiceInfoV5 iGetServiceInfoV5;
	RSat::TGetServiceInfoRspV2 iGetServiceInfoRsp;

	void ExtendedTest1L(	TRequestStatus& reqStatus,
						RSat::TOpenCsChannelV2Pckg& aOpenCsChannelPckg,
						RSat::TOpenGprsChannelV4Pckg& aOpenGprsChannelPckg);

	void ExtendedTest2L(	TRequestStatus& reqStatus);

	void ExtendedTest3L(	TRequestStatus& reqStatus);

	void ExtendedTest4L(	TRequestStatus& reqStatus);

	void ExtendedTest5L(	TRequestStatus& reqStatus);

	void ExtendedTest6L(	TRequestStatus& reqStatus);

	void ExtendedTest7L(	TRequestStatus& reqStatus);

	void ExtendedTest8L(	TRequestStatus& reqStatus);

	};

#endif // _TE_ETELSATPCMDS1D_H_
