// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_ETELSATPCMDS1E_H_
#define _TE_ETELSATPCMDS1E_H_

#include <etel.h>

class CTestSatPCmds1e : public CRSatTestStep
	{
public:
	CTestSatPCmds1e() ;
	~CTestSatPCmds1e(){} ;
	virtual enum TVerdict doTestStepL();

	
private:

	void ExtendedTest1L(	TRequestStatus& reqStatus, 
						RSat::TOpenCsChannelV2& aOpenCsChannel, 
						RSat::TOpenCsChannelV2Pckg& aOpenCsChannelPckg,
						RSat::TOpenGprsChannelV4& aOpenGprsChannel,
						RSat::TOpenGprsChannelV4Pckg& aOpenGprsChannelPckg);
	
	void ExtendedTest2L(	TRequestStatus& reqStatus, RSat::TCloseChannelV2& aCloseChannel); 

	void ExtendedTest3L(	TRequestStatus& reqStatus, RSat::TDeclareServiceV2& aDeclareService); 

	};

#endif

