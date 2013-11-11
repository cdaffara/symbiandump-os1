// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// FaxDefn Header
// 
//



/**
 @file
 @publishedAll
 @released
*/

#ifndef __FAXDEFN_H
#define	__FAXDEFN_H

#include <e32std.h>

//
// Fax class definition
//
/** Defines the fax modem classes supported by the fax client.

The fax class refers to the command set used to control the modem. 

@publishedAll
@released
*/
enum TFaxClass
	{
	/** Automatic detection of the fax modem class. */
	EClassAuto=0,
	/** Fax class 1 */
	EClass1=1,
	/** Fax class 2 */
	EClass2=2,
	/** Fax class 2.0 */
	EClass2point0=4,
	/** Fax class 1.0 */
	EClass1point0=8,
	/** Fax class 2.1 */
	EClass2point1=16
	};

//
// Fax phase definition
//
/** Fax session phases.

Defines what is done if a fax session fails is dependent on the current phase. 
@publishedAll
@released
*/
enum TFaxPhase
	{
	/** The session has not yet begun */
	ENotYetStarted,
	/** Phase 'A'. Establishing connection. */
	ECallEstablishment='A',
	/** Phase 'B'. Negotiating fax protocols. */
	ESessionNegotiation='B',
	/** Phase 'C'. Transferring information */
	EDataTransfer='C',
	/** Phase 'D'. Confirming successful send/receive. */
	EPostPageStatus='D',
	/** Phase 'E'. Disconnecting. */
	EDisconnection='E'
	};

//
// Fax resolution definition
//
/** Defines the supported fax resolutions. 

It is also used by the fax store classes. 

@publishedAll
@released
*/
enum TFaxResolution
	{
	/** Normal fax resolution 3.85 lines/mm */
	EFaxNormal,
	/** Fine fax resolution 7.7 lines/mm */
	EFaxFine
	};

//
// Fax compression definition
//
/** Defines the types of compression supported by the fax client.

This enumeration is also used by the fax store classes. 

@publishedAll
@released
*/
enum TFaxCompression
	{
	/** One-dimensional T.4 encoding. Also known as MH. */
	EModifiedHuffman=0,
	/** Two-dimensional T.4 encoding. Also known as MR. */
	EModifiedRead=1,
	/** Two-dimensional T.4 encoding including uncompressed mode. */
	EUncompressedModifiedRead=2,
	/** T.6 encoding. Not supported by the fax engine. Also known as MMR. */
	EModifiedModifiedRead=3
	};

/**
Scan Line Constants
@publishedAll
@released
*/
const TInt KFaxPixelsPerScanLine(1728);
const TInt KFaxBytesPerScanLine(216);	

const TInt KFaxCmPerScanLine(215);
const TInt KFaxScanLinesPer10Cm(385);
const TInt KFaxFineScanLinesPer10Cm(770);
const TInt KFaxT4MaxDesLength(896);
const TInt KFaxLinesPerBand(64);

/** Defines a buffer to hold a fax sender ID. */
typedef TBuf<20> TFaxBufSenderId;
/** Defines a raw scan line. 

Each scan line is comprised of KFaxBytesPerScanLine (216) bytes. */
typedef TBuf8 < KFaxBytesPerScanLine > TRawScanLine;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "fax_reversebytes.h"
#endif

#endif
