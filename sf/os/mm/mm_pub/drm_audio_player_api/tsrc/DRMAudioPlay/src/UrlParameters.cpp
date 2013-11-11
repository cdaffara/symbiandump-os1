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
* Description:  DRM Player
*
*/


#include "UrlParameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*void CUrlParameters::ConstructL(TDesC8 &aDes8)
	{
	iDes8 = aDes8;
	}
*/

/*CUrlParameters* CUrlParameters::NewLC(TInt aAction, TDesC8 &aDes8)
	{
	CUrlParameters *__self = new (ELeave) CUrlParameters(aAction, aDes8);
	CleanupStack::PushL(__self);
	//__self->ConstructL(aDes8);
	return __self;
	}
*/

/*CUrlParameters* CUrlParameters::NewL(TInt aAction, TDesC8 &aDes8)
	{
	CUrlParameters *__self = CUrlParameters::NewLC(aAction, aDes8);
	CleanupStack::Pop(__self);
	return __self;
	}
*/

//CUrlParameters::CUrlParameters(TInt aAction, TDesC8 &aDes8) : CParameters(aAction) , iDes8(aDes8) {}
CUrlParameters::CUrlParameters(TInt aAction, TDesC &aUrl, TInt aIapId, TDesC8 &aMimeType) : CParameters(aAction) , iMimeType(aMimeType) , iIapId(aIapId) , iUrl(aUrl) {}

TDesC8 &CUrlParameters::GetMimeType()
	{
	return iMimeType;
	}
TDesC &CUrlParameters::GetUrl()
	{
	return iUrl;
	}
TInt CUrlParameters::GetIapId()
	{
	return iIapId;
	}

/*CUrlParameters::~CUrlParameters()
	{
	//delete iDes8;
	}
*/

/*TDesC8 &CUrlParameters::GetDes8()
	{
	return iDes8;
	}
*/

/*
TFileName CFileNameParameters::GetFileName()
	{
	return iFileName;
	}
*/
