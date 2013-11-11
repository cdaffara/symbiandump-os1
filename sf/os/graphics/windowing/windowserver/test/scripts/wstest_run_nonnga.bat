@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@echo on

testexecute.exe z:\wstest\wstest_t_alpha_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_alpha_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_alphawin_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_alphawin_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_anim_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_anim_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_client_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_client_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_crp_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_crp_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_cursor_n_sprite_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_cursor_n_sprite_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_draw_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_draw_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_dsa_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_dsa_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_event_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_event_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_fade_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_fade_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_graphicsresource_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_graphicsresource_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_key_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_key_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_oom_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_oom_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_panic_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_panic_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_pointer_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_pointer_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_redraw_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_redraw_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_screen_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_screen_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_screendevice_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_screendevice_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_screendeviceresswitch_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_screendeviceresswitch_s1_nonnga.script

testexecute.exe z:\wstest\wstest_t_security_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_security_s1_nonnga.script


testexecute.exe z:\wstest\wstest_t_window_s0_nonnga.script
testexecute.exe z:\wstest\wstest_t_window_s1_nonnga.script

@echo off
md e:\logs
md e:\logs\testexecute

copy c:\logs\testexecute\wstest_t_*.htm e:\logs\testexecute\
