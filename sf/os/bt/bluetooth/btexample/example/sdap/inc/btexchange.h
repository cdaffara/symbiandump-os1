// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTEXCHANGE_H
#define BTEXCHANGE_H

class CBluetoothSeeker : public CActive, private MSdpAgentNotifier
	{
	public:
		static CBluetoothSeeker* NewL(RSocketServ &aSession, const TUUID &aUUID);
		static CBluetoothSeeker* NewLC(RSocketServ &aSession, const TUUID &aUUID);
		~CBluetoothSeeker();
		void BeginUpdateL(TRequestStatus &aStatus);
		CArrayFixFlat<TNameEntry>& Names();
	private:
		CBluetoothSeeker(RSocketServ &aSession);
		void ConstructL(const TUUID &aUUID);
		void HandleInquiryResultL();
		void SDPQueryL();
		//virtual overrides from CActive
		virtual void RunL();
		virtual TInt RunError(TInt aErr);
		virtual void DoCancel();
		//virtual overrides from MSdpAgentNotifier
		virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
		virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
		virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);
		//member data
		RHostResolver iResolver;
		RSocketServ &iSession;
		TNameEntry iNameEntry;
		enum {EIdle, EInquiring, EServiceRequest} iState;
		CArrayFixFlat<TNameEntry> iNames;
		TRequestStatus* iUpdateStatus;
		TInt iDeviceIndex;
		CSdpAgent *iSdpAgent;
		CSdpSearchPattern* iSdpSearchPattern;
		TInquirySockAddr iInquiryAddress;
	};

#endif
