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

#ifndef LASREQUESTHANDLER_H
#define LASREQUESTHANDLER_H

#include <e32base.h>
#include <e32property.h>

#include "LbsLasCommon.h"

class CLasRequestHandler : public CActive
	{
public:
	static CLasRequestHandler* NewL(TInt aModuleNum, CLbsLocationSourceGpsBase* aLasModule);
   ~CLasRequestHandler();
	   
   void RunL();
   void DoCancel();
	
   private:
      CLasRequestHandler(TInt aModuleNum, CLbsLocationSourceGpsBase* aLasModule);
	   void ConstructL();
	   
	   void HandleRequest(const TLasRequest& aRequest);
	   
	private:
		TInt iModuleNum;
		CLbsLocationSourceGpsBase* iLocModule;  
		RProperty iProperty;
	};

#endif //LASREQUESTHANDLER_H
