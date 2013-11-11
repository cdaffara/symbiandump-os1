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
// graphicsresourcewrapper.cpp
// 
//

#ifndef __GRAPHICSRESOURCEWRAPPER_H__
#define __GRAPHICSRESOURCEWRAPPER_H__

#include <e32base.h>
#include <graphics/sgresource.h>

class CGraphicsResourceWrapper;

_LIT(KGraphicsResourceWrapperDll, "graphicsresourcewrapper.dll");


/**
@internalComponent
*/
class CGraphicsResourceWrapperFactory : public CBase
	{
public:
	inline ~CGraphicsResourceWrapperFactory();
	inline CGraphicsResourceWrapper* NewGraphicsResourceWrapper();

private:
	enum { EFactoryMethod = 1 };
	RLibrary iLibrary;
	TBool iLoaded;
	};

inline CGraphicsResourceWrapperFactory::~CGraphicsResourceWrapperFactory()
	{
	if (iLoaded)
		iLibrary.Close();
	}

inline CGraphicsResourceWrapper* CGraphicsResourceWrapperFactory::NewGraphicsResourceWrapper()
	{
	if (!iLoaded)
		{
		if (KErrNone == iLibrary.Load(KGraphicsResourceWrapperDll))
			iLoaded = ETrue;
		else
			return NULL;
		}

	typedef CGraphicsResourceWrapper*(* TFactoryMethod)();
	TFactoryMethod f = reinterpret_cast<TFactoryMethod>(iLibrary.Lookup(EFactoryMethod));
	if (f)
		return f();

	return NULL;
	}

/**
A wrapper class around Graphics Resource so that Wserv doesn't need to link
against graphics resource.

@internalComponent
*/
class CGraphicsResourceWrapper : public CBase
	{
public:
	virtual RSgDrawable* NewDrawableL() = 0;
	virtual TInt Open(RSgDrawable& aDrawable, const TSgDrawableId& aDrawableId) = 0;
	virtual void Close(RSgDrawable& aDrawable) = 0;
	virtual TBool IsNull(const RSgDrawable& aDrawable) = 0;
	virtual TSgDrawableId Id(const RSgDrawable& aDrawable) = 0;
	};

#endif //__GRAPHICSRESOURCEWRAPPER_H__
