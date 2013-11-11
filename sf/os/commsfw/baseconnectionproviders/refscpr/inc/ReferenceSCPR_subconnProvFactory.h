// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference SubConnection Provider Factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__REFERENCE_SUBCONNPROVFACTORY_H__)
#define __REFERENCE_SUBCONNPROVFACTORY_H__


#include <comms-infras/ss_subconnprov.h>

//phony protocol family id, replace with the actual
const TUint KReferenceSubConnectionProviderFactoryId = 0xF0F0; 

class CReferenceSubConnectionProviderFactory : public CSubConnectionProviderFactoryBase
/**This is a sample/reference derivation of the subconnection provider factory.
Each such derivation should be able to produce a specific type of subconnection providers,
where the type usually denotes a particular technology (e.g.: IP, UMTS, SIP).

@internalComponent
@released since v9.2
*/
	{
public:
	static CReferenceSubConnectionProviderFactory* NewL(TAny* aConstructionParameters);	
	
protected:

	CReferenceSubConnectionProviderFactory(TUint aFactoryId, CSubConnectionFactoryContainer& aParentContainer);
	virtual ~CReferenceSubConnectionProviderFactory();
	virtual CSubConnectionProviderBase* DoCreateProviderL(CConnectionProviderBase& aConnProvider, RSubConnection::TSubConnType aType);

	};


#endif	// __REFERENCE_SUBCONNPROVFACTORY_H__