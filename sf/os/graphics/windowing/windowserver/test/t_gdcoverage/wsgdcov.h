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
// calls the CGraphicDrawerCoverage tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian coverage test code, 
*/

#ifndef __WSGDCOVERAGE_H__
#define __WSGDCOVERAGE_H__

#include <w32std.h>
#include "gdcovcommon.h"

NONSHARABLE_CLASS(CWsGdCoverage): public CWsGraphic
	{
public:		
	IMPORT_C static CWsGdCoverage* NewL();
	IMPORT_C static CWsGdCoverage* NewL(TInt aScreenId);
	IMPORT_C ~CWsGdCoverage();
	IMPORT_C TInt RunTest(TInt aCode);
	IMPORT_C TInt QueryPlugin(TGdCoverageInfo& aInfo);
	IMPORT_C void SetCallBack(TCallBack aCallBack);
private:
	// override CWsGraphic
	virtual void HandleMessage(const TDesC8& aData);
	virtual void OnReplace();
private:
	TBool iIsReady;
	TGdCoverageInfo* iReq;
	TCallBack iCallBack;
	};

#endif
