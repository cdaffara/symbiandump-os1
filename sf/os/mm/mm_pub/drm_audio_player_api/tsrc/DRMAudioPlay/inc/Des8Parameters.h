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


//#if !defined(AFX_DES8PARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
//#define AFX_DES8PARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#ifndef __DES8_PARAMETERS
#define __DES8_PARAMETERS

#include "Parameters.h"

class CDes8Parameters : public CParameters
{
protected:
	CDes8Parameters(TInt aAction, TDesC8 &aDes8);
public:
	static CDes8Parameters* NewL(TInt aAction, TDesC8 &aDes8);
	static CDes8Parameters* NewLC(TInt aAction, TDesC8 &aDes8);
	virtual ~CDes8Parameters();
	//TFileName GetFileName();
	TDesC8 &GetDes8();

private:
	//void ConstructL(TDesC8 &aDes8);
	//TFileName iFileName;
	TDesC8 &iDes8;
};

//#endif // !defined(AFX_DES8PARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
#endif	//__DES8_PARAMETERS
