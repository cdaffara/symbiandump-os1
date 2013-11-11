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


#ifndef T_CREPOSITORY_DATA_H
#define T_CREPOSITORY_DATA_H

//User Includes
#include "DataWrapperBase.h"

//Epoc Includes
#include <centralrepository.h>//CRepository

class CT_CRepositoryData : public CDataWrapperBase
	{
public:
	~CT_CRepositoryData();
	static	CT_CRepositoryData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	CT_CRepositoryData();
	
private:
	
	void ConstructL();

	void DestroyData();
	
private:
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdDestructor();
    void DoCmdSet(const TTEFSectionName& aSection);
    
private:
	/**
	 * Wrapped object
	 */
    CRepository* iRepository;
    	
	};


#endif //T_CREPOSITORY_DATA_H


