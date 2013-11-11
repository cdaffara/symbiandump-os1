/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalTechnology
@released
*/

#ifndef LBSASSISTANCEDATACACHE_H
#define LBSASSISTANCEDATACACHE_H

#include <e32property.h>
#include <lbs/lbsassistancedatabase.h>
#include "lbsprocessuiddefs.h"

//Max number of assistance data supported
const TInt KAssistanceDataListCount = 9;

/** RAssistanceDataCache is used to exchange assistance data between NG and AGPS Manager*/
class RAssistanceDataCache
    {
public:
    IMPORT_C static void InitializeL();
    IMPORT_C static void ShutDownL();

    IMPORT_C void OpenL();
    IMPORT_C void Close();

    IMPORT_C TInt SetAssistDataItem(TLbsAssistanceDataItem aItem,
			                    const RDataBuilderRootBase& aDataRoot);
    IMPORT_C TInt GetAssistDataItem(TLbsAssistanceDataItem aItem,
			                          RDataReaderRootBase& aDataRoot);
    IMPORT_C TInt ResetAssistanceData(TLbsAsistanceDataGroup aDataMask);

private:
	void AttachToPropertiesL();
	
	static TInt DeletePropertyL(TInt aPropertyKey);
	static TInt GetDataSize(TLbsAssistanceDataItem aItem);
	static TUint32 BitPosToNumber(TLbsAssistanceDataItem aItem);
	
private:
	RProperty iPropertyList[KAssistanceDataListCount];
    };

#endif // LBSASSISTANCEDATACACHE_H
