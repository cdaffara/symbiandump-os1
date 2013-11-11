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


#include "testmetadata.h"
#include <elements/metatype.h>
#include <elements/metabuffer.h>

START_ATTRIBUTE_TABLE(TTestMetaData, KTestMetaDataUid, KTestMetaDataTypeId)
    REGISTER_ATTRIBUTE(TTestMetaData, iValue, TMetaNumber)
    REGISTER_ATTRIBUTE(TTestMetaData, iText, TMetaBuf16)
END_ATTRIBUTE_TABLE()


Meta::SMetaData* TTestMetaData::LoadL(TPtrC8& aDes)
    {
    TTestMetaData* self = new(ELeave)TTestMetaData();
    CleanupStack::PushL(self);
    User::LeaveIfError(self->Load(aDes));
    CleanupStack::Pop();
    return self;
    }
