// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalTechnology
*/

#include "filedump.h"

#ifndef __TOOLS2__
_LIT(DEFAULT_SESSION_PATH, "c:\\");
#endif

/*static*/ CFileDump* CFileDump::NewL(TPtrC aFilename, TPtrC aAppname, TPtrC aVersion, TBool aDebugOn, CConsoleBase* aConsole, TBool aWriteHeader)
	{
	CFileDump* fdump = new (ELeave) CFileDump(aDebugOn, aConsole);
	CleanupStack::PushL(fdump);
	fdump->ConstructL(aFilename, aAppname, aVersion, aWriteHeader);
	CleanupStack::Pop(fdump);
	return fdump;
	}

CFileDump::CFileDump(TInt aDebugOn, CConsoleBase* aConsole)
	:iDebug(aDebugOn), iConsole(aConsole)
	{
	}

void CFileDump::ConstructL(TPtrC aFilename, TPtrC aAppname, TPtrC aVersion, TBool aWriteHeader)
/**
Opens the output file
*/
	{
	User::LeaveIfError(iFsSession.Connect());
	
#ifndef __TOOLS2__
	// Set up the session path for WINSCW / ARMv5 incase the log file name has no path.
	iFsSession.SetSessionPath( DEFAULT_SESSION_PATH );
#endif

	User::LeaveIfError(iFile.Replace(iFsSession, aFilename, EFileShareAny|EFileStream|EFileWrite));
	if (aWriteHeader)
		{
		iBuffer.Format(_L("############################################################\r\n## AUTO-GENERATED CONFIGURATION FILE\r\n## %S\r\n## %S\r\n############################################################\r\n"),
			&aAppname, &aVersion);
		WriteBufferToFile(iBuffer, iFile);
		}
	iInitialised = ETrue;
	}

CFileDump::~CFileDump()
	{
	iFile.Close();
	iFsSession.Close();
	}

void CFileDump::WriteTableHeader(TPtrC aTable)
/**
Writes a table header to the internal buffer

@param table A reference to a descriptor containing the name of a table
*/
	{
	if (iInitialised && aTable.Length())
		{
		iBuffer.Format(_L("\r\n############################################################\r\n## %S\r\n## \r\n[%S]\r\n"),
			&aTable, &aTable);
		WriteBufferToFile(iBuffer, iFile);
		}
	}


void CFileDump::WriteSectionHeader(TInt aCommDbId)
/**
Writes a section header to the internal buffer

@param aCommDbId Unique Id of Communication database
*/
	{
	if (iInitialised)
		{
		switch (aCommDbId)
			{
			case -1:
				iBuffer.Copy(_L("ADD_SECTION\r\n"));
				break;
			case 0:
				iBuffer.Copy(_L("ADD_TEMPLATE\r\n"));
				break;
			case -2:
				iBuffer.Copy(_L("ADD_DEFINITION\r\n"));
				break;
			default:
				iBuffer.Format(_L("ADD_SECTION\r\n# COMMDB_ID = %d\r\n"),
					aCommDbId);
				break;
			}
		WriteBufferToFile(iBuffer, iFile);
		}
	}


void CFileDump::WriteFieldCount(TUint aCount)
/**
Writes a section header to the internal buffer

@param count Field count
*/
	{
	if (iInitialised)
		{
		iBuffer.Format(_L("\tFIELD_COUNT=%d\r\n"), aCount);
		WriteBufferToFile(iBuffer, iFile);
		}
	}


TBool CFileDump::WriteColumnValue(TPtrC &aColumn, TPtrC &aSetting)
/**
Writes a column value to the internal buffer

@param column A reference to a descriptor containing the name of a column
@param setting The setting
@return ETrue if value was written
*/
	{
	TBool write=EFalse;
	if (iInitialised && aColumn.Length() && aSetting.Length())
		{
		// Theoretical worst case is longest field is populated with non-ASCII
		static TBuf<KMaxExpandedFieldLen> TempValue;	// static to keep it off the stack
		TempValue.SetLength(0);
		
		/* replace any special chars with a portable representation that's re-importable
		* Special characters are escaped in the 'C' language fashion with backslash.
		* Specials are: 
		*	\t = TAB 
		*	\n = LF 
		*	\[ = [		- Need for escaping is to prevent INI file processing presuming
		*	\] = ]		- them to be start of section block
		*	\\ = \
		*	\xHHHH 		- Hex value for any other character falling outside the normal
		*				  ASCII range of 32 - 126. This is because we're most likely 
		*				  writing to an ASCII file, so absolutely can't trust that
		*				  UniCode will get preserved on a round-trip through an editor
		*/
		
		TInt i = 0;
		for (i=0;i<aSetting.Length();i++)
			{
			TUint16 x = aSetting[i];
			switch(x)
				{
				case '\n':
					TempValue.Append(_L("\\n"));
					break;
				case '\t':
					TempValue.Append(_L("\\t"));
					break;
				case '[':
					TempValue.Append(_L("\\["));
					break;
				case ']':
					TempValue.Append(_L("\\]"));
					break;
				case '\\':
					TempValue.Append(_L("\\\\"));
					break;
				default:
					if(x < 32 || x > 126)
						{
						TempValue.AppendFormat(_L("\\x%04X"), x);
						}
					else
						{
						TempValue.Append(x);
						}
					break;
				}
			}
		
		aSetting.Set(TempValue);
		iBuffer.Format(_L("\t%S=%S\r\n"), &aColumn, &TempValue);
		WriteBufferToFile(iBuffer, iFile);
		write=ETrue;;
		}
	return write;
	}


