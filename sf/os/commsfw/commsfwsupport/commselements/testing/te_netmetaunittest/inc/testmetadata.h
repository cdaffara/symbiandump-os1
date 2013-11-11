// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TESTMETADATA_H
#define TESTMETADATA_H

#include <e32std.h>
#include <e32base.h>

#include <elements/metadata.h>


const TInt KBuffSize = 24;
const TUint32 KTestMetaDataUid = 0x2FFFFFF;
const TUint32 KTestMetaDataTypeId = 1;

/**
 *  TTestMetaData
 */
class TTestMetaData : public Meta::SMetaData
    {
public:
    TTestMetaData()
        : iValue(0)
        {
        iText = KNullDesC;
        };
        
    static Meta::SMetaData* LoadL(TPtrC8& aDes);
    
    DATA_VTABLE

public:
    TInt iValue;
    TBuf<KBuffSize> iText;
    };

#endif
// TESTMETADATA_H
