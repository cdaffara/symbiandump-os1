/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Collection of classes derived from TImageDataBlock used
*                to store and represent the Image data.
*
*/


// INCLUDE FILES
#include <JP2KImageData.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TUid KJP2KCommentUid     = { KJ2KCommentUid };
const TUid KJP2KIprBoxUid      = { KJ2KIprBoxUid };
const TUid KJP2KXmlBoxUid      = { KJ2KXmlBoxUid };
const TUid KJP2KUuidBoxUid     = { KJ2KUuidBoxUid };
const TUid KJP2KUuidInfoBoxUid = { KJ2KUuidInfoBoxUid };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TJp2kComment::TJp2kComment
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJp2kComment::TJp2kComment() :
    TImageDataBlock( KJP2KCommentUid ),
    iComment( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TJp2kComment::DuplicateL
// To duplicate the image data.
// -----------------------------------------------------------------------------
//
TImageDataBlock* TJp2kComment::DuplicateL( CFrameImageData& aFrameImageData ) const
    {
    TJp2kComment *jp2kComment = new ( ELeave ) TJp2kComment( *this );
    CleanupDeletePushL( jp2kComment );

    jp2kComment->iComment = jp2kComment->iComment->AllocLC();

    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kComment->iComment ) );
    CleanupStack::Pop( 2 ); // jp2kComment and jp2kComment->iComment

    return jp2kComment;
    }

// -----------------------------------------------------------------------------
// TJp2kIprBox::TJp2kIprBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJp2kIprBox::TJp2kIprBox() :
    TImageDataBlock( KJP2KIprBoxUid ),
    iIprData( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TJp2kIprBox::DuplicateL
// To duplicate the image data.
// -----------------------------------------------------------------------------
//
TImageDataBlock* TJp2kIprBox::DuplicateL( CFrameImageData& aFrameImageData ) const
    {
    TJp2kIprBox *jp2kIprBox = new ( ELeave ) TJp2kIprBox( *this );
    CleanupDeletePushL( jp2kIprBox );

    jp2kIprBox->iIprData = jp2kIprBox->iIprData->AllocLC();

    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kIprBox->iIprData ) );
    CleanupStack::Pop( 2 ); // jp2kIprBox and jp2kIprBox->iIprData

    return jp2kIprBox;
    }

// -----------------------------------------------------------------------------
// TJp2kXmlBox::TJp2kXmlBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJp2kXmlBox::TJp2kXmlBox() :
    TImageDataBlock( KJP2KXmlBoxUid ),
    iXmlData( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TJp2kComment::DuplicateL
// To duplicate the image data.
// -----------------------------------------------------------------------------
//
TImageDataBlock* TJp2kXmlBox::DuplicateL( CFrameImageData& aFrameImageData ) const
    {
    TJp2kXmlBox *jp2kXmlBox = new ( ELeave ) TJp2kXmlBox( *this );
    CleanupDeletePushL( jp2kXmlBox );

    jp2kXmlBox->iXmlData = jp2kXmlBox->iXmlData->AllocLC();

    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kXmlBox->iXmlData ) );
    CleanupStack::Pop( 2 ); // jp2kXmlBox and jp2kXmlBox->iXmlData

    return jp2kXmlBox;
    }

// -----------------------------------------------------------------------------
// TJp2kUuidBox::TJp2kUuidBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJp2kUuidBox::TJp2kUuidBox() :
    TImageDataBlock( KJP2KUuidBoxUid ),
    iUuidId( KJ2KUuidIDSize ),
    iUuidData( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TJp2kComment::DuplicateL
// To duplicate the image data.
// -----------------------------------------------------------------------------
//
TImageDataBlock* TJp2kUuidBox::DuplicateL( CFrameImageData& aFrameImageData ) const
    {
    TJp2kUuidBox *jp2kUuidBox = new ( ELeave ) TJp2kUuidBox( *this );
    CleanupDeletePushL( jp2kUuidBox );

    jp2kUuidBox->iUuidData = jp2kUuidBox->iUuidData->AllocLC();

    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kUuidBox->iUuidData ) );
    CleanupStack::Pop( 2 ); // jp2kUuidBox and jp2kUuidBox->iUuidData

    return jp2kUuidBox;
    }

// -----------------------------------------------------------------------------
// TJp2kUuidInfoBox::TJp2kUuidInfoBox
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJp2kUuidInfoBox::TJp2kUuidInfoBox() :
    TImageDataBlock( KJP2KUuidInfoBoxUid ),
    iUuidInfoNu( 0 ),
    iUuidInfoId( 0 ),
    iUuidInfoVersion( 0 ),
    iUuidInfoFlag( 0 ),
    iUuidInfoData( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TJp2kComment::DuplicateL
// To duplicate the image data.
// -----------------------------------------------------------------------------
//
TImageDataBlock* TJp2kUuidInfoBox::DuplicateL( CFrameImageData& aFrameImageData ) const
    {
    TJp2kUuidInfoBox *jp2kUuidInfoBox = new ( ELeave ) TJp2kUuidInfoBox( *this );
    CleanupDeletePushL( jp2kUuidInfoBox );

    jp2kUuidInfoBox->iUuidInfoId = jp2kUuidInfoBox->iUuidInfoId->AllocLC();
    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kUuidInfoBox->iUuidInfoId ) );
    CleanupStack::Pop( 1 ); // jp2kUuidInfoBox->iUuidInfoId

    jp2kUuidInfoBox->iUuidInfoData = jp2kUuidInfoBox->iUuidInfoData->AllocLC();
    User::LeaveIfError( aFrameImageData.AppendImageBuffer( jp2kUuidInfoBox->iUuidInfoData ) );
    CleanupStack::Pop( 2 ); // jp2kUuidInfoBox and jp2kUuidInfoBox->iUuidInfoData

    return jp2kUuidInfoBox;
    }
