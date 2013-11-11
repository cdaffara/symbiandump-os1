// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file is taken from the project t_inter.
// No changes has been made to file.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _TE_GPRSCFGFILE_H_
#define _TE_GPRSCFGFILE_H_

#include <e32base.h>


/**
 * \class CConfigParams
 * This class handles a ini file.
 * Functions for reading a string and a TInt are defined.
 *
 */


class CConfigParams : public CBase
	{
public:
	static CConfigParams* NewL(const TDesC &aCfgFileName);
	const TPtrC FindAlphaVar(const TDesC &aVarName,const TDesC &aDefault);
	TInt	FindNumVar(const TDesC &aVarName, const TInt aDefault);
	CConfigParams(){};
protected:
	void ConstructL(const TDesC &aCfgFileName);
//	CConfigParams(){};
private:
	TBool iFileExist;
	TBuf8<500> iConfigParamBuf8;
	TBuf<1000> iConfigParamBuf;
	};


#endif
