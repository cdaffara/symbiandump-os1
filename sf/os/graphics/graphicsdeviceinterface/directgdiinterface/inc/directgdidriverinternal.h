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
// This class represents the body part of CDirectGdiDriver that defines the HAI. 
// 
//


#ifndef __DIRECTGDIDRIVERINTERNAL_H__
#define __DIRECTGDIDRIVERINTERNAL_H__

/**
@file
@publishedPartner
@prototype 
*/

#include <e32base.h>

//forward declarations
class RSgDrawable;
class RSgImage;
class MDirectGdiEngine;

/**
This class represents the body part of CDirectGdiDriver that defines the HAI. 
An adaptation must derive from this class and implement all the interfaces to 
provide the functionality required by the generic layer.

@publishedPartner
@prototype
@deprecated
*/
class CDirectGdiDriverInternal : public CBase
	{
public:
	IMPORT_C static TInt New(CDirectGdiDriverInternal*& aInternal, RLibrary aLibrary);
	inline RLibrary Library() const;
		
	/**
	@see CDirectGdiDriver::Flush()
	*/
	virtual void Flush() = 0;
	
	/**
	@see CDirectGdiDriver::Finish()
	*/
	virtual void Finish() = 0;
	
	/**
	@see CDirectGdiDriver::GetError()
	*/
	virtual TInt GetError() = 0;
	
	/**
	@see CDirectGdiDriver::SetError()
	*/
	virtual void SetError(TInt aErr) = 0;
	
	/**
	@see CDirectGdiDriver::CreateDrawableSource()
	
	@param  aHandle Handle of the newly created drawable source.
	@param	aRSgDrawable The RSgDrawable object to use when creating the drawable source.
	*/
	virtual TInt CreateDrawableSource(TInt& aHandle, const RSgDrawable& aRSgDrawable) = 0;
	
	/**
	@see CDirectGdiDriver::CloseDrawableSource()
	
	@param	aHandle A handle to a RDirectGdiDrawableSource object to be closed.
	*/
	virtual void CloseDrawableSource(TInt& aHandle) = 0;	
	
	/**
	@see CDirectGdiDriver::CreateImageTarget()
	
	@param  aHandle Handle of the newly created image target.
	@param	aRSgImage The RSgImage to use when creating the image target.
	*/
	virtual TInt CreateImageTarget(TInt& aHandle, const RSgImage& aRSgImage) = 0;
	
	/**
	@see CDirectGdiDriver::CloseImageTarget()
	
	@param	aHandle A handle to a RDirectGdiImageTarget object to be closed.
	*/
	virtual void CloseImageTarget(TInt& aHandle) = 0;
	
	/**
	@see CDirectGdiDriver::CreateEngine()
	*/
	virtual TInt CreateEngine(MDirectGdiEngine*& aEngine) = 0;

	/**
	@see CDirectGdiDriver::DestroyEngine()
	*/
	virtual void DestroyEngine(MDirectGdiEngine* aEngine) = 0;
	
	/**
	@see CDirectGdiDriver::GetInterface()
	 */
	virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface) = 0;
	
protected:
	inline CDirectGdiDriverInternal(RLibrary aLibrary);
private:
	/** Handle to the DirectGDI adapter DLL owned by this thread.*/
	const RLibrary iAdapterLibrary;
	};
	
#include <graphics/directgdidriverinternal.inl>


#endif //__DIRECTGDIDRIVERINTERNAL_H__
