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
//

/**
 @file
 @internalComponent Reference implementation of Open VG hardware accelerated Direct GDI adaptation.
*/

#ifndef DIRECTGDIIMAGETARGETIMPL_H
#define DIRECTGDIIMAGETARGETIMPL_H

#include "directgdiimageref.h"
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <graphics/sgimage.h>
#include <e32hashtab.h>
#include <e32base.h>

class CDirectGdiDriverImpl;

/**
Container for holding an RDirectGdiImageTarget and its associated EGLSurface.
Reference counted as more than one CDirectGdiContext is allowed to use the same RDirectGdiImageTarget.
*/
NONSHARABLE_CLASS(CDirectGdiImageTargetImpl): public CDirectGdiImageRef
	{
public:
	static TInt New(CDirectGdiImageTargetImpl*& aImage, CDirectGdiDriverImpl& aDriver, const RSgImage& aSgImage, RHashMap<TInt, EGLContext>& aContexts, RHashMap<TInt, EGLConfig>& aConfigs, EGLContext& aSharedContext);
	virtual ~CDirectGdiImageTargetImpl();
	TBool Activate();
	
private:
	CDirectGdiImageTargetImpl(CDirectGdiDriverImpl& aDriver);
	TInt Construct(const RSgImage& aSgImage, RHashMap<TInt, EGLContext>& aContexts, RHashMap<TInt, EGLConfig>& aConfigs, EGLContext& aSharedContext);
	
private:
	CDirectGdiDriverImpl& iDriver;
	EGLSurface iSurface;
	EGLContext iContext;
	};

#endif /*DIRECTGDIIMAGETARGETIMPL_H_*/
