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
// The implementation file for the Reference SubConnection Parameter Set.
// 
//

/**
 @file
 @internalComponent
*/


#include <ecom/ecom.h>
#include "ReferenceSCPR_subconparams.h"
#include <comms-infras/metatypearray.h>


START_ATTRIBUTE_TABLE( CSubConReferenceParamSet, KSubConReferenceParamsUid, KSubConReferenceParamsType )
	REGISTER_ATTRIBUTE( CSubConReferenceParamSet, iDummyTIntParameter, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConReferenceParamSet, iDummyName, TMetaNumber )
END_ATTRIBUTE_TABLE()


CSubConExtensionParameterSet* CReferenceSubConnExtensionParamsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
	case (KSubConReferenceParamsType):
		return new (ELeave) CSubConReferenceParamSet;
	default:
		User::Leave(KErrNotFound);
		}
	return NULL;
	}

