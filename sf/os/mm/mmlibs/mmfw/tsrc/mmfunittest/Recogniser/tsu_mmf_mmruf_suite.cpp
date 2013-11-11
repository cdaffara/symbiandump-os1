// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <apmstd.h>
#include "tsu_mmf_mmruf_suite.h"
#include "tsu_mmf_mmruf_step.h"



/*
** The tests that are to be run.
** iFileName refers to files located in the \Recog directory.
*/
static const TTestSpec KTestSpecs[] =
	{
		{ 1, _S("c:\\Recog\\valid_angry_bone.3gp"), _S8("video/3gpp") },
		{ 2, _S("c:\\Recog\\3gpWidWrngExt.wxt"), _S8("video/3gpp") },
		{ 3, _S("c:\\Recog\\corrupt_vid.3gp"), _S8("video/3gpp") },

		{ 4, _S("c:\\Recog\\valid_chand_sifarish.aiff"), _S8("audio/aiff") },
		{ 5, _S("c:\\Recog\\AIFFWidWrngExt.wxt"), _S8("audio/aiff") },
		{ 6, _S("c:\\Recog\\Corrupt.aiff"), _S8("audio/aiff") },

		{ 7, _S("c:\\Recog\\valid_greenday.amr"), _S8("audio/amr") },
		{ 8, _S("c:\\Recog\\AMRWidWrngExt.wxt"), _S8("audio/amr") },
		{ 9, _S("c:\\Recog\\corrupt.amr"), _S8("audio/amr") },

		{ 10, _S("c:\\Recog\\valid_flinstones.awb"), _S8("audio/amr-wb") },
		{ 11, _S("c:\\Recog\\AWBWidWrngExt.wxt"), _S8("audio/amr-wb") },
		{ 12, _S("c:\\Recog\\Corrupt.awb"), _S8("audio/amr-wb") },

		{ 13, _S("c:\\Recog\\valid_chand_sifarish.au"), _S8("audio/basic") },
		{ 14, _S("c:\\Recog\\AUWAvWidWrngExt.wxt"), _S8("audio/basic") },
		{ 15, _S("c:\\Recog\\Corrupt.au"), _S8("audio/basic") },

		{ 16, _S("c:\\Recog\\valid_Budweiser_Streaker.divx"), _S8("video/x-divx") },
		{ 17, _S("c:\\Recog\\DivxWidWrngExt.wxt"), _S8("video/x-divx") },
		{ 18, _S("c:\\Recog\\corrupt.divx"), _S8("video/x-divx") },

		{ 19, _S("c:\\Recog\\valid_ESSSWT1M.DLS"), _S8("audio/dls") },
		{ 20, _S("c:\\Recog\\DLSWidWrngExt.wxt"), _S8("audio/dls") },
		{ 21, _S("c:\\Recog\\Corrupt.DLS"), _S8("audio/dls") },

		{ 22, _S("c:\\Recog\\valid_chand_sifarish.flac"), _S8("audio/flac") },
		{ 23, _S("c:\\Recog\\FLACWidWrngExt.wxt"), _S8("audio/flac") },
		{ 24, _S("c:\\Recog\\Corrupt.flac"), _S8("audio/flac") },

		//{ 25, _S("c:\\Recog\\valid_GSMSample.gsm"), _S8("audio/gsm") },
		//{ 26, _S("c:\\Recog\\GSMWidWrngExt.wxt"), _S8("audio/gsm") },
		{ 27, _S("c:\\Recog\\Corrupt.gsm"), _S8("audio/gsm") },

		{ 28, _S("c:\\Recog\\valid_baby.m3u"), _S8("audio/x-mpegurl") },
		{ 29, _S("c:\\Recog\\M3UWidWrngExt.wxt"), _S8("audio/x-mpegurl") },
		{ 30, _S("c:\\Recog\\Corrupt.m3u"), _S8("audio/x-mpegurl") },

		{ 31, _S("c:\\Recog\\valid_audio_only.m4a"), _S8("audio/mp4") },
		{ 32, _S("c:\\Recog\\M4aWidWrngExt.wxt"), _S8("audio/mp4") },
		{ 33, _S("c:\\Recog\\corrupt_aud.m4a"), _S8("audio/mp4") },

		{ 34, _S("c:\\Recog\\valid_ModernPiano.MID"), _S8("audio/midi") },
		{ 35, _S("c:\\Recog\\MIDIWidWrngExt.wxt"), _S8("audio/midi") },
		{ 36, _S("c:\\Recog\\corrupt.mid"), _S8("audio/midi") },

		{ 37, _S("c:\\Recog\\valid_rondobasi.mld"), _S8("audio/mld") },
		{ 38, _S("c:\\Recog\\MldWidWrngExt.wxt"), _S8("audio/mld") },
		{ 39, _S("c:\\Recog\\corrupt.mld"), _S8("audio/mld") },

		{ 40, _S("c:\\Recog\\valid_rondobasi.mmf"), _S8("audio/mmf") },
		{ 41, _S("c:\\Recog\\MMFWidWrngExt.wxt"), _S8("audio/mmf") },
		{ 42, _S("c:\\Recog\\corrupt.mmf"), _S8("audio/mmf") },

		{ 43, _S("c:\\Recog\\valid_muse_aud_mp4.mp4"), _S8("audio/mp4") },
		{ 44, _S("c:\\Recog\\AUDMP4WidWrngExt.wxt"), _S8("audio/mp4") },
		{ 45, _S("c:\\Recog\\corrupt_aud_mp4.mp4"), _S8("audio/mp4") },

		{ 46, _S("c:\\Recog\\valid_Streetwise.mxmf"), _S8("audio/mobile-xmf") },
		{ 47, _S("c:\\Recog\\MXMFWidWrngExt.wxt"), _S8("audio/xmf") },
		{ 48, _S("c:\\Recog\\Corrupt.mxmf"), _S8("audio/mobile-xmf") },

		{ 49, _S("c:\\Recog\\valid_small.ogg"), _S8("audio/ogg") },
		{ 50, _S("c:\\Recog\\OGGWidWrngExt.wxt"), _S8("audio/ogg") },
		{ 51, _S("c:\\Recog\\corrupt.ogg"), _S8("audio/ogg") },

		{ 52, _S("c:\\Recog\\valid_casadual.ra"), _S8("audio/x-pn-realaudio") },
		{ 53, _S("c:\\Recog\\RAWidWrngExt.wxt"), _S8("audio/x-pn-realaudio") },
		{ 54, _S("c:\\Recog\\Corrupt.ra"), _S8("audio/x-pn-realaudio") },

		{ 55, _S("c:\\Recog\\valid_realaudio.ram"), _S8("audio/x-pn-realaudio") },
		{ 56, _S("c:\\Recog\\RAMWidWrngExt.wxt"), _S8("audio/x-pn-realaudio") },
		{ 57, _S("c:\\Recog\\Corrupt.ram"), _S8("audio/x-pn-realaudio") },

		{ 58, _S("c:\\Recog\\valid_chand_sifarish.rm"), _S8("audio/x-pn-realaudio") },
		{ 59, _S("c:\\Recog\\RMWidWrngExt.wxt"), _S8("audio/x-pn-realaudio") },
		{ 60, _S("c:\\Recog\\Corrupt.rm"), _S8("audio/x-pn-realaudio") },

		{ 61, _S("c:\\Recog\\valid_ModernPiano.rmf"), _S8("audio/x-beatnik-rmf") },
		{ 62, _S("c:\\Recog\\RMFWidWrngExt.wxt"), _S8("audio/x-beatnik-rmf") },
	//	{ 63, _S("c:\\Recog\\Corrupt.rmf"), _S8("audio/x-beatnik-rmf") }, // Removed due to ambiguities with realaudio RMF

		{ 64, _S("c:\\Recog\\valid_SMF_pb.smf"), _S8("audio/midi") },
		{ 65, _S("c:\\Recog\\SMFWidWrngExt.wxt"), _S8("audio/midi") },
		{ 66, _S("c:\\Recog\\Corrupt_SMF.smf"), _S8("audio/midi") },

		{ 67, _S("c:\\Recog\\valid_eyck1.snd"), _S8("audio/basic") },
		{ 68, _S("c:\\Recog\\SNDWidWrngExt.wxt"), _S8("audio/basic") },
		{ 69, _S("c:\\Recog\\Corrupt.snd"), _S8("audio/basic") },

		{ 70, _S("c:\\Recog\\valid_chand_sifarish.wav"), _S8("audio/wav") },
		{ 71, _S("c:\\Recog\\AUDWAVWidWrngExt.wxt"), _S8("audio/wav") },
		{ 72, _S("c:\\Recog\\Corrupt.wav"), _S8("audio/wav") },

		{ 73, _S("c:\\Recog\\valid_Be_With_You.wma"), _S8("audio/x-ms-wma") },
		{ 74, _S("c:\\Recog\\WMAWidWrngExt.wxt"), _S8("audio/x-ms-asf") }, // Unknown extension. Assume video
		{ 75, _S("c:\\Recog\\Corrupt.wma"), _S8("audio/x-ms-wma") },

	//	{ 76, _S("c:\\Recog\\TODO.xmf"), _S8("") }, // audio/xmf
	//	{ 77, _S("c:\\Recog\\TODO.xmf"), _S8("") }, // audio/xmf
	//	{ 78, _S("c:\\Recog\\TODO.xmf"), _S8("") }, // audio/xmf

		{ 79, _S("c:\\Recog\\valid_Apertura.mp4"),			_S8("video/mp4") },
		{ 80, _S("c:\\Recog\\VIDMP4WidWrngExt.wxt"),	_S8("video/mp4") },
		{ 81, _S("c:\\Recog\\Corrupt_vid.mp4"),			_S8("audio/mp4") }, // Because of corruption, video won't be found

		{ 82, _S("c:\\Recog\\valid_MPEG2.mpg"),			_S8("video/mpeg2") },
		{ 83, _S("c:\\Recog\\MPEG2WidWrngExt.wxt"),	_S8("video/mpeg2") },
		{ 84, _S("c:\\Recog\\Corrupt_MPEG2.mpg"),	_S8("video/mpeg2") },

		{ 85, _S("c:\\Recog\\valid_Goodneighbour.wmv"),	_S8("video/x-ms-wmv") },
		{ 86, _S("c:\\Recog\\WMVWidWrngExt.wxt"),	_S8("video/x-ms-asf") }, // Unknown extension. Assume video
		{ 87, _S("c:\\Recog\\Corrupt.wmv"),			_S8("video/x-ms-wmv") },

		{ 88, _S("c:\\Recog\\valid_Aud_sample.3gp"), _S8("audio/3gpp") },
		{ 89, _S("c:\\Recog\\A3gpWidWrngExt.wxt"), _S8("audio/3gpp") },
		{ 90, _S("c:\\Recog\\corrupt_aud.3gp"), _S8("audio/3gpp") },

		{ 91, _S("c:\\Recog\\valid_sample.aac"), _S8("audio/aac") },
		{ 92, _S("c:\\Recog\\AACWidWrngExt.wxt"), _S8("audio/aac") },
		{ 93, _S("c:\\Recog\\corrupt.aac"), _S8("audio/aac") },

		{ 94, _S("c:\\Recog\\valid_test.mp3"), _S8("audio/mpeg") },
		{ 95, _S("c:\\Recog\\MP3WidWrngExt.wxt"), _S8("audio/mpeg") },
		{ 96, _S("c:\\Recog\\corrupt.mp3"), _S8("audio/mpeg") },

		{ 97, _S("c:\\Recog\\valid_mewmew.mkv"), _S8("video/x-matroska") },
		{ 98, _S("c:\\Recog\\MKVWidWrngExt.wxt"), _S8("video/x-matroska") },
		{ 99, _S("c:\\Recog\\Corrupt.mkv"), _S8("video/x-matroska") },

		{ 100, _S("c:\\Recog\\valid_niceday.asf"), _S8("video/x-ms-asf") },
		{ 101, _S("c:\\Recog\\ASFWidWrngExt.wxt"), _S8("video/x-ms-asf") },
		{ 102, _S("c:\\Recog\\Corrupt.asf"), _S8("video/x-ms-asf") },

		// 103
		{ 104, _S("c:\\Recog\\valid_3gpWidOutExt"), _S8("video/3gpp") },
		// 105

		//106
		{ 107, _S("c:\\Recog\\valid_MP3WidOutExt"), _S8("audio/mpeg") },
		//108

		// AAC files with ID3 tag information.
		{ 109, _S("c:\\Recog\\valid_aacid3.aac"), _S8("audio/aac") },
		// 110: AAC file with bad extension.
		// 111: Corrupt AAC file, handled by test case 93.

		// RealMedia file with video content.
		{ 112, _S("c:\\Recog\\valid_rmvideo.rm"), _S8("video/vnd.rn-realvideo") },
		// 113: RealMedia file with bad extension.
		// 114: Corrupt RealMedia file, handled by test case 60.

		// SDP files.
		// 115: SDP never recognised with ECertain
		{ 116, _S("c:\\Recog\\valid_sdp1.sdp"), _S8("application/sdp") },
		// 117

		// 118: SDP never recognised with ECertain
		{ 119, _S("c:\\Recog\\valid_sdp2.sdp"), _S8("application/sdp") },
		// 120

		// 121: SDP never recognised with ECertain
		{ 122, _S("c:\\Recog\\valid_sdp3.sdp"), _S8("application/sdp") },
		// 123

		// 124: SDP never recognised with ECertain
		{ 125, _S("c:\\Recog\\valid_sdp4.sdp"), _S8("application/sdp") },
		// 126

		// 127: SDP never recognised with ECertain
		{ 128, _S("c:\\Recog\\valid_sdp5.sdp"), _S8("application/sdp") },
		// 129

		// 130: SDP never recognised with ECertain
		{ 131, _S("c:\\Recog\\valid_sdp6.sdp"), _S8("application/sdp") },
		// 132

		// 133: SDP never recognised with ECertain
		{ 134, _S("c:\\Recog\\valid_sdp7.sdp"), _S8("application/sdp") },
		// 135

		// 136: SDP never recognised with ECertain
		{ 137, _S("c:\\Recog\\valid_sdp8.sdp"), _S8("application/sdp") },
		// 138

		// Special case SDP file.
		// 139: SDP never recognised with ECertain
		{ 140, _S("c:\\Recog\\valid_sdp3.sdp"), _S8("application/sdp") },
		// 141: Corrupt SDP file, handled by test case 117

		{ 142, _S("c:\\Recog\\valid_80X140.rm"), _S8("video/vnd.rn-realvideo") },
		// 143
		// 144

		{ 145, _S("c:\\Recog\\Valid_3GPP2.3g2"), _S8("video/3gpp2") },
		{ 146, _S("c:\\Recog\\Valid_3GPP2.wxt"), _S8("video/3gpp2") },
		{ 147, _S("c:\\Recog\\Corrupt_3GPP2.3g2"), _S8("audio/3gpp2") },

		{ 148, _S("c:\\Recog\\Valid_3GPP2_BadBrand.3g2"), _S8("video/3gpp2") },
		// 149
		// 150

		// DEF129505 - MMRuf incorrectly recognises unicode text files as MP3 files
		{ 500, _S("c:\\Recog\\valid_unicode.txt"), _S8("text/plain") },

		// Negative Test
		{ 600, _S("c:\\Recog\\Corrupt.wxt"), _S8("") },
		{ 601, _S("c:\\Recog\\valid_CrptOGGWidOutExt"), _S8("") }
	};

