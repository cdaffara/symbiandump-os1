/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "T_ActiveCallbackIO.h"

/*@{*/
_LIT(KStrNull,								" ");

_LIT(KLogErrorFileNotFound,					"File '%S' Not Found. Error %d");
_LIT(KLogErrorFileNotOpened,				"File '%S' Not Opened. Error %d");
_LIT(KLogErrorFormatError,					"Format Error");
/*@}*/

CT_ActiveCallbackIO* CT_ActiveCallbackIO::NewL(CDataWrapperBase& aCallback, TInt aPriority)
	{
	CT_ActiveCallbackIO*	self=new(ELeave) CT_ActiveCallbackIO(aCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CT_ActiveCallbackIO::~CT_ActiveCallbackIO()
	{
	delete iBuffer;
	iBuffer=NULL;
	}

CT_ActiveCallbackIO::CT_ActiveCallbackIO(CDataWrapperBase& aCallback, TInt aPriority)
:	CActiveCallbackBase(aCallback, aPriority)
,	iBuffer(NULL)
	{
	}


TInt CT_ActiveCallbackIO::BufferLength()
	{
	return iBuffer->Length();
	}

const TDesC8& CT_ActiveCallbackIO::Buffer()
	{
	return *iBuffer;
	}

void CT_ActiveCallbackIO::PrepareFromStringL(TInt aRepeat, const TDesC& aString)
	{
	delete iBuffer;
	iBuffer=NULL;

	TInt	bufferLength=aString.Length()*aRepeat;
	iBuffer=HBufC8::NewL(bufferLength);
	iBuffer->Des().Copy(aString);
	ConvertAndRepeatBuffer(aRepeat);
	}

void CT_ActiveCallbackIO::PrepareFromFileL(TInt aRepeat, const TDesC& aFilename)
	{
	delete iBuffer;
	iBuffer=NULL;

	TFindFile	findFile(iDataWrapperBase.FileServer());
	TInt		err=findFile.FindByDir(aFilename, KStrNull);
	if ( err!=KErrNone )
		{
		iDataWrapperBase.ERR_PRINTF3(KLogErrorFileNotFound, &aFilename, err);
		iDataWrapperBase.SetBlockResult(EFail);
		}
	else
		{
		const TDesC&	fileName=findFile.File();
		RFile			file;
		err=file.Open(iDataWrapperBase.FileServer(), fileName, EFileRead|EFileShareAny);
		if ( err!=KErrNone )
			{
			iDataWrapperBase.ERR_PRINTF3(KLogErrorFileNotOpened, &aFilename, err);
			iDataWrapperBase.SetBlockResult(EFail);
			}
		else
			{
			CleanupClosePushL(file);
			iDataWrapperBase.INFO_PRINTF1(_L("File Opened."));

			// get size
			TInt	bufferLength;
			User::LeaveIfError(file.Size(bufferLength));
			bufferLength*=aRepeat;
			iDataWrapperBase.INFO_PRINTF2(_L("File Size=%d."), bufferLength);

			// read file
			iBuffer=HBufC8::NewL(bufferLength);
			iDataWrapperBase.INFO_PRINTF1(_L("Buffer created."));

			TPtr8	fileData = iBuffer->Des();
			User::LeaveIfError(file.Read(fileData));
			CleanupStack::PopAndDestroy(&file);

			ConvertAndRepeatBuffer(aRepeat);
			}
		}
	}

void CT_ActiveCallbackIO::ConvertAndRepeatBuffer(TInt aRepeat)
	{
	ConvertEscapeChars(iBuffer->Des());

	TPtr8	ptr=iBuffer->Des();

	for ( TInt index=1; index<aRepeat; ++index )
		{
		iBuffer->Des().Append(ptr);
		}
	}

void CT_ActiveCallbackIO::ConvertEscapeChars(TPtr8 aBuffer)
	{
	TInt	ret=KErrNone;
	TInt	length=aBuffer.Length();
	TInt	position=0;
	TBool	escapeFound=EFalse;

	for ( TInt index=0; index<length; ++index )
		{
		TChar	nextChar=aBuffer[index];
		if ( escapeFound )
			{
			switch ( nextChar )
				{
			case 'x':
				{
				//	Check that we have a further 2 characters
				if ( (index+2)<length )
					{
					++index;
					TUint charCode = 0;
					TLex8 lex(aBuffer.Mid(index,2));
					ret = lex.Val(charCode ,EHex);
					aBuffer[position++]=TChar(charCode);
					++index;
					nextChar = aBuffer[index];
					}
				else
					{
					ret=KErrArgument;
					}
				escapeFound=EFalse;
				}
				break;
			case 'n':
				//	\n found newline
				{
				aBuffer[position++]='\n';
				escapeFound=EFalse;
				}
				break;
			case 't':
				//	\t found horizontal tab
				{
				aBuffer[position++]='\t';
				escapeFound=EFalse;
				}
				break;
			case 'v':
				//	\v found vertical tab
				{
				aBuffer[position++]='\v';
				escapeFound=EFalse;
				}
				break;
			case 'b':
				//	\b found backspace
				{
				aBuffer[position++]='\b';
				escapeFound=EFalse;
				}
				break;
			case 'r':
				//	\r found carriage return
				{
				aBuffer[position++]='\r';
				escapeFound=EFalse;
				}
				break;
			case 'f':
				//	\f found form feed
				{
				aBuffer[position++]='\f';
				escapeFound=EFalse;
				}
				break;
			case 'a':
				//	\a found audible alert (bell)
				{
				aBuffer[position++]='\a';
				escapeFound=EFalse;
				}
				break;
			default:
				aBuffer[position++]=nextChar;
				escapeFound=EFalse;
				break;
				}
			}
		else
			{
			if ( aBuffer[index]=='\\' )
				{
				escapeFound=ETrue;
				}
			else
				{
				aBuffer[position++]=aBuffer[index];
				}
			}
		}

	if ( ret!=KErrNone )
		{
		iDataWrapperBase.ERR_PRINTF1(KLogErrorFormatError);
		iDataWrapperBase.SetBlockResult(EFail);
		}

	aBuffer.SetLength(position);
	}
