// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NAPUPLINKAUTHORISATIONHELPER_H
#define NAPUPLINKAUTHORISATIONHELPER_H


#include <e32base.h>
#include <btextnotifiers.h>


class MNapUplinkAuthorisationRequester
	{
public:
	virtual void MnuarNapUplinkAuthorisationHelperComplete(TInt aError, TNapConnectionResult aResult) = 0;
	};

	
NONSHARABLE_CLASS(CNapUplinkAuthorisationHelper) : public CActive
	{
public:
	CNapUplinkAuthorisationHelper(MNapUplinkAuthorisationRequester& aNapUplinkAuthorisationRequester);
	~CNapUplinkAuthorisationHelper();

	void PerformNapUplinkAuthorisationL();

	TPanConnectionList& PanConnectionList(); 
private:

	void RunL();
	void DoCancel();
	
private:
	MNapUplinkAuthorisationRequester& iNapUplinkAuthorisationRequester;

	TNapConnectionResultPckg iResultBuf;
	
	RNotifier iNotifier;

	TPanConnectionListPckg iConnectionListPckg;
	};

#endif // NAPUPLINKAUTHORISATIONHELPER_H

