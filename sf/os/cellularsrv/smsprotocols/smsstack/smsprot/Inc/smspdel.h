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
 @internalAll 
*/


#if !defined (__SMSPDEL_H__)
#define __SMSPDEL_H__

#include "smspcomm.h"
#include <e32base.h>
#include <etelmm.h>
#include "smsuact.h"

class TSmsSettings;

/**
 *  @internalComponent
 */
class CSmsPDUDelete : public CSmsuActiveBase
	{
public:
	static CSmsPDUDelete* NewL(const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging);
	~CSmsPDUDelete();

	void Start(CArrayFix<TGsmSmsSlotEntry>& aLocationArray, TRequestStatus& aStatus);

protected:
	void DoRunL();
	void DoCancel();

private:
    CSmsPDUDelete(const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging);
	void ConstructL();
	void OpenStore();
	void DeletePDU();
	void Complete(TInt aError);

private:
	enum TSmsPDUDeleteState
		{
		ESmsPDUDeleteIdle,
		ESmsPDUDeleteOpeningStore,
		ESmsPDUDeleteDeletingPDU
		};
private:
	TSmsPDUDeleteState iState;			//< The current state of the state machine

	const TSmsSettings& iSmsSettings;	//< Global SMS Protocol settings

	RMobileSmsMessaging& iSmsMessaging;	//< Reference to the RMobileSmsMessaging class
	RMobileSmsStore iSmsStorage;		//< Reference to the ETEL Mobile Sms Store

	CArrayFix<TGsmSmsSlotEntry>* iLocationArray;		//< Pointer to the TGsmSmsSlotEntry class array. Not "owned" by this class.
	RMobilePhone::TMobileName iCurrentStore;	//< Class attribute for current store name
	};



//#include "smspdel.inl"

#endif // !defined __SMSPDEL_H__
