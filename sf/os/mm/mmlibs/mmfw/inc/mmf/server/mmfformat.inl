// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\server\mmfformat.inl
// CMMFFormatDecode
// 
//

void CMMFFormatDecode::BufferEmptiedL(CMMFBuffer* /*aBuffer*/)
{
}

void CMMFFormatDecode::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
{
}

CMMFBuffer* CMMFFormatDecode::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
{
	return NULL;
}

TFourCC CMMFFormatDecode::SinkDataTypeCode(TMediaId /*aMediaId*/)
{
	TFourCC fourCC; return fourCC;
}

void CMMFFormatDecode::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
{
	User::Leave(KErrNotSupported);
}

CMMFMetaDataEntry* CMMFFormatDecode::MetaDataEntryL(TInt /*aIndex*/)
{
	User::Leave(KErrNotSupported); return NULL;
}

TInt CMMFFormatDecode::SetNumChannels(TUint /*aChannels*/)
{
	return KErrNotSupported;
}

TInt CMMFFormatDecode::SetSampleRate(TUint /*aSampleRate*/)
{
	return KErrNotSupported;
}

TInt CMMFFormatDecode::SetBitRate(TUint /*aBitRate*/)
{
	return KErrNotSupported;
}

void CMMFFormatDecode::GetSupportedSampleRatesL(RArray<TUint>& /*aSampleRates*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatDecode::GetSupportedBitRatesL(RArray<TUint>& /*aBitRates*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatDecode::GetSupportedNumChannelsL(RArray<TUint>& /*aNumChannels*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatDecode::GetSupportedDataTypesL(TMediaId /*aMediaId*/, RArray<TFourCC>& /*aDataTypes*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatDecode::SuggestSourceBufferSize(TUint /*aSuggestedBufferSize*/)
{
}

void CMMFFormatDecode::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
{
	User::Leave(KErrNotSupported);
}

// CMMFFormatEncode

void CMMFFormatEncode::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/, TMediaId /*aMediaId*/)
{
}

CMMFBuffer* CMMFFormatEncode::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/)
{
	return NULL;
}

TFourCC CMMFFormatEncode::SourceDataTypeCode(TMediaId /*aMediaId*/)
{
	TFourCC fourCC; return fourCC;
}

void CMMFFormatEncode::BufferFilledL(CMMFBuffer* /*aBuffer*/)
{
}

void CMMFFormatEncode::CropL(TTimeIntervalMicroSeconds /*aPosition*/, TBool /*aToEnd*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
{
	User::Leave(KErrNotSupported);
}

CMMFMetaDataEntry* CMMFFormatEncode::MetaDataEntryL(TInt /*aIndex*/)
{
	User::Leave(KErrNotSupported); return NULL;
}

void CMMFFormatEncode::AddMetaDataEntryL(const CMMFMetaDataEntry& /*aNewEntry*/)
{
	User::Leave(KErrNotSupported);
}

TInt CMMFFormatEncode::RemoveMetaDataEntry(TInt /*aIndex*/)
{
	return KErrNotSupported;
}

void CMMFFormatEncode::ReplaceMetaDataEntryL(TInt /*aIndex*/, const CMMFMetaDataEntry& /*aNewEntry*/)
{
	User::Leave(KErrNotSupported);
}

TInt CMMFFormatEncode::SetNumChannels(TUint /*aChannels*/)
{
	return KErrNotSupported;
}

TInt CMMFFormatEncode::SetSampleRate(TUint /*aSampleRate*/)
{
	return KErrNotSupported;
}

TInt CMMFFormatEncode::SetBitRate(TUint /*aBitRate*/)
{
	return KErrNotSupported;
}

void CMMFFormatEncode::GetSupportedSampleRatesL(RArray<TUint>& /*aSampleRates*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::GetSupportedBitRatesL(RArray<TUint>& /*aBitRates*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::GetSupportedNumChannelsL(RArray<TUint>& /*aNumChannels*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::GetSupportedDataTypesL(TMediaId /*aMediaId*/, RArray<TFourCC>& /*aDataTypes*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::SetMaximumClipSizeL(TInt /*aBytes*/)
{
	User::Leave(KErrNotSupported);
}

void CMMFFormatEncode::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
{
	User::Leave(KErrNotSupported);
}

