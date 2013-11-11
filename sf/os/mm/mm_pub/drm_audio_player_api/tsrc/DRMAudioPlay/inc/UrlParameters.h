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


//#if !defined(AFX_URLPLAYPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
//#define AFX_URLPLAYPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#ifndef __URLPLAY_PARAMETERS
#define __URLPLAY_PARAMETERS

#include "Parameters.h"

class CUrlParameters : public CParameters
{
protected:

public:
	CUrlParameters(TInt aAction, TDesC &aUrl, TInt aIapId, TDesC8 &aMimeType);

	//static CUrlParameters* NewL(TInt aAction, TDesC &aUrl, TInt aIapId, TDesc8 &aMimeType);
	//static CUrlParameters* NewLC(TInt aAction, TDesC &aUrl, TInt aIapId, TDesc8 &aMimeType);
	//virtual ~CUrlParameters();

	TDesC8 &GetMimeType();
	TDesC &GetUrl();
	TInt GetIapId();

private:
	//void ConstructL(TDesC8 &aDes8);
	TDesC8 &iMimeType;
	TInt iIapId;
	TDesC &iUrl;
};

//#endif // !defined(AFX_URLPLAYPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
#endif	//__URLPLAY_PARAMETERS
