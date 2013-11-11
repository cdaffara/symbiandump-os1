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


#ifndef T_RSOCKETSERVDATA_H
#define T_RSOCKETSERVDATA_H

#include "DataWrapperBase.h"
#include <es_sock.h>

class CT_RSocketServData: public CDataWrapperBase
	{
public:
	static CT_RSocketServData* NewL();
	virtual ~CT_RSocketServData();

private:
	CT_RSocketServData();
	void ConstructL();
	
public:	
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

private:
	void DoCmdConnect();
	void DoCmdClose();

private:
	RSocketServ* 	iSocketServ;
	
	};
	
#endif // T_RSOCKETSERVDATA_H

	