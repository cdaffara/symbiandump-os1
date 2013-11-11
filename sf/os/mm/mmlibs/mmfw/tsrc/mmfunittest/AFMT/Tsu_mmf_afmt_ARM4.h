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
//

#ifndef __TSU_MMF_AFMT_ARM4_H
#define __TSU_MMF_AFMT_ARM4_H

// WAV PCM Test files
_LIT(KWavPcm16BitMono22Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono22khz.wav");
_LIT(KWavPcm16BitMono8Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono8khz.wav");
_LIT(KWavPcm8BitMono11Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono11khz.wav");
_LIT(KWavPcm16BitStereo8Khz,	"c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khz.wav");
_LIT(KWavPcm8BitMono44khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono44khz.wav");
	  
// AU PCM Test files
_LIT(KAuPcm16BitMono22Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono22khz.au");
_LIT(KAuPcm16BitMono8Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono8khz.au");
_LIT(KAuPcm8BitMono11Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono11khz.au");
_LIT(KAuPcm16BitStereo8Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khz.au");
_LIT(KAuPcm8BitMono44khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono44khz.au");

// RAW PCM Test Files
_LIT(KRawPcm16BitMono22Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono22khz.raw");
_LIT(KRawPcm16BitMono8Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm16mono8khz.raw");
_LIT(KRawPcm8BitMono11Khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono11khz.raw");
_LIT(KRawPcm16BitStereo8Khz,	"c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khz.raw"); 
_LIT(KRawPcm8BitMono44khz,		"c:\\mm\\mmf\\testfiles\\afmt\\pcm8mono44khz.raw");

// Compare files (WAV/AU/RAW)
_LIT(KWavPcm16BitStereo8KhzCompare, "c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khzCompare.Wav");
_LIT(KAuPcm16BitStereo8KhzCompare,	"c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khzCompare.Au");
_LIT(KRawPcm16BitStereo8KhzCompare, "c:\\mm\\mmf\\testfiles\\afmt\\pcm16stereo8khzCompare.raw");

// WAV Adpcm files for testing FillBufferL
_LIT(KWavAdpcm16BitStereo8Khz,			"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16stereo8khz.Wav");
_LIT(KWavAdpcm16BitStereo8KhzCompare,	"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16stereo8khzCompare.Wav");
_LIT(KWavAdpcm16BitMono8Khz,			"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16mono8khz.Wav");
_LIT(KWavAdpcm16BitMono8KhzCompare,		"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16mono8khzCompare.Wav");

// WAV GSM files for testing FillBufferL
_LIT(KWavGsmMono8Khz,		 "c:\\mm\\mmf\\testfiles\\afmt\\gsmMono8khz.Wav");
_LIT(KWavGsmMono8KhzCompare, "c:\\mm\\mmf\\testfiles\\afmt\\gsmMono8khzCompare.Wav");

// RAW Adpcm files for testing FillBufferL
_LIT(KRawAdpcm16BitStereo8Khz,			"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16stereo8khz.Raw");
_LIT(KRawAdpcm16BitStereo8KhzCompare,	"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16stereo8khzCompare.Raw");
_LIT(KRawAdpcm16BitMono8Khz,			"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16mono8khz.Raw");
_LIT(KRawAdpcm16BitMono8KhzCompare,		"c:\\mm\\mmf\\testfiles\\afmt\\adpcm16mono8khzCompare.Raw");


// RAW GSM files for testing FillBufferL
_LIT(KRawGsmMono8Khz,		 "c:\\mm\\mmf\\testfiles\\afmt\\gsmMono8khz.Raw");
_LIT(KRawGsmMono8KhzCompare, "c:\\mm\\mmf\\testfiles\\afmt\\gsmMono8khzCompare.Raw");

// NON-existing sink files (for SetNumChannels tests etc)
// These are kept on C: as they need to be writeable.
_LIT(KNonExistingWavFile,	"c:\\mm\\mmf\\testfiles\\afmt\\ThisFileShouldNotExist.wav");
_LIT(KNonExistingAuFile,	"c:\\mm\\mmf\\testfiles\\afmt\\ThisFileShouldNotExist.au");
_LIT(KNonExistingRawFile,	"c:\\mm\\mmf\\testfiles\\afmt\\ThisFileShouldNotExist.raw");

// Blank sink files (for OOM tests etc)
_LIT(KEmptyWavFile,	"c:\\mm\\mmf\\testfiles\\afmt\\EmptyFile.wav");
_LIT(KEmptyAuFile,	"c:\\mm\\mmf\\testfiles\\afmt\\EmptyFile.au");
_LIT(KEmptyRawFile,	"c:\\mm\\mmf\\testfiles\\afmt\\EmptyFile.raw");

#endif
