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
// directhdidriverprocessstate.inl
// 
//

#ifndef DIRECTGDIDRIVERPROCESSSTATE_INL
#define DIRECTGDIDRIVERPROCESSSTATE_INL


inline const TSgDrawableId& CImageSourceData::SgImageId() const
	{
	return iSgImageId;
	}

inline EGLImageKHR CImageSourceData::EglImage() const
	{
	return iEglImage;
	}

inline VGImage CImageSourceData::VgImage() const
	{
	return iVgImage;
	}

inline TInt CImageSourceData::Open()
	{
	return ++iRefCount;
	}
	
inline TInt CImageSourceData::Close()
	{
	return --iRefCount;
	}


/**
Returns how the creation of the mutex in the constructor went. If this is not KErrNone, the mutex
was not created and the process state cannot be used.
*/ 
inline TInt XDirectGdiDriverProcessState::MutexCreationStatus() const
	{
	return iCreateMutexErr;
	}

#endif // DIRECTGDIDRIVERPROCESSSTATE_INL
