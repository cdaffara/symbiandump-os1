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

#ifndef __SSMCOMMANDLISTUTILS_H__
#define __SSMCOMMANDLISTUTILS_H__

#include <e32base.h>
#include <ssm/ssmcmd.hrh>

class CSsmCommandList;

class CSsmCommandListUtils : public CBase	
	{
public:
	IMPORT_C static TBool IsValidStateList(const CSsmCommandList& aCommandList);
	IMPORT_C static TBool IsValidSwpList(const CSsmCommandList& aCommandList);
	
private:
	static TBool IsValidList(const CSsmCommandList& aCommandList, TSsmCommandType aRequiredType);
	static TBool ValidateDeferredWaitCommand(const CSsmCommandList& aCommandList);

private:
	CSsmCommandListUtils();
	~CSsmCommandListUtils();
	};
	
#endif // __SSMCOMMANDLISTUTILS_H__
