/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: DRM Player
*
*/

#include "FileNameParameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CFileNameParameters::ConstructL(TDesC &aFileName)
	{
	iFileName = aFileName;
	}

CFileNameParameters* CFileNameParameters::NewLC(TInt aAction, TDesC &aFileName)
	{
	CFileNameParameters *__self = new (ELeave) CFileNameParameters(aAction);
	CleanupStack::PushL(__self);
	__self->ConstructL(aFileName);
	return __self;
	}

CFileNameParameters* CFileNameParameters::NewL(TInt aAction, TDesC &aFileName)
	{
	CFileNameParameters *__self = CFileNameParameters::NewLC(aAction, aFileName);
	CleanupStack::Pop(__self);
	return __self;
	}

CFileNameParameters::CFileNameParameters(TInt aAction) : CParameters(aAction) {}

CFileNameParameters::~CFileNameParameters()
	{
	//delete iFileName;
	}

TFileName CFileNameParameters::GetFileName()
	{
	return iFileName;
	}
