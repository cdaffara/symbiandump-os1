// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ControllerInitialisationCmndParser.h"

#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/vendordebugcommand.h>

#include <bluetooth/logger.h>

#include <es_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_INITIALISOR_TI);
#endif

/*static*/ CCommandParser* CCommandParser::NewL(const TDesC& aCommandFile)
	{
	LOG_STATIC_FUNC

	CCommandParser* self = new(ELeave) CCommandParser;
	CleanupStack::PushL(self);
	self->ConstructL(aCommandFile);
	CleanupStack::Pop(self);
	return self;
	}

void CCommandParser::ConstructL(const TDesC& aCommandFile)
	{
	LOG_FUNC

	LEAVEIFERRORL(iFs.Connect());
	LEAVEIFERRORL(iFile.Open(iFs, aCommandFile,
	                              EFileRead || EFileShareReadersOnly || EFileStream)); 

	TInt seekOffset = KScriptHeaderLength;
	LEAVEIFERRORL(iFile.Seek(ESeekCurrent, seekOffset));
	}

CCommandParser::~CCommandParser()
	{
	LOG_FUNC

	iFile.Close();
	iFs.Close();
	}

// Private Constructor
CCommandParser::CCommandParser()
	{
	LOG_FUNC
	}
	
TInt CCommandParser::GetNextVendorCommand(CVendorDebugCommand& aVendorCommand)
	{
	LOG_FUNC

	TInt rerr = KErrNone;

	// First find a vendor command in the script.
	TBuf8<KScriptElementHeaderLength> scriptHeaderBuffer; 
	while(rerr == KErrNone)
		{
		rerr = iFile.Read(scriptHeaderBuffer, KScriptElementHeaderLength);
		
		if(rerr == KErrNone)
			{
			if(scriptHeaderBuffer.Length() == KScriptElementHeaderLength)
				{
				TUint16 action = LittleEndian::Get16(scriptHeaderBuffer.Ptr());
				// A TInt is used for the length so that is can be used in the
				// call to RFile::Seek.
				TInt length = LittleEndian::Get16(scriptHeaderBuffer.Ptr() + KScriptLengthOffset);
				
				if(action == KSendCommand)
					{
					break;
					}
				
				// Move to the next element in the file.
				rerr = iFile.Seek(ESeekCurrent, length);
				}
			else
				{
				// Failed to read enough header bytes, must be at end of file.
				rerr = KErrEof;
				}
			}
		}
		
	if(rerr == KErrNone)
		{
		TBuf8<KScriptHCIHeaderLength> hciHeaderBuffer;
		
		// Read the Opcode.
		rerr = iFile.Read(hciHeaderBuffer, KScriptHCIHeaderLength);
		if(rerr == KErrNone)
			{
			if(hciHeaderBuffer.Length() == KScriptHCIHeaderLength)
				{
				THCIOpcode opcode = LittleEndian::Get16(hciHeaderBuffer.Ptr() + KScriptHCIHeaderOpcodeOffset);
				if((opcode & KOGFMask) == KVendorDebugOGF)
					{
					// Re-initialise the vendor command with the new Op-code.
					aVendorCommand.Reset(opcode);
					rerr = iFile.Read(aVendorCommand.Command(), hciHeaderBuffer[KScriptHCICmdHdrLengthOffset]);
					
					// Check that the correct number of bytes were read into the
					// the buffer.
					if((rerr == KErrNone) && (aVendorCommand.Command().Length() != hciHeaderBuffer[KScriptHCICmdHdrLengthOffset]))
						{
						rerr = KErrEof;
						}
					}
				else
					{
					rerr = KErrCorrupt;
					}
				}
			else
				{
				rerr = KErrEof;
				}
			}
		}
	return rerr;
	}
