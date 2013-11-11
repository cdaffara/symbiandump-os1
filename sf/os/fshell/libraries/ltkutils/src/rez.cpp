// rez.cpp
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
#include <f32file.h>
#include <bautils.h>
#include <barsc.h>

// Horrible variadic macro to avoid leaving from within a variadic function, because GCCE doesn't like that
#define LeaveIfErr(err, errBuf, fmt...) \
	do \
		{ \
		TInt __err = (err); \
		if (__err < 0 && errBuf) \
			{ \
			*errBuf = HBufC::NewL(512); \
			FormatIntoBuf(*errBuf, fmt); \
			} \
		User::LeaveIfError(__err); \
		} \
		while (0)


void FormatIntoBuf(HBufC* aErrBuf, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	TPtr ptr = aErrBuf->Des();
	ptr.FormatList(aFmt, list);
	VA_END(list);
	}


EXPORT_C HBufC8* LtkUtils::Rez8L(const TDesC& aIdentifier, RFs* aFs, HBufC** aErrBuf)
	{
	TPtrC id(aIdentifier);
	if (aIdentifier.Left(2) != _L("R:"))
		{
		// Just return the string as-is, ie unlocalised
		HBufC8* result = HBufC8::NewL(aIdentifier.Length());
		result->Des().Copy(aIdentifier);
		return result;
		}

	RFs stackfs;
	if (!aFs)
		{
		CleanupClosePushL(stackfs);
		LeaveIfErr(stackfs.Connect(), aErrBuf, _L("Couldn't connect to RFs"));
		aFs = &stackfs;
		}
	else
		{
		CleanupStack::PushL((CBase*)NULL); // Just so the cleanup stack is the same depth in both cases - this is ok to destroy
		}
	RFs& fs = *aFs;

	id.Set(id.Mid(2));
	TInt colon = id.Locate(':');
	if (colon == KErrNotFound || colon == 0)
		{
		LeaveIfErr(KErrArgument, aErrBuf, _L("Resource identifier must include a resource id - R:FILENAME:ID[:OFFSET]"));
		}

	TPtrC resname(id.Left(colon));
	id.Set(id.Mid(colon+1));

	colon = id.Locate(':');
	TPtrC offset;
	if (colon != KErrNotFound)
		{
		offset.Set(id.Mid(colon+1));
		id.Set(id.Left(colon));
		}

	TFileName resFileName = resname;
	resFileName.Insert(0, _L("\\Resource\\"));
	resFileName.Append(_L(".rsc")); // Seems daft but you have to have this to keep Bafl happy
	BaflUtils::NearestLanguageFile(fs, resFileName);

	RResourceFile resFile;
	CleanupClosePushL(resFile);
	TRAPD(err, resFile.OpenL(fs, resFileName));
	LeaveIfErr(err, aErrBuf, _L("Couldn't open resource file %S"), &resFileName);

	TRAP(err, resFile.ConfirmSignatureL());
	LeaveIfErr(err,  aErrBuf, _L("Failed to confirm signature"));

	TBool hex = EFalse;
	if (id.Left(2) == _L("0x"))
		{
		id.Set(id.Mid(2));
		hex = ETrue;
		}
	TUint resourceId;
	TLex lex(id);
	LeaveIfErr(lex.Val(resourceId, hex ? EHex : EDecimal), aErrBuf, _L("Couldn't parse resource id from %S"), &id);

	HBufC8* resdata = NULL;
	TRAP(err, resdata = resFile.AllocReadL(resourceId));
	LeaveIfErr(err, aErrBuf, _L("Couldn't read resource %d from %S"), resourceId, &resFileName);
	CleanupStack::PushL(resdata);

	TPtrC8 result(*resdata);
	if (offset.Length())
		{
		char last = (char)offset[offset.Length() - 1];
		if (last != 'd' && last != 'D')
			{
			LeaveIfErr(KErrArgument, aErrBuf, _L("Offset '%S' does not end in a 'd' or a 'D'. This is needed to indicate whether the actual target is 8-bit or 16-bit"), &offset);
			}
		TResourceReader reader;
		reader.SetBuffer(resdata);
		TLex lex(offset);
		while (!lex.Eos())
			{
			TChar ch = lex.Get();
			if (ch == 'd')
				{
				result.Set(reader.ReadTPtrC8());
				}
			else if (ch == 'D')
				{
				TPtrC16 wptr = reader.ReadTPtrC16();
				result.Set(TPtrC8((TUint8*)wptr.Ptr(), wptr.Size()));
				}
			else
				{
				lex.UnGet();
				TInt amountToSkip;
				LeaveIfErr(lex.Val(amountToSkip), aErrBuf, _L("Error lexing offset"));
				reader.Advance(amountToSkip);
				}
			}
		}

	HBufC8* resultBuf = result.AllocL();
	CleanupStack::PopAndDestroy(3); // resData, resFile, fs
	return resultBuf;
	}

EXPORT_C HBufC* LtkUtils::RezL(const TDesC& aIdentifier, RFs* aFs, HBufC** aErrBuf)
	{
	if (aIdentifier.Left(2) != _L("R:"))
		{
		// Just return the string as-is, ie unlocalised
		return aIdentifier.AllocL();
		}
	HBufC8* buf8 = Rez8L(aIdentifier, aFs, aErrBuf);
	CleanupStack::PushL(buf8);
	TPtrC16 wptr((TUint16*)buf8->Ptr(), buf8->Size()/2);
	HBufC* result = wptr.AllocL();
	CleanupStack::PopAndDestroy(buf8);
	return result;
	}

EXPORT_C HBufC* LtkUtils::RezLC(const TDesC& aIdentifier, RFs* aFs, HBufC** aErrBuf)
	{
	HBufC* res = RezL(aIdentifier, aFs, aErrBuf);
	CleanupStack::PushL(res);
	return res;
	}

EXPORT_C HBufC8* LtkUtils::Rez8LC(const TDesC& aIdentifier, RFs* aFs, HBufC** aErrBuf)
	{
	HBufC8* res = Rez8L(aIdentifier, aFs, aErrBuf);
	CleanupStack::PushL(res);
	return res;
	}
