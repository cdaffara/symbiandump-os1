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

#include "graphicsresourcewrapper.h"

class CGraphicsResourceWrapperImpl : public CGraphicsResourceWrapper
	{
public:
	IMPORT_C static CGraphicsResourceWrapper* New();

	RSgDrawable* NewDrawableL();
	TInt Open(RSgDrawable& aDrawable, const TSgDrawableId& aDrawableId);
	void Close(RSgDrawable& aDrawable);
	TBool IsNull(const RSgDrawable& aDrawable);
	TSgDrawableId Id(const RSgDrawable& aDrawable);
	};

EXPORT_C CGraphicsResourceWrapper* CGraphicsResourceWrapperImpl::New()
	{
	return new CGraphicsResourceWrapperImpl;
	}

RSgDrawable* CGraphicsResourceWrapperImpl::NewDrawableL()
	{
	return new(ELeave) RSgDrawable;
	}

TInt CGraphicsResourceWrapperImpl::Open(RSgDrawable& aDrawable, const TSgDrawableId& aDrawableId)
	{
	return aDrawable.Open(aDrawableId);
	}

void CGraphicsResourceWrapperImpl::Close(RSgDrawable& aDrawable)
	{
	return aDrawable.Close();
	}

TBool CGraphicsResourceWrapperImpl::IsNull(const RSgDrawable& aDrawable)
	{
	return aDrawable.IsNull();
	}

TSgDrawableId CGraphicsResourceWrapperImpl::Id(const RSgDrawable& aDrawable)
	{
	return aDrawable.Id();
	}
