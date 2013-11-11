// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants for the Gdi component.

#ifndef GDICONSTS_H
#define GDICONSTS_H
/**
The default screen number. 
@internalTechnology
*/
const TInt KDefaultScreenNo = 0;

/**
Declaration of constant TUids for APIExtension to use as identifiers.
@internalTechnology
@released
*/
const TUid KGetUnderlineMetrics = {0x102827FB};
const TUid KSetFastBlendDisabled = {0x10285A30};
const TUid KSetShadowColor = {0x10282DA1};
const TUid KGetShadowColor = {0x10282DA2};
const TUid KDrawTextInContextUid = {0x10285BB3};
const TUid KDrawTextInContextVerticalUid = {0x10285BB4};
const TUid KDrawBoxTextInContextUid = {0x10285D4E};
const TUid KDrawBoxTextInContextVerticalUid = {0x10285D4F};
const TUid KTextInContextWidthInPixelsUid = {0x10285D50};
const TUid KUidIsFbsBitmapGc = {0x10285BBE};


#endif /* GDICONSTS_H */
