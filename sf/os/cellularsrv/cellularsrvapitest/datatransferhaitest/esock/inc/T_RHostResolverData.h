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


#ifndef T_RHOSTRESOLVER_H
#define T_RHOSTRESOLVER_H

#include "DataWrapperBase.h"
#include "es_sock.h"
#include "in_sock.h"


class CT_RHostResolverData : public CDataWrapperBase
{
public:
	static CT_RHostResolverData* NewL();
	static CT_RHostResolverData* NewLC();
	~CT_RHostResolverData();
	
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void RunL(CActive* aActive, TInt aIndex);
	
	void DoCmdClose();
	void DoCmdGetByName(const TTEFFunction& aSection, const TInt aAsyncErrorIndex);
	void DoCmdOpen( const TTEFFunction& aSection);
	void DoCmdDestructor();

	TNameEntry GetNameEntry();
	
private:
	void DestroyData();
	
protected:
	CT_RHostResolverData();
	void ConstructL();
	
private:
	RHostResolver*		iHostResolver;
	CActiveCallback*	iActiveCallback;
	TNameEntry			iNameEntry;
};

#endif // T_RHOSTRESOLVER_H

