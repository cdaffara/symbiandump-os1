/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* NetDial Database Access Class Header.
* 
*
*/



/**
 @file Nd_dbacc.h
 @internalComponent
*/

#ifndef __ND_DBACC_H__
#define __ND_DBACC_H__

#include <etelmm.h>

#include <comms-infras/connectionsettings.h>
#include <comms-infras/dbaccess.h>

class CCommsDbNetDialAccess : public CBase
/**
CCommsDbNetDialAccess, one of these per state machine object.
*/
	{
public:
	static CCommsDbNetDialAccess* NewL(CCommsDbAccess* aDbAccess);
	~CCommsDbNetDialAccess();
	void Close();

// modem information
	void GetTsyNameL(TDes& aTsyName);
	void SetCommPortL(const RCall::TCommPort& aCommPort);
	void GetCommPortForDirectConnectL(RCall::TCommPort& aCommPort);
	void GetCommConfigForDirectConnectL(TCommConfig& aCommConfig);
	void GetCommPortRoleL(TCommRole& aRole);
	void GetCallParamsL(RCall::TCallParams& aParams);
	TInt GetRedialAttempts(TUint32& aAttempts);
	void GetBcaStackL(TDes& aBcaStack);
	TInt GetIapId();
	
// service information
	void GetRemoteParty(TDes& aRemoteParty);
	void IsDefaultTelNumL(TBool& aIsTelNum);
	void GetUsePctL(TBool& aUsePct);
	void GetScriptDetailsL(TBool& aUseScript,TInt& aLength);
	HBufC* GetScriptLC();	
	void GetLoginParamsL(TBool& aPromptForLogin,TDes& aLoginName,TDes& aLoginPass,TDes& aIPAddress);
	TInt GetCallbackTimeout(TUint32& aTimeout);

	void GetMmHscsdParametersL(RMobileCall::TMobileHscsdCallParamsV1& aMmHscsdParams);
	TBool GetMmCallParams(RMobileCall::TMobileDataCallParamsV1& aMmParams);

	TBool HscsdSettingsAvailableL();
	void CopyIspInitStringToModemL();

// dialling resolution
	void DoDiallingResolutionL(TDialString& aTelNum, TParseMode aMode = EForDialing);

private:
	CCommsDbNetDialAccess(CCommsDbAccess* aDbAccess);
	void OpenModemAndLocationTablesL();
	TInt AssertDialOutTable() const;
	void SetUnresolvedDialStringL(TDialString& aTelNum,const TDialString& aFullNum, TParseMode aMode = EForDialing);
	void PopulateModemCommSettingsL(TCommConfigV01& aCfg);
	CDefaultRecordAccess* ConvertFieldNameL(TDes& aName);

	void GetDefaultSettingL(CDefaultRecordAccess& aSetting, TConnectionSettings& aSettings);
	void OpenDefaultRecordAndSetL(CDefaultRecordAccess& aDefault, TUint32 aId, TBool aIAP);
	void GetDefaultL(const TDesC& aName,TUint32& aVal,TBool& aOverridden);
private:
	CCommsDbAccess* iDbAccess;
	};
	
#endif
