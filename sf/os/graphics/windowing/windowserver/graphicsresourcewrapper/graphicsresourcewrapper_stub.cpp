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

/*
This file is used to create a stub DLL for ROMs where graphics
resource isn't present, hence all functions are empty. However,
it is not expected this DLL will ever be loaded since the user
won't be able to create a RSgDrawable in the first place (and
this class is only used when trying to draw an existing RSgDrawable).
*/

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
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CGraphicsResourceWrapperImpl::Open(RSgDrawable& /*aDrawable*/, const TSgDrawableId& /*aDrawableId*/)
	{
	return KErrNotSupported;
	}

void CGraphicsResourceWrapperImpl::Close(RSgDrawable& /*aDrawable*/)
	{
	}

TBool CGraphicsResourceWrapperImpl::IsNull(const RSgDrawable& /*aDrawable*/)
	{
	return ETrue;
	}

TSgDrawableId CGraphicsResourceWrapperImpl::Id(const RSgDrawable& /*aDrawable*/)
	{
	return TSgDrawableId();
	}
