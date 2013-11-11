// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMOCKMSGHANDLER_H
#define CMOCKMSGHANDLER_H

#include "mmmmesshandlerbase.h"

class CMmMessageRouter;
class CMmDataPackage;


class CMockMsgHandler: public CBase, public MMmMessHandlerBase
	{
public: 
  	static CMockMsgHandler* NewL(CMmMessageRouter* aMessageRouter);
 	virtual ~CMockMsgHandler();

public: 
      virtual TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage);

private:
	CMockMsgHandler(CMmMessageRouter* aMessageRouter);
	void ConstructL();

private:
 	CMmMessageRouter* iMessageRouter;
	};

#endif