// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the body of the MMF recognizer utility class
// 
//

#ifndef __RECMMFUTILBODY_H__
#define __RECMMFUTILBODY_H__

#include "mmfcontrollerpluginresolver.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/taggeddataparser.h>
#endif

/**
 * @internalAll
 *
 * Body of MMF utility class used by MMF recognizer
 * Maintains an array of CMMFControllerImplementationInformation objects
 * so that data headers can be speedily matched against.
 * Updates the array when notified by ECOM of a change to the global
 * interface implementation registration data.
 */
NONSHARABLE_CLASS( CMmfRecognizerUtil::CBody ) : public CActive
	{
public:
	static void GetMimeTypesL(CDesC8Array* aMimeTypes);

	static CBody* NewL();

	~CBody();
	TMatchLevel GetMimeTypeL(const TDesC& aFileName, const TDesC8& aImageData, TDes8& aMimeType);

protected:
	virtual void DoCancel();
	virtual void RunL();

private:
	CBody();
	void ConstructL();
	void StartNotification();
	void BuildControllerListL();

private:
	/** an array of pointers to CMMFControllerImplementationInformation objects */
	RMMFControllerImplInfoArray iControllers; 
	/** for opening a session to the ECom server */
	REComSession iEcomSession;

	};

#endif
