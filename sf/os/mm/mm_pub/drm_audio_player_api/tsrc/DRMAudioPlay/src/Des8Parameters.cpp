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


#include "Des8Parameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*void CDes8Parameters::ConstructL(TDesC8 &aDes8)
	{
	iDes8 = aDes8;
	}
*/

CDes8Parameters* CDes8Parameters::NewLC(TInt aAction, TDesC8 &aDes8)
	{
	CDes8Parameters *__self = new (ELeave) CDes8Parameters(aAction, aDes8);
	CleanupStack::PushL(__self);
	//__self->ConstructL(aDes8);
	return __self;
	}

CDes8Parameters* CDes8Parameters::NewL(TInt aAction, TDesC8 &aDes8)
	{
	CDes8Parameters *__self = CDes8Parameters::NewLC(aAction, aDes8);
	CleanupStack::Pop(__self);
	return __self;
	}

CDes8Parameters::CDes8Parameters(TInt aAction, TDesC8 &aDes8) : CParameters(aAction) , iDes8(aDes8) {}

CDes8Parameters::~CDes8Parameters()
	{
	//delete iDes8;
	}

TDesC8 &CDes8Parameters::GetDes8()
	{
	return iDes8;
	}
/*
TFileName CFileNameParameters::GetFileName()
	{
	return iFileName;
	}
*/
