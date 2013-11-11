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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <e32def.h>

//
// Recognised file extensions.
//
#define KExtAAC			_S(".aac")
#define KExt3GA			_S(".3ga")
#define KExt3GP			_S(".3gp")
#define KExt3G2			_S(".3g2")
#define KExtAIFF_1		_S(".aif")
#define KExtAIFF_2		_S(".aifc")
#define KExtAIFF_3		_S(".aiff")
#define KExtASF			_S(".asf")
#define KExtAMR			_S(".amr")
#define KExtAVI			_S(".avi")
#define KExtAMRWB		_S(".awb")
#define KExtAU			_S(".au")
#define KExtDIVX		_S(".divx")
#define KExtDLS			_S(".dls")
#define KExtFLAC_1		_S(".fla")
#define KExtFLAC_2		_S(".flac")
#define KExtGSM			_S(".gsm")
#define KExtMAT_A		_S(".mka")
#define KExtMAT_V		_S(".mkv")
#define KExtM3U			_S(".m3u")
#define KExtM4A			_S(".m4a")
#define KExtM4V			_S(".m4v")
#define KExtMID			_S(".mid")
#define KExtMLD			_S(".mld")
#define KExtMMF			_S(".mmf")
#define KExtMP3			_S(".mp3")
#define KExtMP4			_S(".mp4")
#define KExtMPEG_1		_S(".mpg")
#define KExtMPEG_2		_S(".m2v")
#define KExtMPEG_3		_S(".mp2")
#define KExtMPEG_4		_S(".mp2v")
#define KExtMPEG_5		_S(".mpeg")
#define KExtMPEG_6		_S(".mpv2")
#define KExtMPEG_7		_S(".m1v")
#define KExtMPEG_8		_S(".mpa")
#define KExtMXMF		_S(".mxmf")
#define KExtOGG			_S(".ogg")
#define KExtRA			_S(".ra")
#define KExtRAM			_S(".ram")
#define KExtRM			_S(".rm")
#define KExtRMF			_S(".rmf")
#define KExtSDP			_S(".sdp")
#define KExtSMF			_S(".smf")
#define KExtSND			_S(".snd")
#define KExtWAV			_S(".wav")
#define KExtWMA			_S(".wma")
#define KExtWMV			_S(".wmv")
#define KExtXMF			_S(".xmf")
#define KExtXPS			_S(".xps")
#define KExtBeatnikRMF	_S(".rmf")
#define KExtMOV         _S(".mov")  // for .mov extension

//
// Recognised MIME-types.
//
#define KMime3GA		_S8("audio/3ga")
#define KMime3GP_A		_S8("audio/3gpp")
#define KMime3GP_V		_S8("video/3gpp")
#define KMime3G2_A		_S8("audio/3gpp2")
#define KMime3G2_V		_S8("video/3gpp2")
#define KMimeAAC		_S8("audio/aac")
#define KMimeAIFF		_S8("audio/aiff")
#define KMimeAMR		_S8("audio/amr")
#define KMimeAMRWB		_S8("audio/amr-wb")
#define KMimeAVI		_S8("video/x-msvideo")
#define KMimeAU			_S8("audio/basic")
#define KMimeDIVX		_S8("video/x-divx")
#define KMimeDLS		_S8("audio/dls")
#define KMimeFLAC		_S8("audio/flac")
#define KMimeGSM		_S8("audio/gsm")
#define KMimeMAT_A		_S8("audio/x-matroska")
#define KMimeMAT_V		_S8("video/x-matroska")
#define KMimeM3U		_S8("audio/x-mpegurl")
#define KMimeMID		_S8("audio/midi")
#define KMimeMLD		_S8("audio/mld")
#define KMimeMMF		_S8("audio/mmf")
#define KMimeMPEG1_A	_S8("audio/x-mpeg")
#define KMimeMPEG1_V	_S8("video/mpeg")
#define KMimeMPEG2_A	_S8("audio/x-mpeg2")
#define KMimeMPEG2_V	_S8("video/mpeg2")
#define KMimeMP3		_S8("audio/mpeg")
#define KMimeMP4_A		_S8("audio/mp4")
#define KMimeMP4_V		_S8("video/mp4")
#define KMimeMXMF		_S8("audio/mobile-xmf")
#define KMimeOGG		_S8("audio/ogg")
#define KMimeRA			_S8("audio/x-pn-realaudio")
#define KMimeRAM		KMimeRA
#define KMimeRM_A		KMimeRA
#define KMimeRM_V		_S8("video/vnd.rn-realvideo")
#define KMimeSDP		_S8("application/sdp")
#define KMimeSMF		_S8("audio/midi")
#define KMimeSND		KMimeAU
#define KMimeWAV		_S8("audio/wav")
#define KMimeASF_A		_S8("audio/x-ms-asf")
#define KMimeASF_V		_S8("video/x-ms-asf")
#define KMimeWMA		_S8("audio/x-ms-wma")
#define KMimeWMV		_S8("video/x-ms-wmv")
#define KMimeXMF		_S8("audio/xmf")
#define KMimeXPS		_S8("application/x-ext-packetsrc")
#define KMimeBeatnikRMF _S8("audio/x-beatnik-rmf")
#define KMimeQuickV     _S8("video/quicktime")  // for .mov extension

