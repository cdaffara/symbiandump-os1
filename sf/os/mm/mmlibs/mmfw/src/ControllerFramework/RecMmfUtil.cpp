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
//

#include "RecMmfUtilBody.h"


/**
 * @internalAll
 *
 * constructor for MMF Recognizer utility class
 */
CMmfRecognizerUtil::CMmfRecognizerUtil()
	{
	}

CMmfRecognizerUtil::~CMmfRecognizerUtil()
	{
	delete iBody;
	}

void CMmfRecognizerUtil::ConstructL()
	{
	iBody = CMmfRecognizerUtil::CBody::NewL();
	}

/**
 * @internalAll
 *
 * Determine whether the supplied data header on its own is recognized,
 * or the data header plus the file suffix together are recognized,
 * and if so return the associated MIME type.
 *
 * @param	aFileName
 *			The name of the file
 * @param	aImageData
 *          A descriptor containing the header
 * @param   aMimeType
 *          A user-supplied descriptor in which the MIME type is returned
 * @leave   KErrUnderflow 
 *			Not enough data in descriptor to identify whether it is supported
 *          by any plugin.
 * @leave	This method may also leave with one of the system-wide error codes.
 * @return  EMatchNone if a match was not found.
 *			EMatchData if a data match was found.
 *			EMatchName if a data and file suffix match was found.
 * @post    If recognized, the caller's descriptor is filled with the MIME types
 */
EXPORT_C CMmfRecognizerUtil::TMatchLevel CMmfRecognizerUtil::GetMimeTypeL(const TDesC& aFileName, const TDesC8& aImageData, TDes8& aMimeType)
	{
	return iBody->GetMimeTypeL(aFileName, aImageData, aMimeType);
	}

/**
 * @internalAll
 *
 * Static factory constructor. Uses two phase
 * construction and leaves nothing on the cleanup stack
 *
 * @leave KErrNoMemory
 * @return A pointer to the newly created CMmfRecognizerUtil object
 *
 */
EXPORT_C CMmfRecognizerUtil* CMmfRecognizerUtil::NewL()
	{
	CMmfRecognizerUtil* self=new (ELeave) CMmfRecognizerUtil();   
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * @internalAll
 *
 * Get all the mime types supported by MMF
 * @param	aMimeTypes
 *          A caller-supplied array of descriptors.
 * @leave	This method may leave with one of the system-wide error codes.
 * @post    The caller's array is filled with the supported MIME types
 */
EXPORT_C void CMmfRecognizerUtil::GetMimeTypesL(CDesC8Array* aMimeTypes)
	{
	CMmfRecognizerUtil::CBody::GetMimeTypesL(aMimeTypes);
	}

