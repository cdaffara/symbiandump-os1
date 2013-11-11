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

//#if !defined(AFX_FILENAMEPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
//#define AFX_FILENAMEPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#ifndef __FILE_NAME_PARAMETERS
#define __FILE_NAME_PARAMETERS

#include "Parameters.h"

class CFileNameParameters : public CParameters
{
protected:
	CFileNameParameters(TInt aAction);
public:
	static CFileNameParameters* NewL(TInt aAction, TDesC &aFileName);
	static CFileNameParameters* NewLC(TInt aAction, TDesC &aFileName);
	virtual ~CFileNameParameters();
	TFileName GetFileName();

private:
	void ConstructL(TDesC &aFileName);
	TFileName iFileName;
};

//#endif // !defined(AFX_FILENAMEPARAMETERS_H__77EC4487_6A81_4A9B_9CAE_C4D9913B5017__INCLUDED_)
#endif	//__FILE_NAME_PARAMETERS
