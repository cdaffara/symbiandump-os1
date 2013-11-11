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
* Description:  Interface for quering system version information.
*
*/



// -----------------------------------------------------------------------------
// VersionInfo::TVersionBase::TVersionBase
// -----------------------------------------------------------------------------
//
inline VersionInfo::TVersionBase::TVersionBase( TInt aType ) :
        iType( aType )
    {
    }

// -----------------------------------------------------------------------------
// VersionInfo::TPlatformVersion::TPlatformVersion
// -----------------------------------------------------------------------------
//
inline VersionInfo::TPlatformVersion::TPlatformVersion() :
        TVersionBase( VersionInfo::EPlatformVersion )
    {
    }

// End of File