void CFileDump::WriteSectionFooter(TInt aCommDbId)
/**
Writes a section footer to the internal buffer

@param aCommDbId Unique Id of Communication database
*/
	{
	if (iInitialised)
		{
		if (aCommDbId == 0)
			{
			iBuffer.Copy(_L("END_TEMPLATE\r\n\r\n"));
			}
		else if (aCommDbId == -2)
			{
			iBuffer.Copy(_L("END_DEFINITION\r\n\r\n"));
			}
		else
			{
			iBuffer.Copy(_L("END_ADD\r\n\r\n"));
			}
		WriteBufferToFile(iBuffer, iFile);
		}
	}


void CFileDump::WriteErrorMessage(const TPtrC &aMessage)
/**
Writes a message to the file

@param message The error message 
*/
	{
	if (iInitialised && aMessage.Length() && aMessage.Length() < (MAX_COL_LONG_VAL_LEN - 140))
		{
		iBuffer.Format(_L("############################################################\r\n## ERROR\r\n## %S\r\n############################################################\r\n"),
			&aMessage);
		WriteBufferToFile(iBuffer, iFile);
		}
	}

void CFileDump::Msg(TPtrC aText, ...)
	{
	if (iInitialised && aText.Length() && aText.Length() < MAX_COL_LONG_VAL_LEN)
		{
		VA_LIST list;
		VA_START(list, aText);
		
		iBuffer.FormatList(aText, list);
		iBuffer.Append(_L("\r\n"));
		
		if (iConsole)
			{
			iConsole->Printf(iBuffer);
			}
		
		WriteBufferToFile(iBuffer, iFile);
		
		VA_END(list);
		}
	}

void CFileDump::Dbg(TPtrC aText, ...)
	{
	if (iInitialised && iDebug && aText.Length() && aText.Length() < MAX_COL_LONG_VAL_LEN)
		{
		VA_LIST list;
		VA_START(list, aText);
		
		iBuffer.FormatList(aText, list);
		iBuffer.Append(_L("\r\n"));
		
		if (iConsole)
		    {
			iConsole->Printf(iBuffer);
			}
		
		WriteBufferToFile(iBuffer, iFile);
		
		VA_END(list);
		}
	}

TBool CFileDump::WriteBufferToFile(TDesC16 &aBuffer, RFile &aHandle)
	{
	// keep buffer small to reduce memory usage
	static TBuf8<5> outputBuffer;
	static TPtrC16 ptr;
	static TInt consumed;
	static TInt remainder = 0;
	
	TBool valid = EFalse;
	
	if (aBuffer.Length())
		{
		ptr.Set(aBuffer);
		do
			{
			// get something to write
			consumed = Min(outputBuffer.MaxLength(), ptr.Length());
			
			// write it
			outputBuffer.Copy(ptr.Left(consumed));
			if (aHandle.Write(outputBuffer) != KErrNone)
			    {
				return EFalse;
				}
			
			// get the next chunk
			remainder = ptr.Length() - consumed;
			if (remainder > 0)
				{
				ptr.Set(ptr.Right(remainder));
				}
			
			} 
		while (remainder > 0);
			
		valid = ETrue;
		}
	
	return valid;
	}

/**
Writes attribute section header to the output file
*/
void CFileDump::WriteAttributeSectionHeader()
	{
	iBuffer.Copy(_L("ADD_ATTRIBUTES\r\n"));
	WriteBufferToFile(iBuffer, iFile);
	}

/**
Writes attributes to the output file
*/
void CFileDump::WriteAttributes(TPtrC &aColumn)
	{
	if (iInitialised && aColumn.Length())
		{
		iBuffer.Format(_L("\t%S\r\n"), &aColumn);
		WriteBufferToFile(iBuffer, iFile);
		}	
	}
	
