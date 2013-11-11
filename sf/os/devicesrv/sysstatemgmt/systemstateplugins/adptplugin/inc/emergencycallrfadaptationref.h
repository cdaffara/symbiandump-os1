// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EMERGENCYCALLRFADAPTATION_H__
#define __EMERGENCYCALLRFADAPTATION_H__

#include <ssm/ssmadaptation.h>

/**
 Reference Emergency Call RF adaptation plug-in.
 Implements MEmergencyCallRfAdaptation to Activate and Deactivate RF for emergency calls.

 @internalComponent
 @released

 @see MEmergencyCallRfAdaptation
*/
class CEmergencyCallRfAdaptationRef : public CBase, public MEmergencyCallRfAdaptation
	{
public:
	static CEmergencyCallRfAdaptationRef* NewL();

	//from MEmergencyCallRfAdaptation
	void Release();
	void ActivateRfForEmergencyCall(TRequestStatus& aStatus);
	void DeactivateRfForEmergencyCall(TRequestStatus& aStatus);
	void Cancel();
	
private:
	CEmergencyCallRfAdaptationRef();
	~CEmergencyCallRfAdaptationRef();
	void ConstructL();
	//Helper function to check for P&S Key
    TBool IsTestPsKeyDefined();
	
private:    
	MEmergencyCallRfAdaptation* iSaaEmergencyCallRfAdaptationDll;
	RLibrary iSaaEmergencyCallRfAdaptationLib;
	};

#endif // __EMERGENCYCALLRFADAPTATION_H__
