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
// CORECPR.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef DUMMYADDRESSBOOK_H
#define DUMMYADDRESSBOOK_H

#include <e32base.h>

namespace Messages
{
	class TNodeId;
}

namespace Dummy
{

//Global helpers hiding away the RootSvr's "address book" mechanism.
//The address book functionality is never part of the state (core) libraries,
//and belongs to the server runtime-contexts introduction mechanism.
struct TRuntimeCtxAddressBook
	{
	//Global helpers stubbing away the RootSvr address exchange ("address book") mechanism
	IMPORT_C static const Messages::TNodeId& ObjectBroker();
	};

} //Dummy

#endif
//DUMMYADDRESSBOOK_H

