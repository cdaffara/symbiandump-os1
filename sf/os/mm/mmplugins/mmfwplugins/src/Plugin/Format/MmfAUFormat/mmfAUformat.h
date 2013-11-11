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

#ifndef __MMF_AU_FORMAT_H__
#define __MMF_AU_FORMAT_H__

#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfclip.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmfutilities.h>

#include "MMFFormatBaseClasses.h"

const TUint KAuFileUncompressedDataHeaderSize = 24 ; // 6 times 4 byte quantities

const TUint KAuSignature = 0x2e736e64 ;	//.snd
const TInt  KAuDataSizeOffset = 8 ;


class TMdaAuHeader
	{
public:
	TUint32 iMagicNumber ; // 0x2e736e64
	TUint32 iHeaderSize ; // Minimum 24, larger if Information block before data
	TUint32 iDataSize ; // File length less iHeaderSize.  May be set to 0xffffffff as can be calculated	
	TUint32 iEncoding ; // Encoding format - See constants
	TUint32 iSampleFrequency ; // In Hz
	TUint32 iChannels ;
	};



/**
 * @internalComponent
 * This provides a mechanism for sharing code between the Read and Write Formats 
 * without resorting to multiple inheritence.
 *
 */
template <class T> 
class TMMFAuConsolidationMethods
	{
public:
	TMMFAuConsolidationMethods(T& aThat) : iThat(aThat){};

	void ProcessFormatHeaderL();

	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

private:
	T& iThat;
	};







class CMMFAuFormatRead : public CMMFFormatRead
	{
friend class TMMFAuConsolidationMethods<CMMFAuFormatRead>;

public:
	static CMMFFormatDecode* NewL(MDataSource* aSource);
	~CMMFAuFormatRead();


protected:
	CMMFAuFormatRead() : CMMFFormatRead() {};
	void ConstructL(MDataSource* aSource);

	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);
	void ProcessFormatHeaderL();


private:
	TMdaAuHeader iHeader ;

	TMMFAuConsolidationMethods<CMMFAuFormatRead>* iAuFormatConsolidator;	
	};



class CMMFAuFormatWrite : public CMMFFormatWrite
	{
friend class TMMFAuConsolidationMethods<CMMFAuFormatWrite>;

public:
	static CMMFFormatEncode* NewL(MDataSink* aSink);
	~CMMFAuFormatWrite();

private:
	CMMFAuFormatWrite() : CMMFFormatWrite() {};
	void ConstructL(MDataSink* aSink);

	TInt DetermineIfValidClip();
	void SetDefaultHeaderParametersL();
	void ProcessFormatHeaderL();
	void WriteHeaderL();

	TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId); 
	void NegotiateL(MDataSource& aSource);
	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

private:
	TMdaAuHeader iHeader ;

	TMMFAuConsolidationMethods<CMMFAuFormatWrite>* iAuFormatConsolidator;	
	};
#endif

