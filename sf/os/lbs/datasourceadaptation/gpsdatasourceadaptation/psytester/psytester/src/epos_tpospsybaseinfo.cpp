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



// INCLUDE FILES
#include    "epos_tpospsybaseinfo.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TPosPSYBaseInfo::TPosPSYBaseInfo() :  
    iName(NULL), 
    iVersion(0), 
    iUid(TUid::Uid(0))
    {
    }

// C++ constructor can NOT contain any code, that
// might leave.
//
TPosPSYBaseInfo::TPosPSYBaseInfo(
    const TUid aModuleId, 
    const TDesC& aDisplayName, 
    const TInt aVersion)   
    : iName(NULL),
      iVersion(aVersion), 
      iUid(aModuleId)
    {
    TInt copyLength = Min(aDisplayName.Length(), KMaximumNameLength);
    iName.Copy(aDisplayName.Left(copyLength)); 
    }

// ---------------------------------------------------------
// TPosPSYBaseInfo::Version
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYBaseInfo::Version() const
    {
    return iVersion;
    }

// ---------------------------------------------------------
// TPosPSYBaseInfo::Name
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TDesC& TPosPSYBaseInfo::Name()
    {
    return iName;
    }

// ---------------------------------------------------------
// TPosPSYBaseInfo::Uid
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid TPosPSYBaseInfo::Uid() const
    {
    return iUid;
    }

//  End of File