//
// Recognised header signatures.
// These should not be defined with _S8 because using
// sizeof() in the HEADER_SIG macro will not work as intended
// due to some signatures containing the value '0x00'.
//
#define KSigAIFF		"????????AIFF*"
#define KSigAMR_1		"#!AMR\n*"
#define KSigAMR_2		"#!AMR_MC1.0\n*"	// Multi-channel AMR
#define KSigAMRWB_1		"#!AMR-WB\n*"
#define KSigAMRWB_2		"#!AMR-WB_MC1.0\n*" // Multi-channel AMR-WB
#define KSigAU			".snd*"
#define KSigAVI			"RIFF????AVI *"
#define KSigDIVX_1		"*vidsyv12*"
#define KSigDIVX_2		"*vidsdivx*"
#define KSigDLS			"????????DLS *"
#define KSigFLAC		"fLaC*"
#define KSigGSM			"\x72\xF6\x63\x1E\x5E\x47\xD2\x11\x87\x99\x00\x20\xAF\xE6\x36\xEE" "*"
#define KSigM3U			"#EXTM3U*"
#define KSigMID			"MThd*"
#define KSigMLD			"melo*"
#define KSigMMF			"MMMD*"
#define KSigMXMF		"XMF_*"
#define KSigOGG			"OggS*"
#define KSigRA			".ra\xfd" "*"
#define KSigRAM			"?t?p://*"
#define KSigSDP			"v=?*\n*\ns=*"
#define KSigSMF			KSigMID
#define KSigSND			KSigAU
#define KSigWAV_1		"RIFF????WAVEfmt ????\x01\x00" "*"	// WAVE_FORMAT_PCM - PCM audio in integer format
#define KSigWAV_2		"RIFF????WAVEfmt ????\x02\x00" "*"	// WAVE_FORMAT_ADPCM - Microsoft adaptive PCM
#define KSigXMF			"XMF_*"
#define KSigXPS			"v=?*\n*"
#define KSigBeatnikRMF	"IREZ*"


//
// New constants. Saves on typing.
//
#define KConfNotRecognised	CApaDataRecognizerType::ENotRecognized
#define KConfPossible		CApaDataRecognizerType::EPossible
#define KConfProbable		CApaDataRecognizerType::EProbable
#define KConfCertain		CApaDataRecognizerType::ECertain


//
// Converts several bits into an integer
// where 'a' signifies the highest bit to be set.
// e.g. MAKE_BYTE2(a, b)       -> [0000 00ab]
//      MAKE_BYTE3(a, b, c)    -> [0000 0abc]
//      MAKE_BYTE4(a, b, c, d) -> [0000 abcd]
//
#define MAKE_BYTE2(a, b)		(((TUint8)(a & 0x01) << 1) | (TUint8)(b & 0x01))
#define MAKE_BYTE3(a, b, c)		((MAKE_BYTE2(a, b) << 1) | (TUint8)(c & 0x01))
#define MAKE_BYTE4(a, b, c, d)	((MAKE_BYTE2(a, b) << 2) | MAKE_BYTE2(c, d))


//
// Packs four bytes into a 32bit number.
//
//  3
//  1                                 0
// +--------+--------+--------+--------+
// |aaaaaaaa|bbbbbbbb|cccccccc|dddddddd|
// +--------+--------+--------+--------+
//
#define MAKE_INT32(a, b, c, d)	(TUint32)(((TUint8)a << 24) | \
								((TUint8)b << 16) | \
								((TUint8)c << 8) | \
								(TUint8)d)

//
// The following macros are used to read individual bit fields.
//
//  3
//  1                                 0
// +--------+--------+--------+--------+
// |aaaaaaaa|bbbbbbbb|cccccccc|dddddddd|
// +--------+--------+--------+--------+
//
//
//                          +--------+
// HIGH_BYTE32 will return: |aaaaaaaa|
//                          +--------+--------+
// HIGH_WORD32 will return: |aaaaaaaa|bbbbbbbb|
//                          +--------+--------+
// LOW_BYTE    will return: |dddddddd|
//                          +--------+--------+
// LOW_WORD    will return: |cccccccc|dddddddd|
//                          +--------+--------+
//
#define HIGH_BYTE32(a)			(TUint8)((TUint32)(a) >> 24)
#define HIGH_WORD32(a)			(TUint16)((TUint32)(a) >> 16)
#define LOW_BYTE(a)				(TUint8)((a) & 0xff)
#define LOW_WORD(a)				(TUint16)((a) & 0xffff)

//
// These macros will only work at compile-time with
// char* parameters in order for sizeof() to work correctly.
// The terminating '\0' of charPtr is not included.
//
#define MAKE_TPtrC8(charPtr)	TPtrC8(_S8((charPtr)), sizeof(charPtr) - 1)

#endif

