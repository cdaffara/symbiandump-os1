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


#if (!defined __DATA_WRAPPER_BASE__)
#define __DATA_WRAPPER_BASE__

//	EPOC includes
#include <datawrapper.h>

class CDataWrapperBase : public CDataWrapper
	{
public:
	TBool	GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult);
	TBool	GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	TBool	GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);
	TBool	GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	virtual void	InitialiseL();
	inline RFs&		FileServer() { return iFs; }

protected:
	CDataWrapperBase();
	virtual ~CDataWrapperBase();


private:
	TBool	GetCommandStringParameterL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);

private:
	// Included ini files
	RPointerArray<CIniData>	iInclude;
	RPointerArray<HBufC>	iBuffer;
	RFs                     iFs;
	};

#endif /* __DATA_WRAPPER_BASE__ */
