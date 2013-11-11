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


#include "BTTestConsole.h"

#include <bt_sock.h>
#include <e32test.h>
#include <bluetooth/logger.h>

#include <bt_subscribe.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bt_subscribe_partner.h>
#endif

#include <btsdp.h>
#include <bluetooth/hci/hciipc.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_TESTCONSOLE);
#endif

#ifdef _DEBUG
PANICCATEGORY("bttestcons");
#endif

#ifdef __ARMCC__
// Disable warning for this cpp file:
// "variable ... was declared but never referenced"
#pragma diag_suppress 177 
#endif // __ARMCC__

_LIT_SECURE_ID(KL2CAPSID,0x101F7989);

TBTTestBuffer::TBTTestBuffer()
	{
	Reset();
	}

void TBTTestBuffer::Reset()
	{
	for(TInt i=0;i<EMaxBufferSize;i++)
		{
		iInputBuffer[i] = 0;
		}
	iLength = 0;
	}

TChar TBTTestBuffer::At(TInt aIndex)
	{
	if(aIndex >= 0 && aIndex < iLength)
		{
		return iInputBuffer[aIndex];
		}
	return 0;
	}

TBool TBTTestBuffer::DecGetValue(TInt64 & aValue)
	{
	if(!iLength)
		{
		return EFalse;
		}

	TInt64 value = 0;
	TInt power = 1;
	for(TInt i=iLength-1;i>=0;i--)
		{
		if(!iInputBuffer[i].IsDigit())
			{
			return EFalse;
			}
		TInt64 bufValue;
		bufValue = iInputBuffer[i].GetNumericValue();
		value += bufValue * power;
		power *= 10;
		}
	aValue = value;
	return ETrue;
	}

TBool TBTTestBuffer::HexGetValue(TInt64 & aValue)
	{
	if(!iLength)
		{
		return EFalse;
		}

	TInt64 value = 0;
	TInt64 bufValue;
	TInt power = 0;
	for(TInt i=iLength-1;i>=0;i--)
		{
		if(!iInputBuffer[i].IsHexDigit())
			{
			return EFalse;
			}
		if(iInputBuffer[i].IsDigit())
			{
			bufValue = iInputBuffer[i].GetNumericValue();
			value += (bufValue << (power++ * 4));
			}
		else 
			{
			iInputBuffer[i].LowerCase();
			TInt64 temp = 10 + (iInputBuffer[i] - TUint('a'));
			value += (temp << (power++ * 4));
			}
		}
	aValue = value;
	return ETrue;
	}

TBool TBTTestBuffer::Add(TChar aKey, TInt aMaxLength)
	{
	if(iLength + 1 > aMaxLength)
		{
		return EFalse;
		}
	else 
		{
		iInputBuffer[iLength++] = aKey;
		return ETrue;
		}
	}

TBool TBTTestBuffer::AddNumber(TChar aKey, TInt aMaxLength)
	{
	if(!aKey.IsDigit())
		{
		return EFalse;
		}
	else 
		{
		return Add(aKey, aMaxLength);
		}
	}

TBool TBTTestBuffer::AddHexNumber(TChar aKey, TInt aMaxLength)
	{
	if(!aKey.IsHexDigit())
		{
		return EFalse;
		}
	else 
		{
		return Add(aKey, aMaxLength);
		}
	}

void TBTTestBuffer::RemoveChar()
	{
	iInputBuffer[iLength--] = 0;
	}

void TBTTestBuffer::WriteIntToBuffer(TInt64 aValue)
	{
	Reset();
	TBuf8<EMaxBufferSize> buf;
	buf.Format(_L8("%x"),aValue);
	for (TUint i=0;i<buf.Length();i++)
		{
		iInputBuffer[i] = buf[i];
		iLength++;
		}
	}

void TBTTestBuffer::WriteTDesC8ToBuffer(const TDesC8& aDes)
	{
	TBool pastSignificantDigit = EFalse;
	for (TUint i=0;i<aDes.Length();i++)
		{
		// There are two hex digits in 8 bytes.
		TBuf8<2> buf;
		buf.Format(_L8("%02x"),aDes[i]);
		for (TUint j=0;j<buf.Length();j++)
			{
			if (!pastSignificantDigit)
				{
				if (buf[j] == '0')
					{
					continue;
					}
				else
					{
					pastSignificantDigit = ETrue;
					}
				}
			iInputBuffer[iLength++] = buf[j];
			}
		}	
	}
	
void TBTTestBuffer::PrintBuffer(CConsoleBase& aConsole)
	{
	for (TInt i=0;i<iLength;i++)
		{
		TUint ch(iInputBuffer[i]);
		aConsole.Printf(_L("%c"),ch);
		}
	}



TInputProcessorBase::TInputProcessorBase(MMenuUser& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: iConsole(aConsole),
  iParent(aParent),
  iCb(aCb)
	{
	iCb = TCallBack(HandleKeyPress, this);
	}

TInt TInputProcessorBase::HandleKeyPress(TAny* aInstance)
	{
	TInt rval = ((TInputProcessorBase*)aInstance)->ProcessKey(((TInputProcessorBase*)aInstance)->GetKey());
	if(rval == EMenuComplete)
		{
		return ((TInputProcessorBase*)aInstance)->MenuClosed();
		}
	return ETrue;
	}

TChar TInputProcessorBase::GetKey()
	{
	return iConsole.KeyCode();
	}

TBool TInputProcessorBase::MenuClosed()
	{
	return iParent.ChildMenuClosed();
	}

TBool TInputProcessorBase::ChildMenuClosed()
	{
	ProcessChildMenuClosed();
	iCb = TCallBack(HandleKeyPress, this);
	return ETrue;
	}


XInputMainMenu::XInputMainMenu(CBTTestConsole& aTestConsole, TCallBack& aCb, CConsoleBase& aConsole)
: TInputProcessorBase(aTestConsole, aCb, aConsole),
  iConsoleApp(aTestConsole),
  iSelectedConnection(-1),
  iCurrentSubMenu(ENone),
  iSubMenu(0),
  iLastConnectDetails(NULL),
  iLastLegacyConnectDetails(NULL),
  iLastListenDetails(NULL),
  iLastLegacyListenDetails(NULL),
  iLastBapDetails(NULL),
  iLastEScoDetails(NULL),
  iLastScoDetails(NULL)
	{
	}

XInputMainMenu::~XInputMainMenu()
	{
	if (iSubMenu != iLastConnectDetails)
		{
		delete iLastConnectDetails;
		}
	else if (iSubMenu != iLastLegacyConnectDetails)
		{
		delete iLastLegacyConnectDetails;
		}
	else if (iSubMenu != iLastListenDetails)
		{
		delete iLastListenDetails;
		}
	else if (iSubMenu != iLastLegacyListenDetails)
		{
		delete iLastLegacyListenDetails;
		}
	else if (iSubMenu != iLastBapDetails)
		{
		delete iLastBapDetails;
		}
	else if (iSubMenu != iLastEScoDetails)
		{
		delete iLastEScoDetails;
		}
	else if (iSubMenu != iLastScoDetails)
		{
		delete iLastScoDetails;
		}
	delete iSubMenu;
	}
	
void XInputMainMenu::DeleteSubMenuIfAppropriate()
	{
	if (iSubMenu != iLastConnectDetails
	 && iSubMenu != iLastLegacyConnectDetails
	 && iSubMenu != iLastListenDetails
	 && iSubMenu != iLastLegacyListenDetails
	 && iSubMenu != iLastBapDetails
	 && iSubMenu != iLastEScoDetails
	 && iSubMenu != iLastScoDetails)
		{
		delete iSubMenu;
		iSubMenu = NULL;
		}
	}

void XInputMainMenu::Draw()
	{
	iConsole.Printf(_L("--------------------------------------------------\n\n"));
	iConsole.Printf(_L("l, c, b - Listen, Sock. Connect, Phys. Connect.\n"));
	iConsole.Printf(_L("*, ^    - Legacy Listen, Legacy Sock. Connect.\n"));
	iConsole.Printf(_L("!       - Dedicated Bond.\n"));
	iConsole.Printf(_L("i       - Enter L2Cap Config.\n")); 
	iConsole.Printf(_L("G       - Debug menu (DEBUG builds only).\n"));
	iConsole.Printf(_L("N       - Set Host Name.\n"));
	iConsole.Printf(_L("y, t    - Read, Refresh Local OOB Data\n"));
	iConsole.Printf(_L("z, u    - Provide Remote OOB Data, From RNR\n"));
	iConsole.Printf(_L("B, Q    - Inquiry&Page Scan, Page Scan Only.\n"));
	iConsole.Printf(_L("J, Y    - Limited Discoverable Mode On/Off.\n"));
	iConsole.Printf(_L("$, &    - Paired Connections Only On/Off.\n"));
	iConsole.Printf(_L("%%       - Debug Mode On.\n"));
	iConsole.Printf(_L("C       - HCI Server Menu.\n"));

	if(iSelectedConnection >= 0)
		{
		iConsole.Printf(_L("a       - Accept Connection (from listener).\n"));
		iConsole.Printf(_L("d, f, D	- Shutdown or Disc, Fast SD, Close + Del.\n"));
		iConsole.Printf(_L("L       - L2CAP / RFCOMM Data Menu.\n"));
		iConsole.Printf(_L("r       - RFCOMM Menu.\n"));
		iConsole.Printf(_L("p       - Physical Link Menu.\n"));
		iConsole.Printf(_L("g       - Get Remote Host Name details. \n"));
		iConsole.Printf(_L("\n"));
		
		iConsole.Printf(_L("I, U    - Update Config. [IOCTL], [SetOpt]\n"));
		iConsole.Printf(_L("s, S    - Passive, Active SCO.\n"));
		iConsole.Printf(_L("o, O    - Passive, Active eSCO.\n"));
		iConsole.Printf(_L("R       - Register SDP record.\n"));
	
		iConsole.Printf(_L("e       - Send Echo Request.\n"));
		iConsole.Printf(_L("Z       - Clean All Resources.\n"));
		iConsole.Printf(_L("k       - Specify supported data packets.\n"));
		iConsole.Printf(_L("\n"));
		
		iConsole.Printf(_L("h       - Display Event History.\n"));
		}

	if(iConsoleApp.NumberOfConnections())
		{
		iConsole.Printf(_L("\n[0..99] - Select a connection.\n"));
		}
	
	iConsole.Printf(_L("\nx       - Exit.\n"));
	}

TInt XInputMainMenu::ProcessKey(TChar aKey)
	{
	TInt ret(EMenuContinue);
	TRAPD(err, ret = ProcessKeyL(aKey));
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error %d when processing key press."), err);
		}
	return ret;
	}

