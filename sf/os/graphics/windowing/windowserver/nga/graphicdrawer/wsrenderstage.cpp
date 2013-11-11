// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base class for window server render stages
// 
//

#include "graphics/WsRenderStage.h"

// CWsRenderStage::CPimpl \\\\\\\\\\\\\\\\\\\\\\\\

/** @internalComponent
	@prototype
*/
NONSHARABLE_CLASS(CWsRenderStage::CPimpl): public CBase
	{
public:
	CWsRenderStage * Next() { return iNext; }
	void SetNext(CWsRenderStage * aNext) { iNext = aNext; }
private:
	CWsRenderStage * iNext;
	};

// CWsRenderStage \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C CWsRenderStage::CWsRenderStage()
	{
	}

EXPORT_C CWsRenderStage::~CWsRenderStage()
	{
	// A given render stage is responsible for the deletion of its
	// children.
	delete iPimpl->Next();
	delete iPimpl;
	}

EXPORT_C void CWsRenderStage::BaseConstructL()
	{
	iPimpl = new (ELeave) CPimpl();
	}

EXPORT_C CWsRenderStage * CWsRenderStage::Next()
	{
	return iPimpl->Next();
	}
	
EXPORT_C void CWsRenderStage::SetNext(CWsRenderStage * aNext)
	{
	iPimpl->SetNext(aNext);
	}

EXPORT_C TAny * CWsRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	if (aTypeId == KCWsRenderStage)
		return this;
	else if(Next())
		return Next()->ResolveObjectInterface(aTypeId);
	else
		return NULL;
	}

