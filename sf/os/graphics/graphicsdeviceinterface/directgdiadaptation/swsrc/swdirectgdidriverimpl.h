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
// Declaration of a Direct Graphics Device Interface (GDI) software implementation,
// this class actually implements the HAI.
// 
//

#ifndef __SWDIRECTGDIDRIVERIMPL_H__
#define __SWDIRECTGDIDRIVERIMPL_H__

/**
@file
@internalComponent
*/

#include "swdirectgdiengine.h"
#include <gdi.h>
#include <pixelformats.h>
#include <graphics/directgdidriverinternal.h>

class CFbsDrawDevice;
class CSwDirectGdiImageTargetImpl;
class CSwDirectGdiImageSourceImpl;

/**
This class represents the body part of CDirectGdiDriver that actually implements
the HAI. It derives from the CDirectGdiDriverInternal class and implements all the 
interfaces to provide the functionality required by the generic layer.
*/
NONSHARABLE_CLASS(CSwDirectGdiDriverImpl): public CDirectGdiDriverInternal
	{
public:		
	virtual ~CSwDirectGdiDriverImpl();
	
	// From CDirectGdiDriverInternal
	void Flush() ;
	void Finish() ;
	TInt GetError();
	void SetError(TInt aErr);
	TInt CreateDrawableSource(TInt& aHandleRet, const RSgDrawable& aSgDrawable);
	void CloseDrawableSource(TInt& aHandle);
	TInt CreateImageTarget(TInt& aHandleRet, const RSgImage& aRSgImage);
	void CloseImageTarget(TInt& aHandle);
	TInt CreateEngine(MDirectGdiEngine*& aMDirectGdiEngine);
	void DestroyEngine(MDirectGdiEngine* aMDirectGdiEngine);
	TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	
	TBool ValidImageSource(TInt aHandle) const;
	TBool ValidImageTarget(TInt aHandle) const;
	void Deactivate(CSwDirectGdiImageTargetImpl* aRenderingTarget);
	TInt RegisterSourceImage(const CSwDirectGdiImageSourceImpl& aImageSource);
	TInt RegisterTargetImage(const CSwDirectGdiImageTargetImpl& aImageTarget);
	TInt UnregisterTargetImage(const CSwDirectGdiImageTargetImpl& aImageTarget);
	TInt UnregisterSourceImage(const CSwDirectGdiImageSourceImpl& aImageSource);
	CSwDirectGdiDriverImpl(RLibrary aLibrary);

private:
	TInt iErrorCode;	/**< The first error code to occur in any CSwDirectGdiEngine instance.*/
	RPointerArray<MDirectGdiEngine> iEngines;
	RPointerArray<CSwDirectGdiImageTargetImpl> iTargets;
	RPointerArray<CSwDirectGdiImageSourceImpl> iImageSources;	
	};

#endif //__SWDIRECTGDIDRIVERIMPL_H__