TInt XInputMainMenu::ProcessKeyL(TChar aKey)
	{			
	TInt rerr = KErrNone;
	TBool redraw = ETrue;

	if(aKey.IsDigit())
		{
		redraw = SelectConnection(aKey.GetNumericValue());
		}
	else
		{
		switch(aKey)
			{
			case 'l':
					{
					TInputBTConnDetails* oldDetails = iLastListenDetails;
					iLastListenDetails = new (ELeave) TInputBTConnDetails((
														TInputBTConnDetails::EPortNumber | 
														TInputBTConnDetails::EProtocol | 
														TInputBTConnDetails::ESecurity |
														TInputBTConnDetails::EL2CAPConfig), 
														*this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastListenDetails; 
				iCurrentSubMenu = EListen;
				redraw = EFalse;
				break;
				
			case '*':
					{
					TInputBTConnDetails* oldDetails = iLastLegacyListenDetails;
					iLastLegacyListenDetails = new (ELeave) TInputBTConnDetails((
														TInputBTConnDetails::EPortNumber | 
														TInputBTConnDetails::EProtocol | 
														TInputBTConnDetails::ELegacySecurity |
														TInputBTConnDetails::EL2CAPConfig), 
														*this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastLegacyListenDetails;
				iCurrentSubMenu = EListen;
				redraw = EFalse;
				break;

			case 'a':
				rerr = iConsoleApp.Accept(iSelectedConnection);
				break;

			case 'c':
					{
					TInputBTConnDetails* oldDetails = iLastConnectDetails;
					iLastConnectDetails = new (ELeave) TInputBTConnDetails((
														TInputBTConnDetails::EPortNumber | 
														TInputBTConnDetails::EProtocol | 
														TInputBTConnDetails::EAddress |
														TInputBTConnDetails::ESecurity |
														TInputBTConnDetails::EL2CAPConfig), 
														*this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastConnectDetails; 
				iCurrentSubMenu = EConnect;
				redraw = EFalse;
				break;
			
			case '^':
				{
				TInputBTConnDetails* oldDetails = iLastLegacyConnectDetails;
				iLastLegacyConnectDetails = new (ELeave) TInputBTConnDetails((
													TInputBTConnDetails::EPortNumber | 
													TInputBTConnDetails::EProtocol | 
													TInputBTConnDetails::EAddress |
													TInputBTConnDetails::ELegacySecurity |
													TInputBTConnDetails::EL2CAPConfig), 
													*this, iCb, iConsole, oldDetails);
				delete oldDetails;
				}
				iSubMenu = iLastLegacyConnectDetails; 
				iCurrentSubMenu = EConnect;
				redraw = EFalse;
				break;

			case 'b':
					{
					TInputBTConnDetails* oldDetails = iLastBapDetails;
					iLastBapDetails = new (ELeave) TInputBTConnDetails(
													TInputBTConnDetails::EAddress, 
													*this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastBapDetails;
				iCurrentSubMenu = EBAPConnect;
				redraw = EFalse;
				break;

			case 'i':
				iSubMenu = new (ELeave) TInputAPIL2CapConfig(*this, iCb, iConsole);
				iSubMenu->Draw();
				iCurrentSubMenu = EL2CapAPIConfig;
				redraw = EFalse;
				break;

			case 'I':
				// Use IOCTL to configure link.
				rerr = iConsoleApp.ConfigureLink(ETrue);
				break;
	
			case 'U':
				// Use SetOpt to configure link.
				rerr = iConsoleApp.ConfigureLink(EFalse);
				break;
			
			case 'G':
#ifdef _DEBUG				
				iSubMenu = new (ELeave) TDebugMenu(*this, iCb, iConsole);
				iSubMenu->Draw();
				iCurrentSubMenu = EDebugMenu;
#else
				iConsole.Printf(_L("This menu is only available in UDEB builds."));
#endif				
				redraw = EFalse;
				break;

			case 'N':
				rerr = iConsoleApp.SetHostName();
				break;
			
			case 'y':
				rerr = iConsoleApp.ReadLocalOobData();
				redraw = EFalse;
				break;
			case 't':
				iConsoleApp.RefreshLocalOobData();
				redraw = EFalse;
				break;
			case 'z':
				rerr = iConsoleApp.ProvideRemoteOobData();
				redraw = EFalse;
				break;
			case 'u':
				rerr = iConsoleApp.ProvideRemoteOobDataFromRNR();
				redraw = EFalse;
				break;
			case '!':
				rerr = iConsoleApp.DedicatedBond();
				redraw = EFalse;
				break;
			case 'g':
				if(!iConsoleApp.ConnInState(iSelectedConnection, TConnProfile::EConnConnected))
					{
					iConsole.Printf(_L("Please select a valid connection."));
					rerr = KErrNotReady;
					}
				else
					{
					rerr = iConsoleApp.GetRemoteHostName(iSelectedConnection);
					}
				redraw = EFalse;
				break;
	

			case 'B':
				iConsoleApp.SetScanMode(EInquiryAndPageScan);
				redraw = EFalse;
				break;

			case 'Q':
				iConsoleApp.SetScanMode(EPageScanOnly);
				redraw = EFalse;
				break;

			case 'J':
				iConsoleApp.SetDiscoverability(ETrue);
				redraw = EFalse;
				break;

			case 'Y':
				iConsoleApp.SetDiscoverability(EFalse);
				redraw = EFalse;
				break;
			
			case '$':
				iConsoleApp.SetPairedConnectionsOnly(ETrue);
				redraw = EFalse;
				break;
				
			case '&':
				iConsoleApp.SetPairedConnectionsOnly(EFalse);
				redraw = EFalse;
				break;
				
			case '%':
				iConsoleApp.SetDebugMode();
				redraw = EFalse;
				break;

			case 'L':
				if(!iConsoleApp.ConnInState(iSelectedConnection, TConnProfile::EConnConnected))
					{
					iConsole.Printf(_L("Please select a valid connection."));
					rerr = KErrNotReady;
					}
				else
					{
					iSubMenu = new TDataMenu(*this, iCb, iConsole);
					iSubMenu->Draw();
					iCurrentSubMenu = EDataMenu;
					}
				redraw = EFalse;
				break;

			case 'r':
				iSubMenu = new (ELeave) TRFCommMenu(*this, iCb, iConsole);
				iSubMenu->Draw();
				iCurrentSubMenu = ERFCommMenu;
				redraw = EFalse;
				break;

			case 'p':
				iSubMenu = new (ELeave) TPhysicalLinkMenu(*this, iCb, iConsole);
				iSubMenu->Draw();
				iCurrentSubMenu = EPhysicalLinkMenu;
				redraw = EFalse;
				break;
			
			case 'd':
				rerr = iConsoleApp.ShutdownSocket(RSocket::ENormal);
				break;

			case 'f':
				rerr = iConsoleApp.ShutdownSocket(RSocket::EImmediate);
				break;

			case 'D':
				rerr = iConsoleApp.CloseAndDelete();
				break;
				
			case 'k':
				if(!iConsoleApp.ConnInState(iSelectedConnection, TConnProfile::EConnConnected))
					{
					iConsole.Printf(_L("Please select a valid connection."));
					rerr = KErrNotReady;
					}
				else 
					{
					iSubMenu = new (ELeave) TInputPacketTypes(*this, iCb, iConsole); 
					iSubMenu->Draw();
					iCurrentSubMenu = EPacketTypes;
					redraw = EFalse;
					}
				break;

			case 'h':
				if(!iConsoleApp.IsActiveConnection(iSelectedConnection))
					{
					iConsole.Printf(_L("Please select a valid connection."));
					rerr = KErrGeneral;
					}
				else 
					{
					iSubMenu = new (ELeave) TInputListEvents(*(iConsoleApp.GetConn(iSelectedConnection)), *this, iCb, iConsole); 
					iSubMenu->Draw();
					iCurrentSubMenu = EListEvents;
					redraw = EFalse;
					}
				break;

			case 'R':
				rerr = iConsoleApp.RegisterSDPRecordL();
				break;
				
			case 'o':
				rerr = iConsoleApp.PassiveESCO();
				break;
				
			case 'O':
					{
					TInputBTConnDetails* oldDetails = iLastEScoDetails;
					iLastEScoDetails = new (ELeave) TInputBTConnDetails(TInputBTConnDetails::EAddress, 
					                               *this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastEScoDetails;
				iCurrentSubMenu = EeSCOConnect;
				redraw = EFalse;
				break;
						
			case 's':
				rerr = iConsoleApp.PassiveSCO();
				break;
			case 'S':
					{
					TInputBTConnDetails* oldDetails = iLastScoDetails;
					iLastScoDetails = new (ELeave) TInputBTConnDetails(TInputBTConnDetails::EAddress, 
					                               *this, iCb, iConsole, oldDetails);
					delete oldDetails;
					}
				iSubMenu = iLastScoDetails;
				iCurrentSubMenu = ESCOConnect;
				redraw = EFalse;
				break;

			case 'e':
				iSubMenu = new (ELeave) TInputEchoRequest(*this, iCb, iConsole); 
				iSubMenu->Draw();
				iCurrentSubMenu = EEchoRequest;
				redraw = EFalse;
				break;
				
			case 'Z':
				iConsoleApp.CleanAllResources();
				break;

			case 'C':
				iSubMenu = new THciServerDetails(*this, iCb, iConsole); 
				iSubMenu->Draw();
				iCurrentSubMenu = EHciServer;
				redraw = EFalse;
				break;

			case 'W':
				rerr = iConsoleApp.AllowEDRPackets(EFalse);
				break;
				
			case 'w':
				rerr = iConsoleApp.AllowEDRPackets(ETrue);
				break;
			
			case 'x':
				return EMenuComplete;
			};
		}

	if(redraw && rerr == KErrNone)
		{
		iConsoleApp.Draw();
		}
	if(rerr == KErrDisconnected)
		{
		// Place the associated connection in a disconnected state.
		iConsoleApp.ServerDisconnect(iSelectedConnection);
		iConsoleApp.Draw();
		}

	return EMenuContinue;
	}

void XInputMainMenu::ProcessChildMenuClosed()
	{
	TBool redraw = ETrue;
	switch(iCurrentSubMenu)
		{
		case ENone:
			break;

		case EEchoRequest:
			{
			TInputEchoRequest* connDetails = static_cast<TInputEchoRequest*>(iSubMenu);
			iConsoleApp.SendEchoRequest(connDetails->GetEchoRequestLength());
			}
			break;

		case EConnect: 
			{
			TInputBTConnDetails* connDetails = static_cast<TInputBTConnDetails*>(iSubMenu);
			iConsoleApp.Connect(connDetails->GetAddress(), connDetails->GetPort(), connDetails->GetProtocol(), connDetails->GetSecurity(), connDetails->UseDefaultConfig());
			}
			break;

		case EListen:
			{
			TInputBTConnDetails* connDetails = static_cast<TInputBTConnDetails*>(iSubMenu);
			iConsoleApp.Listen(connDetails->GetPort(), connDetails->GetProtocol(), connDetails->GetSecurity(), connDetails->UseDefaultConfig());
			}
			break;
			
		case EBAPConnect:
			{
			TInputBTConnDetails* connDetails = static_cast<TInputBTConnDetails*>(iSubMenu);
			iConsoleApp.BAPConnect(connDetails->GetAddress());
			}
			break;

		case ESCOConnect:
			{
			TInputBTConnDetails* connDetails = static_cast<TInputBTConnDetails*>(iSubMenu);
			iConsoleApp.ActiveSCO(connDetails->GetAddress());
			}
			break;
			
		case EeSCOConnect:
			{
			TInputBTConnDetails* connDetails = static_cast<TInputBTConnDetails*>(iSubMenu);
			iConsoleApp.ActiveESCO(connDetails->GetAddress());
			}
			break;

		case EPacketTypes:
			{
			TInputPacketTypes* packetTypes = static_cast<TInputPacketTypes*>(iSubMenu);
			iConsoleApp.ChangePacketTypes(packetTypes->GetPacketTypes());
			}
			break;

		case EL2CapAPIConfig:
			{
			TInputAPIL2CapConfig* config = static_cast<TInputAPIL2CapConfig*>(iSubMenu);
			iConsoleApp.SetL2CapAPIConfig(config->GetL2CapConfig());
			}
			break;
		
		case EDebugMenu:
			{
#ifdef _DEBUG
			TSubMenu* subMenu = static_cast<TSubMenu*>(iSubMenu);
			redraw = iConsoleApp.DebugMenu(static_cast<TDebugMenu::TDebugMenuType>(subMenu->GetInfoType()), subMenu->GetBasicValue());
#endif
			}
			break;

		case EDataMenu:
			{
			TSubMenu* subMenu = static_cast<TSubMenu*>(iSubMenu);
			redraw = iConsoleApp.DataMenu(static_cast<TDataMenu::TDataMenuType>(subMenu->GetInfoType()), subMenu->GetBasicValue());
			}
			break;

		case ERFCommMenu:
			{
			TSubMenu* subMenu = static_cast<TSubMenu*>(iSubMenu);
			redraw = iConsoleApp.RFCommMenu(static_cast<TRFCommMenu::TRFCommMenuType>(subMenu->GetInfoType()), subMenu->GetBasicValue());
			}
			break;

		case EPhysicalLinkMenu:
			{
			TSubMenu* subMenu = static_cast<TSubMenu*>(iSubMenu);
			redraw = iConsoleApp.PhysicalLinkMenu(static_cast<TPhysicalLinkMenu::TPhysicalLinkMenuType>(subMenu->GetInfoType()), subMenu->GetBasicValue());
			}
			break;
		
		case EListEvents:
			break;
		};
	iCurrentSubMenu = ENone;
	DeleteSubMenuIfAppropriate();
	if(redraw)
		{
		iConsoleApp.Draw();
		}
	}

TBool XInputMainMenu::SelectConnection(TInt aConnIx)
	{
	TBool rCode = EFalse;
	if(aConnIx != iSelectedConnection)
		{
		if(!iConsoleApp.IsActiveConnection(aConnIx))
			{
			iConsole.Printf(_L("Please select a valid connection."));
			}
		else 
			{
			iSelectedConnection = aConnIx;
			rCode = ETrue;
			}
		}
	return rCode;
	}

TInputEchoRequest::TInputEchoRequest(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TInputProcessorBase(aParent, aCb, aConsole)
	{
	// Nothing to do but pass data up to the superclass
	}

void TInputEchoRequest::Draw()
	{
	iConsole.Printf(_L("\nEnter Echo Request Length:  "));
	}

TInt TInputEchoRequest::ProcessKey(TChar aKey)
	{
	TInt rerr = KErrNone;
	
	// Handle delete
	if(aKey == 8 && iBuffer.Length() > 0)
		{
		iBuffer.RemoveChar();
		TPoint cur = iConsole.CursorPos();
		cur.iX--;
		iConsole.SetCursorPosAbs(cur);
		iConsole.ClearToEndOfLine();
		return KErrNone;
		}
		
	// Process input
	if(iBuffer.AddNumber(aKey, 2))
		{
		iConsole.Printf(_L("%c"), TUint(aKey));
		}
	
	// We have our number
	if ( (aKey == 13 || aKey==32) && iBuffer.Length() > 0)
		{
		TInt64 length;
		iBuffer.DecGetValue(length);
		iEchoRequestLength = length;		
		iBuffer.Reset();
		Draw();
		return EMenuComplete;
		// HERE
		}
		
	return(rerr);
	}

TInputBTConnDetails::TInputBTConnDetails(TUint aRequiredDetails, TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole, TInputBTConnDetails* aPreviousDetails)
: TInputProcessorBase(aParent, aCb, aConsole),
  iUseDefaultConfig(ETrue), iUseLegacySecurity(EFalse), iUsePreviousDetails(EFalse), iCurrentDetail(0), iCurrentSecuritySetting(0)
	{
	iSecurity.SetAuthentication(EFalse);
	iSecurity.SetEncryption(EFalse);
	iSecurity.SetAuthorisation(EFalse);
	iSecurity.SetDenied(EFalse);
	iSecurity.SetPasskeyMinLength(0);

	// Get the details from the previous menu before it is deleted.
	if (aPreviousDetails)
		{
		iUsePreviousDetails = ETrue;
		iPreviousProtocol = aPreviousDetails->GetProtocol();
		iPreviousPort = aPreviousDetails->GetPort();
		iPreviousAddress = aPreviousDetails->GetAddress().Des();
		iPreviousSecurity = aPreviousDetails->GetSecurity();
		iPreviousUseDefaultConfig = aPreviousDetails->UseDefaultConfig();
		}
	
	iRequiredDetails[iCurrentDetail++] = EDone;

	if(aRequiredDetails & EPortNumber)
		{
		iRequiredDetails[iCurrentDetail++] = EPortNumber;
		}
	if(aRequiredDetails & EProtocol)
		{
		iRequiredDetails[iCurrentDetail++] = EProtocol;
		}
	if(aRequiredDetails & EAddress)
		{
		iRequiredDetails[iCurrentDetail++] = EAddress;
		}
	if(aRequiredDetails & ESecurity)
		{
		iRequiredDetails[iCurrentDetail++] = ESecurity;
		}
	else if(aRequiredDetails & ELegacySecurity)
		{
		iUseLegacySecurity = ETrue;
		iRequiredDetails[iCurrentDetail++] = ELegacySecurity;
		}
	if(aRequiredDetails & EL2CAPConfig)
		{
		iRequiredDetails[iCurrentDetail++] = EL2CAPConfig;
		}
	NextInput();
	}
	
_LIT(KPreviousDetailDefault, "(%c)");
const TUint KPreviousDetailDefaultLength = 3; // as it is two brackets and one character

void TInputBTConnDetails::Draw()
	{
	switch(iRequiredDetails[iCurrentDetail])
		{
		case EPortNumber:
			iConsole.Printf(_L("\nEnter Port Number:  0x"));
			break;
		case EProtocol:
			iConsole.Printf(_L("\nChoose a protocol.  r - RFCOMM, l - L2CAP. "));
			if (iUsePreviousDetails)
				{
				iConsole.Printf(KPreviousDetailDefault(), TUint(iPreviousProtocol));
				}
			break;
		case EAddress:
			iConsole.Printf(_L("\nEnter Bluetooth Address:  0x"));
			break;
		case ELegacySecurity:
			// fall-through
		case ESecurity:
			iConsole.Printf(_L("\nSecurity:  Mitm  Conf  Encr  Diss  PassKey "));
			iConsole.Printf(_L("\n             "));
			PrintSecurityDefault();
			break;
		case EL2CAPConfig:
			iConsole.Printf(_L("\nUse Default L2Cap Config [y/n]. "));
			if (iUsePreviousDetails)
				{
				TUint yorn = (TUint)(iPreviousUseDefaultConfig ? 'y' : 'n');
				iConsole.Printf(KPreviousDetailDefault(), yorn);
				}
			break;
			
		default:
			break;
		};
	}

TInt TInputBTConnDetails::ProcessKey(TChar aKey)
	{
	TInt rerr = KErrNone;

	switch(iRequiredDetails[iCurrentDetail])
		{
		case EPortNumber:
			rerr = CollectPortNumber(aKey);
			break;
		case EProtocol:
			rerr = CollectProtocol(aKey);
			break;
		case EAddress:
			rerr = CollectAddress(aKey);
			break;
		case ESecurity:
		case ELegacySecurity:
			rerr = CollectSecurity(aKey);
			break;
		case EL2CAPConfig:
			rerr = CollectConfig(aKey);
			break;
		default:
			break;
		};
	if(rerr != KErrNone || iRequiredDetails[iCurrentDetail] == EDone)
		{
		return EMenuComplete;
		}
	else 
		{
		return EMenuContinue;
		}
	}
	
void TInputBTConnDetails::SetupAnyPreviousValues()
	{
	if (!iUsePreviousDetails)
		{
		return;
		}
	switch(iRequiredDetails[iCurrentDetail])
		{
		case EPortNumber:
			iBuffer.WriteIntToBuffer(iPreviousPort);
			iBuffer.PrintBuffer(iConsole);
			break;
		case EProtocol:
			// already setup, as we directly access member variable.
			break;
		case EAddress:
			iBuffer.WriteTDesC8ToBuffer(iPreviousAddress.Des());
			iBuffer.PrintBuffer(iConsole);
			break;
		case ESecurity:
			// initial entry is available as a member variable,
			// the buffer usage is set-up within the ESecurity "state"
			break;
		case EL2CAPConfig:
			// already setup, as we directly access member variable.
			break;
		default:
			break;
		};
	}

TInt TInputBTConnDetails::NextInput()
	{
	iBuffer.Reset();
	iCurrentDetail--;
	Draw();
	SetupAnyPreviousValues();
	return KErrNone;
	}

TInt TInputBTConnDetails::CollectPortNumber(TChar aKey)
	{
	if (HandleBackspace(aKey))
		{
		return KErrNone;
		}

	if(!iBuffer.AddHexNumber(aKey, 4))
		{
		TInt64 bufferValue;
		if(!iBuffer.HexGetValue(bufferValue))
			{
			iConsole.Printf(_L("\nInvalid port number."));
			Draw();
			return KErrNone;
			}
		else
			{
			iPort = *((TUint16*)(&bufferValue));
			return NextInput();
			}
		}
	iConsole.Printf(_L("%c"), TUint(aKey));
	return KErrNone;
	}


TInt TInputBTConnDetails::CollectProtocol(TChar aKey)
	{
	aKey.LowerCase();
	// here we allow the user to press l or r to explicitly set the
	// protocol to use.  If a previous detail is available the user
	// need only press return to select that.
	if (aKey != 'l' && aKey != 'r' && ((aKey != '\r' && aKey != ' ') || !iUsePreviousDetails))
		{
		Draw();
		}
	else
		{
		if (iUsePreviousDetails)
			{
			// the previous character has been printed
			// so we need to override it if we type
			// another valid character.
			ConsoleBackspace(KPreviousDetailDefaultLength);
			}
		if (aKey == '\r' || aKey == ' ')
			{
			iProtocol = iPreviousProtocol;
			}
		else
			{
			iProtocol = aKey;
			}
		iConsole.Printf(_L(" %c"), TUint(iProtocol));
		NextInput();
		}
	return KErrNone;
	}

TInt TInputBTConnDetails::CollectConfig(TChar aKey)
	{
	aKey.LowerCase();
	if (aKey != 'y' && aKey != 'n' && ((aKey != '\r' && aKey != ' ') || !iUsePreviousDetails))
		{
		Draw();
		}
	else
		{
		if (iUsePreviousDetails)
			{
			// the previous character has been printed
			// so we need to override it if we type
			// another valid character.
			ConsoleBackspace(KPreviousDetailDefaultLength);
			}
		if (aKey == '\r' || aKey == ' ')
			{
			aKey = iUseDefaultConfig ? 'y' : 'n';
			}
		
		iUseDefaultConfig = ((aKey == 'y') ? ETrue : EFalse);
		iConsole.Printf(_L(" %c"), TUint(aKey));
		NextInput();
		}
	return KErrNone;
	}

TInt TInputBTConnDetails::CollectAddress(TChar aKey)
	{
	if (HandleBackspace(aKey))
		{
		return KErrNone;
		}

	if(!iBuffer.AddHexNumber(aKey, 12))
		{
		TInt64 addr;
		if(!iBuffer.HexGetValue(addr))
			{
			iConsole.Printf(_L("\nInvalid BT Address."));
			Draw();
			return KErrNone;
			}
		else
			{
			iAddress = TBTDevAddr(addr);
			return NextInput();
			}
		}

	iConsole.Printf(_L("%c"), TUint(aKey));
	return KErrNone;
	}

TInt TInputBTConnDetails::CollectSecurity(TChar aKey)
	{
	// Handle the Min Passkey Length in a separate function
	if (iCurrentSecuritySetting == EPasskeyMinLength)
		{
		return CollectPasskeyMinLength(aKey);
		}
	
	// Handle the backspace in the list of options
	if(aKey == '\b' && iCurrentSecuritySetting != EAuthenticate)
		{
		iCurrentSecuritySetting--;
		if (iUsePreviousDetails)
			{
			// delete the default display and the space padding
			ConsoleBackspace(KPreviousDetailDefaultLength+5);
			}
		else
			{
			// delete the single character entry and space padding
			ConsoleBackspace(6); 
			}
		PrintSecurityDefault();
		return KErrNone;
		}
	
	// This will handle the selection to use the default value
	if ((aKey == '\r' || aKey == ' ') && iUsePreviousDetails)
		{
		aKey = GetSecurityDefault();
		}

	// Check a valid key has been entered.
	TBool validKey = EFalse;
	switch(iCurrentSecuritySetting)
		{
	case EAuthenticate:
		if(!iUseLegacySecurity)
			{
			// If using MITM protection then required/desired/not-required
			validKey = (aKey == 'r' || aKey == 'd' || aKey == 'n');
			break;
			}
		// else fallthrough
	default:
		// default is yes and no.
		validKey = (aKey == 'y' || aKey == 'n');
		break;
		}

	// This handles the actual selection
	if (validKey)
		{
		if (iUsePreviousDetails)
			{
			// this is to remove the default char display
			ConsoleBackspace(KPreviousDetailDefaultLength); // delete the default display
			iConsole.Printf(_L(" ")); // add the space which was removed to centre the default
			}
		iConsole.Printf(_L("%c     "), TUint(aKey));
		if(UpdateSecurity(aKey))
			{
			// Settings collection complete.
			return NextInput();
			}
		else
			{
			PrintSecurityDefault();
			}
		}
	return KErrNone;
	}
	
TInt TInputBTConnDetails::CollectPasskeyMinLength(TChar aKey)
	{
	if(HandleBackspace(aKey))
		{
		return KErrNone;
		}
	
	if ((aKey == '\r' || aKey == ' ') && iBuffer.Length() > 0)
		{
		//enter data
		TInt64 val64;
		iBuffer.DecGetValue(val64);
		TUint val = (TUint)val64;
		if (val>16)
			{
			val=0;
			}
		iSecurity.SetPasskeyMinLength(val);
		iBuffer.Reset();
		return NextInput();
		}
	
	if(iBuffer.AddNumber(aKey, 2))
		{
		iConsole.Printf(_L("%c"), TUint(aKey));
		}
	return KErrNone;
	}

TChar ToYesOrNo(TBool aValue)
	{
	return (aValue) ? 'y' : 'n';
	}

TChar ToMitmValue(TBluetoothMitmProtection aValue)
	{
	switch(aValue)
		{
	case EMitmNotRequired:
		return 'n';
	case EMitmDesired:
		return 'd';
	case EMitmRequired:
		return 'r';
	default:
		ASSERT_DEBUG(EFalse);
		return 'x';
		}
	}


TChar TInputBTConnDetails::GetSecurityDefault()
	{
	TChar ret('x');
	switch (iCurrentSecuritySetting)
		{
		case EAuthorised:
			ret = ToYesOrNo(iPreviousSecurity.AuthorisationRequired());
			break;
		case EAuthenticate:
			if(iUseLegacySecurity)
				{
				ret = ToYesOrNo(iPreviousSecurity.AuthenticationRequired());
				}
			else
				{
				ret = ToMitmValue(iPreviousSecurity.MitmProtection());
				}
			break;
		case EEncrypt:
			ret = ToYesOrNo(iPreviousSecurity.EncryptionRequired());
			break;
		case EDenied:
			ret = ToYesOrNo(iPreviousSecurity.Denied());
			break;
		default:
			ASSERT_DEBUG(EFalse);
			break;
		};
	return ret;
	}
	
void TInputBTConnDetails::PrintSecurityDefault()
	{
	if (iUsePreviousDetails && iCurrentSecuritySetting != EPasskeyMinLength) // don't do default for min pin.
		{
		TUint chr = GetSecurityDefault();
		ConsoleBackspace(1); // to keep the char central.
		iConsole.Printf(KPreviousDetailDefault(), chr);
		}
	}

TBool YesOrNo(TChar aKey)
	{
	return (aKey == 'y');
	}

TBluetoothMitmProtection MitmValue(TChar aKey)
	{
	switch(aKey)
		{
	case 'n':
		return EMitmNotRequired;
	case 'd':
		return EMitmDesired;
	case 'r':
		return EMitmRequired;
	default:
		ASSERT_DEBUG(EFalse);
		return (TBluetoothMitmProtection)(0xff);
		}
	}

TBool TInputBTConnDetails::UpdateSecurity(TChar aSetting)
	{
	switch(iCurrentSecuritySetting)
		{
		case EAuthorised:
			iSecurity.SetAuthorisation(YesOrNo(aSetting));
			iCurrentSecuritySetting++;
			break;
		case EAuthenticate:
			if(iUseLegacySecurity)
				{
				iSecurity.SetAuthentication(YesOrNo(aSetting));
				}
			else
				{
				iSecurity.SetAuthentication(MitmValue(aSetting));
				}
			iCurrentSecuritySetting++;
			break;
		case EEncrypt:
			iSecurity.SetEncryption(YesOrNo(aSetting));
			iCurrentSecuritySetting++;
			break;
		case EDenied:
			iSecurity.SetDenied(YesOrNo(aSetting));
			// Get Min PassKey len
			iCurrentSecuritySetting++;
			if (iUsePreviousDetails)
				{
				iBuffer.WriteIntToBuffer(iPreviousSecurity.PasskeyMinLength());
				iBuffer.PrintBuffer(iConsole);
				}
			break;
		};
	return EFalse;
	}
	
TBool TInputBTConnDetails::HandleBackspace(TChar aKey)
	{
	TBool ret(EFalse);
	if (aKey == '\b' && iBuffer.Length() > 0)
		{
		iBuffer.RemoveChar();
		ConsoleBackspace(1);
		ret = ETrue;
		}
	return ret;
	}

void TInputBTConnDetails::ConsoleBackspace(TUint aNumberOfChars)
	{
	TPoint cur = iConsole.CursorPos();
	cur.iX -= aNumberOfChars;
	iConsole.SetCursorPosAbs(cur);
	iConsole.ClearToEndOfLine();
	}


THciServerDetails::THciServerDetails(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TInputProcessorBase(aParent, aCb, aConsole)
	{
	iPowerControl.Open();
	iHCIDirectAccess.Open();
	}

THciServerDetails::~THciServerDetails()
	{
	iPowerControl.Close();
	iHCIDirectAccess.Close();
	}

void THciServerDetails::FormatVendorCmd(TDes8& aCmd)
	{
	aCmd.SetLength(0);
	
	// opcode
	aCmd.Append(0x24);
	aCmd.Append(0xff);

	// parameter length
	aCmd.Append(0x00);
	}
		
void THciServerDetails::Draw()
	{
	iConsole.Printf(_L("\ng		- Get controller power state\n"));
	iConsole.Printf(_L("s		- Set controller power state on\n"));
	iConsole.Printf(_L("o		- Set controller power state off\n"));
	iConsole.Printf(_L("\nd		- Send Vendor Specific Command (Only works with TI hardware)\n"));
	}

TInt THciServerDetails::ProcessKey(TChar aKey)
	{
	TInt ret = EMenuContinue;
	TBTPowerState state(EBTOn);
	
	switch(aKey)
		{
	case 'g':
		{
		TBTPowerState powerState;

		TInt err = iPowerControl.GetPower(powerState, NULL);
		if (err == KErrNone)
			{
			if (powerState == EBTOn)
				{
				iConsole.Printf(_L("\nController power state is on\n"));
				}
			else
				{
				iConsole.Printf(_L("\nController power state is off\n"));
				}
			}
		else
			{
			iConsole.Printf(_L("\nError getting controller power state: %d\n"), err);
			}
		}
		break;
	
	case 'o':
		state = EBTOff; // fallthrough
	case 's':
		{
		TRequestStatus request;
		
		iPowerControl.SetPower(state, NULL, request);

		iConsole.Printf(_L("\nWaiting for Set Power to complete..."));

		User::WaitForRequest(request);
		
		iConsole.Printf(_L("\nSetting controller power state returned %d"), request.Int());
		}
		break;
		
	case 'd':
		{
		TRequestStatus request;

		FormatVendorCmd(iVendorCommand);
		iHCIDirectAccess.AsyncMessage(KHCIDirectAccessVendorCommand,
									// Need to choose a bit pattern
									// that results in a hardware
									// response
									request, &iVendorCommand);

		iConsole.Printf(_L("\nWaiting for Vendor Command to complete..."));

		// This will hang until we get the the opcode right
		User::WaitForRequest(request);

		iConsole.Printf(_L("\nVendor Debug Command completed with status %d"), request.Int());
		}
		break;

	default:
		ret = EMenuComplete;
		}
	
	return(ret);
	}

TInputListEvents::TInputListEvents(CBTTestConnection& aConn, TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TInputProcessorBase(aParent, aCb, aConsole),
  iConn(aConn),
  iCurrentEvent(0),
  iComplete(EFalse)
	{
	}

void TInputListEvents::Draw()
	{
	iConsole.ClearScreen();
	iComplete = iConn.PrintEvents(iCurrentEvent, EEventsPerPage);
	if(iComplete)
		{
		iConsole.Printf(_L("\n\n\nEvent Listing Complete."));
		}
	else
		{
		iConsole.Printf(_L("\n\n\nPress 'c' to view more events."));
		iCurrentEvent += EEventsPerPage;
		}
	iConsole.Printf(_L("\nPress 'd' to clear event list."));
	iConsole.Printf(_L("\nPress 'e' to exit."));
	}

TInt TInputListEvents::ProcessKey(TChar aKey)
	{
	TBool done = EFalse;
	switch(aKey)
		{
		case 'c':
			if(aKey == 'c' && !iComplete)
				{
				Draw();
				}
			break;

		case 'd':
			iConn.ClearEventStore();
			done = ETrue;
			break;

		case 'e':
			done = ETrue;
			break;

		default:
			break;
		};

	if(done)
		{
		return EMenuComplete;
		}
	else 
		{
		return EMenuContinue;
		}
	}



TSubMenu::TSubMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TInputProcessorBase(aParent, aCb, aConsole),
  iInfoType(0),
  iState(EBasicMenu),
  iValue(0)
	{
	}

TInt TSubMenu::ProcessKey(TChar aKey)
	{
	TInt rcode = EMenuComplete;
	switch(iState)
		{
		case EBasicMenu:
			rcode = SubMenuProcessKey(aKey);
			// Must be requesting a numeric value from the interface.
			if(rcode == EMenuContinue)
				{
				iConsole.Printf(_L("\nEnter Value: "));
				iState = EGetBasicValue;
				}
			// Must be requesting a choice from the interface.
			else if(rcode == EChoiceValue)
				{
				iConsole.Printf(_L("\n\nDo you want to check the data continuation? [y/n].\n"));
				iState = EGetChoiceValue;				
				}
			break;
		case EGetBasicValue:
			if(!iBuffer.AddHexNumber(aKey, 4))
				{
				TInt64 val;
				iBuffer.HexGetValue(val);
				iValue = (*((TUint16*)(&val)));
				iBuffer.Reset();
				}
			else
				{
				iConsole.Printf(_L("%c"), TUint(aKey));
				rcode = EMenuContinue;
				}
			break;
		//Getting the choice value
		case EGetChoiceValue:
			iConsole.Printf(_L("%c"), TUint(aKey));
			aKey.LowerCase();
			//menu will keep asking for right choice input 
			if(aKey != 'y' && aKey != 'n')
				{
				iConsole.Printf(_L("\n\nDo you want to check the data continuation? [y/n].\n"));
				rcode = EMenuContinue;
				}
			else																			 
				{
				//menu continue,will run RecvContinuationData function after getting value for recv buffer size
				if(aKey == 'y')
					{
					// Must be requesting a value from the interface.
					iConsole.Printf(_L("\nEnter Value: "));
					iState = EGetBasicValue;
					iInfoType = TDataMenu::ERecvContinuationData;
					rcode = EMenuContinue;
					}
				//menu completed, will run RecvOneShotData function straightaway
				else
					{
					iState = EBasicMenu;
					iInfoType = TDataMenu::ERecvOneShotData;
					rcode = EMenuComplete;
					}
				}
			break;

		default:
			__DEBUGGER();  // TODO
			break;
		};

	return rcode;
	}

TUint16 TSubMenu::GetInfoType() const
	{
	return iInfoType;
	}
	
TUint16 TSubMenu::GetBasicValue() const
	{
	return iValue;
	}
	
// ----------------------------------------------------------------------------
//							DEBUG MENU
// ----------------------------------------------------------------------------
#ifdef _DEBUG
TDebugMenu::TDebugMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TSubMenu(aParent, aCb, aConsole)
	{
	iInfoType = EInvalidDebugType;
	}

TInt TDebugMenu::SubMenuProcessKey(TChar aKey)
	{
	TInt rcode = EMenuComplete;
	switch(aKey)
		{
		case 'a':
			iInfoType = EAllocations;
			break;

		case 'P':
			iInfoType = ESetPDUTimerCID;
			rcode = EMenuContinue;
			break;

		case 'p':
			iInfoType = EGetPDUTimers;
			break;

		case 'f':
			iInfoType = EGetFlushCounters;
			break;

		case 'F':
			iInfoType = EIssueManualFlush;
			break;
			
		case 'd':
			iInfoType = EGetCurrentDataIndex;
			break;
				
		case 'm':
			iInfoType = ECurrentMemoryInfo;
			break;
			
		case '<':
			iInfoType = EDebugMark;
			break;

		case '>':
			iInfoType = EDebugMarkEnd;
			break;

		case 'C':
			iInfoType = EDataPlaneConfig;
			break;

		default:
			break;			
		};
	return rcode;
	}

void TDebugMenu::Draw()
	{
	iConsole.Printf(_L("\nEnter Debug Menu Option:"));
	iConsole.Printf(_L("\n a - Allcation Debug"));
	iConsole.Printf(_L("\n P - Set PDU Debug Timer CID"));
	iConsole.Printf(_L("\n p - PDU Timer Debug"));
	iConsole.Printf(_L("\n f - Flush Counter Debug"));
	iConsole.Printf(_L("\n F - Issue Manual Flush"));
	iConsole.Printf(_L("\n m - Display Current Memory Info."));
	iConsole.Printf(_L("\n < - Debug Mark."));
	iConsole.Printf(_L("\n > - Debug Mark End."));
	iConsole.Printf(_L("\n C - Data Plane Config."));
	iConsole.Printf(_L("\n d - Display Current Data Index"));

	iConsole.Printf(_L("\n\n  PRESS SPACE TO EXIT MENU."));
	}
#endif


// ----------------------------------------------------------------------------
//							DATA MENU
// ----------------------------------------------------------------------------
TDataMenu::TDataMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TSubMenu(aParent, aCb, aConsole)
	{
	iInfoType = EInvalidDataMenuType;
	}

TInt TDataMenu::SubMenuProcessKey(TChar aKey)
	{
	TInt rcode = EMenuComplete;
	switch(aKey)
		{
		case 't':
			iInfoType = ESetIncomingMTU;
			rcode = EMenuContinue;
			break;

		case 'T':
			iInfoType = ESetOutgoingMTU;
			rcode = EMenuContinue;
			break;

		case 's':
			iInfoType = ESendOneShotData;
			rcode = EMenuContinue;
			break;
		
		case 'r':
			iInfoType = ERecvOneShotData;
			//will get a choice from interface
			rcode = EChoiceValue;
			break;

		case 'R':
			iInfoType = EDataDlgRecv;
			break;

		case 'S':
			iInfoType = EDataDlgSend;
			break;

		case 'w':
			iInfoType = ESendData;
			break;

		case 'W':
			iInfoType = ERecvData;
			break;

		case 'N':
			iInfoType = ERecvAnyData;
			break;

		case 'M':
			iInfoType = ESimultanousSendReceiveData;
			break;

		case 'X':
			iInfoType = ERTXTimer;
			rcode = EMenuContinue;
			break;
			
		case 'E':
			iInfoType = EERTXTimer;
			rcode = EMenuContinue;
			break;

		case 'F':
			iInfoType = EPretendSduQFullOn;
			break;

		case 'f':
			iInfoType = EPretendSduQFullOff;
			break;

		case 'c':
			iInfoType = EGetChannelMode;
			break;

		case 'P':
			iInfoType = EGetLocalPSM;
			break;

		default:
			break;			
		};
	return rcode;
	}



void TDataMenu::Draw()
	{
	iConsole.Printf(_L("\nEnter Data Menu Option:"));
	iConsole.Printf(_L("\n t - Set Incoming MTU."));
	iConsole.Printf(_L("\n T - Set Outgoing MTU."));
	iConsole.Printf(_L("\n s - Send One Shot Data."));
	iConsole.Printf(_L("\n r - Recv One Shot Data."));
	iConsole.Printf(_L("\n S - Data Dialog - Initial Send."));
	iConsole.Printf(_L("\n R - Data Dialog - Initial Recv."));
	iConsole.Printf(_L("\n w - Send Data."));
	iConsole.Printf(_L("\n W - Recv Data, verify correctness - sender has to use the 'w - Send Data' option."));
	iConsole.Printf(_L("\n N - Recv Data /dev/null style, accept anything."));
	iConsole.Printf(_L("\n M - Simultaneous send/receive."));
	iConsole.Printf(_L("\n X - Set RTX  Timer Duration."));
	iConsole.Printf(_L("\n E - Set ERTX Timer Duration."));
	iConsole.Printf(_L("\n F - Pretend SDU Queue is full - On."));
	iConsole.Printf(_L("\n f - Pretend SDU Queue is full - Off."));
	iConsole.Printf(_L("\n c - Get negotiated L2CAP channel mode."));
	iConsole.Printf(_L("\n P - Get local PSM."));
	iConsole.Printf(_L("\n\n  PRESS SPACE TO EXIT MENU."));
	}


// ----------------------------------------------------------------------------
//							RFCOMM MENU
// ----------------------------------------------------------------------------
TRFCommMenu::TRFCommMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TSubMenu(aParent, aCb, aConsole)
	{
	iInfoType = EInvalidRFCommMenuType;
	}

TInt TRFCommMenu::SubMenuProcessKey(TChar aKey)
	{
	TInt rcode = EMenuComplete;
	switch(aKey)
		{
		case 'i':
			iInfoType = ERequestRFCommConfChgInd;
			break;

		case 'm':
			iInfoType = ESetLocalModemStatus;
			rcode = EMenuContinue;
			break;

		default:
			break;			
		};
	return rcode;
	}

void TRFCommMenu::Draw()
	{
	iConsole.Printf(_L("\nEnter RFCOMM Menu Option:"));
	iConsole.Printf(_L("\n i - Request config change indication."));
	iConsole.Printf(_L("\n m - Set local modem status."));

	iConsole.Printf(_L("\n\n  PRESS SPACE TO EXIT MENU."));
	}
	

// ----------------------------------------------------------------------------
//							PHYSICAL LINK MENU
// ----------------------------------------------------------------------------
TPhysicalLinkMenu::TPhysicalLinkMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole)
: TSubMenu(aParent, aCb, aConsole)
	{
	iInfoType = EInvalidPhysicalLinkMenuType;
	}

TInt TPhysicalLinkMenu::SubMenuProcessKey(TChar aKey)
	{
	TInt rcode = EMenuComplete;
	switch(aKey)
		{
		case 'n':
			iInfoType = EActiveRequest;
			break;

		case 's':
			iInfoType = ESniffRequest;
			break;

		case 'p':
			iInfoType = EParkRequest;
			break;

		case 'S':
			iInfoType = EToggleSniffModeAllowed;
			break;

		case 'P':
			iInfoType = EToggleParkModeAllowed;
			break;

		case 'H':
			iInfoType = EToggleHoldModeAllowed;
			break;

		case 'M':
			iInfoType = ERequestMasterRole;
			break;

		case 'm':
			iInfoType = ERequestSlaveRole;
			break;

		case 'R':
			iInfoType = EToggleRoleSwitchAllowed;
			break;

		case 'A':
			iInfoType = EAuthenticationRequest;
			break;

		case 'l':
			iInfoType = EPhysicalLinkMetricsDisplay;
			break;
		case 'L':
			iInfoType = EPhysicalLinkMetricsCancel;
			break;
		case 'v':
		case 'V':		
			iInfoType = EToggleLPMRequestsIfActive;
			break;

		case 'k':
			iInfoType = EKillLink;
			break;

		case 'K':
			iInfoType = EKillAllLinks;
			break;

		case 'e':
			iInfoType = EGetConnectedLinks;
			break;

		case '5':
			iInfoType = EToggleAutoSniff;
			break;
			
		case 'a':
			iInfoType = EExplicitActiveRequest;
			break;

		default:
			break;			
		};
	return rcode;
	}

void TPhysicalLinkMenu::Draw()
	{
	iConsole.Printf(_L("\nEnter Physical Link Menu Option:"));
	iConsole.Printf(_L("\nn, s, p, a - Request Active, Sniff, Park, Explicit Active Mode."));

	iConsole.Printf(_L("\nS,P,H,R - Toggle Disallow Sniff, Park, Hold, Role."));
	iConsole.Printf(_L("\nV - Toggle Low Power Requests Persist when Active."));
	iConsole.Printf(_L("\n5 - Toggle AutoSniff Mode with 5 sec timeout."));
	
	iConsole.Printf(_L("\nM, m	  - Change Role Master, Slave."));
	iConsole.Printf(_L("\nA		  - Request Authentication."));
	
	iConsole.Printf(_L("\n\nl       - Display link metrics"));
	iConsole.Printf(_L("\nL       - Cancel link metrics"));
	
	iConsole.Printf(_L("\n\nk       - Kill Selected Physical Link."));
	iConsole.Printf(_L("\nK       - Kill All Physical Links."));
	iConsole.Printf(_L("\ne       - Display Current BT Phys Connections."));

	iConsole.Printf(_L("\n\n  PRESS SPACE TO EXIT MENU."));
	}
	

// ----------------------------------------------------------------------------
//							PACKET TYPES MENU
// ----------------------------------------------------------------------------
TInputPacketTypes::TInputPacketTypes(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole)
: TInputProcessorBase(aParent, aCb, aConsole),
  iCurrentType(0)
	{
	}

TInt TInputPacketTypes::ProcessKey(TChar aKey)
	{
	if(aKey == 8 && iCurrentType > 0)
		{
		iCurrentType--;
		TPoint cur = iConsole.CursorPos();
		cur.iX -= 4;
		iConsole.SetCursorPosAbs(cur);
		iConsole.ClearToEndOfLine();
		return KErrNone;
		}

	if(aKey == 'y' || aKey == 'n')
		{
		iTypes[iCurrentType++] = (aKey == 'y');
		iConsole.Printf(_L("%c   "), TUint(aKey));
		}
	if(iCurrentType == 6)
		{
		return EMenuComplete;
		}
	else 
		{
		return EMenuContinue;
		}
	}

TUint16 TInputPacketTypes::GetPacketTypes()
	{
	TUint16 packetTypes = 0;
	if(iTypes[0])
		{
		packetTypes |= EPacketsDM1;
		}
	if(iTypes[1])
		{
		packetTypes |= EPacketsDM3;
		}
	if(iTypes[2])
		{
		packetTypes |= EPacketsDM5;
		}
	if(iTypes[3])
		{
		packetTypes |= EPacketsDH1;
		}
	if(iTypes[4])
		{
		packetTypes |= EPacketsDH3;
		}
	if(iTypes[5])
		{
		packetTypes |= EPacketsDH5;
		}
	return packetTypes;
	}	

void TInputPacketTypes::Draw()
	{
	iConsole.Printf(_L("\nSupport data type (y/n): DM1 DM3 DM5 DH1 DH3 DH5"));
	iConsole.Printf(_L("\n                          "));
	}


TInputAPIL2CapConfig::TInputAPIL2CapConfig(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole)
: TInputProcessorBase(aParent, aCb, aConsole),
  iInputState(ETransmitMTU)
	{
	}

TInt TInputAPIL2CapConfig::ProcessKey(TChar aKey)
	{
	TBool done = EFalse;
	switch(iInputState)
		{
		case ETransmitMTU:
			if(aKey == '.')
				{
				iConsole.Printf(_L("Use Default"));
				iInputState++;
				Draw();
				iBuffer.Reset();
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 4))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					iL2CapConfig.SetMaxTransmitUnitSize(*((TUint16*)(&val)));
					iInputState++;
					Draw();
					iBuffer.Reset();
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}		
			break;

		case ETransmitMinMTU:
			if(aKey == '.')
				{
				iConsole.Printf(_L("Accept all values"));
				iInputState++;
				Draw();
				iBuffer.Reset();
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 4))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					iL2CapConfig.SetMinMTU(*((TUint16*)(&val)));
					iInputState++;
					Draw();
					iBuffer.Reset();
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}		
			break;

		case EReceiveMTU:
			if(aKey == '.')
				{
				iConsole.Printf(_L("Use Default"));
				iInputState++;
				Draw();
				iBuffer.Reset();
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 4))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					iL2CapConfig.SetMaxReceiveUnitSize(*((TUint16*)(&val)));
					iInputState++;
					Draw();
					iBuffer.Reset();
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}		
			break;

		case EReceiveMinMTU:
			if(aKey == '.')
				{
				iConsole.Printf(_L("Accept all values"));
				iInputState++;
				Draw();
				iBuffer.Reset();
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 4))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					iL2CapConfig.SetMinMRU(*((TUint16*)(&val)));
					iInputState++;
					Draw();
					iBuffer.Reset();
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}		
			break;

		case EChannelPriority:
			switch(aKey)
				{
				case 'h':
					iL2CapConfig.SetChannelPriority(TL2CapConfig::EHigh);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;

				case 'm':
					iL2CapConfig.SetChannelPriority(TL2CapConfig::EMedium);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;

				case 'l':
					iL2CapConfig.SetChannelPriority(TL2CapConfig::ELow);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;

				case '.':
					iConsole.Printf(_L("Default [Low]"));
					iInputState++;
					Draw();
					break;

				default:
					break;
				};
			break;	

		case EChannelReliability:
			switch(aKey)
				{
				case 'r':
					iL2CapConfig.ConfigureReliableChannel(TL2CapConfig::EDefaultRetransmission);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;
				case 'R':
					iL2CapConfig.SetLegacyModesDisallowed(ETrue);
					iL2CapConfig.ConfigureReliableChannel(TL2CapConfig::EDefaultRetransmission);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;

				case 'u':
					iL2CapConfig.ConfigureUnreliableChannel(TL2CapConfig::EDefaultDataObsolescenceTimeout);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;
				case 'U':
					iL2CapConfig.SetLegacyModesDisallowed(ETrue);
					iL2CapConfig.ConfigureUnreliableChannel(TL2CapConfig::EDefaultDataObsolescenceTimeout);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;
				case 'x':
					iL2CapConfig.ConfigureUnreliableDesiredChannel(TL2CapConfig::EDefaultDataObsolescenceTimeout,
																   TL2CapConfig::EDefaultRetransmission);
					iConsole.Printf(_L("%c"), TUint(aKey));
					iInputState++;
					Draw();
					break;

				case '.':
					{
					iConsole.Printf(_L("Default Reliable Channel"));
					iInputState++;
					Draw();
					break;	
					}

				default:
					break;
				};
			break;	

		case EReliabilityTimer:
			{
			TBool specified = EFalse;
			TL2CapConfig::TChannelReliability reliability = iL2CapConfig.ChannelReliability(specified);
			// A bit hackich but does the job: if the selected reliability doesn't involve
			// the retransmission timer, just skip this one on the first keypress.
			if(aKey == '.' || !specified || reliability == TL2CapConfig::EUnreliableChannel)
				{
				if (aKey == '.' && specified && reliability != TL2CapConfig::EUnreliableChannel)
					{
					iConsole.Printf(_L("Use Default [0xffff]"));
					}
				else if (!specified)
					{
					iConsole.Printf(_L("Reliability was not specified, skipping retransmission timer"));
					}
				else
					{
					iConsole.Printf(_L("Selected reliability does not involve retransmission timer"));
					}

				iInputState++;
				Draw();
				iBuffer.Reset();
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 5))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					TUint16 timer = *((TUint16*)(&val));
					UpdateReliabilityTimer(timer);
					iInputState++;
					Draw();
					iBuffer.Reset();
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}
			}
			break;

		case ESDUFlushTimer:
			{
			TBool specified = EFalse;
			TL2CapConfig::TChannelReliability reliability = iL2CapConfig.ChannelReliability(specified);
			// A bit hackich but does the job: if the selected reliability doesn't involve
			// the obsolescence timer, just skip this one on the first keypress.
			if(aKey == '.' || !specified || reliability == TL2CapConfig::EReliableChannel)
				{
				if (aKey == '.' && specified && reliability != TL2CapConfig::EReliableChannel)
					{
					iConsole.Printf(_L("Use Default [0xffff]"));
					}
				else if (!specified)
					{
					iConsole.Printf(_L("Reliability was not specified, skipping obsolescence timer"));
					}
				else
					{
					iConsole.Printf(_L("Selected reliability does not involve obsolescence timer"));
					}
				iInputState++;
				Draw();
				iBuffer.Reset();

				done = ETrue;
				}
			else
				{								
				if(!iBuffer.AddNumber(aKey, 5))
					{
					TInt64 val;
					iBuffer.DecGetValue(val);
					TUint16 timer = *((TUint16*)(&val));
					UpdateObsolescenceTimer(timer);
					iInputState++;
					Draw();
					iBuffer.Reset();
					done = ETrue;
					}
				else
					{
					iConsole.Printf(_L("%c"), TUint(aKey));
					}
				}
			}
			break;
		}

	if(done)
		{
		return EMenuComplete;
		}
	else 
		{
		return EMenuContinue;
		}
	}

void TInputAPIL2CapConfig::UpdateReliabilityTimer(TUint16 aTimer)
	{
	TBool specified = EFalse;
	TL2CapConfig::TChannelReliability reliability = iL2CapConfig.ChannelReliability(specified);
	if (!specified || reliability == TL2CapConfig::EUnreliableChannel)
		{
		return;
		}
	else if (reliability == TL2CapConfig::EReliableChannel)
		{
		iL2CapConfig.ConfigureReliableChannel(aTimer);
		}
	else if (reliability == TL2CapConfig::EUnreliableDesiredChannel)
		{
		TUint16 obsolescenceTimer = iL2CapConfig.ObsolescenceTimer(specified);
		iL2CapConfig.ConfigureUnreliableDesiredChannel(obsolescenceTimer, aTimer);
		}
	}

void TInputAPIL2CapConfig::UpdateObsolescenceTimer(TUint16 aTimer)
	{
	TBool specified = EFalse;
	TL2CapConfig::TChannelReliability reliability = iL2CapConfig.ChannelReliability(specified);
	if (!specified || reliability == TL2CapConfig::EReliableChannel)
		{
		return;
		}
	else if (reliability == TL2CapConfig::EUnreliableChannel)
		{
		iL2CapConfig.ConfigureUnreliableChannel(aTimer);
		}
	else if (reliability == TL2CapConfig::EUnreliableDesiredChannel)
		{
		TUint16 retransmissionTimer = iL2CapConfig.RetransmissionTimer(specified);
		iL2CapConfig.ConfigureUnreliableDesiredChannel(aTimer, retransmissionTimer);
		}
	}

TL2CapConfig& TInputAPIL2CapConfig::GetL2CapConfig()
	{
	return iL2CapConfig;
	}

void TInputAPIL2CapConfig::Draw()
	{
	switch(iInputState)
		{
		case ETransmitMTU:
			iConsole.Printf(_L("\nPreferred Transmit MTU Size: "));
			break;

		case ETransmitMinMTU:
			iConsole.Printf(_L("\nMinimum acceptable Transmit MTU Size: "));
			break;
	
		case EReceiveMTU:
			iConsole.Printf(_L("\nPreferred Receive MTU Size: "));
			break;

		case EReceiveMinMTU:
			iConsole.Printf(_L("\nMinimum acceptable Receive MTU Size: "));
			break;

		case EChannelPriority:
			iConsole.Printf(_L("\nChannel Priority: h: High, m: Medium Ctrl, l: Low: "));
			break;

		case EChannelReliability:
			iConsole.Printf(_L("\nChannel Reliability: r: Reliable w. fallback, R: Reliable (ERTM only),"));
			iConsole.Printf(_L("\n\t... u: Unreliable w. fallback, U: Unreliable (Streaming only),"));
			iConsole.Printf(_L("\n\t... x: Unreliable Desired (prefer Unreliable but accept anything really)"));
			iConsole.Printf(_L("\n\t... : "));
			break;

		case EReliabilityTimer:
			iConsole.Printf(_L("\nReliability Timer [ms]: "));
			break;

		case ESDUFlushTimer:
			iConsole.Printf(_L("\nSDU Flush Timer [ms]: "));
			break;
		};
	}






CBTTestConsole* CBTTestConsole::NewL()
	{
	CBTTestConsole* self = NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

CBTTestConsole* CBTTestConsole::NewLC()
	{
	CBTTestConsole* self = new (ELeave) CBTTestConsole();
	CleanupStack::PushL(self);
	return self;
	}

CBTTestConsole::CBTTestConsole()
: test(_L("Bluetooth Test Console")),iLPMRequestsIfActive(ETrue)
	{
	}

void CBTTestConsole::Start()
	{
	TRAPD(err, StartL());
	if (err == KErrNone)
		{
		Draw();
		//__UHEAP_MARK;
		CActiveScheduler::Start();
		}
	}
	
void CBTTestConsole::StartL()
	{
	iActiveConsole = CActiveConsole::NewL(*this);
	iMainMenu = new (ELeave) XInputMainMenu(*this, iInputCallback, *(test.Console()));
	}

CBTTestConsole::~CBTTestConsole()
	{
	CleanAllResources();
	delete iActiveConsole;
	delete iMainMenu;
	iHostResolver.Close();
	iOobData.Close();
	iPairingServer.Close();
	}

void CBTTestConsole::CleanAllResources()
	{
	test.Printf(_L("Cleaning ."));
	iTestConn.ResetAndDestroy();
	StopProtocols();
	iPhyLinks.Close();

#ifdef _DEBUG
	DeletePublications();
#endif
	}


TBool CBTTestConsole::ProcessKey()
	{
	return iInputCallback.CallBack();
	}

TBool CBTTestConsole::ChildMenuClosed()
	{
	return EFalse;
	}

TInt CBTTestConsole::AllowEDRPackets(TBool aEnable)
	{
	TUint16 packetTypes;
	if(aEnable)
		{
		packetTypes = 0xcc18;
		}
	else
		{
		packetTypes = 0xff1e;
		}

	return ChangePacketTypes(packetTypes);	
	}
		
TInt CBTTestConsole::ChangePacketTypes(TUint16 aPacketTypes)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Connection is now invalid."));
		return KErrGeneral;
		}

	rerr = iTestConn[selConn]->ChangeSupportedPacketTypes(aPacketTypes);
	if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iPackets = aPacketTypes;
			}
	return rerr;
	}

