// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// trasterizerbase.h
// 
//

#include "trasterizerbase.h"
#include <fbs.h>
#include <e32uid.h>


CTRasterizerBase::~CTRasterizerBase()
	{
	}

void CTRasterizerBase::GetExampleRasterizerL()
	{
	iRasterizer = CFbsBitmap::Rasterizer();
	TESTL(iRasterizer != NULL);
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTRasterizerBase::doTestStepPreambleL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTRasterizerBase::doTestStepPostambleL()
	{
	RFbsSession::Disconnect();
	return TestStepResult();	
	}
