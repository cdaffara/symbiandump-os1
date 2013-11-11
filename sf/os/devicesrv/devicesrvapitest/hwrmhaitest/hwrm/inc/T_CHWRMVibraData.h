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

 
#ifndef T_HWRMVIBRA_DATA_H
#define T_HWRMVIBRA_DATA_H

//User Includes
#include "datawrapperbase.h"

//System includes
#include <hwrmvibra.h>

/**
 * CT_CHWRMVibraData class
 *
 */
class CT_CHWRMVibraData : public CDataWrapperBase
	{
public:
	~CT_CHWRMVibraData();
	static	CT_CHWRMVibraData* NewL();

	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_CHWRMVibraData();
	void ConstructL();

private:
	TInt DoCmdNewL(const TTEFSectionName& aSection);
	TInt DoCmdStartVibraL(const TTEFSectionName& aSection);
	TInt DoCmdReserveVibraL(const TTEFSectionName& aSection);
	TInt DoCmdVibraStatus(const TTEFSectionName& aSection);
	TInt DoCmdDestructor();


private:
	void DestroyData();
	
	
private:
	
/**
 * HWRM vibra instance
 */
	CHWRMVibra* iVibra;

	static const TEnumEntryTable iEnumVibraStatus[];
	};

#endif // T_HWRMVIBRA_DATA_H