TInt CBTTestConsole::SetL2CapAPIConfig(TL2CapConfig& aL2CapAPIConfig)
	{
	TInt rerr = KErrNone;
	iL2CapAPIConfig = aL2CapAPIConfig;
	return rerr;	
	}

TL2CapConfig& CBTTestConsole::L2CapAPIConfig()
	{
	return iL2CapAPIConfig;
	}

TInt CBTTestConsole::PassiveSCO()
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		rerr = iTestConn[ix]->PassiveSCO();
		}
	return rerr;
	}

TInt CBTTestConsole::ActiveSCO(TBTDevAddr& aAddr)
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		rerr = iTestConn[ix]->ActiveSCO(aAddr);
		}
	return rerr;
	}
	
TInt CBTTestConsole::PassiveESCO()
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		rerr = iTestConn[ix]->PassiveESCO();
		}
	return rerr;
	}

TInt CBTTestConsole::ActiveESCO(TBTDevAddr& aAddr)
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		rerr = iTestConn[ix]->ActiveESCO(aAddr);
		}
	return rerr;
	}

TInt CBTTestConsole::Listen(TUint16 aPort, TChar aProtocol, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig)
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		if(aProtocol == 'l')
			{
			rerr = iTestConn[ix]->CreateListeningSocket(aPort, _L("L2CAP"), aSecurity, aUseDefaultConfig);
			}
		else
			{
			rerr = iTestConn[ix]->CreateListeningSocket(aPort, _L("RFCOMM"), aSecurity, ETrue);
			iTestConn[ix]->iProfile.iIsStreamProtocol = ETrue;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::Connect(TBTDevAddr aAddr, TUint16 aPort, TChar aProtocol, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig)
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		if(aProtocol == 'l')
			{
			rerr = iTestConn[ix]->ConnectToPeer(aAddr, aPort, _L("L2CAP"), aSecurity, aUseDefaultConfig);
			}
		else
			{
			rerr = iTestConn[ix]->ConnectToPeer(aAddr, aPort, _L("RFCOMM"), aSecurity, ETrue);
			iTestConn[ix]->iProfile.iIsStreamProtocol = ETrue;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::BAPConnect(TBTDevAddr aAddr)
	{
	TInt rerr = KErrNone;
	TInt ix = NewConnection();
	if(ix < 0)
		{
		test.Printf(_L("\nNo free connection records."));
		rerr = ix;
		}
	else
		{
		rerr = iTestConn[ix]->ConnectUsingBAP(aAddr);
		iTestConn[ix]->iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnConnecting :
															  TConnProfile::EConnFailed;
		iTestConn[ix]->iProfile.iErrorCode = rerr;		
		}
	return rerr;
	}

TInt CBTTestConsole::ConfigureLink(TBool aUseIoctl)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!GetConn(selConn) ||
	   ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnServerDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnFailed))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr= iTestConn[selConn]->ConfigureLink(aUseIoctl);
		}
	return rerr;
	}

