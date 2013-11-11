// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent 
*/

#ifndef _CWRAPGETDETNET_H_
#define _CWRAPGETDETNET_H_

#include <etelmm.h>
#include <mmretrieve.h>

// Wrapper object for CRetrieveMobilePhoneDetectedNetworks.
// The CRetrieveMobilePhoneDetectedNetworks active object needs
// both a CActiveScheduler installed and for it to have been started. 
// As a result of starting the CActiveScheduler, another AO must be 
// implemented to stop the CActiveScheduler when the
// CRetrieveMobilePhoneDetetctedNetworks active object completes.

class CWrapperGetDetectedNetworks : public CActive
	{
public:
	static CWrapperGetDetectedNetworks* NewL(RMobilePhone& aPhone);
	~CWrapperGetDetectedNetworks();
	void Start();
	CMobilePhoneNetworkList *RetrieveList();

protected:
	CWrapperGetDetectedNetworks(RMobilePhone& aPhone);
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();

private:
	CRetrieveMobilePhoneDetectedNetworks* iRetrieve;
	RMobilePhone& iPhone3;
	};

#endif
