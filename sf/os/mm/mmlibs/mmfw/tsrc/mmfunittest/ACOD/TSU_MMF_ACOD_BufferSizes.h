/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __TSU_MMF_ACOD_BufferSizes_H__
#define __TSU_MMF_ACOD_BufferSizes_H__

const TUint KDefaultBufferSize = 0x1000;
const TUint KLargeBufferSize = 10000;
const TUint KSmallBufferSize = 50;
const TUint KSmallBufferSizeGSM = 640;
const TUint KMaxBufferLength = 10000;

//Optimum source buffer sizes
const TUint KAlawToPCM16OptimumSrc = 0x800;
const TUint KPCM16ToAlawOptimumSrc = 0x1000;
const TUint KPCM8ToPCM16OptimumSrc = 0x800;
const TUint KPCM16ToPCM8OptimumSrc = 0x1000;
const TUint KPCM16ToPCM16BOptimumSrc = 0x1000;
const TUint KPCM16BToPCM16OptimumSrc = 0x1000;
const TUint KPCMU16ToPCMU16BOptimumSrc = 0x1000;
const TUint KPCMU16BToPCMU16OptimumSrc = 0x1000;
const TUint KPCM16ToPCMU16BOptimumSrc = 0x1000;
const TUint KPCMU16BToPCM16OptimumSrc = 0x1000;
const TUint KPCM16ToPCMU16OptimumSrc = 0x1000;
const TUint KPCMU16ToPCM16OptimumSrc = 0x1000;
const TUint KPCM16ToPCMU8OptimumSrc = 0x1000;
const TUint KPCMU8ToPCM16OptimumSrc = 0x800;
const TUint KMulawToPCM16OptimumSrc = 0x800;
const TUint KPCM16ToMulawOptimumSrc = 0x1000;
const TUint KAdpcmToPCM16OptimumSrc = 0x400;
const TUint KPCM16ToAdpcmOptimumSrc = 0xFC8;
const TUint KStereoAdpcmToPCM16OptimumSrc = 0x400;
const TUint KPCM16ToStereoAdpcmOptimumSrc = 0xF90;
const TUint KGSM610ToPCM16OptimumSrc = 0x104;
const TUint KPCM16ToGSM610OptimumSrc = 0xA00;

const TUint KGSM610ToPCM16OptimumSrcGSM = 0x280;  // 640 (Dec)

//Optimum dest buffer sizes
const TUint KAlawToPCM16OptimumDst = 0x1000;
const TUint KPCM16ToAlawOptimumDst = 0x800;
const TUint KPCM8ToPCM16OptimumDst = 0x1000;
const TUint KPCM16ToPCM8OptimumDst = 0x800;
const TUint KPCM16ToPCM16BOptimumDst = 0x1000;
const TUint KPCM16BToPCM16OptimumDst = 0x1000;
const TUint KPCMU16ToPCMU16BOptimumDst = 0x1000;
const TUint KPCMU16BToPCMU16OptimumDst = 0x1000;
const TUint KPCM16ToPCMU16BOptimumDst = 0x1000;
const TUint KPCMU16BToPCM16OptimumDst = 0x1000;
const TUint KPCM16ToPCMU16OptimumDst = 0x1000;
const TUint KPCMU16ToPCM16OptimumDst = 0x1000;
const TUint KPCM16ToPCMU8OptimumDst = 0x800;
const TUint KPCMU8ToPCM16OptimumDst = 0x1000;
const TUint KMulawToPCM16OptimumDst = 0x1000;
const TUint KPCM16ToMulawOptimumDst = 0x800;
const TUint KAdpcmToPCM16OptimumDst = 0xFC8;
const TUint KPCM16ToAdpcmOptimumDst = 0x400;
const TUint KStereoAdpcmToPCM16OptimumDst = 0xF90;
const TUint KPCM16ToStereoAdpcmOptimumDst = 0x400;
const TUint KGSM610ToPCM16OptimumDst = 0xA00;
const TUint KPCM16ToGSM610OptimumDst = 0x104;


const TUint KPCM16ToGSM610OptimumDstGSM = 0x280;
const TUint KGSM610ToPCM16OptimumDstGSM = 0x186A0; //100000 0x32C8; // 13000 (Dec)


#endif
