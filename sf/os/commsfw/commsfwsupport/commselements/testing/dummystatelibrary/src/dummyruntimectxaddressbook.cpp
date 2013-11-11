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
//


#include "dummyruntimectxaddressbook.h"
#include <elements/factory.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingdmyrn, "ElemTestingdmyrn");
#endif

using namespace Dummy;
using namespace Messages;

EXPORT_C const TNodeId& TRuntimeCtxAddressBook::ObjectBroker()
	{
	Factories::CFactoryContainerNode* fc = reinterpret_cast<Factories::CFactoryContainerNode*>(Dll::Tls());
	__ASSERT_DEBUG(fc!=NULL, User::Panic(KSpecAssert_ElemTestingdmyrn, 1));
	return fc->Id();
	}



