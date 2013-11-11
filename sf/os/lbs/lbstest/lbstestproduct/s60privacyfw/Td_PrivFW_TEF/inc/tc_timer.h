/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32base.h>

class CTdPrivFW;

class CTcTimer : public CTimer
	{
	public:
		CTcTimer(CTdPrivFW& aObserver);

		virtual ~CTcTimer();
		void ConstructL();
		
	protected:
		virtual void DoCancel();

		virtual void RunL();

	private:
		CTdPrivFW& iObserver;
	};
