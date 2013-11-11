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
//

/**
 @file 
 @internalComponent 
*/

#if !defined(SS_PROTPROV_H)
#define SS_PROTPROV_H

#include <e32def.h>
#include <comms-infras/metadata.h>
#include <es_ini.h>
#include <comms-infras/ss_subconnflow.h>
#include <ss_fact.h>

class CServProviderBase;
class CProtocolBase;
class CProtocolFamilyBase;

namespace ESock
{
    
//=======================================================================================
const TInt KProtocolFamilyInterfaceUid = 0x102046A0;
/** Types of an KProtocolFamilyInterfaceUid interface represented by CProtocolFamilyFactoryBase. 
Could be exteded either by native types less than EProtocolFamilyFactoryPrivate or user types 
greater than EProtocolFamilyFactoryPrivate
@internalTechnology
@released since 9.1
*/
enum TProtocolFamilyImplementations
	{
	EIPEventNotifierFactoryId = 1,

	EProtocolFamilyFactoryPrivate = 128
	};

class CProtocolFamilyFactoryContainer;
class CProtocolFamilyFactoryBase : public CCommsFactoryBase
/** Base class for all protocol families' provider factories. Specifies the interface
    that all protocol families' providers factories must implement. It also providers the
	ECOM framework (via CCommsFactoryBase) to load sub-connection provider factories
	from other Dynamic Linked Libraries.

@internalTechnology
@released Since 9.1 
*/
	{
	friend class CProtocolFamilyBase;
public:
	IMPORT_C static CProtocolFamilyFactoryBase* NewL(const TDesC8& aName, CProtocolFamilyFactoryContainer& aParentContainer);

	IMPORT_C CProtocolBase* FindProtocol( TUint aProtocolId );

protected:
	IMPORT_C CProtocolFamilyFactoryBase(TUid aFactoryId, CProtocolFamilyFactoryContainer& aParentContainer);
private:
	IMPORT_C ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);
//	CProtocolFamilyFactoryBase() {}
	};

class CProtocolFamilyFactoryContainer : public CCommsFactoryContainer
/** Container for protocol families' providers

@internalTechnology
@released Since 9.1 */
	{
protected:
    enum
        {
        EId = EProtocolFamilyFactoryContainer
        };
public:
	static CProtocolFamilyFactoryContainer* NewL();
	IMPORT_C CProtocolFamilyFactoryBase* FindOrCreateL(const TDesC8& aName, TUid aFactoryId);	
	CProtocolFamilyFactoryBase* Get(TInt aIndex) const;	
	~CProtocolFamilyFactoryContainer();
	
protected:
	CProtocolFamilyFactoryContainer();
	};

} //namespace ESock

#endif	
// SS_PROTPROV_H

