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
// Dummy PR Factories class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>

#include "dummy_protocol_parameter_sets.h"

using namespace ESock;


START_ATTRIBUTE_TABLE(CDummyProtocolParameterSet, KUidDummyParameterSetImpl, KUidDummyProtocolParameterSetType)
	REGISTER_ATTRIBUTE(CDummyProtocolParameterSet, iStatus, TMeta<TBool>)
END_ATTRIBUTE_TABLE_BASE(CSubConnectionProtocolParameterSet, 0)


//-=========================================================
//
// ECOM data
//
//-=========================================================	
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(CDummyParamsFactory::KFactoryUid, CDummyParamsFactory::NewL)
	};

/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
   }

//-=========================================================
//
// CDummyParamsFactory methods
//
//-=========================================================	
CSubConGenericParameterSet* CDummyParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (KUidDummyProtocolParameterSetType):
		{
		CDummyProtocolParameterSet* dummyParams = new (ELeave) CDummyProtocolParameterSet;
		CleanupStack::PushL(dummyParams);
		dummyParams->ConstructL();
		CleanupStack::Pop(dummyParams);
		return dummyParams;
		}
		// break;

	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}



// CDummyProtocolParameterSet
//---------------------------
void CDummyProtocolParameterSet::ConstructL()
	{
	TProtocolExtensionSpecifier protocolSpecifier(
		TUid::Uid(2048), // network tier uid KAfInet =0x0800
		TUid::Uid(0x10285D8D)); // dummy extended scpr uid

	User::LeaveIfError(AddExtension(protocolSpecifier));
	}

