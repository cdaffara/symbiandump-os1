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
// Declaration of a Direct Graphics Device Interface (GDI) Driver implementation,
// this class actually implements the HAI.
// 
//


#ifndef __DIRECTGDIDRIVERIMPL_H__
#define __DIRECTGDIDRIVERIMPL_H__

/**
@file
@internalComponent Reference implementation of Open VG hardware accelerated DirectGDI adaptation.
*/
#include "vgimagecache.h"
#include "directgdidrawableref.h"
#include "directgdiimageref.h"
#include "directgdidriverprocessstate.h"
#include "glyphimagecache.h"
#include <graphics/directgdidriverinternal.h>
#include <graphics/directgdiextensioninterfaces.h>
#include <e32hashtab.h>
#include <e32cmn.h>
#include <pls.h>


// Forward declarations.
//
class CDirectGdiImageTargetImpl;
class CDirectGdiImageSourceImpl;
class CVgEngine;
class CDirectGdiDrawableRef;
class RDirectGdiImageTarget;

/**
Represents the body part of CDirectGdiDriver that actually implements the HAI. Derives from the CDirectGdiDriverInternal class and implements all the 
interfaces to provide the functionality required by the generic layer.
*/
NONSHARABLE_CLASS(CDirectGdiDriverImpl): public CDirectGdiDriverInternal, public MDirectGdiDriverCacheSize
	{
public:
	static TInt New(CDirectGdiDriverInternal*& aInternal, RLibrary aLibrary);
	~CDirectGdiDriverImpl();

	// From CDirectGdiDriverInternal	
	void Flush();
	void Finish();	
	TInt GetError();
	void SetError(TInt aErr);
	TInt CreateDrawableSource(TInt& aHandleRet, const RSgDrawable& aSgDrawable);
	void CloseDrawableSource(TInt& aHandle) ;	
	TInt CreateImageTarget(TInt& aHandleRet, const RSgImage& aImageDestination);
	void CloseImageTarget(TInt& aHandle);
	TInt CreateEngine(MDirectGdiEngine*& aDirectGdiEngine);
	void DestroyEngine(MDirectGdiEngine* aDirectGdiEngine);
	TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	
	// From MDirectGdiDriverCacheSize
	TInt SetMaxImageCacheSize(TInt aSize);	
	TInt MaxImageCacheSize() const;
	TInt SetMaxGlyphCacheSize(TInt aSize);
	TInt MaxGlyphCacheSize() const;
	
	void SetCurrentEngine(CVgEngine* aCurrentEngine);
	void SetCurrentTarget(CDirectGdiImageTargetImpl* aCurrentTarget);
	TBool IsCurrentEngine(const CVgEngine* aCurrentEngine) const;
	TBool IsCurrentTarget(const CDirectGdiImageTargetImpl* aCurrentTarget) const;
	void Activate(CDirectGdiImageTargetImpl* aRenderingTarget);
	void Reactivate(CDirectGdiImageTargetImpl* aRenderingTarget);
	void Deactivate(CDirectGdiImageTargetImpl* aRenderingTarget);
	CDirectGdiImageSourceImpl* GetImageSourceFromHandle(TInt aHandle) const;	
	CDirectGdiImageTargetImpl* GetImageTargetFromHandle(TInt aHandle) const;
	TBool IsImageSource(TInt aHandle) const;
	
	CFontGlyphImageStorage* FontGlyphImageStorage() const;
	TInt PreAllocateFontGlyphImages(); //images will be used to draw glyphs if the system runs out of memory
	CVgImageCache* VgImageCache() const;

	// VGImage cache commands
	VGImage GetVgImageFromCache(const CFbsBitmap& aBitmap, const TPoint& aOrigin) const;
	TBool AddVgImageToCache(const CFbsBitmap& aBitmap, VGImage& aImage, const TPoint& aOrigin);
	
	TInt RegisterSourceImage(const CDirectGdiImageSourceImpl& aImage);
	TInt RegisterTargetImage(const CDirectGdiImageTargetImpl& aImage);
	TInt UnregisterSourceImage(const CDirectGdiImageSourceImpl& aImage);
	TInt UnregisterTargetImage(const CDirectGdiImageTargetImpl& aImage);
	
	inline EGLDisplay EglDisplay() const;
	inline XDirectGdiDriverProcessState& ProcessState() const;
	
	static TInt GetVgError();

private:
	CDirectGdiDriverImpl(RLibrary aLibrary);
	TInt Construct();
	TInt InitializeEglConfigs(EGLDisplay display);
		
#ifdef _DEBUG
	void CheckSourceIsValid(TInt aHandle) const;
	void CheckTargetIsValid(TInt aHandle) const;
#endif // _DEBUG

private:
	TInt iErrorCode; 	/**< The first error code to occur in any CVgEngine instance.*/
	RHashMap<TInt, EGLContext> iPixelContextMap;
	RHashMap<TInt, EGLConfig> iPixelConfigMap;
	CVgEngine* iCurrentEngine;
	CDirectGdiImageTargetImpl* iCurrentTarget;
	EGLContext iSharedContext;
	EGLDisplay iDisplay;
	RPointerArray<CDirectGdiImageRef> iSourceArray;	/**< An array of pointers to all sources created and currently held open by this driver.*/
	RPointerArray<CDirectGdiImageRef> iTargetArray;	/**< An array of pointers to all targets created and currently held open by this driver.*/
	
	CFontGlyphImageStorage* iGlyphImageStorage;
	CVgImageCache* iVgImageCache;

#ifndef __WINS__
	static XDirectGdiDriverProcessState gProcessState;
#endif
	};

/**
@return A reference to the process state object.
*/
inline XDirectGdiDriverProcessState& CDirectGdiDriverImpl::ProcessState() const
	{
#ifdef __WINS__
	const TUid KDirectGdiAdapterUid = {0x102858E9};
	return *Pls<XDirectGdiDriverProcessState>(KDirectGdiAdapterUid);
#else
	return gProcessState;
#endif
	}

/**
@return A handle to the EGLDisplay associated with this driver.
*/
inline EGLDisplay CDirectGdiDriverImpl::EglDisplay() const 
	{
	return iDisplay; 
	}

#endif //__DIRECTGDIDRIVERIMPL_H__