TInt CBTTestConsole::ShutdownSocket(RSocket::TShutdown aShutdownType)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!GetConn(selConn) ||
	   ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnIdle) ||
	   ConnInState(selConn, TConnProfile::EConnAccepting))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrNotFound;
		}
	else
		{
		if(iTestConn[selConn]->iProfile.iIsSocketBased)
			{
			static_cast<void>(iTestConn[selConn]->SetupPhysAdapter()); // only best effort
			rerr = iTestConn[selConn]->ShutdownSocket(aShutdownType);
			}
		else
			{
			rerr = iTestConn[selConn]->DisconnectLink();
			}
		}

	return rerr;
	}
	
TInt CBTTestConsole::IncreaseRTXTimer(TUint aTime)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnIdle))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrNotFound;
		}
	else
		{
		if(iTestConn[selConn]->iProfile.iIsSocketBased)
			{
			rerr = iTestConn[selConn]->IncreaseRTXTimer(aTime);
			}
		}

	return rerr;
	}

TInt CBTTestConsole::CloseAndDelete()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!IsActiveConnection(selConn))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrNotReady;
		}
	delete iTestConn[selConn];
	iTestConn[selConn] = NULL;
	CleanupPhyLinks();
	iMainMenu->SetSelectedConn(-1);
	return KErrNone;
	}





