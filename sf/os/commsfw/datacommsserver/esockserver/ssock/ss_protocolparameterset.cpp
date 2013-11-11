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


#include "ss_protocolparameterset.h"

namespace ESock
{
EXPORT_C CSubConnectionProtocolParameterSet* CSubConnectionProtocolParameterSet::NewL(
	CSubConParameterFamily& aFamily,
	CSubConParameterFamily::TParameterSetType aType)
	{
	CSubConnectionProtocolParameterSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

EXPORT_C CSubConnectionProtocolParameterSet* CSubConnectionProtocolParameterSet::NewL(
	RParameterFamily& aFamily,
	RParameterFamily::TParameterSetType aType)
	{
	CSubConnectionProtocolParameterSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.AddParameterSetL(obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

EXPORT_C CSubConnectionProtocolParameterSet* CSubConnectionProtocolParameterSet::NewL()
	{
	STypeId typeId = STypeId::CreateSTypeId(CSubConnectionProtocolParameterSet::EUid, CSubConnectionProtocolParameterSet::EType);
	return new(ELeave) CSubConnectionProtocolParameterSet;
	}

EXPORT_C CSubConnectionProtocolParameterSet::CSubConnectionProtocolParameterSet()
	{
	}

EXPORT_C TInt CSubConnectionProtocolParameterSet::FindAndRemoveExtension(
	TProtocolExtensionSpecifier& aExtensionSpec,
	const TUid& aTierId)
	{
	TInt index = 0;

	// This ought to be a short list so just walk it
	while(index < iExtensions.Count())
		{
		if(iExtensions[index].iTierId == aTierId)
			{
			aExtensionSpec = iExtensions[index];
			iExtensions.Remove(index);
			return KErrNone;
			}
		index++;
		}
	return KErrNotFound;
	}

EXPORT_C TInt CSubConnectionProtocolParameterSet::AddExtension(const TProtocolExtensionSpecifier& aExtensionSpec)
	{
	return iExtensions.Append(aExtensionSpec);
	}


EXPORT_C /*virtual*/ CSubConnectionProtocolParameterSet::~CSubConnectionProtocolParameterSet()
	{
	iExtensions.Close();
	}

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(CSubConnectionProtocolParameterSet, CSubConnectionProtocolParameterSet::EUid, CSubConnectionProtocolParameterSet::EType)
	REGISTER_ATTRIBUTE(CSubConnectionProtocolParameterSet, iExtensions, TMetaArray<TProtocolExtensionSpecifier>)
END_ATTRIBUTE_TABLE()



// CFlowRequestParameters
//-----------------------

EXPORT_C CFlowRequestParameters* CFlowRequestParameters::NewL(
	CSubConParameterFamily& aFamily,
	CSubConParameterFamily::TParameterSetType aType)
	{
	CFlowRequestParameters* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

EXPORT_C CFlowRequestParameters* CFlowRequestParameters::NewL()
	{
	STypeId typeId = STypeId::CreateSTypeId(CFlowRequestParameters::EUid, CFlowRequestParameters::EType);
	CFlowRequestParameters* paramSet = static_cast<CFlowRequestParameters*>(CSubConParameterSet::NewL(typeId));
	return paramSet;
	}
}

