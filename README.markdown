WMouseXP Bluetooth Presentation Remote Control Software
=====
WMouseXP turns compatible java-enabled and S60 phones into a wireless presenter-mouse and media player remote-control for your computer/notebook. For Windows XP.

![WMouseXP PC-Side](http://www.clearevo.com/wmousexp/WMouseXP_bluetooth_presenter_remote.jpg)

![WMouseXP Mobile-Side](http://www.clearevo.com/wmousexp/WMouseXP_j2me_bluetooth_remote.gif)

How to start using
======

- Download WMouseXP from <http://github.com/ykasidit/WMouseXP/archives/master>.

- Extract the ZIP archive somewhere on your machine

- Double-click Start_WMouseXP.bat

- Start your computer's Bluetooth. Make sure your PC's Bluetooth is plugged-in & started in the system-tray. (or if your Notebook has internal Bluetooth - start it using its assigned Fn+ bluetooth button or hardware bluetooth/wireless button)

*Note: WMouseXP is compatible with PC Bluetooth running on Toshiba, BlueSoleil, Microsoft or Broadcom/WIDCOMM Blueooth Stacks/Drivers (ask your computer store about that).*

- Double-click Start_WMouseXP.bat

- Press the "Send & Install on phone" button to send the Mobile-Side program to your mobile phone and install it there. (Make sure your phone's Bluetooth is ON and discoverable - "Show to all".)

*Note: If send fails (Motorola U9, Nokia 6230, etc), then open your phone's WAP browser to wap.ClearEvo.com and download/install WMouseXP on your phone from there.*

- On your phone, start the Mobile-Side WMouseXP program, allow it to access Bluetooth until you see "Awaiting PC Side".

*Note: On S60 2nd Ed phones , you MUST press the (left) soft key for "Yes" explicitly - because pressing the Joystick "in" would result in a default "No".*

- On your PC's WMouseXP, press the "Connect" button. Wait until the PC side automatically finds the mobile side on your phone, it would then connect and minimize itself - showing the "Wireless Control Active..." balloon.

**DONE! Move your phone's joystick and see your PC's mouse cursor magically move on your PC - try double-clicking, opening and closing stuff on your computer from your phone**

*Test it - Open a Powerpoint Presentation, press 3 on the phone to start the presentation, click/press the phone's joystick to go to the next step/slide, press 2 to go back, press 1 to exit.*

For a WIFI version: please try [AgilePresenter WIFI Remote](http://www.clearevo.com/agilepresenter/).

Please see <http://www.clearevo.com/wmousexp/> for more info.

License
=======
Starting from version 3.0 - WMouseXP is now FREE SOFTWARE - GNU GPL:

![GNU GPL](http://www.clearevo.com/incallert/gplv3-127x51.png)

Copyright (C) 2007-2010 Kasidit Yusuf.

This file is part of WMouseXP.

WMouseXP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WMouseXP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WMouseXP. If not, see <http://www.gnu.org/licenses/>.

Source code and compiling
=========================
Source code is in the "src" folder in the archive from <http://github.com/ykasidit/WMouseXP/archives/master>.

*Please note that the source code was written when I was just starting to learn programming - it might be low-standard in quality and/or convention.*

Unfortunately, at the time I started coding WMouseXP a long time back when I was much younger - didn't know about [Qt](http://qt.nokia.com) or [wxWidgets](http://www.wxwidgets.org) - I didn't know that you can write native GUI apps with drag-and-drop without using MS non-free/closed IDEs - and coded it in MFC using VC6 - didn't understand/realize the [free-software philosophy](http://www.gnu.org/philosophy/free-sw.html) at that time too.

I hope one day some helpful programmer or myself can port this app to Qt so we can easily port it to Ubuntu GNU/Linux - it should be somewhat easier by modifying Ookjor's source-code - <http://github.com/ykasidit/Ookjor> - and using its same bluetooth-engine: <http://wiki.github.com/ykasidit/PatienceBTEngine/>.

As for the mobile-side j2me client - use [Netbeans IDE](http://www.netbeans.org/) to compile/build it.

History
=======
In the past WMouseXP was a shareware program. Now, since my work has got much more deep and challenging - developing network-test app engines decoding GSM RR/CC/MM, WCDMA RRC, vendor-specific L1 packets, I came to learn much more from the Free-Software/Open-source world - and now mainly using Ubuntu GNU/Linux. Now I want to give my small contribution back to the society too - so I release it under GPL. 

Apart from that, I need much more focus on my work and peaceful rest on weekends. I don't want to have conflicts/distraction of "commercial"/demanding/committed interest/efforts with my job/specialization. Happiness comes whan you are satisfied with what you have - after working moderately/balanced - not too much/less - so I think it should be "enough" commercial work to focus on my job. 

Another point that touched me was solving licence activation problems (of the DRM anti-features) especially when the customer already bought the full-version serial and then can't use it because of the DRM - and has to mail the support, wait, work hard to try solutions, just to get over the DRM.

As now I don't have personal commercial/closed apps to distract me, I hope I can use more weekends time on learning GSM/WCDMA and related free-software/open-source projects: trying OsmocomBB, OpenBSC, etc. Also, use more time away from the computer (as I easily have neck-pain when using the computer after many years of coding and sitting too long) by jogging, cooking, travel and my home-brewed biodiesel hobby.


*All the best wishes,
Kasidit*