// ----------------------------------------------------------------------------
//							DATA MENU METHODS
// ----------------------------------------------------------------------------
TInt CBTTestConsole::SendMTUIoctl(TBool aIsIncomingMTU, TUint16 aNewValue)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!GetConn(selConn) ||
	   ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnServerDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnFailed))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr = iTestConn[selConn]->SendMTUIoctl(aIsIncomingMTU, aNewValue);
		}
	return rerr;
	}

TInt CBTTestConsole::SetPretendIncomingSduQFull(TBool aPretendIncomingSduQFull)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!GetConn(selConn) ||
	   ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnServerDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnFailed))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr = iTestConn[selConn]->SendPretendIncomingSduQFullIoctl(aPretendIncomingSduQFull);
		}
	return rerr;
	}

TInt CBTTestConsole::RecvOneShotData()
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(ConnectionUsable(selConn))
		{
		iTestConn[selConn]->iProfile.iData = TConnProfile::EDataRecvOneShot;
		rerr = iTestConn[selConn]->RecvOneShotData();
		}
	return rerr;
	}

TInt CBTTestConsole::SendOneShotData(TUint16 aDataSize)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(ConnectionUsable(selConn))
		{
		iTestConn[selConn]->iProfile.iData = TConnProfile::EDataSendOneShot;
		rerr = iTestConn[selConn]->SendOneShotData(aDataSize);
		}
	return rerr;
	}

TInt CBTTestConsole::ToggleDataDialog(TConnProfile::TDataState aDataType)
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}

	if(iTestConn[selConn]->iProfile.iData == TConnProfile::EDataIdle)
		{
		iTestConn[selConn]->iProfile.iData = aDataType;
		iTestConn[selConn]->StartACLData();
		}
	else
		{
		iTestConn[selConn]->StopACLData();
		iTestConn[selConn]->iProfile.iData = TConnProfile::EDataIdle;
		}
	return KErrNone;
	}

TInt CBTTestConsole::RecvContinuationData(TUint16 aDataSize)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(ConnectionUsable(selConn))
		{
		iTestConn[selConn]->iProfile.iData = TConnProfile::ERecvContinuationData;
		rerr = iTestConn[selConn]->RecvContinuationData(aDataSize);
		}
	return rerr;
	}

TInt CBTTestConsole::GetChannelMode()
	{
	TInt err = KErrNone;
	CBTTestConnection* connection = GetConn(iMainMenu->GetSelectedConn());
	if (connection)
		{
		TL2CapChannelMode mode;
		err = connection->GetChannelMode(mode);
		if (err == KErrNone)
			{
			TBuf<100> modeStr;
			switch (mode)
				{
				case EL2CAPBasicMode:
					modeStr = _L("Basic");
					break;
				case EL2CAPRetransmissionMode:
					modeStr = _L("Retransmission Mode [RTM]");
					break;
				case EL2CAPFlowControlMode:
					modeStr = _L("Flow Control Mode [FC]");
					break;
				case EL2CAPEnhancedRetransmissionMode:
					modeStr = _L("Enhanced Retransmission Mode [ERTM]");
					break;
				case EL2CAPStreamingMode:
					modeStr = _L("Streaming Mode [SM]");
					break;
				default:
					modeStr = _L("Unknown mode number ");
					modeStr.AppendNum(mode);
					break;
				}
			test.Printf(_L("\nNegotiated mode: %S [press a key]"), &modeStr);
			test.Getch();
			}
		}
	
	return err;
	}

TInt CBTTestConsole::GetLocalPSM()
	{
	TInt err = KErrNone;
	CBTTestConnection* connection = GetConn(iMainMenu->GetSelectedConn());
	if (connection)
		{
		TL2CAPPort psm;
		err = connection->GetLocalPSM(psm);
		if (err == KErrNone)
			{
			test.Printf(_L("\nLocal PSM: 0x%X [press a key]"), psm);
			test.Getch();
			}
		}

	return err;
	}

TBool CBTTestConsole::ConnectionUsable(TInt aConn)
	{
	TBool result = ETrue;
	TInt tmpppp = iTestConn[aConn]->iProfile.iData;
	if(!ConnInState(aConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		result = EFalse;
		}
	else if(iTestConn[aConn]->iProfile.iData != TConnProfile::EDataIdle)
		{
		test.Printf(_L("\n\nData are already being passed across this connection.."));
		result = EFalse;
		}

	return result;
	}
// ----------------------------------------------------------------------------
//							RFCOMM MENU METHODS
// ----------------------------------------------------------------------------
TInt CBTTestConsole::RequestRFCommConfChgInd()
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrNotReady;
		}
	else
		{
		rerr = iTestConn[selConn]->RequestRFCommConfChgInd();
		}
	return rerr;	
	}
	
TInt CBTTestConsole::SetLocalModemStatus(TUint16 aModemStatus)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrNotReady;
		}
	else
		{
		rerr = iTestConn[selConn]->SetLocalModemStatus(static_cast<TUint8>(aModemStatus));
		}
	return rerr;	
	}

// ----------------------------------------------------------------------------
//							PHYSICAL LINK MENU METHODS
// ----------------------------------------------------------------------------
TInt CBTTestConsole::ActiveRequest()
	{
	return MakeModeRequest(TConnProfile::EActive);
	}

TInt CBTTestConsole::SniffRequest()
	{
	return MakeModeRequest(TConnProfile::ESniff);
	}

TInt CBTTestConsole::ParkRequest()
	{
	return MakeModeRequest(TConnProfile::EPark);
	}
	
TInt CBTTestConsole::ExplicitActiveRequest()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(IsConnSocketBased(selConn))
		{
		test.Printf(_L("Cannot set CBluetoothSocket explicitly to Active mode"));
		return KErrGeneral;
		}
	return MakeModeRequest(TConnProfile::EExplicitActive);
	}

TInt CBTTestConsole::MakeModeRequest(TConnProfile::TConnMode aMode)
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	if(aMode != TConnProfile::EActive && iTestConn[selConn]->iProfile.iMode2 == aMode)
		{
		test.Printf(_L("%S mode is currently selected."), &TConnProfile::ModeName(aMode));
		return KErrGeneral;
		}

	TInt rerr = iTestConn[selConn]->RequestLPM(aMode);
	if(rerr == KErrNone)
		{
		iTestConn[selConn]->iProfile.iMode2 = aMode;
		}
	return rerr;
	}

TInt CBTTestConsole::ToggleSniffModeAllowed()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr;
	if(iTestConn[selConn]->iProfile.iModeAllowed & ESniffMode)
		{
		rerr = iTestConn[selConn]->PreventLPM(ESniffMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed ^= ESniffMode;
			}
		}
	else
		{
		rerr = iTestConn[selConn]->SupportLPM(ESniffMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed |= ESniffMode;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::ToggleParkModeAllowed()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr;
	if(iTestConn[selConn]->iProfile.iModeAllowed & EParkMode)
		{
		rerr = iTestConn[selConn]->PreventLPM(EParkMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed ^= EParkMode;
			}
		}
	else
		{
		rerr = iTestConn[selConn]->SupportLPM(EParkMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed |= EParkMode;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::ToggleHoldModeAllowed()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr;
	if(iTestConn[selConn]->iProfile.iModeAllowed & EHoldMode)
		{
		rerr = iTestConn[selConn]->PreventLPM(EHoldMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed ^= EHoldMode;
			}
		}
	else
		{
		rerr = iTestConn[selConn]->SupportLPM(EHoldMode);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iModeAllowed |= EHoldMode;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::RequestRoleSwitch(TBTBasebandRole aRole)
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr;
	rerr = iTestConn[selConn]->RequestRoleChange(aRole);
	return rerr;
	}
		
TInt CBTTestConsole::ToggleSwitchRoleAllowed()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr;
	if(iTestConn[selConn]->iProfile.iSwitchAllowed)
		{
		rerr = iTestConn[selConn]->RoleChangeAllowed(EFalse);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iSwitchAllowed = EFalse;
			}
		}
	else
		{
		rerr = iTestConn[selConn]->RoleChangeAllowed(ETrue);
		if(rerr == KErrNone)
			{
			iTestConn[selConn]->iProfile.iSwitchAllowed = ETrue;
			}
		}
	return rerr;
	}

TInt CBTTestConsole::PlmDisplay()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TRAPD(err, iTestConn[selConn]->StartPlmDisplayL());
	return err;
	}

TInt CBTTestConsole::PlmCancel()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TRAPD(err, iTestConn[selConn]->StopPlmDisplayL());
	return err;
	}

TInt CBTTestConsole::AuthenticationRequest()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
 
	TInt rerr;
	rerr = iTestConn[selConn]->RequestAuthentication();
	if(rerr == KErrAlreadyExists)
		{
		test.Printf(_L("Link is already authenticated."));
		// Consume the error, as this is the correct behaviour
		rerr = KErrNone;
		}
	return rerr;
	}

TInt CBTTestConsole::ToggleLPMRequestsAllowed()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("Please select a valid connection."));
		return KErrGeneral;
		}
	TInt rerr = KErrNone;
	if(iLPMRequestsIfActive)
		{
		iLPMRequestsIfActive = EFalse;
		}
	else
		{
		iLPMRequestsIfActive = ETrue;
		}
	return rerr;
	}

TInt CBTTestConsole::GetConnectedPhyLinks()
	{
	TInt rerr = KErrNone;
	CBTTestConnection* tempConn = NULL;
	TRAP(rerr, tempConn = CBTTestConnection::NewL(this, iSocketServer));
	if(rerr == KErrNone)
		{
		rerr = tempConn->PrintBTPhyConnections();	
		delete tempConn;
		}
		
	return rerr;
	}
	
TInt CBTTestConsole::KillAllLinks()
	{
	TInt connIx;
	
	// Use the first connection we find
	for(connIx=0;connIx<iTestConn.Count();connIx++)
		{
		if(iTestConn[connIx])
			{
			return iTestConn[connIx]->KillAllLinks(-36);
			}
		}			

	test.Printf(_L("No connections found."));	
	return KErrNotFound;
	}

TInt CBTTestConsole::KillLink()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!IsActiveConnection(selConn))
		{
		test.Printf(_L("Select a valid connection."));
		return KErrGeneral;
		}

	return iTestConn[selConn]->KillLink(-36);
	}

TInt CBTTestConsole::ToggleAutoSniff()
	{
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!IsActiveConnection(selConn))
		{
		test.Printf(_L("Select a valid connection."));
		return KErrGeneral;
		}

	test.Printf(_L("\nToggling AutoSniff...\n"));
	return iTestConn[selConn]->ToggleAutoSniff();
	}



TBool CBTTestConsole::DataMenu(TDataMenu::TDataMenuType aInfoType, TUint16 aBasicValue)
	{
	TInt err = KErrNone;
	TBool redraw = ETrue;
	
	switch(aInfoType)
		{
		case TDataMenu::ESetIncomingMTU:
			err = SendMTUIoctl(ETrue, aBasicValue);
			break;

		case TDataMenu::ESetOutgoingMTU:
			err = SendMTUIoctl(EFalse, aBasicValue);
			break;

		case TDataMenu::ERecvOneShotData:
			err = RecvOneShotData();
			break;

		case TDataMenu::ESendOneShotData:
			err = SendOneShotData(aBasicValue);
			break;

		case TDataMenu::EDataDlgRecv:
			err = ToggleDataDialog(TConnProfile::EDataDialog1);
			break;

		case TDataMenu::EDataDlgSend:
			err = ToggleDataDialog(TConnProfile::EDataDialog2);
			break;

		case TDataMenu::ESendData:	
			err = ToggleDataDialog(TConnProfile::EDataSend);
			break;
		case TDataMenu::ERecvData:
			err = ToggleDataDialog(TConnProfile::EDataRecv);
			break;
		case TDataMenu::ERecvAnyData:
			err = ToggleDataDialog(TConnProfile::EDataRecvAny);
			break;
		case TDataMenu::ESimultanousSendReceiveData:
			err = ToggleDataDialog(TConnProfile::EDataSendReceive);
			break;
		case TDataMenu::ERecvContinuationData:
			err = RecvContinuationData(aBasicValue);
			break;

		case TDataMenu::ERTXTimer:
			err = IncreaseRTXTimer(aBasicValue);
			break;

		case TDataMenu::EPretendSduQFullOn:
			err = SetPretendIncomingSduQFull(ETrue);
			break;

		case TDataMenu::EPretendSduQFullOff:
			err = SetPretendIncomingSduQFull(EFalse);
			break;

		case TDataMenu::EGetChannelMode:
			err = GetChannelMode();
			break;

		case TDataMenu::EGetLocalPSM:
			err = GetLocalPSM();
			break;

		default:
			test.Printf(_L("\n\nPlease select a valid option.\n"));
			redraw = EFalse;
			break;	
		};
			
	if(err != KErrNone)
		{
		test.Printf(_L("\n\nDATA MENU ERROR [%d]\n"), err);
		redraw = EFalse;
		}
	return redraw;
	}

	
