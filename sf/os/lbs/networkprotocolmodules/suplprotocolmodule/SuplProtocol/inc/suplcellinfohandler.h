// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of cell info handler sub-component of SUPL Protocol Module
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef SUPLCELLINFOHANDLER_H
#define SUPLCELLINFOHANDLER_H

#include <etelmm.h>

class MSuplNetworkInfoObserver;

/** CSuplCellInfoHandler

This class uses a previously opened RMobilePhone subsession
reference to track changes to ETEL's cell information. The
observed values are reported back to the observer (tipically, the
SUPL Protocol Manager, who needs to know the Timing Advance value)
*/
NONSHARABLE_CLASS(CSuplCellInfoHandler) : public CActive
	{
public:
	static CSuplCellInfoHandler* NewL(MSuplNetworkInfoObserver& aObserver, const RMobilePhone& aCellPhone);
	~CSuplCellInfoHandler();

	void StartGettingCellInfo();

private:

	CSuplCellInfoHandler(MSuplNetworkInfoObserver& aObserver, const RMobilePhone& aCellPhone);
	void ConstructL();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void MonitorCellInfoChange();

private:

	MSuplNetworkInfoObserver& iObserver;
	const RMobilePhone& iCellPhone;
	
	RMobilePhone::TMobilePhoneCellInfoV9 iCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg iCellInfoPckg;
	
	TBool iMonitoring;
	};

#endif // SUPLCELLINFOHANDLER_H
