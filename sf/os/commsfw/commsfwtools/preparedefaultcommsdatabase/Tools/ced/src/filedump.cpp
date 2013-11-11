// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "filedump.h"

/*static*/ CFileDump* CFileDump::NewL(RFs& fsSession, TPtrC filename, TPtrC appname, TPtrC version, TInt bDebugOn, CConsoleBase* aConsole, TBool bWriteHeader)
	{
	CFileDump* fd = new (ELeave) CFileDump;
	CleanupStack::PushL(fd);
	fd->ConstructL(fsSession, filename, appname, version, bDebugOn, aConsole, bWriteHeader);
	CleanupStack::Pop(fd);
	return fd;	
	}
	
CFileDump::~CFileDump()
	{
	file.Close();
	bInitialised = EFalse;
	}
	
	
void CFileDump::ConstructL(RFs &fsSession, TPtrC filename, TPtrC appname, TPtrC version, TInt bDebugOn, CConsoleBase* aConsole, TBool bWriteHeader)
/**
Opens the output file

@param fsSession A session with the file server
@param filename
@param appname
@param version
@param bDebugOn
@param console
@param bWriteHeader
@return ETrue if successful or EFalse
*/
	{
	if (oErr.Evaluate(file.Replace(fsSession, filename, EFileShareAny|EFileStream|EFileWrite)))
		{
		if (bWriteHeader)
			{
			buf.Format(_L("############################################################\r\n## AUTO-GENERATED CONFIGURATION FILE\r\n## %S\r\n## %S\r\n############################################################\r\n"),
				&appname, &version);
			WriteBufferToFile(buf, file);
			}
		
		bInitialised = ETrue;
		}
	bDebug = bDebugOn;
	pConsole = aConsole;
	}


void CFileDump::WriteTableHeader(TPtrC table)
/**
Writes a table header to the internal buffer

@param table A reference to a descriptor containing the name of a table
*/
	{
	if (bInitialised && table.Length())
		{
		buf.Format(_L("\r\n############################################################\r\n## %S\r\n## \r\n[%S]\r\n"),
			&table, &table);
		WriteBufferToFile(buf, file);
		}
	}


void CFileDump::WriteSectionHeader(TInt aCommDbId)
/**
Writes a section header to the internal buffer

@param aCommDbId Unique Id of Communication database
*/
	{
	if (bInitialised)
		{
		switch (aCommDbId)
			{
			case -1:
				buf.Copy(_L("ADD_SECTION\r\n"));
				break;
			case 0:
				buf.Copy(_L("ADD_TEMPLATE\r\n"));
				break;
			default:
				buf.Format(_L("ADD_SECTION\r\n# COMMDB_ID = %d\r\n"),
					aCommDbId);
				break;
			}
		WriteBufferToFile(buf, file);
		}
	}


void CFileDump::WriteFieldCount(TUint count)
/**
Writes a section header to the internal buffer

@param count Field count
*/
	{
	if (bInitialised)
		{
		buf.Format(_L("\tFIELD_COUNT=%d\r\n"), count);
		WriteBufferToFile(buf, file);
		}
	}


void CFileDump::WriteColumnValue(TPtrC &column, TPtrC &setting)
/**
Writes a column value to the internal buffer

@param column A reference to a descriptor containing the name of a column
@param setting The setting
*/
	{
	if (bInitialised && column.Length() && setting.Length())
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
		for (i=0;i<setting.Length();i++)
			{
			TUint16 x = setting[i];
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
		
		setting.Set(TempValue);
		buf.Format(_L("\t%S=%S\r\n"), &column, &TempValue);
		WriteBufferToFile(buf, file);
		}
	}


void CFileDump::WriteSectionFooter(TInt aCommDbId)
/**
Writes a section footer to the internal buffer

@param aCommDbId Unique Id of Communication database
*/
	{
	if (bInitialised)
		{
		if (aCommDbId == 0)
			{
			buf.Copy(_L("END_TEMPLATE\r\n\r\n"));
			}
		else
			{
			buf.Copy(_L("END_ADD\r\n\r\n"));
			}
		WriteBufferToFile(buf, file);
		}
	}


void CFileDump::WriteErrorMessage(const TPtrC &message)
/**
Writes a message to the file

@param message The error message 
*/
	{
	if (bInitialised && message.Length() && message.Length() < (MAX_COL_LONG_VAL_LEN - 140))
		{
		buf.Format(_L("############################################################\r\n## ERROR\r\n## %S\r\n############################################################\r\n"),
			&message);
		WriteBufferToFile(buf, file);
		}
	}

void CFileDump::Msg(TPtrC text, ...)
	{
	if (text.Length() && text.Length() < MAX_COL_LONG_VAL_LEN)
		{
		VA_LIST list;
		VA_START(list, text);
		
		buf.FormatList(text, list);
		buf.Append(_L("\r\n"));
		
		// Only output to the console if the debug option is enabled 
		if (pConsole && bDebug)
			pConsole->Printf(buf);
		
		if (bInitialised)
			{
			// Always write to the log file
			WriteBufferToFile(buf, file);
			}
		
		VA_END(list);
		}
	}

void CFileDump::Dbg(TPtrC text, ...)
	{
	if (bDebug && text.Length() && text.Length() < MAX_COL_LONG_VAL_LEN)
		{
		VA_LIST list;
		VA_START(list, text);
		
		buf.FormatList(text, list);
		buf.Append(_L("\r\n"));
		
		// Always write to the console if available
		if (pConsole)
			pConsole->Printf(buf);
		
		if (bInitialised)
			{
			// Always write to the log file
			WriteBufferToFile(buf, file);
			}
		
		VA_END(list);
		}
	}

void CFileDump::Error(TPtrC text, ...)
	{
	if (bInitialised && text.Length() && text.Length() < MAX_COL_LONG_VAL_LEN)
		{
		VA_LIST list;
		VA_START(list, text);
		
		buf.FormatList(text, list);
		buf.Append(_L("\r\n"));
		
		// Always write to the console if available
		if (pConsole)
			pConsole->Printf(buf);
		
		if (bInitialised)
			{
			// Always write to the log file
			WriteBufferToFile(buf, file);
			}
		
		VA_END(list);
		}
	}

TBool CFileDump::WriteBufferToFile(TDesC16 &buffer, RFile &handle)
	{
	static TBuf8<1024> outputBuffer;
	static TPtrC16 ptr;
	static TInt consumed;
	static TInt remainder = 0;
	
	TBool valid = EFalse;
	
	if (buffer.Length())
		{
		ptr.Set(buffer);
		do
			{
			// get something to write
			consumed = Min(outputBuffer.MaxLength(), ptr.Length());
			
			// write it
			outputBuffer.Copy(ptr.Left(consumed));
			if (handle.Write(outputBuffer) != KErrNone)
				return EFalse;
			
			// get the next chunk
			remainder = ptr.Length() - consumed;
			if (remainder > 0)
				ptr.Set(ptr.Right(remainder));
			
			}while (remainder > 0);
			
			valid = ETrue;
		}
	
	return valid;
	}