TBool CBTTestConsole::RFCommMenu(TRFCommMenu::TRFCommMenuType aInfoType, TUint16 aBasicValue)
	{
	TInt err = KErrNone;
	TBool redraw = ETrue;
	
	switch(aInfoType)
		{
		case TRFCommMenu::ERequestRFCommConfChgInd:
			err = RequestRFCommConfChgInd();
			break;

		case TRFCommMenu::ESetLocalModemStatus:
			err = SetLocalModemStatus(aBasicValue);
			break;

		default:
			test.Printf(_L("\n\nPlease select a valid option.\n"));
			redraw = EFalse;
			break;	
		};
			
	if(err != KErrNone)
		{
		test.Printf(_L("\n\nRFCOMM MENU ERROR [%d]\n"), err);
		redraw = EFalse;
		}
	return redraw;
	}


TBool CBTTestConsole::PhysicalLinkMenu(TPhysicalLinkMenu::TPhysicalLinkMenuType aInfoType, TUint16 aBasicValue)
	{
	(void)aBasicValue;
	TInt err = KErrNone;
	TBool redraw = ETrue;
	
	switch(aInfoType)
		{
		case TPhysicalLinkMenu::EActiveRequest:
			err = ActiveRequest();
			break;
		case TPhysicalLinkMenu::ESniffRequest:
			err = SniffRequest();
			break;
		case TPhysicalLinkMenu::EParkRequest:
			err = ParkRequest();
			break;
		case TPhysicalLinkMenu::EExplicitActiveRequest:
			err = ExplicitActiveRequest();
			break;

		case TPhysicalLinkMenu::EToggleSniffModeAllowed:
			err = ToggleSniffModeAllowed();
			break;
		case TPhysicalLinkMenu::EToggleParkModeAllowed:
			err = ToggleParkModeAllowed();
			break;
		case TPhysicalLinkMenu::EToggleHoldModeAllowed:
			err = ToggleHoldModeAllowed();
			break;

		case TPhysicalLinkMenu::ERequestMasterRole:
			err = RequestRoleSwitch(EMaster);
			break;
		case TPhysicalLinkMenu::ERequestSlaveRole:
			err = RequestRoleSwitch(ESlave);
			break;
		case TPhysicalLinkMenu::EToggleRoleSwitchAllowed:
			err = ToggleSwitchRoleAllowed();
			break;
		case TPhysicalLinkMenu::EToggleLPMRequestsIfActive:
			err = ToggleLPMRequestsAllowed();
			break;
		case TPhysicalLinkMenu::EAuthenticationRequest:
			err = AuthenticationRequest();
			redraw = EFalse;
			break;

		case TPhysicalLinkMenu::EPhysicalLinkMetricsDisplay:
			err = PlmDisplay();
			break;
		case TPhysicalLinkMenu::EPhysicalLinkMetricsCancel:
			err = PlmCancel();
			break;
		case TPhysicalLinkMenu::EKillAllLinks:
			err = KillAllLinks();
			break;

		case TPhysicalLinkMenu::EKillLink:
			err = KillLink();
			break;

		case TPhysicalLinkMenu::EGetConnectedLinks:
			err = GetConnectedPhyLinks();
			redraw = EFalse;
			break;

		case TPhysicalLinkMenu::EToggleAutoSniff:
			err = ToggleAutoSniff();
			redraw = EFalse;
			break;

		default:
			test.Printf(_L("\n\nPlease select a valid option.\n"));
			redraw = EFalse;
			break;	
		};
			
	if(err != KErrNone)
		{
		test.Printf(_L("\n\nPHYSICAL LINK MENU ERROR [%d]\n"), err);
		redraw = EFalse;
		}
	return redraw;
	}






TBool CBTTestConsole::ConnInState(TInt aIx, TConnProfile::TConnState aState)
	{
	return (IsActiveConnection(aIx) && iTestConn[aIx]->iProfile.iState == aState);
	}

TBool CBTTestConsole::IsConnSocketBased(TInt aIx)
	{
	return IsActiveConnection(aIx) && iTestConn[aIx]->iProfile.iIsSocketBased;
	}

TBool CBTTestConsole::IsActiveConnection(TInt aIx)
	{
	return (aIx >= 0 && aIx < iTestConn.Count() && iTestConn[aIx] != NULL);
	}

TInt CBTTestConsole::NewConnection()
	{
	// Create a new Connection.
	CBTTestConnection* newConnPtr = NULL;
	TInt newConnIx = 0;
	
	TRAPD(rerr, newConnPtr = CBTTestConnection::NewL(this, iSocketServer));
	
	if(rerr == KErrNone)
		{
		// Try and find a free entry in the array.
		for(newConnIx=0;newConnIx<iTestConn.Count();newConnIx++)
			{
			if(!iTestConn[newConnIx])
				{
				break;
				}
			}

		if(newConnIx == iTestConn.Count())
			{
			// There are no free entries.  Try to create one.
			rerr = iTestConn.Append(newConnPtr);
			if(rerr != KErrNone)
				{
				delete newConnPtr;
				}
			}
		else
			{
			iTestConn[newConnIx] = newConnPtr;
			}
		}
	return (rerr == KErrNone ? newConnIx : rerr);	
	}

TInt CBTTestConsole::NumberOfConnections()
	{
	TInt count = 0;
	for(TInt i=0;i<iTestConn.Count();i++)
		{
		if(iTestConn[i])
			{
			count++;
			}
		}
	return count;
	}

void CBTTestConsole::CreatePhyRecord(const TBTDevAddr& aBTAddr)
	{
	TInt count = iPhyLinks.Count();
	for(TInt i=0;i<count;i++)
		{
		if(iPhyLinks[i].iRemoteBTAddr.BTAddr() == aBTAddr)
			{
			iPhyLinks[i].iState = TConnProfile::EConnConnected;
			return;
			}
		}
	// Create a new phy record.
	TConnProfile cp;
	TBTSockAddr sa;
	sa.SetBTAddr(aBTAddr);
	cp.iRemoteBTAddr = sa;
	cp.iState = TConnProfile::EConnConnected;

	iPhyLinks.Append(cp);
	}

TConnProfile* CBTTestConsole::FindPhyRecord(const TBTDevAddr& aBTAddr)
	{
	TInt count = iPhyLinks.Count();
	for(TInt i=0;i<count;i++)
		{
		if(iPhyLinks[i].iRemoteBTAddr.BTAddr() == aBTAddr)
			{
			return &(iPhyLinks[i]);
			}
		}
	return NULL;
	}
		
void CBTTestConsole::CleanupPhyLinks()
	{
	for (TInt i=0; i<iPhyLinks.Count(); ++i)
		{
		TBool connectionAvailable = EFalse;
		for (TInt j=0; j<iTestConn.Count(); ++j)
			{
			CBTTestConnection* ptr = iTestConn[j];
			if(ptr && iPhyLinks[i].iRemoteBTAddr.BTAddr() == ptr->iProfile.iRemoteBTAddr.BTAddr())
				{
				connectionAvailable = ETrue;
				break;
				}
			}
		if (!connectionAvailable)
			{
			// Remove the phy record
			iPhyLinks.Remove(i);
			--i; // As an entry has been removed we must go back one.
			}
		}
	}

void CBTTestConsole::Draw()
	{
	test.Console()->ClearScreen();
	DrawAddress();
	DrawConnections();
	iMainMenu->Draw();
	}

void CBTTestConsole::DrawAddress()
	{
	RProperty property;
	TBuf8<6> addr;

	TInt err = property.Get(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addr);

	if (err != KErrNone)
		{
		test.Printf(_L("P&S: Error retrieving local address - error %d\n"), err);
		}
	else
		{
		if (addr.Length()!=6)
			{
			property.Attach(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress);
			TRequestStatus status;
			property.Subscribe(status);
			User::WaitForRequest(status);
			err = property.Get(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress, addr);
			}
		if (err != KErrNone || addr.Length()!=6)
			{
			test.Printf(_L("P&S: Error retrieving local address - error %d\n"), err);
			}
		else
			{
			TBTDevAddr localAddress(addr);
			TBuf<20> dispBuf;
			localAddress.GetReadable(dispBuf);
			test.Printf(_L("Local address = 0x%S\n"),&dispBuf);
			}
		}

	}

void CBTTestConsole::DrawConnections()
	{
	test.Printf(_L("CONNECTIONS\n"));
#if defined (__WINS__)
	test.Printf(_L("   Ix  State        Role Mode[R]    Allowed    Packets  Data   Max Slots  Enc\n"));
	test.Printf(_L("                    M/S  S P E A    S P H M V  DM  DV   Rx/Tx   (1/3/5)      \n"));
#else
	// Reduce output for smaller screens on hardware.
	test.Printf(_L("   Ix  State        Role Mode[R]    Allowed    Data\n"));
	test.Printf(_L("                    M/S  S P E A    S P H M V  Rx/Tx\n"));
#endif

	for(TInt i=0;i<iTestConn.Count();i++)
		{
		if(iTestConn[i])
			{
			if(iTestConn[i]->iProfile.iState != TConnProfile::EConnConnected)
				{
				DrawConnection(i);
				}
			else
				{
				TConnProfile* cp = FindPhyRecord(iTestConn[i]->iProfile.iRemoteBTAddr.BTAddr());
				if(cp && cp->iState != TConnProfile::EConnConnected)
					{
					iTestConn[i]->iProfile.iState = cp->iState;
					DrawConnection(i);
					}
				}
			}		
		}

	TInt count = iPhyLinks.Count();
	for(TInt i=0;i<count;i++)
		{
		test.Printf(_L("\nPHY "));
		TBuf<20> bdaddrbuf;
		iPhyLinks[i].iRemoteBTAddr.BTAddr().GetReadable(bdaddrbuf);
		if(iPhyLinks[i].iState == TConnProfile::EConnConnected)
			{
			test.Printf(_L("   %S"), &bdaddrbuf);

			test.Printf(_L("  %S"), &TConnProfile::RoleName(iPhyLinks[i].iRole));
			test.Printf(_L("   %S  "), &TConnProfile::ModeName(iPhyLinks[i].iMode2));

#if defined (__WINS__)
			test.Printf(_L("            "));
			if(iPhyLinks[i].iPackets & EPacketsDM1)
				{
				test.Printf(_L("  1"));
				}
			else
				{
				test.Printf(_L("  -"));
				}
			if(iPhyLinks[i].iPackets & EPacketsDM3)
				{
				test.Printf(_L("3"));
				}
			else
				{
				test.Printf(_L("-"));
				}
			if(iPhyLinks[i].iPackets & EPacketsDM5)
				{
				test.Printf(_L("5"));
				}
			else
				{
				test.Printf(_L("-"));
				}

			if(iPhyLinks[i].iPackets & EPacketsDH1)
				{
				test.Printf(_L(" 1"));
				}
			else
				{
				test.Printf(_L(" -"));
				}
			if(iPhyLinks[i].iPackets & EPacketsDH3)
				{
				test.Printf(_L("3"));
				}
			else
				{
				test.Printf(_L("-"));
				}
			if(iPhyLinks[i].iPackets & EPacketsDH5)
				{
				test.Printf(_L("5"));
				}
			else
				{
				test.Printf(_L("-"));
				}

			test.Printf(_L("             %d"), iPhyLinks[i].iMaxSlots);
			if(iPhyLinks[i].iEncryption)
				{
				test.Printf(_L("      On"));
				}
			else
				{
				test.Printf(_L("      Off"));
				}
#endif
			test.Printf(_L("\n"));
			}
		else
			{
			test.Printf(_L("  %S\n"), &TConnProfile::StateName(iPhyLinks[i].iState));
			}

		for(TInt j=0;j<iTestConn.Count();j++)
			{
			if(iTestConn[j] && iTestConn[j]->iProfile.iState == TConnProfile::EConnConnected && iPhyLinks[i].iRemoteBTAddr.BTAddr() == iTestConn[j]->iProfile.iRemoteBTAddr.BTAddr())
				{
				DrawConnection(j);
				}
			}
		}
	}

void CBTTestConsole::ServerDisconnect(TInt aConnIx)
	{
	if(IsActiveConnection(aConnIx))
		{
		iTestConn[aConnIx]->iProfile.iState = TConnProfile::EConnServerDisconnected;
		}
	}

void CBTTestConsole::DrawConnection(TInt aIx)
	{
	if(iMainMenu->GetSelectedConn() == aIx)
		{
		test.Printf(_L(">> "));
		}
	else
		{
		test.Printf(_L("   "));
		}
	test.Printf(_L("%d"), aIx);

	if(iTestConn[aIx]->iProfile.iIsSCO)
		{
		test.Printf(_L(" s "));
		}
	else
		{
		test.Printf(_L(" a "));
		}

	test.Printf(_L("%S"), &TConnProfile::StateName(iTestConn[aIx]->iProfile.iState));

	if(iTestConn[aIx]->iProfile.iErrorCode != KErrNone)
		{
		test.Printf(_L("[%d]"), iTestConn[aIx]->iProfile.iErrorCode);
		}

	if(iTestConn[aIx]->iProfile.iState == TConnProfile::EConnConnected) 
		{
		test.Printf(_L("  %S"), &TConnProfile::RoleName(iTestConn[aIx]->iProfile.iRole));

		if(iTestConn[aIx]->iProfile.iMode2 == TConnProfile::ESniff)
			{
			test.Printf(_L("   S"));
			}
		else
			{
			test.Printf(_L("   -"));
			}

		if(iTestConn[aIx]->iProfile.iMode2 == TConnProfile::EPark)
			{
			test.Printf(_L(" P"));
			}
		else
			{
			test.Printf(_L(" -"));
			}

		if(iTestConn[aIx]->iProfile.iMode2 == TConnProfile::EExplicitActive)
			{
			test.Printf(_L(" E"));
			}
		else
			{
			test.Printf(_L(" -"));
			}
			
		if(iTestConn[aIx]->iProfile.iAutoSniffActive)
			{
			test.Printf(_L(" A"));
			}
		else
			{
			test.Printf(_L(" -"));
			}
			
			


		if(iTestConn[aIx]->iProfile.iModeAllowed & ESniffMode)
			{
			test.Printf(_L("    S"));
			}
		else
			{
			test.Printf(_L("    -"));
			}

		if(iTestConn[aIx]->iProfile.iModeAllowed & EParkMode)
			{
			test.Printf(_L(" P"));
			}
		else
			{
			test.Printf(_L(" -"));
			}

		if(iTestConn[aIx]->iProfile.iModeAllowed & EHoldMode)
			{
			test.Printf(_L(" H"));
			}
		else
			{
			test.Printf(_L(" -"));
			}
		if(iTestConn[aIx]->iProfile.iSwitchAllowed)
			{
			test.Printf(_L(" M"));
			}
		else
			{
			test.Printf(_L(" -"));
			}
			
		if(iLPMRequestsIfActive)
			{
			test.Printf(_L(" V"));
			}
		else
			{
			test.Printf(_L(" -"));
			}

#if defined (__WINS__)
		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDM1)
			{
			test.Printf(_L("  1"));
			}
		else
			{
			test.Printf(_L("  -"));
			}
		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDM3)
			{
			test.Printf(_L("3"));
			}
		else
			{
			test.Printf(_L("-"));
			}
		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDM5)
			{
			test.Printf(_L("5"));
			}
		else
			{
			test.Printf(_L("-"));
			}

		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDH1)
			{
			test.Printf(_L(" 1"));
			}
		else
			{
			test.Printf(_L(" -"));
			}
		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDH3)
			{
			test.Printf(_L("3"));
			}
		else
			{
			test.Printf(_L("-"));
			}
		if(iTestConn[aIx]->iProfile.iPackets & EPacketsDH5)
			{
			test.Printf(_L("5"));
			}
		else
			{
			test.Printf(_L("-"));
			}
#endif
		test.Printf(_L("  %S"), &TConnProfile::DataStateName(iTestConn[aIx]->iProfile.iData));
		}
	test.Printf(_L("\n"));
	}



