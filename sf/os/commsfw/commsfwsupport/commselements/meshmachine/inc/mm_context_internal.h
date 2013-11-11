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
 @internalTechnology
*/

#ifndef SYMBIAN_MM_CONTEXT_INTERNAL_H
#define SYMBIAN_MM_CONTEXT_INTERNAL_H

#include <elements/mm_context.h>

namespace MeshMachine
{
class CNodeActivityBase;
class AMMNodeBase;

class TNodeNullContext : public TNodeContextBase
/**
@internalTechnology
*/
	{
public:
	explicit TNodeNullContext(AMMNodeBase& aNode, CNodeActivityBase* aActivity = NULL)
	:	TNodeContextBase(aNode, iNullMessageSig, Messages::TNodeCtxId::NullId(), Messages::TNodeCtxId::NullId(), aActivity)
		{
		}

	Messages::TEBase::TNull iNullMessageSig;
	};

} //namespace MeshMachine

#endif
	//SYMBIAN_MM_CONTEXT_H


