// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <f32file.h>
#include "CIniFile.h"
#include "dbdef.h"

CIniData::CIniData() 
/**
Default constructor

*/
	: iPtr(NULL,0), section(NULL,0), block(NULL,0), BlockState(E_UNKNOWN),
	  blockStart(0), blockEnd(0), scanStart(0)
	{
	__DECLARE_NAME(_S("CIniData"));
	}

CIniData::~CIniData()
/**
Destructor

*/
	{
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}

CIniData* CIniData::NewL(const TDesC& aName)
/**
Allocates and constructs a new CIniData object

@param aName Name of the ini file to be read
@return A newly created CIniData object
*/
	{
	CIniData* p = new(ELeave) CIniData;
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	CleanupStack::Pop(p);
	return p;
	}


void CIniData::ConstructL(const TDesC& aName)
/**
Allocates a buffer and reads file's contents into iPtr

@param aName Name of the ini file to be read
*/
	{
 	// Allocate space for token
	iToken=HBufC::NewL(MAX_COL_NAME_LEN + 2);	// 2 extra chars for [tokenName]

	// Connect to file server
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	// Find file, given name
	TFindFile ff(fs.iObj);
	User::LeaveIfError(ff.FindByPath(aName, NULL));
	iName=ff.File().AllocL();

	// Open file
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileRead));
	file.PushL();

	// Get file size and read in
	User::LeaveIfError(file.iObj.Size(size));
	TText* data=(TText*)User::AllocL(size);
	iPtr.Set(data, TUint(size)/sizeof(TText), TUint(size)/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>=(TInt)sizeof(TText) && iPtr[0]==0xFEFF)
		{
		// UNICODE Text file so lose the FFFE
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, TUint(size)/sizeof(TText)-1, TUint(size)/sizeof(TText)-1);
		}
	else if(size)
		{
		// NON-UNICODE so convert to UNICODE
		TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			{
			iPtr[i]=ptr[i];
			}
		delete data;
		}

	file.Pop();
	fs.Pop();
	}


TInt CIniData::OpenSetBlock(const TDesC &aSection)
/**
Locates and opens a section for 'updates'

@param aSection Section to work with
@return ETrue if successful or EFalse
*/
	{
	BlockState = E_SET;
	return OpenBlock(aSection);
	}


TInt CIniData::OpenTemplateBlock(const TDesC &aSection)
/**
Locates and opens a section for 'template'

@param aSection Section to work with
@return ETrue if successful or EFalse
*/
	{
	BlockState = E_TEMPLATE;
	return OpenBlock(aSection);
	}


TInt CIniData::OpenAddBlock(const TDesC &aSection)
/**
Locates and opens a section for 'adds'

@param aSection Section to work with
@return ETrue if successful or EFalse
*/
	{
	BlockState = E_ADD;
	return OpenBlock(aSection);
	}


TInt CIniData::OpenBlock(const TDesC &aSection)
/**
Reads in the entire table section of all ADD / SET blocks into 'section'

@param aSection Section to work with
@return ETrue if successful or EFalse
*/
	{
    if (BlockState != E_UNKNOWN)
		{
	    blockStart = 0;
	    blockEnd = 0;
	    scanStart = 0;

	    TPtr sectionToken = iToken->Des();
		_LIT(sectionTokenString,"[%S]");
		sectionToken.Format(sectionTokenString,&aSection);

	    // locate the section
		TInt sectionStart = iPtr.FindF(sectionToken);
	    if (sectionStart == KErrNotFound)
			{
	        return EFalse;
			}

		// step to the end of the section name
		TPtrC tempSection = iPtr.Mid(sectionStart);
        sectionStart += tempSection.Find(TPtrC(_S("]")));

		if (sectionStart == KErrNotFound)
			{
			return EFalse;
			}
		sectionStart++;

		// we are now at the start of the section data
        tempSection.Set(iPtr.Mid(sectionStart));

		// loop until we reach the end of the section
		TUint32 i=0;
		TUint32 lMaxLen = tempSection.Length();

        for (i=0;i<lMaxLen;i++)
			{
	        if (tempSection.Ptr()[i] == '[' &&
		        tempSection.Ptr()[i - 1] != '\\')
				{
			    i--;
			    break;
				}
			}
		
		// set the actual section to work with
        tempSection.Set(iPtr.Mid(sectionStart, i));
		section.Set((unsigned short *)tempSection.Ptr(), tempSection.Length(), tempSection.Size());
		}

	return StepToNextBlock();	// find the first block
	}


