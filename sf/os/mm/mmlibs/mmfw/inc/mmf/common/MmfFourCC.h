/**
@file
@publishedAll
@released
*/

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\mmf\common\mmffourcc.h
// 
//

#ifndef __MMF_COMMON_FOURCC_H__
#define __MMF_COMMON_FOURCC_H__


//Standard FourCC codes

/** 8-bit PCM
*/
const TUint32 KMMFFourCCCodePCM8 = 0x38502020;		//( ' ', ' ', 'P', '8' )

/** 8-bit unsigned PCM
*/
const TUint32 KMMFFourCCCodePCMU8 = 0x38555020;		//(' ', 'P', 'U', '8') 

/** 16-bit PCM
*/
const TUint32 KMMFFourCCCodePCM16 = 0x36315020;		//(' ', 'P', '1', '6')

/** 16-bit unsigned PCM
*/
const TUint32 KMMFFourCCCodePCMU16 = 0x36315550;	//('P', 'U', '1', '6')

/** Most-Significant-Byte-first (big endian) 16-bit PCM
*/
const TUint32 KMMFFourCCCodePCM16B = 0x42363150;	//( 'P', '1', '6', 'B' )

/** Most-Significant-Byte-first  (big endian)16-bit unsigned PCM
*/
const TUint32 KMMFFourCCCodePCMU16B = 0x42365550;	//( 'P', 'U', '6', 'B' )

/** IMA ADPCM
*/
const TUint32 KMMFFourCCCodeIMAD = 0x44414d49;		//('I', 'M', 'A', 'D')

/** IMA stereo ADPCM
*/
const TUint32 KMMFFourCCCodeIMAS = 0x53414d49;		//('I', 'M', 'A', 'S')

/** ALAW
*/
const TUint32 KMMFFourCCCodeALAW = 0x57414c41;		//('A', 'L', 'A', 'W')

/** MuLAW
*/
const TUint32 KMMFFourCCCodeMuLAW = 0x57414c75;		//('u', 'L', 'A', 'W')

/** GSM 610
*/
const TUint32 KMMFFourCCCodeGSM610 = 0x364d5347;	//('G', 'S', 'M', '6')

/** FourCC used if the actual fourCC is not known or not applicable
*/
const TUint32 KMMFFourCCCodeNULL = 0x4c4c554e;		//('N','U','L','L')

/** AMR
*/
const TUint32 KMMFFourCCCodeAMR = 0x524d4120;		//(' ','A','M','R')

/** Advanced Audio Codec (MPEG4, Audio layer 3)
*/
const TUint32 KMMFFourCCCodeAAC = 0x43414120;		//(' ','A','A','C')

/** Wideband AMR
*/
const TUint32 KMMFFourCCCodeAWB = 0x42574120;		//(' ','A','W','B')

/** MPEG, Audio layer 3
*/
const TUint32 KMMFFourCCCodeMP3 = 0x33504d20;		//(' ','M','P','3')

/** ATRAC3
*/
const TUint32 KMMFFourCCCodeATRAC3 = 0x33525441;	//('A','T','R','3')

/** SBC
*/
const TUint32 KMMFFourCCCodeSBC = 0x43425320;		//(' ','S','B','C')

/** AMR Wideband
*/
const TUint32 KMMFFourCCCodeAMRW = 0x57524d41;		//{'A', 'M', 'R', 'W'}

/** AAC Variant ADTS
*/
const TUint32 KMMFFourCCCodeADTS = 0x53544441;		//{'A', 'D', 'T', 'S'}

/** Windows Media Audio  (WMA)
*/
const TUint32 KMMFFourCCCodeWMA = 0x414d5720;		//{' ', 'W', 'M', 'A'}

/** Dolby Digital (AC-3)
*/
const TUint32 KMMFFourCCCodeAC3 = 0x33434120;		//{' ', 'A', 'C', '3'}

/** Dolby Digital Plus (E-AC-3)
*/
const TUint32 KMMFFourCCCodeEAC3 = 0x33434145;		//{'E', 'A', 'C', '3'}

#include <mmf/common/mmfutilities.h>


#endif
