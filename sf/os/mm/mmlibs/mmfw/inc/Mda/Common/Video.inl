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
// Mda\Common\Video.inl
// 
//

inline TMdaBmpClipFormat::TMdaBmpClipFormat():
	TMdaClipFormat(KUidMdaBmpClipFormat,sizeof(TMdaBmpClipFormat))
	{}

inline TMda1BppBmpCodec::TMda1BppBmpCodec():
	TMdaPackage(KUidMdaBmpCodec,KUidMda1BppBmpCodec,sizeof(TMda1BppBmpCodec))
	{}

inline TMda4BppBmpCodec::TMda4BppBmpCodec():
	TMdaPackage(KUidMdaBmpCodec,KUidMda4BppBmpCodec,sizeof(TMda4BppBmpCodec))
	{}

inline TMda8BppBmpCodec::TMda8BppBmpCodec():
	TMdaPackage(KUidMdaBmpCodec,KUidMda8BppBmpCodec,sizeof(TMda8BppBmpCodec))
	{}

inline TMda24BppBmpCodec::TMda24BppBmpCodec():
	TMdaPackage(KUidMdaBmpCodec,KUidMda24BppBmpCodec,sizeof(TMda24BppBmpCodec))
	{}

inline TMdaJpgSettings::TMdaJpgSettings():
	iSampleScheme(EColor420),
	iQualityFactor(50)
	{}

inline TMdaJfifClipFormat::TMdaJfifClipFormat():
	TMdaClipFormat(KUidMdaJfifClipFormat,sizeof(TMdaJfifClipFormat)),
	iSettings()
	{}

inline TMdaJpgQTable::TMdaJpgQTable():
	TMdaPackage(KUidMdaJpgQTable,KUidMdaJpgQTable,sizeof(TMdaJpgQTable)),
	iTableIndex(ELumaTable),
	iEntries(KMdaJpgQTableEntries)
	{}

inline TMdaJpgComment::TMdaJpgComment():
	TMdaPackage(KUidMdaJpgComment,KUidMdaJpgComment,sizeof(TMdaJpgComment)),
	iCommentIndex(0),
	iCommentBufferIndex(0)
	{}

inline TMdaMbmClipFormat::TMdaMbmClipFormat():
	TMdaClipFormat(KUidMdaMbmClipFormat,sizeof(TMdaMbmClipFormat))
	{}

inline TMdaWbmpClipFormat::TMdaWbmpClipFormat():
	TMdaClipFormat(KUidMdaWbmpClipFormat,sizeof(TMdaWbmpClipFormat))
	{}

inline TMdaWbmpCodec::TMdaWbmpCodec():
	TMdaPackage(KUidMdaWbmpCodec,KUidMdaWbmpCodec,sizeof(TMdaWbmpCodec))
	{}

inline TMdaOtaClipFormat::TMdaOtaClipFormat():
	TMdaClipFormat(KUidMdaOtaClipFormat,sizeof(TMdaOtaClipFormat))
	{}

inline TMdaOtaCodec::TMdaOtaCodec():
	TMdaPackage(KUidMdaOtaCodec,KUidMdaOtaCodec,sizeof(TMdaOtaCodec))
	{}

inline TMdaPngClipFormat::TMdaPngClipFormat():
	TMdaClipFormat(KUidMdaPngClipFormat,sizeof(TMdaPngClipFormat))
	{}

inline TMdaPngCodec::TMdaPngCodec():
	TMdaPackage(KUidMdaPngCodec,KUidMdaPngCodec,sizeof(TMdaPngCodec))
	{}

inline TMdaWmfClipFormat::TMdaWmfClipFormat():
	TMdaClipFormat(KUidMdaWmfClipFormat,sizeof(TMdaWmfClipFormat))
	{}

inline TMdaWmfApmClipFormat::TMdaWmfApmClipFormat():
	TMdaClipFormat(KUidMdaWmfApmClipFormat,sizeof(TMdaWmfApmClipFormat))
	{}

inline TMdaWmfClpClipFormat::TMdaWmfClpClipFormat():
	TMdaClipFormat(KUidMdaWmfClpClipFormat,sizeof(TMdaWmfClpClipFormat))
	{}

inline TMdaWmfCodec::TMdaWmfCodec():
	TMdaPackage(KUidMdaWmfCodec,KUidMdaWmfCodec,sizeof(TMdaWmfCodec))
	{}

inline TMdaGif87aClipFormat::TMdaGif87aClipFormat():
	TMdaClipFormat(KUidMdaGif87aClipFormat,sizeof(TMdaGif87aClipFormat))
	{}

inline TMdaGif89aClipFormat::TMdaGif89aClipFormat():
	TMdaClipFormat(KUidMdaGif89aClipFormat,sizeof(TMdaGif89aClipFormat))
	{}

inline TMdaGifCodec::TMdaGifCodec():
	TMdaPackage(KUidMdaGifCodec,KUidMdaGifCodec,sizeof(TMdaGifCodec))
	{}

