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
// include\mmf\common\mmfutilities.inl
// 
//

/**
 
 Constructs the class with the supplied arguments.
 
 This constructor is used when there are multiple streams of the same media type on the same
 MDataSource.
 
 @param  aMediaType
 The media type (video, audio etc.).
 @param  aStreamId
 Identifies a specific stream when there are multiple streams of the same media type on 
 the same MDataSource.
*/
inline TMediaId::TMediaId(TUid aMediaType, TUint aStreamId) : iMediaType(aMediaType), iStreamId(aStreamId), iReserved1(0), iReserved2(0), iReserved3(0) 
	{
	}

/**
Constructs the class with the supplied argument.
	
This constructor is used when there is a single stream of the same media type on the same
MDataSource.

@param  aMediaType
        The media type (video, audio etc.).
*/
inline TMediaId::TMediaId(TUid aMediaType) : iMediaType(aMediaType), iStreamId(0), iReserved1(0), iReserved2(0), iReserved3(0) 
	{
	}

/**
Default constructor.
*/
inline TMediaId::TMediaId() : iMediaType(KNullUid), iStreamId(0), iReserved1(0), iReserved2(0), iReserved3(0) 
	{
	}

/**
Unpacks the FourCC code in little-endian format into the space provided by the argument.

The argument must be a pointer to a buffer of 4 bytes or greater.

@param aDes
       A buffer to hold the unpacked FourCC code.
*/
inline void TFourCC::FourCC( TPtr8* aDes ) const
	{
	TUint8 char1 = TUint8( iFourCC ) ;
	TUint8 char2 = TUint8( iFourCC >> 8 ) ;
	TUint8 char3 = TUint8( iFourCC >> 16 ) ;
	TUint8 char4 = TUint8( iFourCC >> 24 ) ;

	aDes->Format( _L8("%1c%1c%1c%1c"), char1, char2, char3, char4 ) ;
	}

