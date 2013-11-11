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

 
#ifndef T_RPROPERTYDATA_H
#define T_RPROPERTYDATA_H

//User Includes
#include "datawrapperbase.h"

//System includes
#include <e32property.h>


/**
 * CT_RPropertyData class
 *
 */
class CT_RPropertyData : public CDataWrapperBase
	{
public:
	~CT_RPropertyData();
	static	CT_RPropertyData* NewL();
	
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void RunL(CActive* aActive, TInt aIndex);

protected:
	CT_RPropertyData();
	void ConstructL();

private:
	void DoCmdNewL();
	void DoCmdGet(const TTEFSectionName& aSection);
	void DoCmdAttach(const TTEFSectionName& aSection);
	void DoCmdSubscribe(const TInt aAsyncErrorIndex);
	void DoCmdDestructor();

private:
	void DestroyData();

private:
	RProperty* iProperty; // wrapped object
	
	CActiveCallback* iActiveCallback;
	
	static const TEnumEntryTable iEnumPropertyKeys[];
	};

#endif /*T_RPROPERTYDATA_H*/
