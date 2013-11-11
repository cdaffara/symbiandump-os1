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
//



/**
 @file
 @test
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include "t_testrenderstagefactory.h"
#include "t_testfader.h"
#include "t_logfile.h"

LOCAL_C const TImplementationProxy KImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(CTestRenderStageFactory::EImplUid,CTestRenderStageFactory::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestFader::EImplUid,CTestFader::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestFader_Data::EImplUid,CTestFader_Data::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestFader_InvalidConstruct::EImplUid,CTestFader_InvalidConstruct::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestRenderStageFactory_InvalidROI::EImplUid,CTestRenderStageFactory_InvalidROI::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestRenderStageFactory_Invalid::EImplUid,CTestRenderStageFactory_Invalid::CreateL),
	IMPLEMENTATION_PROXY_ENTRY(CTestServicePlugin::EImplUid,CTestServicePlugin::CreateL)
	};
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
	return KImplementationTable;
	}