const TInt KTestCount = sizeof(KTestSpecs) / sizeof(TTestSpec);

/*
**
*/
static const TTestSpec KAllocTestSpecs[] =
	{
		{ 1, _S("c:\\Recog\\valid_angry_bone.3gp"), _S8("video/3gpp") },
		{ 2, _S("c:\\Recog\\3gpWidWrngExt.wxt"), _S8("video/3gpp") },
		{ 3, _S("c:\\Recog\\corrupt_vid.3gp"), _S8("video/3gpp") }
	};

const TInt KAllocTestCount = sizeof(KAllocTestSpecs) / sizeof(TTestSpec);


/*
**
*/
EXPORT_C CTSU_MMF_MMRUF* NewTestSuiteL()
    {
	CTSU_MMF_MMRUF* pTS = new (ELeave) CTSU_MMF_MMRUF();
	CleanupStack::PushL(pTS);
	pTS->ConstructL();
	CleanupStack::Pop(); // pTS
	return pTS;
    }

/*
**
*/
CTSU_MMF_MMRUF::~CTSU_MMF_MMRUF()
	{
	}

/*
**
*/
void CTSU_MMF_MMRUF::InitialiseL()
	{
	iSuiteName = _L("TSU_MMF_MMRUF");

	for (TInt i = 0; i < KTestCount; i++)
		{
		AddTestStepL(RMMRUFTestStep::NewL(&KTestSpecs[i]));
		}

	for (TInt i = 0; i < KAllocTestCount; i++)
		{
		AddTestStepL(RTestStepMMRUFOOM::NewL(&KAllocTestSpecs[i]));
		}

	// Try to recognise non-multimedia files. It should fail.
	AddTestStepL(RMMRUFDirTestStep::NewL());
	}

/*
**
*/
TPtrC CTSU_MMF_MMRUF::GetVersion() const
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}
