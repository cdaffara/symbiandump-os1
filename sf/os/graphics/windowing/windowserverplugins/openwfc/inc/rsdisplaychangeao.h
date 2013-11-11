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
//

#ifndef RS_DISP_CHANGE_NOTIFY
#define RS_DISP_CHANGE_NOTIFY

#include <e32base.h>
#include <graphics/wsdisplaycontrol.h>

class CDisplayPolicy;
class CRsDisplayChangeNotifier : public CActive
	{
	public:
		static CRsDisplayChangeNotifier* NewL(MWsDisplayControl* aNextLevelInterface, CDisplayPolicy* aDisplayPolicy);
		void IssueNotificationRequest();
		void CancelNotificationRequest();
		void RegisterActiveStatus(TRequestStatus &aStatus);
		~CRsDisplayChangeNotifier();
	protected:
		virtual void RunL();
		virtual void DoCancel();
	private:
		CRsDisplayChangeNotifier(MWsDisplayControl* aNextLevelInterface, CDisplayPolicy* aDisplayPolicy);
		
	private:
		TRequestStatus* iWsStatus;
		MWsDisplayControl* iNextLevelInterface;
		CDisplayPolicy* iDisplayPolicy;
	};

class CRsConfigChangeNotifier : public CActive
	{
	public:
		static CRsConfigChangeNotifier* NewL(MWsDisplayControl* aNextLevelInterface);
		void IssueNotificationRequest();
		void CancelNotificationRequest();
		void RegisterActiveStatus(TRequestStatus &aStatus);
		~CRsConfigChangeNotifier();
	protected:
		virtual void RunL();
		virtual void DoCancel();
	private:
		CRsConfigChangeNotifier(MWsDisplayControl* aNextLevelInterface);
		
	private:
		TRequestStatus* iWsStatus;
		MWsDisplayControl* iNextLevelInterface;
	};

#endif
