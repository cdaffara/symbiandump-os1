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
// include\server\mmfrawformat.h
// 
//

#ifndef __MMF_RAW_FORMAT_H__
#define __MMF_RAW_FORMAT_H__

#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfclip.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmfutilities.h>

#include "MMFFormatBaseClasses.h"


class CMMFRawFormatRead : public CMMFFormatRead
	{
public:
	static CMMFFormatDecode* NewL(MDataSource* aSource);
	~CMMFRawFormatRead();

	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId); 
	virtual TInt SetNumChannels(TUint aChannels);
	virtual TInt SetSampleRate(TUint aSampleRate);
	virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

protected:
	virtual void ProcessFormatHeaderL();



protected:
	CMMFRawFormatRead() : CMMFFormatRead() {};
	};




class CMMFRawFormatWrite : public CMMFFormatWrite
	{
public:
	static CMMFFormatEncode* NewL(MDataSink* aSink);
	 ~CMMFRawFormatWrite();

private:
	CMMFRawFormatWrite() : CMMFFormatWrite() {};

	TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId); 
	void NegotiateL(MDataSource& aSource);

	void ProcessFormatHeaderL();
	void SetDefaultHeaderParametersL();

	void WriteHeaderL() {};

	TInt SetNumChannels(TUint aChannels);
	TInt SetSampleRate(TUint aSampleRate);

	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	TInt DetermineIfValidClip();
	};
#endif

