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

/**
 @file
 @publishedAll
*/

CDummyProtocolParameterSet* CDummyProtocolParameterSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
	{
	CDummyProtocolParameterSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CDummyProtocolParameterSet* CDummyProtocolParameterSet::NewL()
	{
	STypeId typeId = STypeId::CreateSTypeId(KUidDummyParameterSetImpl, KUidDummyProtocolParameterSetType);
	CDummyProtocolParameterSet* params = static_cast<CDummyProtocolParameterSet*>(CSubConParameterSet::NewL(typeId));
	return params;
	}


