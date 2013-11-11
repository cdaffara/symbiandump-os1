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
//

/**
 @file coretiermanager.h
 @internalTechnology
*/

#ifndef SYMBIAN_CORETIERMANAGER_H
#define SYMBIAN_CORETIERMANAGER_H

#include <comms-infras/ss_tiermanager.h>

namespace TMStates
{
	class TSelectProvider;
}

namespace ESock
{
	class MProviderSelector;
}

//
//CCoreTierManager
class CCoreTierManager : public ESock::CTierManagerBase
/**
@internalTechnology
*/
    {
	friend class TMStates::TSelectProvider;

public:
	IMPORT_C virtual ~CCoreTierManager();

public:
	IMPORT_C virtual TBool HandleContentionL(ESock::CMetaConnectionProviderBase* aMcpr, Messages::TNodeId& aPendingCprId, TUint aPriority);
	IMPORT_C virtual TBool IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* aMetaConnectionProvider) const;

protected:
    IMPORT_C CCoreTierManager(ESock::CTierManagerFactoryBase& aFactory,
                              const MeshMachine::TNodeActivityMap& aActivityMap);

    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);

protected:
	virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences) = 0;
	};

#endif
//SYMBIAN_CORETIERMANAGER_H

