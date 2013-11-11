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
// Declaration of a Direct Graphics Device Interface (GDI) Driver, this class 
// defines the API which is intended to be used by DirectGDI client applications.
// 
//

#ifndef DIRECTGDIDRIVER_H
#define DIRECTGDIDRIVER_H

/**
@file
@publishedPartner
@prototype
*/

#include <e32base.h>


class RSgDrawable;
class RSgImage;
class RDirectGdiImageTarget;
class RDirectGdiDrawableSource;
class CDirectGdiContext;
class MDirectGdiEngine;
class CDirectGdiDriverInternal;
class CTDirectGdiDriver;

/**
Provides an interface which is intended to be used by DirectGDI client applications. It will be implemented using 
the handle/body idiom, where the handle part will form the public and private API that is intended to be used by 
DirectGDI client applications and other DirectGDI generic parts, and the body part defines the HAI that needs to be 
implemented by an adaptation. The implementation of the handle part will be generic for all adaptations. The implementation 
of the body part is adaptation specific and provides interfaces for initialisation, termination, construction 
of the concrete drawing engine and the creation of source or target rendering. Initialisation and termination are 
intended to give opportunities for an adaptation to perform any thread specific tasks which may be required. This class
is intended to be a singleton within a thread. It delegates to an adaptation-specific implementation that provides a 
platform-specific mechanism. 

@publishedPartner
@prototype
@deprecated
*/
NONSHARABLE_CLASS(CDirectGdiDriver): public CBase
	{
	friend class CDirectGdiContext;
	friend class RDirectGdiImageTarget;
	friend class RDirectGdiDrawableSource;
	friend class CTDirectGdiDriver;
	
public:
	IMPORT_C static CDirectGdiDriver* Static();
	IMPORT_C static TInt Open();
	IMPORT_C void Close();
	IMPORT_C void Flush();
	IMPORT_C void Finish();
	IMPORT_C TInt GetError();	
	IMPORT_C TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	
private:
	CDirectGdiDriver();
	~CDirectGdiDriver();
	TInt CreateDrawableSource(RDirectGdiDrawableSource& aRDirectGdiDrawableSource, const RSgDrawable& aRSgDrawable);
	void CloseDrawableSource(RDirectGdiDrawableSource& aRDirectGdiDrawableSource);
	TInt CreateImageTarget(RDirectGdiImageTarget& aRDirectGdiImageTarget, const RSgImage& aRSgImage);
	void CloseImageTarget(RDirectGdiImageTarget& aRDirectGdiImageTarget);
	TInt CreateEngine(MDirectGdiEngine*& aMDirectGdiEngine);
	void DestroyEngine(MDirectGdiEngine* aMDirectGdiEngine);
	IMPORT_C void SetError(TInt aErr);
#ifdef __WINS__
	TInt CreateInternalDriver(const RLibrary& aLibrary);
#else
	TInt CreateInternalDriver();
#endif

private:	
	TInt iOpenCount;	/**< A count of how many times this driver object has been opened.*/
	CDirectGdiDriverInternal* iDriverInternal;	/**< To delegate calls to adaptation.*/
	};

#endif //__DIRECTGDIDRIVER_H__
