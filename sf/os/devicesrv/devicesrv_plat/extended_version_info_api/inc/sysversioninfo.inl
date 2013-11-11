/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Extended Version Info API provides extended version 
*                information i.e. firmware, operator etc version information.
*
*/



// -----------------------------------------------------------------------------
// SysVersionInfo::TVersionBase::TVersionBase
// -----------------------------------------------------------------------------
//
inline SysVersionInfo::TVersionBase::TVersionBase( TInt aType ) :
        iType( aType )
    {
    }

// -----------------------------------------------------------------------------
// SysVersionInfo::TSymbianOSVersion::TSymbianOSVersion
// -----------------------------------------------------------------------------
//
inline SysVersionInfo::TSymbianOSVersion::TSymbianOSVersion() :
        TVersionBase( SysVersionInfo::ESymbianOSVersion )
    {
    }

// -----------------------------------------------------------------------------
// SysVersionInfo::TProductVersion::TProductVersion
// -----------------------------------------------------------------------------
//
inline SysVersionInfo::TProductVersion::TProductVersion() :
        TVersionBase( SysVersionInfo::EProductVersion )
    {
    }

// End of File