TInt CBTTestConsole::Accept(TInt aConnIx)
	{
	TInt rerr = KErrNone;
	if(!ConnInState(aConnIx, TConnProfile::EConnListening))
		{
		test.Printf(_L("Please select a valid connection."));
		rerr = KErrNotFound;
		}
	else
		{
		TInt ix = NewConnection();
		if(ix < 0)
			{
			test.Printf(_L("Please select a valid connection."));
			rerr = ix;
			}
		else
			{
			rerr = iTestConn[ix]->Accept(iTestConn[aConnIx]->GetBTSocket());
			if(rerr == KErrNone)
				{
				// Tie the accepting socket to the listener.  When the listener
				// async returns, the accepter will become connected.
				iTestConn[aConnIx]->iProfile.iAccepterIx = ix;
				
				iTestConn[aConnIx]->iProfile.iState = TConnProfile::EConnListeningAccept;
				}
			}
		Draw();
		}
	return rerr;
	}

TInt CBTTestConsole::SendEchoRequest(TInt aPayloadSize)
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!GetConn(selConn) ||
	   ConnInState(selConn, TConnProfile::EConnDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnServerDisconnected) ||
	   ConnInState(selConn, TConnProfile::EConnFailed))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr = iTestConn[selConn]->SendEchoRequest(aPayloadSize);
		if(rerr != KErrNone)
			{
			test.Printf(_L("Echo Request Failed: Error - %d\n") ,rerr);
			}
		}
	return rerr;
	}
	
void CBTTestConsole::InitialiseL()
	{
	_LIT(KLDDName, "ECOMM");

#if defined (__WINS__)
	test.Printf(_L("Initialise using EMULATOR PDD\n"));

	_LIT(KPDDName, "ECDRV");		
#else	
	test.Printf(_L("Initialise using H2 PDD\n"));

	_LIT(KPDDName, "EUART1");
#endif
	
	TInt rerr = KErrNone;
	
	rerr = User::LoadPhysicalDevice(KPDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		test.Printf(_L("Failed Load Physical Device %d!\n\r") ,rerr);
		User::Leave(rerr);
		}
	else
		{ 
		test.Printf(_L("Loaded LDD\n"));
		}
		
	rerr = User::LoadLogicalDevice(KLDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		test.Printf(_L("Failed Load Logical Device%d!\n\r") ,rerr);
		User::Leave(rerr);
		}
	else
		{
		test.Printf(_L("Loaded PDD\n"));
		}

	User::LeaveIfError(iSocketServer.Connect(20)); // 20 message slots

	TProtocolDesc iProtocolInfo;
	TRequestStatus status;

	User::LeaveIfError(iSocketServer.FindProtocol(_L("L2CAP"), iProtocolInfo));


	iSocketServer.StartProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);

	User::LeaveIfError(iSocketServer.FindProtocol(_L("RFCOMM"), iProtocolInfo));
		
	User::LeaveIfError(iSocketServer.FindProtocol(_L("BTLinkManager"), iProtocolInfo));
	
	User::LeaveIfError(iHostResolver.Open(iSocketServer, iProtocolInfo.iAddrFamily, iProtocolInfo.iProtocol));
	
	// These are v2.1 APIs and so might not be available.
	TInt err = iPairingServer.Connect();
	if(err == KErrNone)
		{
	User::LeaveIfError(iOobData.Open(iPairingServer));
		iPairingServerAvailable = ETrue;
		}

	
#ifdef _DEBUG
	rerr = DefinePublications();
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		test.Printf(_L("Failed to Define Publications %d!\n\r") ,rerr);
		User::Leave(rerr);
		}
#endif
	}

void CBTTestConsole::StopProtocols()
	{
	TInt err;
	TProtocolDesc iProtocolInfo;
	TRequestStatus status;
	
	if(!iSocketServer.Handle())
		{
		// if the socket server has not even been opened
		// then there is no point trying to unload protocols
		return;
		}

	err = iSocketServer.FindProtocol(_L("L2CAP"), iProtocolInfo);
	if(err)
		{
		test.Printf(_L("Failed to find L2CAP protocol. ERROR %d"), err);
		}

	iSocketServer.StopProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);

	err = iSocketServer.FindProtocol(_L("RFCOMM"), iProtocolInfo);
	if(err)
		{
		test.Printf(_L("Failed to find RFCOMM protocol. ERROR %d"), err);
		}

	iSocketServer.StopProtocol(iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol, status);
	User::WaitForRequest(status);	
	}

CBTTestConnection* CBTTestConsole::GetConn(TInt aIndex)
	{
	return ((aIndex < 0 || aIndex >= iTestConn.Count()) ? NULL : iTestConn[aIndex]);
	}

TInt CBTTestConsole::RegisterSDPRecordL()
	{
	TInt rerr = KErrNone;
	
	RSdpDatabase sdpDB;
	RSdp sdp;

	rerr = sdp.Connect();
	if(rerr == KErrNone)
		{
		rerr = sdpDB.Open(sdp);
		}

	test.Printf(_L("\n\nConnect to SDP database.  Error %d\n"), rerr);
	if(rerr == KErrNone)
		{
		TSdpServRecordHandle recHandle;
		CSdpAttrValue* attrVal = NULL;
		CSdpAttrValueDES* attrValDES = NULL;

		// Set Attr 1 (service class list) to list with UUID = 0x1116
		sdpDB.CreateServiceRecordL(0x1116, recHandle); 

		// Protocol Descriptor List
		attrValDES = CSdpAttrValueDES::NewDESL(0); //Qualified
		attrValDES->StartListL() //Qualified
			->BuildDESL()->StartListL() //Qualified 
				->BuildUUIDL(TUUID(TUint16(0x0100))) //Qualified					// L2CAP
				->BuildUintL(TSdpIntBuf<TUint16>(0x000f)) //Qualified				// PSM = BNEP
			->EndListL() //Qualified
			->BuildDESL()->StartListL() //Qualified
				->BuildUUIDL(TUUID(TUint16(0x000f))) //Qualified					// BNEP
				->BuildUintL(TSdpIntBuf<TUint16>(0x0100))				// 0x0100
				->BuildDESL()->StartListL() 
					->BuildUintL(TSdpIntBuf<TUint16>(0x0806)) 				// ARP
					->BuildUintL(TSdpIntBuf<TUint16>(0x0800)) 				// IPv4
					->BuildUintL(TSdpIntBuf<TUint16>(0x86dd))				// IPv6
				->EndListL() 
			->EndListL() 
		->EndListL(); 
		sdpDB.UpdateAttributeL(recHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES); 
		delete attrValDES;
		attrValDES = NULL;

		// Language
		attrValDES = CSdpAttrValueDES::NewDESL(0); 
		attrValDES->StartListL() 
			->BuildUintL(TSdpIntBuf<TUint16>(KLanguageEnglish)) 
			->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdCharacterEncodingUTF8)) 
			->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdBasePrimaryLanguage)) 
		->EndListL();  
		sdpDB.UpdateAttributeL(recHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES); 
		delete attrValDES;
		attrValDES = NULL;

		// BT Profile Description
		attrValDES = CSdpAttrValueDES::NewDESL(0); 
		attrValDES->StartListL() 
			->BuildDESL()->StartListL() 
				->BuildUUIDL(KPanGnUUID) 
				->BuildUintL(TSdpIntBuf<TUint16>(0x0100)) 
			->EndListL() 
		->EndListL(); 
		sdpDB.UpdateAttributeL(recHandle, KSdpAttrIdBluetoothProfileDescriptorList, *attrValDES); 
		delete attrValDES;
		attrValDES = NULL;

		attrVal = CSdpAttrValueString::NewStringL(_L8("Group Ad-hoc Network Service")); 
		sdpDB.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName, *attrVal); 
		delete attrVal;
		attrVal = NULL;

		attrVal = CSdpAttrValueString::NewStringL(_L8("Personal Group Ad-hoc Network Service")); 
		sdpDB.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription, *attrVal); 
		delete attrVal;
		attrVal = NULL;

		// Security
		attrVal = CSdpAttrValueInt::NewIntL(TSdpIntBuf<TUint16>(0x0000)); 
		sdpDB.UpdateAttributeL(recHandle, 0x030a, *attrVal); 
		delete attrVal;
		attrVal = NULL;
		}
	return rerr;
	}

TInt CBTTestConsole::SetHostName()
	{
	test.Printf(_L("Set Host Name to: "));
	TKeyCode key;
	TBuf16<255> buf;
	
	while((key = test.Getch())!=EKeyEnter && buf.Length()<buf.MaxLength())
		{
		if(key == EKeyBackspace&&buf.Length()!=0)
			{
			buf.SetLength(buf.Length()-1);
			}
		else 
			{
			buf.Append(key);
			}
		
		test.Printf(_L("%c"), key);
		}
		
	iHostResolver.SetHostName(buf);
	test.Printf(_L("\n"));
	return KErrNone;
	}

TInt CBTTestConsole::GetRemoteHostName(TInt aSelectedConnection)
	{
	TNameEntry result;
	TInt err = iTestConn[aSelectedConnection]->GetRemoteName(result);

	if (err!=KErrNone)
		{
		test.Printf(_L("\nError RHostResolver::GetByAddress() %i"), err);
		}
	else
		{
		TNameRecord name = result();
		test.Printf(_L("\nRemote Host Name: "));
		test.Printf(name.iName);
		test.Printf(_L("\nName length: %i"), name.iName.Length());
		}

	return KErrNone;	
	}

TInt CBTTestConsole::ReadLocalOobData()
	{
	if (!iPairingServerAvailable)
		{
		test.Printf(_L("Pairing Server is not available\n"));
		return KErrNotSupported;
		}
	TInt err = KErrNone;
	TBluetoothSimplePairingHash hash;
	TBluetoothSimplePairingRandomizer randomizer;
	test.Printf(_L("Reading local OOB Data...\n"));
	
	TRequestStatus status;
	iOobData.ReadLocalOobData(hash, randomizer, status);
	User::WaitForRequest(status);
	
	test.Printf(_L("OOB Data Retrieved, error %d\n"), status.Int());
	if (status.Int() == KErrNone)
		{
		test.Printf(_L("Hash C: 0x"));
		for (TUint i = 0; i < hash.Length(); i++)
			{
			test.Printf(_L("%x"), hash[i]);
			}
		
		test.Printf(_L("\nRandomizer R: 0x"));
		for (TUint i = 0; i < randomizer.Length(); i++)
			{
			test.Printf(_L("%x"), randomizer[i]);
			}
		test.Printf(_L("\n"));
		}
	
	test.Printf(_L("Set name as OOB data? (y)"));
	
	
	TKeyCode key = test.Getch();
	
	while(key != 'y' && key != 'n' && key != EKeyEnter)
		{
		key = test.Getch();
		}
	
	test.Printf(_L("%c%c%c"), EKeyBackspace, EKeyBackspace, EKeyBackspace);
	
	if (key == EKeyEnter || key == 'y')
		{
		test.Printf(_L("y\n"));
		TBuf8<32> oobDataName8;
		TBuf<64> oobDataName;
		for (TUint i = 0; i < KBluetoothSimplePairingHashSize; i++)
			{
			oobDataName.AppendNumFixedWidthUC(hash[i], EHex, 2);
			}
		for (TUint i = 0; i < KBluetoothSimplePairingRandomizerSize; i++)
			{
			oobDataName.AppendNumFixedWidthUC(randomizer[i], EHex, 2);
			}
		TInt err = iHostResolver.SetHostName(oobDataName);
		test.Printf(_L("Local name set, error %d\n"), err);
		}
	else
		{
		test.Printf(_L("n\n"));
		}
	
	
	test.Printf(_L("Dump OOB to file? (y)"));
	key = test.Getch();
	
	while(key != 'y' && key != 'n' && key != EKeyEnter)
		{
		key = test.Getch();
		}
	
	test.Printf(_L("%c%c%c"), EKeyBackspace, EKeyBackspace, EKeyBackspace);
	
	if(key == EKeyEnter || key == 'y')
		{
		RFs fs;
		err = fs.Connect();
		if(err == KErrNone)
			{
			RFile oobDump;
			_LIT(KName, "C:\\oobdatadump.bin");
			err = oobDump.Create(fs, KName(), EFileShareAny|EFileWrite);
			if(err == KErrNone)
				{
				TBuf8<32> oobData;
				for(TUint i = 0; i < KBluetoothSimplePairingHashSize; i++)
					{
					oobData.Append(hash[i]);
					}
				for(TUint i = 0; i < KBluetoothSimplePairingRandomizerSize; i++)
					{
					oobData.Append(randomizer[i]);
					}
				}
			oobDump.Close();
			fs.Close();
			}
		}
	
	return err;
	}

void CBTTestConsole::RefreshLocalOobData()
	{
	if (!iPairingServerAvailable)
		{
		test.Printf(_L("Pairing Server is not available\n"));
		return;
		}
	iOobData.RefreshLocalOobData();
	test.Printf(_L("Local OOB Data Refreshed."));
	}	

TInt CBTTestConsole::ProvideRemoteOobData()
	{
	if (!iPairingServerAvailable)
		{
		test.Printf(_L("Pairing Server is not available\n"));
		return KErrNotSupported;
		}
	test.Printf(_L("Enter Bluetooth Address: 0x"));
	TKeyCode key;
	
	TBuf<12> addrBuf;
	
	while((key = test.Getch())!=EKeyEnter)
		{
		if(key == EKeyBackspace&&addrBuf.Length()!=0)
			{
			addrBuf.SetLength(addrBuf.Length()-1);
			test.Printf(_L("%c"), key);
			}
		else if ( addrBuf.Length() < addrBuf.MaxLength())
			{
			addrBuf.Append(key);
			test.Printf(_L("%c"), key);
			}
		}
	test.Printf(_L("\n"));
	TBTDevAddr addr;
	TInt err = addr.SetReadable(addrBuf); 
	if (err < KErrNone)
		{
		test.Printf(_L("Couldn't parse address, error %d\n"), err);			
		return KErrNone;
		}
	
	test.Printf(_L("Enter Hash C: "));
	
	TBuf16<32> buf;
	
	while((key = test.Getch())!=EKeyEnter)
		{
		if(key == EKeyBackspace&&buf.Length()!=0)
			{
			buf.SetLength(buf.Length()-1);
			test.Printf(_L("%c"), key);
			}
		else if ( buf.Length()<buf.MaxLength())
			{
			buf.Append(key);
			test.Printf(_L("%c"), key);
			}		
		}
	test.Printf(_L("\n"));
	
	TBluetoothSimplePairingHash hash;
	hash.SetMax();
	TLex16 lex;
	
	if (buf.Length() != buf.MaxLength())
		{
		test.Printf(_L("Incorrect Length for hash\n"));
		return KErrNone;
		}
	for (TUint i = 0; i < KBluetoothSimplePairingHashSize; i++)
		{
		lex.Assign(buf.Mid(i*2, 2));
		TInt err = lex.Val(hash[i], EHex);
		if (err != KErrNone)
			{
			test.Printf(_L("Couldn't parse hash\n"));			
			return KErrNone;
			}
		}
	

	test.Printf(_L("Enter Randomizer R (leave blank if unavailable): "));
	buf.Zero();
	
	while((key = test.Getch())!=EKeyEnter)
		{
		if(key == EKeyBackspace&&buf.Length()!=0)
			{
			buf.SetLength(buf.Length()-1);
			test.Printf(_L("%c"), key);
			}
		else if ( buf.Length()<buf.MaxLength())
			{
			buf.Append(key);
			test.Printf(_L("%c"), key);
			}		
		}
	test.Printf(_L("\n"));

	if (buf.Length() == 0)
		{
		iOobData.ProvideRemoteOobData(addr, hash);
		}
	else
		{
		TBluetoothSimplePairingRandomizer randomizer;
		randomizer.SetMax();

	
		if (buf.Length() != buf.MaxLength())
			{
			test.Printf(_L("Incorrect Length for randomizer\n"));
			return KErrNone;
			}
		for (TUint i = 0; i < KBluetoothSimplePairingRandomizerSize; i++)
			{
			lex.Assign(buf.Mid(i*2, 2));
			TInt err = lex.Val(randomizer[i], EHex);
			if (err != KErrNone)
				{
				test.Printf(_L("Couldn't parse randomizer\n"));
				return KErrNone;
				}
			}
		iOobData.ProvideRemoteOobData(addr, hash, randomizer);
		}
	
	
	return KErrNone;
	}

