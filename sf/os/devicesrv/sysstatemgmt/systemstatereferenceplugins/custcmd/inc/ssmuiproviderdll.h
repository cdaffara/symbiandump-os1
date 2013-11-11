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

/**
 @file
 @publishedPartner
 @released
*/
#ifndef __SSMUIPROVIDERDLL_H__
#define __SSMUIPROVIDERDLL_H__

#include <ssm/strtsecuritystatus.h>
#include <e32def.h>
#include <e32cmn.h>
#include <e32base.h>
#include <f32file.h>

class CSsmUiSpecific : public CBase
    {
public:

	IMPORT_C ~CSsmUiSpecific();
	IMPORT_C static TUid StartupPSUid();
	IMPORT_C static TUid SecurityPinNotifierUid();
	IMPORT_C static TUint EmergencyCallPropertyKey();
	IMPORT_C static TUid EmergencyCallPropertyCategory();
	IMPORT_C static TBool IsSimSupported();
	IMPORT_C void SetSecurityStatus(const TStrtSecurityStatus& aSecurityStatus);
	IMPORT_C TStrtSecurityStatus SecurityStatus() const;
	IMPORT_C static CSsmUiSpecific* InstanceL();
	IMPORT_C static void Release();
	IMPORT_C static TUid ScreenOutputChannelUid();
	IMPORT_C static TUint SimStatusPropertyKey();
	IMPORT_C static TBool IsSimStateChangeAllowed();
	IMPORT_C static TBool IsAmaStarterSupported();
	IMPORT_C static HBufC* GetTsyModuleNameL();
	IMPORT_C static TUid StarterPSUid();
	IMPORT_C static HBufC* PhoneTsyNameL();
	IMPORT_C static TBool IsSimPresent();
	IMPORT_C static TBool IsSimlessOfflineSupported();
	IMPORT_C static TUint RFStatusPropertyKey();
	IMPORT_C static TUid RFStatusPropertyCategory();
	IMPORT_C static TBool IsNormalBoot();
	IMPORT_C static TBool IsSimChangedReset();
	IMPORT_C static TUint ValidateRTCPropertyKey();
	IMPORT_C static TUid ValidateRTCPropertyCategory();
	IMPORT_C static TInt PhoneMemoryRootDriveId();
	IMPORT_C TInt ReservePhoneMemorySpace();
	IMPORT_C TInt FreeReservedPhoneMemorySpace(const TInt aSpaceToFree);

private:
	CSsmUiSpecific();
	void ConstructL();
	
    //Helper function to check for P&S Key
    static TBool IsTestPsKeyDefined();
private:
	TStrtSecurityStatus iStrtSecurityStatus;
	TInt iReferenceCount;
	RFs iReservedPhoneMemoryFs;
	TInt iReservedPhoneMemory;
    };

#endif // __SSMUIPROVIDERDLL_H__
