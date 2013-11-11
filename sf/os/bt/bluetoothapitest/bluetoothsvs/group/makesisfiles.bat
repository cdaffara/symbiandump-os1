@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
copy ..\release\armv5\urel\T_BTSockAddrAPI.exe .
copy ..\release\armv5\urel\T_BTSdpAPI.exe .
copy ..\release\armv5\urel\T_BTSockAPI.exe .
copy ..\release\armv5\urel\T_BTUserAPI.exe .
copy ..\release\armv5\urel\t_btgavdpapi.exe .

call makesis t_btsockaddrapi.pkg t_btsockaddrapi.sis
call makesis t_btsdpapi.pkg t_btsdpapi.sis
call makesis t_btsockapi.pkg t_btsockapi.sis
call makesis t_btuserapi.pkg t_btuserapi.sis
call makesis t_btgavdpapi.pkg t_btgavdpapi.sis

call signsis -s t_btsockaddrapi.sis t_btsockaddrapi.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_btsockapi.sis t_btsockapi.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_btsdpapi.sis t_btsdpapi.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_btuserapi.sis t_btuserapi.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_btgavdpapi.sis t_btgavdpapi.sis zeus.cer SymbianACS.key caforstat
