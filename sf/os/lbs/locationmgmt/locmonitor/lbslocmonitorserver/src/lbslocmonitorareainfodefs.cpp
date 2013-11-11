/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "lbslocmonitorareainfodefs.h"
#include "lbsdevloggermacros.h"

TLbsLocMonitorAreaInfoBase::TLbsLocMonitorAreaInfoBase()
	{
	LBSLOG(ELogP1,"->TLbsLocMonitorAreaInfoBase::TLbsLocMonitorAreaInfoBase");
	}

TUint32 TLbsLocMonitorAreaInfoBase::AreaInfoClassType() const
	{
	LBSLOG(ELogP1,"->TLbsLocMonitorAreaInfoBase::AreaInfoClassType");
	return iAreaInfoClassType;
	}

TLbsLocMonitorAreaInfoGci::TLbsLocMonitorAreaInfoGci():
	iMcc(0), iMnc(0), iLac(0), iCid(0), iValidity(EFalse), iIs3gNetworkMode(EFalse)
	{
	LBSLOG(ELogP1,"->TLbsLocMonitorAreaInfoBase::TLbsLocMonitorAreaInfoGci");
	iAreaInfoClassType = EAreaGciClass;
	}
