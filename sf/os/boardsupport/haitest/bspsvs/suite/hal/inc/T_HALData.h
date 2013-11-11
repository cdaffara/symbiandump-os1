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


#if (!defined __T_HAL_DATA_H__)
#define __T_HAL_DATA_H__


//	User Includes
#include "DataWrapperBase.h"

//	EPOC Includes
#include <hal.h>

typedef	TBool (*THalFunctionSetPrepare)(CDataWrapperBase* aDataWrapper, const TDesC& aSectName, TInt& aValue);
typedef	TBool (*THalFunctionGetPrepare)(CDataWrapperBase* aDataWrapper, const TDesC& aSectName, TInt& aValueStart, TInt& aValueEnd);
typedef	void (*THalFunctionGetValidation)(CDataWrapperBase* aDataWrapper, const TDesC& aSectName, TInt aValue, TBool aForceValidation);
typedef	void (*THalFunctionStore)(CDataWrapperBase* aDataWrapper, TInt aValue);

/**
 * Test Active Notification class
 *
 */
class CT_HALData : public CDataWrapperBase
	{
public:
	struct THalTableLookup
		{
		const TDesC&				iHalString;
		HALData::TAttribute			iHalAttribute;
		THalFunctionSetPrepare		iHalFunctionSetPrepare;
		THalFunctionGetPrepare		iHalFunctionGetPrepare;
		THalFunctionGetValidation	iHalFunctionGetValidation;
		THalFunctionStore			iHalFunctionStore;
		TBool						iHalForceValidation;
		};

public:
	CT_HALData(const THalTableLookup* aTable);
	~CT_HALData();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	static void		GetValidationBool(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation);
	static void		GetValidationInt(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation);
	static void		GetValidationUint(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation);

	static TBool	SetPrepareInt(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValue);
	static TBool	SetPrepareUint(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValue);

protected:
	const THalTableLookup*		LookUp(const TDesC& aValue);

private:
	TInt	DoCmdSet(const TDesC& aSection);
	TInt	DoCmdGet(const TDesC& aSection);

protected:
	const THalTableLookup*	iTable;
	TInt					iValue;
	};

#endif /* __T_HAL_DATA_H__ */
