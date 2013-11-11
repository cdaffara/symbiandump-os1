/**
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/

#ifndef WS_DISP_CHANGE_NOTIFY
#define WS_DISP_CHANGE_NOTIFY


#include <graphics/wsdisplaycontrol.h>
#include <e32base.h>
#include <e32cmn.h>
#include "EVENT.H"

class CWsDisplayChangeNotifier : public CActive
	{
	public:
		static CWsDisplayChangeNotifier* NewL(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner);
		void IssueNotificationRequest();
		void CancelNotificationRequest();
		~CWsDisplayChangeNotifier();
	protected:
		void ConstructL();
		virtual void RunL();
		virtual void DoCancel();
	private:
		CWsDisplayChangeNotifier(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner);
		TBool IsResListEqual(RArray<MWsDisplayControl::TResolution>& aResListA, RArray<MWsDisplayControl::TResolution>& aResListB);
		
		TRequestStatus* iRsStatus;
		MWsDisplayControl* iNextLevelInterface;
		CScreen *iOwner;
		CEventQueueRetry *iRetry;
				
		RArray<MWsDisplayControl::TResolution> iLastResList;
		TInt iLastErr;
	};

class CWsConfigChangeNotifier : public CActive
	{
	public:
		static CWsConfigChangeNotifier* NewL(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner);
		void IssueNotificationRequest();
		void CancelNotificationRequest();
		void UpdateLastSetConfiguration(TDisplayConfiguration& aNewConfig);
		~CWsConfigChangeNotifier();
	protected:
		void ConstructL();
		virtual void RunL();
		virtual void DoCancel();
	private:
		CWsConfigChangeNotifier(MWsDisplayControl* aNextLevelInterface, CScreen *aOwner);
		TBool IsConfigEqual(TDisplayConfiguration& aConfigA, TDisplayConfiguration& aConfigB);
		
		TRequestStatus* iRsStatus;
		MWsDisplayControl* iNextLevelInterface;
		CScreen *iOwner;
		CEventQueueRetry *iRetry;
		
		TDisplayConfiguration iLastConfig;
		// last set screen device SetConfiguration call
		TDisplayConfiguration iLastSetConfig;
	};

#endif
