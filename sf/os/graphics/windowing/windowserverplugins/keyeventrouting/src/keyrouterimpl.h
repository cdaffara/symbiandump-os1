// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference implementation of Key Event Routing plug-in

/**
@file
@internalTechnology
@prototype
*/

#ifndef KEYROUTERIMPL_H
#define KEYROUTERIMPL_H

#include <graphics/wskeyrouter.h>

/** Key Capture Translation Table entry */
struct TTranslationEntry
	{
	TUint	iRequestScanCode;
	TUint	iCaptureScanCode;
	TUint	iRequestKeyCode;
	TUint	iCaptureKeyCode;
	};

/** Restricted Key Table entry */
struct TRestrictedKeyEntry
	{
	TUint			iCode;
	TKeyCaptureType	iType;
	TInt			iAppUidValue;
	};

/** Application Priority Table entry */
struct TAppPriorityEntry
	{
	TUint			iCode;
	TKeyCaptureType	iType;
	TInt			iAppUidValue;
	TInt			iAppPriority;
	};

#ifdef KEYROUTER_TEST
/** Blocked Key Table entry */
struct TBlockedKeyEntry
	{
	TUint			iCode;
	TKeyCaptureType	iType;
	};
#endif // KEYROUTER_TEST

#define TABLE_SIZE(table) (sizeof(table) / sizeof((table)[0]))

/**
Key Event Router implementation class
*/
NONSHARABLE_CLASS(CKeyEventRouterImpl) : public CKeyEventRouter
	{
public:
	CKeyEventRouterImpl();
	~CKeyEventRouterImpl();

	// From CKeyEventRouter
	void AddCaptureKeyL(const TKeyCaptureRequest& aRequest);
	void UpdateCaptureKeyL(const TKeyCaptureRequest& aRequest);
	void CancelCaptureKey(TKeyCaptureType aType, TAny* aHandle);
	void RouteKey(const TKeyEventRouterInput& aInput,
				  TKeyEventRouterOutput& aOutput);
private:
	void CheckCaptureKeyL(const TKeyCaptureRequest& aRequest);
	void ProcessAppPriorities(TKeyCaptureRequest& aRequest);
	void TranslateCaptureKey(TKeyCaptureType aType, TUint& aCode);
	TBool IsRestrictedKey(const TKeyCaptureRequest& aRequest);
#ifdef KEYROUTER_TEST
	TBool IsKeyBlocked(TKeyCaptureType aType, TUint aCode);
#endif

private:
	RArray<TKeyCaptureRequest>	iCaptureKeys;
	};

#endif // KEYROUTERIMPL_H
