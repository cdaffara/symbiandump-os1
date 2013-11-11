// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// testagentfactory.cpp
// 
//

#include <caf/caf.h>
#include "testagentfactory2.h"

using namespace ContentAccess;

CAgentFactory* CTestAgentFactory::NewL()
	{
	return new (ELeave) CTestAgentFactory();
	}
	
CTestAgentFactory::~CTestAgentFactory()
	{
		
	}
	
CAgentContent* CTestAgentFactory::CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	return CTestAgentContent::NewL(aURI, aShareMode);	
	}
	
CAgentContent* CTestAgentFactory::CreateContentBrowserL(RFile& /*aFile*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;	
	}
	
CAgentData* CTestAgentFactory::CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	return CTestAgentData::NewL(aVirtualPath.URI(), aShareMode);	
	}
	
CAgentData* CTestAgentFactory::CreateDataConsumerL(RFile& /*aFile*/, const TDesC& /*aUniqueId*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;	
	}
	
CAgentManager*  CTestAgentFactory::CTestAgentFactory::CreateManagerL()
	{
	return CTestAgentManager::NewL();
	}
	
CAgentImportFile* CTestAgentFactory::CreateImporterL(const TDesC8& /*aMimeType*/, const CMetaDataArray& /*aMetaDataArray*/, const TDesC& /*aOutputDirectory*/, const TDesC& /*aSuggestedFileName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;	
	}
	
CAgentImportFile* CTestAgentFactory::CreateImporterL(const TDesC8& /*aMimeType*/, const CMetaDataArray& /*aMetaDataArray*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;	
	}
	
CAgentRightsManager* CTestAgentFactory::CreateRightsManagerL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}
	
	
