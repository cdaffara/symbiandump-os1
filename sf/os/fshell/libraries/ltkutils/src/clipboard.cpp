// clipboard.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/ltkutils.h>
#include <baclipb.h>
#include <S32MEM.H>
#include <S32UCMP.H>
const TUid KClipboardUidTypePlainText = {268450333}; // from TXTETEXT.h

// This could probably be optimised away, but I don't feel that confident
NONSHARABLE_CLASS(TFieldMapExternalizer) : public MExternalizer<TStreamRef>
	{
public:
	TFieldMapExternalizer(const CStoreMap& aMap);
	void ExternalizeL(const TStreamRef& anObject,RWriteStream& aStream) const;
private:
	const CStoreMap* iMap;
	};

RFs OpenFsIfNeededLC(RFs* aFsPtr)
	{
	if (!aFsPtr)
		{
		RFs stackFs;
		CleanupClosePushL(stackFs);
		User::LeaveIfError(stackFs.Connect());
		return stackFs;
		}
	else
		{
		CleanupStack::PushL((CBase*)NULL); // Just so the cleanup stack is the same depth in both cases - this is ok to destroy
		return *aFsPtr;
		}
	}


EXPORT_C void LtkUtils::CopyToClipboardL(const TDesC& aText, RFs* aFs)
	{
	/*
	The simple way of implementing this is as follows. However this gives you a dependancy on etext.dll so is not desirable

	CClipboard* cb = CClipboard::NewForWritingLC(FsL());
	CPlainText* text = CPlainText::NewL();
	CleanupStack::PushL(text);
	text->InsertL(0, aText);
	text->CopyToStoreL(cb->Store(), cb->StreamDictionary(), 0, text->DocumentLength());
	cb->CommitL();
	CleanupStack::PopAndDestroy(2, cb); // text, cb
	*/

	RFs fs = OpenFsIfNeededLC(aFs);
	CClipboard* cb = CClipboard::NewForWritingLC(fs);
	CStreamStore& store = cb->Store();
	CStreamDictionary& dictionary = cb->StreamDictionary();
	// Following adapted from CPlainText::DoCopyToStoreL
	if (aText.Length())
		{
		CStoreMap* map=CStoreMap::NewLC(store);
		//CopyComponentsL(aStore,*map,aPos,aLength);

		// create custom externalizer over the map
		TFieldMapExternalizer fMap(*map);
		RStoreWriteStream stream(fMap);
		TStreamId id=stream.CreateLC(store);
		//CopyToStreamL(stream,aPos,aLength);
		stream.WriteInt32L(aText.Length());
		CBufFlat* textBuf = CBufFlat::NewL(256);
		CleanupStack::PushL(textBuf);
		textBuf->InsertL(0, TPtrC8((const TText8*)aText.Ptr(), aText.Size()));
		RBufReadStream input_stream(*textBuf, 0);
		TMemoryStreamUnicodeSource source(input_stream);
		TUnicodeCompressor c;
		c.CompressL(stream,source,KMaxTInt,aText.Length());
		input_stream.Close();
		stream.WriteUint8L(EFalse); // Indicating !fieldSetPresent
		CleanupStack::PopAndDestroy(textBuf);
		stream.CommitL();

		dictionary.AssignL(KClipboardUidTypePlainText,id);
		map->Reset();
		CleanupStack::PopAndDestroy(2);
		}
	cb->CommitL();
	CleanupStack::PopAndDestroy(cb);
	CleanupStack::PopAndDestroy(); // stackfs
	}

EXPORT_C HBufC* LtkUtils::GetFromClipboardL(RFs* aFs)
	{
	/*
	The simple way of implementing this is as follows. However this gives you a dependancy on etext.dll so is not desirable

	CClipboard* cb = CClipboard::NewForReadingLC(FsL());
	CPlainText* text = CPlainText::NewL();
	CleanupStack::PushL(text);
	text->PasteFromStoreL(cb->Store(), cb->StreamDictionary(), 0);
	HBufC* res = HBufC::NewL(text->DocumentLength());
	TPtr ptr(res->Des());
	text->Extract(ptr, 0);
	CleanupStack::PopAndDestroy(2, cb); // text, cb
	return res;
	*/

	RFs fs = OpenFsIfNeededLC(aFs);
	HBufC* result = NULL;
	CClipboard* cb = CClipboard::NewForReadingLC(fs);
	CStreamStore& store = cb->Store();
	CStreamDictionary& dictionary = cb->StreamDictionary();

	// Adapted from CPlainText::PasteFromStoreL, 
	TStreamId id=dictionary.At(KClipboardUidTypePlainText);
	if (id != KNullStreamId)
		{
		RStoreReadStream stream;
		stream.OpenLC(store, id);
		CBufSeg* buffer = CBufSeg::NewL(512);
		CleanupStack::PushL(buffer);
		TInt length = stream.ReadInt32L();
		RBufWriteStream output_stream(*buffer);
		TMemoryStreamUnicodeSink sink(output_stream);
		TUnicodeExpander e;
		e.ExpandL(sink,stream,length);
		output_stream.CommitL();
		output_stream.Close();

		result = HBufC::NewMaxL(buffer->Size()/sizeof(TText));
		TPtr8 ptr = TPtr8((TText8*)result->Ptr(), 0, buffer->Size());
		buffer->Read(0, ptr);
		
		CleanupStack::PopAndDestroy(2, &stream); // buffer, stream
		}

	CleanupStack::PopAndDestroy(cb);
	CleanupStack::PopAndDestroy(); // OpenFsIfNeededLC
	if (!result) result = KNullDesC().AllocL();
	return result;
	}


TFieldMapExternalizer::TFieldMapExternalizer(const CStoreMap& aMap)
	: iMap(&aMap)
	{}

void TFieldMapExternalizer::ExternalizeL(const TStreamRef& aRef,RWriteStream& aStream) const
// Write the stream id bound to aRef to aStream. If not bound, write KNullStreamId
//
	{
	TSwizzleC<TAny> swizzle=aRef;
	aStream<<iMap->At(swizzle);
	}
