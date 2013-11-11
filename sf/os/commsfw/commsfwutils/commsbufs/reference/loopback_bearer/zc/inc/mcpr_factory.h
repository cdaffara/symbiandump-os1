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
#ifndef __ZEROCOPY_LOOPBACK_MCPR_FACTORY_H__
#define __ZEROCOPY_LOOPBACK_MCPR_FACTORY_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <ss_metaconnprov.h>
#else
#include <comms-infras/ss_metaconnprov.h>
#endif

class CZeroCopyLoopbackMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x10286A0D };
	static CZeroCopyLoopbackMetaConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CZeroCopyLoopbackMetaConnectionProviderFactory(TUid aFactoryId, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif // __ZEROCOPY_LOOPBACK_MCPR_FACTORY_H__