TInt CIniData::StepToNextBlock()
/**
Locates the next available block of data within a section

@return ETrue if successful or EFalse
*/
	{
	if (BlockState != E_UNKNOWN)
		{
		// get unscanned portion of the section
		TPtrC temp = section.Mid(scanStart);

		// find the start of the next block
		if (BlockState == E_SET)
			{
			blockStart = temp.FindF(TPtrC(BEGIN_SET));
			blockEnd = temp.FindF(TPtrC(END_SET));
			}
		else if (BlockState == E_TEMPLATE)
			{
			blockStart = temp.FindF(TPtrC(BEGIN_TEMPLATE));
			blockEnd = temp.FindF(TPtrC(END_TEMPLATE));
			}
		else if (BlockState == E_ADD)
			{
			blockStart = temp.FindF(TPtrC(BEGIN_ADD));
			blockEnd = temp.FindF(TPtrC(END_ADD));
			}

		if (blockStart != KErrNotFound && blockEnd != KErrNotFound)
			{
			// set the start point for the next block search
			scanStart += blockEnd;
			scanStart++;

			// set the actual block to work with
			blockEnd -= blockStart;
			TPtrC tempBlock = temp.Mid(blockStart,blockEnd);
			block.Set((unsigned short *)tempBlock.Ptr(), tempBlock.Length(), tempBlock.Size());
			return ETrue;
			}
		}

	return EFalse;
	}


TInt CIniData::GetSetting(const TDesC &aValue, TPtrC &aSetting)
/**
Retrieves the value for a particular setting within a block

@param aValue The setting to be retrieved 
@param aSetting On return it contains the value for the setting
@return KErrNone if successful or KErrArgument for fields simply left blank. If the global BlockState is unknown returns KErrNotReady.
*/
	{
	TInt valid(KErrNotReady);	

	if (BlockState != E_UNKNOWN)
	    {
		// first check for CONDITION field
		// if we are NOT looking for this field, we need to trim off any condition
		// field from the block because there may be a setting within it we 
		// want to ignore during our search for this field
        TPtrC pBlock(block);
		TInt tempEnd = blockEnd;
		TPtrC Condition = CONDITION;
		if (aValue.Compare(Condition) != 0)
		    {
			TPtr varCondToken = iToken->Des();
			_LIT(varCondTokenString, "%S=");
			varCondToken.Format(varCondTokenString, &Condition);
			TInt CondPos = pBlock.FindF(varCondToken);
			if (CondPos != KErrNotFound)
				{
				tempEnd = CondPos;
				}
		    }

		// now look for the actual value
        TBool searchAgain = EFalse;
        _LIT(varTokenString, "%S=");
		TPtr varToken = iToken->Des();
		varToken.Format(varTokenString, &aValue);

        TInt pos = KErrNotFound;
        do
            {
            searchAgain = EFalse;
            pos = pBlock.FindF(varToken);
		    if (pos != KErrNotFound && pos < tempEnd)
		        {
                // check that this is a complete match, not a substring
                // match against another similar field, e.g. LoginScript
                // must only match against "LoginScript" not "UseLoginScript"
                if  (pos > 0)
                    {
                    // Previous character must be white space
                    const TChar previousCharacter = pBlock[pos - 1];
                    if  (previousCharacter.IsSpace())
                        {
			            // make sure we haven't overrun our block
			            TInt length = varToken.Length();
			            if (pos + length < tempEnd)
			                {
				            TLex lex(pBlock.Mid(pos + length));

				            // if enclosed by quotes, extract the text within
				            if (lex.Peek() == '"')
				                {
					            lex.SkipAndMark(1);			// start of data

					            // stop at end of quote or line
					            while(lex.Peek() != '"' && lex.Peek() != 10 && lex.Peek() != 13)
									{
						            lex.Inc();
									}
				                }
							else if(lex.Peek() == 10 || lex.Peek() == 13)	// skip empty or blank field value
								{
								return KErrArgument;
								}
							else	// skip any unwanted spaces or tabs in a field value
								{
								TBool fieldValFound=EFalse;
								while(lex.Peek() != 10 && lex.Peek() != 13) 
									{
									if(!fieldValFound)
										{
										while(lex.Peek() == 9 || lex.Peek() == 32) // skip any space or a tab
											{
											lex.SkipAndMark(1);
											}
										if(lex.Peek() == 10 || lex.Peek() == 13) // field value simply filled with space or tab
											{
											return KErrArgument;
											}
										}
									fieldValFound=ETrue;	// start of real data
									lex.Inc();
									}
								}

				            aSetting.Set(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
							valid = KErrNone;
                            }
                        }
                    else
                        {
                        // E.g. LoginScript matched against UseLoginScript -> must look
                        // for another match after the current one
                        pBlock.Set(pBlock.Mid(pos+1));
                        searchAgain = ETrue;
                        }
                    }
			    }
            }
        while(searchAgain);

	    }

	return valid;
    }
