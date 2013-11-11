// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "config.h"

TConfig::TConfig()
	{	
	}

TBool TConfig::IsSupported(TInt aIpc)
	{
	TBool ret= ETrue;
	
	RFs	fs;
	RFile file;
	
	if( fs.Connect() == KErrNone)
		{
		if( file.Open(fs, KConfigFile, EFileRead) == KErrNone)
			{
			TBuf8<1000> buf;
			TBuf<100> temp;
			TInt bufLength=1000;
			TBuf<100> ipc;
			ipc.Num(aIpc);
			
			TBool commentFlag=EFalse;
			TBool valueFlag=EFalse;
			TBool storeFlag=EFalse;
			TBool valueRetrieved=EFalse;
			
			
			while (bufLength==1000)
				{
				file.Read(buf,1000);
				bufLength=buf.Length();
			
				for (TInt i=0; i<bufLength; ++i)
					{
					if(buf[i]=='#') 
						{
						commentFlag=ETrue;
						}
					else
						{
						if(!commentFlag)
							{//read the content	
							if(buf[i]!=' ' && buf[i]!='\n' && buf[i]!='\r' && buf[i]!='=' && buf[i]!='\t')
								{
								temp.Append(buf[i]);
								//end of the file and the file doesn't have a new line at the end
								if((i==bufLength-1 && bufLength<1000) && (valueFlag))
									valueRetrieved=ETrue;
								}
							else if ((valueFlag)&& buf[i]=='\r')
								{
								valueRetrieved=ETrue;	
								}
							else
								{
								storeFlag=ETrue;	
								}
							
							//storing a port names						
							if (storeFlag) 
								{
								if(temp.CompareC(ipc) == 0) valueFlag = ETrue;
								temp.Zero();
								storeFlag=EFalse;
								}
							else
							if(valueRetrieved)
								{
								temp.LowerCase();
								if (temp.CompareC(_L("notsupported")) == 0)
									{
									file.Close();
									fs.Close();
									return EFalse;
									}
								else
									{
									file.Close();
									fs.Close();
									return ETrue;
									}						
								}
							}
						else //skip the comment until \n
						if(buf[i]=='\n') 
							{
							commentFlag=EFalse;	
							}
						}
					}//for
				}//while
			}
		}
	
	file.Close();
	fs.Close();
	return ret;
	}
	
TInt TConfig::SetSupportedValue(TInt aIpc, TBool aSupported)
	{
	TInt ret=KErrGeneral;

	RFs	fs;
	RFile file;

	if(fs.Connect() == KErrNone)
		{
		if( file.Replace(fs, KConfigFile, EFileWrite) == KErrNone)
			{
			// write the content
			TBuf8<1000> buf;
			_LIT8(KTemp, "%d ");
			_LIT8(KSupported, "supported");
			_LIT8(KNotSupported, "notsupported");
			
			buf.Format(KTemp,aIpc);
			if (aSupported)
				{
				buf.Append(KSupported);
				}
			else
				{
				buf.Append(KNotSupported);
				}
			ret = file.Write(buf);
			}
		}
	
	file.Close();
	fs.Close();
	return ret;
	}

TInt TConfig::Reset()
	{
	TInt ret=KErrGeneral;
	RFs	fs;
		
	if(fs.Connect() == KErrNone)
		{
		ret = fs.Delete(KConfigFile);
		}
	
	fs.Close();
	return ret;
	}

/* static */
void TConfig::ResetThis(TAny* aThis)
	{
	(void)(static_cast<TConfig*>(aThis))->Reset();
	}

void TConfig::PushL()
	{
	CleanupStack::PushL(TCleanupItem(ResetThis, this));
	}

