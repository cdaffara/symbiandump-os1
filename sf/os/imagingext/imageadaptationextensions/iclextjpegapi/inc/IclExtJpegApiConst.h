/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of constants for IclExtJpegApi
*
*/

#ifndef _ICLEXTJPEGAPICONST_
#define _ICLEXTJPEGAPICONST_

//----------------------------------------------------------------------------
// Common const definitions:
//----------------------------------------------------------------------------
_LIT8( KJpgMimeType,"image/jpeg" );
_LIT( KJpgFileExt,".jpg" );

const TInt KMinSwVersion                = 64;
const TInt KMinHwVersion                = 0;
const TInt KMaxSwVersion                = 127;
const TInt KMaxHwVersion                = 63;

//----------------------------------------------------------------------------
// V2 specific variations:
//----------------------------------------------------------------------------

const TUid KUidExtIclImageFrameFormat   = { 0x11111111 };

//----------------------------------------------------------------------------
// Initial version specific variations:
//----------------------------------------------------------------------------

#endif // _ICLEXTJPEGAPICONST_


// End of File