TInt CBTTestConsole::ProvideRemoteOobDataFromRNR()
	{
	if (!iPairingServerAvailable)
		{
		test.Printf(_L("Pairing Server is not available\n"));
		return KErrNotSupported;
		}
	test.Printf(_L("Enter Bluetooth Address: 0x"));
	TKeyCode key;
	
	TBuf<12> addrBuf;
	
	while((key = test.Getch())!=EKeyEnter)
		{
		if(key == EKeyBackspace&&addrBuf.Length()!=0)
			{
			addrBuf.SetLength(addrBuf.Length()-1);
			test.Printf(_L("%c"), key);
			}
		else if ( addrBuf.Length() < addrBuf.MaxLength())
			{
			addrBuf.Append(key);
			test.Printf(_L("%c"), key);
			}
		}
	test.Printf(_L("\n"));
	TBTDevAddr addr;
	TInt err = addr.SetReadable(addrBuf); 
	if (err < KErrNone)
		{
		test.Printf(_L("Couldn't parse address, error %d\n"), err);			
		return KErrNone;
		}
	
	TNameEntry name;
	
	TInquirySockAddr inqAddr;
	inqAddr.SetBTAddr(addr);
	inqAddr.SetAction(KHostResName|KHostResIgnoreCache);
	
	iHostResolver.GetByAddress(inqAddr, name);
	
	if (name().iName.Length() != 32 && name().iName.Length() != 64)
		{
		test.Printf(_L("Got invalid host name length: %d\n"), name().iName.Length());
		return KErrNone;
		}
	
	
	TBluetoothSimplePairingHash hash;
	hash.SetMax();

	TLex lex;
	
	for (TUint i = 0; i < hash.Length(); i++)
		{
		lex.Assign(name().iName.Mid(i * 2, 2));
		TInt err = lex.Val(hash[i], EHex);
		if (err != KErrNone)
			{
			test.Printf(_L("Couldn't parse hash, error %d"), err);
			}
		}
	
	test.Printf(_L("Got Hash C: 0x"));
	for (TUint i = 0; i < hash.Length(); i++)
		{
		test.Printf(_L("%x"), hash[i]);
		}
	test.Printf(_L("\n"));
	
	
	if (name().iName.Length() == 64)
		{
		TBluetoothSimplePairingRandomizer randomizer;
		
		randomizer.SetMax();
		
		for (TUint i = 0; i < randomizer.Length(); i++)
			{
			lex.Assign(name().iName.Mid(i * 2 + 32, 2));
			TInt err = lex.Val(randomizer[i], EHex);
			if (err != KErrNone)
				{
				test.Printf(_L("Couldn't parse randomizer, error %d"), err);
				}
			}

		
		test.Printf(_L("Got Randomizer R: 0x"));
		for (TUint i = 0; i < randomizer.Length(); i++)
			{
			test.Printf(_L("%x"), randomizer[i]);
			}
		test.Printf(_L("\n"));
		iOobData.ProvideRemoteOobData(addr, hash, randomizer);
		}
	else
		{
		iOobData.ProvideRemoteOobData(addr, hash);
		}
	
	
	return KErrNone;
	}

TInt CBTTestConsole::DedicatedBond()
	{
	if (!iPairingServerAvailable)
		{
		test.Printf(_L("Pairing Server isn't available\n"));
		return KErrNotSupported;
		}
	test.Printf(_L("Enter Bluetooth Address: 0x"));
	TKeyCode key;
	
	TBuf<12> addrBuf;
	
	while((key = test.Getch())!=EKeyEnter)
		{
		if(key == EKeyBackspace&&addrBuf.Length()!=0)
			{
			addrBuf.SetLength(addrBuf.Length()-1);
			test.Printf(_L("%c"), key);
			}
		else if ( addrBuf.Length() < addrBuf.MaxLength())
			{
			addrBuf.Append(key);
			test.Printf(_L("%c"), key);
			}
		}
	test.Printf(_L("\n"));
	TBTDevAddr addr;
	TInt err = addr.SetReadable(addrBuf); 
	if (err < KErrNone)
		{
		test.Printf(_L("Couldn't parse address, error %d\n"), err);
		return KErrNone;
		}
	
	TRequestStatus status;
	RBluetoothDedicatedBondingInitiator bonder;
	bonder.Start(iPairingServer, addr, status);
	User::WaitForRequest(status);
	bonder.Close();
	
	test.Printf(_L("Bonding completed, error %d"), status.Int());
	
	return KErrNone;
	}

void CBTTestConsole::SetDiscoverability(TBool aLimitedDiscoverability)
	{
	TInt err;
	RProperty property;
	// Ensure the property is defined (if not already)...
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);	
	err=property.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetLimitedDiscoverableStatus,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy
										
	if (err && (err != KErrAlreadyExists) )
		{
		test.Printf(_L("\nError %d defining property, continuing anyway\n"),err);
		}
		
	err=property.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, aLimitedDiscoverability);
	
	test.Printf(_L("\nDiscoverability set: Error %d \n"),err);
	}

void CBTTestConsole::SetScanMode(THCIScanEnable aScanMode)
	{
	TInt err;
	RProperty property;
	// Ensure the property is defined (if not already)...
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	err=property.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetScanningStatus,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy
										
	if (err && (err != KErrAlreadyExists) )
		{
		test.Printf(_L("\nError %d defining property, continuing anyway\n"),err);
		}
		
	err=property.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, aScanMode);
	
	test.Printf(_L("\nScan mode set: Error %d \n"),err);
	}
	
void CBTTestConsole::SetPairedConnectionsOnly(TBool aPairedConnectionsOnly)
	{
	TInt err;
	RProperty property;
	// Ensure the property is defined (if not already)...
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	err=property.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetAcceptPairedOnlyMode,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy

	if (err && (err != KErrAlreadyExists) )
		{
		test.Printf(_L("\nError %d defining property, continuing anyway\n"),err);
		}
		
	err=property.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetAcceptPairedOnlyMode, aPairedConnectionsOnly);
	
	test.Printf(_L("\nPaired connections only set: Error %d \n"),err);
	}

void CBTTestConsole::SetDebugMode()
	{
	TInt err;
	RProperty property;
	// Ensure the property is defined (if not already)...
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	err=property.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetSimplePairingDebugMode,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy

	if (err && (err != KErrAlreadyExists) )
		{
		test.Printf(_L("\nError %d defining property, continuing anyway\n"),err);
		}
		
	err=property.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetSimplePairingDebugMode, ETrue);
	
	test.Printf(_L("\nDebug Mode set: Error %d \n"),err);
	}

// ----------------------------------------------------------------------------
//							DEBUG METHODS
// ----------------------------------------------------------------------------
#ifdef _DEBUG
TInt CBTTestConsole::DebugMenu(TDebugMenu::TDebugMenuType aInfoType, TUint16 aBasicValue)
	{
	TInt err = KErrNone;
	TBool redraw = ETrue;

	switch(aInfoType)
		{
		case TDebugMenu::EAllocations:
			err = DebugAllocations();
			redraw = EFalse;
			break;

		case TDebugMenu::ESetPDUTimerCID:
			err = iProperty.Set(KPropertyUidBluetoothControlCategory,
								KPropertyKeyBluetoothL2DBGSetTimerGrpCID,
								aBasicValue);
			break;
			
		case TDebugMenu::EGetPDUTimers:
			err = DebugGetPDUTimers();
			redraw = EFalse;
			break;
			
		case TDebugMenu::EGetFlushCounters:
			err = DebugGetFlushCounters();
			redraw = EFalse;
			break;
			
		case TDebugMenu::EIssueManualFlush:
			err = IssueManualFlush();
			break;
			
		case TDebugMenu::EGetCurrentDataIndex:
			err = GetCurrentDataIndex();
			redraw = EFalse;
			break;

		case TDebugMenu::ECurrentMemoryInfo:
			err = CurrentMemoryInfo();
			redraw = EFalse;
			break;
			
		case TDebugMenu::EDebugMark:
			DebugMark();
			break;

		case TDebugMenu::EDebugMarkEnd:
			DebugMarkEnd();
			break;
		
		case TDebugMenu::EDataPlaneConfig:
			err = DataPlaneConfig();
			redraw = EFalse;
			break;

		default:
			test.Printf(_L("\n\nPlease select a valid option.\n"));
			redraw = EFalse;
			break;
		};
			
	if(err != KErrNone)
		{
		test.Printf(_L("\n\nDEBUG MENU ERROR [%d]\n"), err);
		redraw = EFalse;
		}
	return redraw;
	}

TInt CBTTestConsole::DefinePublications()
	{
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);	
	return iProperty.Define(KPropertyKeyBluetoothL2DBGSetTimerGrpCID,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy);
	}
	
void CBTTestConsole::DeletePublications()
	{
	iProperty.Delete(KL2CAPSID,
					 KPropertyKeyBluetoothL2DBGSetTimerGrpCID);
	}

TInt CBTTestConsole::DebugAllocations()
	{
	_LIT(KPDUBase,					"PDU-Bs");
	_LIT(KPDUBFrame,				"PDU-BF");
	_LIT(KPDUIFrame,				"PDU-IF");
	_LIT(KPDUSFrame,				"PDU-SF");
	_LIT(KPDUCFrame,				"PDU-CF");
	_LIT(KPDUGFrame,				"PDU-GF");
	_LIT(KPDUDataFrame,				"PDU-DF");
	_LIT(KPDUFragFrame,				"PDU-FF");
	_LIT(KSDU,						"SDU-Bs");
	_LIT(KMuxes,					"Muxes ");
	_LIT(KSAPs,						"SAPs  ");
	_LIT(KSigHs,					"Sig-Hd");
	_LIT(KDataP,					"Data-P");
	_LIT(KL2CapCommands,			"L2Cmds");

	const TUint8 allocationTypes = 14;
	const TDesC* allocTypeName[allocationTypes] = {
		&KPDUBase,
		&KPDUBFrame,
		&KPDUIFrame,
		&KPDUSFrame,
		&KPDUCFrame,
		&KPDUGFrame,
		&KPDUDataFrame,
		&KPDUFragFrame,
		&KSDU,
		&KMuxes,
		&KSAPs,
		&KSigHs,
		&KDataP,
		&KL2CapCommands,
		};

	TInt allocated = 0;	
	TInt deleted = 0;
	TInt peak = 0;

	TInt rerr = KErrNone;
	
	test.Printf(_L("\nAllocation Debug Info\n"));
	for(TInt allocType=0;allocType<allocationTypes;allocType++)
		{
		rerr = iProperty.Get(KL2CAPSID,
							 KPropertyKeyBluetoothL2CapDebugBase + (allocType * 0x10),
							 allocated);			
		rerr = iProperty.Get(KL2CAPSID,
							 KPropertyKeyBluetoothL2CapDebugBase + (allocType * 0x10) + 1,
							 deleted);			
		rerr = iProperty.Get(KL2CAPSID,
							 KPropertyKeyBluetoothL2CapDebugBase + (allocType * 0x10) + 2,
							 peak);			

		test.Printf(_L("%S - Al[%5d], Dl[%5d], Pk[%5d]\n"), 
					   allocTypeName[allocType], allocated, deleted, peak);
		}
	return rerr;
	}	

TInt CBTTestConsole::DebugGetPDUTimers()
	{
	TInt rerr = KErrNone;

	TInt cid = 0;	
	TInt minGetTimer = 0;
	TInt maxGetTimer = 0;
	TInt minSentTimer = 0;
	TInt maxSentTimer = 0;
	TInt aveGetTimer = 0;
	TInt aveSentTimer = 0;
	
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGGetTimerGrpCID,
						 cid);			

	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGMinPDUGetTime,
						 minGetTimer);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGMaxPDUGetTime,
						 maxGetTimer);			

	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGMinPDUSentTime,
						 minSentTimer);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGMaxPDUSentTime,
						 maxSentTimer);			

	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGAvePDUGetTime,
						 aveGetTimer);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothL2DBGAvePDUSentTime,
						 aveSentTimer);			

	test.Printf(_L("\nPDU Timer Debug Info\n"));
	test.Printf(_L("CID[%4d] - GET: Mx[%5d], Mn[%5d], Av[%5d]\n"), 
				   cid, maxGetTimer, minGetTimer, aveGetTimer);
	test.Printf(_L("           SENT: Mx[%5d], Mn[%5d], Av[%5d]\n"), 
				   maxSentTimer, minSentTimer, aveSentTimer);

	return rerr;	
	}

TInt CBTTestConsole::DebugGetFlushCounters()
	{
	TInt rerr = KErrNone;
	TInt queuedSDUFlushedCounter = 0;
	TInt partialSentSDUFlushedCounter = 0;
	TInt sentSDUFlushedCounter = 0;
	TInt pduMuxerResends = 0;
	TInt pduDataFlowResends = 0;
	TInt pduIFramesFlushed = 0;
	
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothQueuedSDUFlushedCounter,
						 queuedSDUFlushedCounter);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothPartialSentSDUFlushedCounter,
						 partialSentSDUFlushedCounter);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothSentSDUFlushedCounter,
						 sentSDUFlushedCounter);			

	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothFlushedPDUMuxerResend,
						 pduMuxerResends);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothFlushedPDUDataFlowResend,
						 pduDataFlowResends);			
	rerr = iProperty.Get(KL2CAPSID,
						 KPropertyKeyBluetoothFlushedPDUIFrame,
						 pduIFramesFlushed);			

	test.Printf(_L("\nFlush Counter Debug Info\n"));
	test.Printf(_L("Queued SDU[%d], Partial Sent SDU[%d], Sent SDU[%d]\n"), 
				   queuedSDUFlushedCounter, partialSentSDUFlushedCounter, sentSDUFlushedCounter);
	test.Printf(_L("Muxer Resends[%d], FEC Resends[%d]\n"), 
				   pduMuxerResends, pduDataFlowResends);
	test.Printf(_L("I-Frames Flushed[%d]\n"), 
				   pduIFramesFlushed);

	return rerr;		
	}

TInt CBTTestConsole::IssueManualFlush()
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr = iTestConn[selConn]->IssueManualFlush();
		}
	return rerr;
	}

TInt CBTTestConsole::GetCurrentDataIndex()
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		test.Printf(_L("\nConnection[%d] - Data Index = %d."), selConn, iTestConn[selConn]->iDataDialogCount);
		}
	return rerr;
	}

TInt CBTTestConsole::CurrentMemoryInfo()
	{
	// MBufs
	test.Printf(_L("\n\nTotal MBufs - %d\n"), iSocketServer.__DbgMbufTotalSpace());
	test.Printf(_L("Free MBufs - %d\n"), iSocketServer.__DbgMbufFreeSpace());
	test.Printf(_L("Used MBufs - %d\n"), iSocketServer.__DbgMbufTotalSpace() - iSocketServer.__DbgMbufFreeSpace());

	TProtocolDesc protocolInfo;
	TInt rerr = iSocketServer.FindProtocol(_L("L2CAP"), protocolInfo);
	if(rerr == KErrNone)
		{
		RSocket tempSocket;
		rerr = tempSocket.Open(iSocketServer, KBTAddrFamily, 
							   protocolInfo.iSockType,
							   protocolInfo.iProtocol);
		if(rerr == KErrNone)
			{
			TInt value = 0;
			TInt rerr = tempSocket.GetOpt(KL2CAPHeapAlloc, KSolBtL2CAP, value);
		
			if(rerr == KErrNone)
				{
				test.Printf(_L("Heap Alloc = %d.\n"), value);
				}
			tempSocket.Close();
			}
		}
	if(rerr != KErrNone)
		{
		test.Printf(_L("Heap Alloc FAILED: Error %d.\n"), rerr);
		}
	return KErrNone;
	}
	
TInt CBTTestConsole::DataPlaneConfig()
	{
	TInt rerr = KErrNone;
	TInt selConn = iMainMenu->GetSelectedConn();
	if(!ConnInState(selConn, TConnProfile::EConnConnected))
		{
		test.Printf(_L("\n\nPlease select a valid connection."));
		rerr = KErrGeneral;
		}
	else
		{
		rerr = iTestConn[selConn]->DataPlaneConfig();
		}
	return rerr;
	}

void CBTTestConsole::DebugMark()
	{
	TInt rcode = iSocketServer.__DbgMarkHeap();
	test.Printf(_L("\n\nDebug Mark: Error %d\n"), rcode);
	}

void CBTTestConsole::DebugMarkEnd()
	{
	TInt rcode = iSocketServer.__DbgMarkEnd(0);
	test.Printf(_L("\n\nDebug Mark End: Error %d\n"), rcode);
	}

#endif	



// ----------------------------------------------------------------------------
//							Active Console
// ----------------------------------------------------------------------------


CActiveConsole::CActiveConsole(CBTTestConsole& aTestConsole)
: CActive(EPriorityStandard), iTestConsole(aTestConsole)
	{
	}


CActiveConsole::~CActiveConsole()
	{
	// must cancel
	Cancel();
	}

CActiveConsole* CActiveConsole::NewL(CBTTestConsole& aTestConsole)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aTestConsole);
	CleanupStack::PushL(console);
	console->ConstructL();
	CleanupStack::Pop();
	return console;
	}

void CActiveConsole::ConstructL()
	{
	CActiveScheduler::Add(this);
	RequestKey();	
	}

void CActiveConsole::DoCancel()
	{
	iTestConsole.test.Console()->ReadCancel();
	}

void CActiveConsole::RequestKey()
	{
	iTestConsole.test.Console()->Read(iStatus);
	SetActive();
	}

void CActiveConsole::RunL()
	{
	// key has been pressed
	if(iTestConsole.ProcessKey())
		{
		RequestKey();
		}
	else
		{
		CActiveScheduler::Stop();
		}
	}



void RunConsoleL()
	{
	CBTTestConsole * btTestConsole = CBTTestConsole::NewLC();

	btTestConsole->test.Printf(_L("Bluetooth Protocols Loading..."));	
	btTestConsole->InitialiseL();
	btTestConsole->test.Printf(_L("Bluetooth Protocols Loaded."));

	btTestConsole->Start();
	CleanupStack::PopAndDestroy();
	}

TInt E32Main()
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);
	TRAPD(err,RunConsoleL());
	delete cleanup;
	return err;
	}
