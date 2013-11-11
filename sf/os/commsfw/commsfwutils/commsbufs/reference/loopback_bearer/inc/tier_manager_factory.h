/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __LEGACY_LOOPBACK_TIER_MANAGER_FACTORY_H__
#define __LEGACY_LOOPBACK_TIER_MANAGER_FACTORY_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <ss_tiermanager.h>
#else
#include <comms-infras/ss_tiermanager.h>
#endif

class CLegacyLoopBackTierManagerFactory : public ESock::CTierManagerFactoryBase
	{
public:
    enum { iUid = 0x102871C5 };
	static CLegacyLoopBackTierManagerFactory* NewL(TAny* aParentContainer);

protected:
	CLegacyLoopBackTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif // __LEGACY_LOOPBACK_TIER_MANAGER_FACTORY_H__

