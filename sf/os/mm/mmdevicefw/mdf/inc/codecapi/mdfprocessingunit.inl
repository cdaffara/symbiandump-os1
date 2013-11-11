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

#include <ecom/ecom.h>

inline CMdfProcessingUnit* CMdfProcessingUnit::NewL(TUid aImplementationUid)
	{
	TAny* interface = REComSession::CreateImplementationL(aImplementationUid, _FOFF(CMdfProcessingUnit, iDtor_ID_Key));
	return reinterpret_cast<CMdfProcessingUnit*> (interface);
	}

inline CMdfProcessingUnit::~CMdfProcessingUnit()
	{
	// Destroy any instance variables and then inform ecom that this specific
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
