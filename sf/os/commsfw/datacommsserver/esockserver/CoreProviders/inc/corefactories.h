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
// Core factories class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORE_FACTORIES_H
#define SYMBIAN_CORE_FACTORIES_H

#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_metaconnprov.h>


class CCoreConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x102822F3 };
	static CCoreConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CCoreConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};
	
	
class CCoreSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x102822F4 };
	static CCoreSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CCoreSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_CORE_FACTORIES_H

