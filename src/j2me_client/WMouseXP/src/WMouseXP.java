/*
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
*/


import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

/**
 *
 * @author Administrator
 */
public class WMouseXP extends MIDlet implements CommandListener {
    
    /** Creates a new instance of HelloMidlet */
    MainCanvas maincanvas;
    public WMouseXP() {
        curInstance = this;
        
        if(checBTAPISupport())
        {
        maincanvas = new MainCanvas(false);        
        Display.getDisplay(this).setCurrent(maincanvas);
        }
        else
        {
        Display.getDisplay(this).setCurrent(get_NoBTAPIForm());
        }
    }
    
    private Command exitCommand;//GEN-BEGIN:MVDFields
    private Form NoBTForm;
    private StringItem stringItem1;
    private Command okCommand2;
    private Form BTOffForm;
    private StringItem stringItem2;
    private Form StartFailedForm;
    private StringItem stringItem3;
    private Command helpCommand;
    private Form helpForm;
    private StringItem stringItem4;
    private Form NoBTAPIForm;
    private StringItem stringItem8;
    private Form PrevNotClosedForm;
    private StringItem stringItem5;//GEN-END:MVDFields
    public static WMouseXP curInstance;
//GEN-LINE:MVDMethods

    /** This method initializes UI of the application.//GEN-BEGIN:MVDInitBegin
     */
    private void initialize() {//GEN-END:MVDInitBegin
        // Insert pre-init code here
//GEN-LINE:MVDInitInit
        // Insert post-init code here
    }//GEN-LINE:MVDInitEnd
    
    /** Called by the system to indicate that a command has been invoked on a particular displayable.//GEN-BEGIN:MVDCABegin
     * @param command the Command that ws invoked
     * @param displayable the Displayable on which the command was invoked
     */
    public void commandAction(Command command, Displayable displayable) {//GEN-END:MVDCABegin
        // Insert global pre-action code here
        if (displayable == NoBTForm) {//GEN-BEGIN:MVDCABody
            if (command == okCommand2) {//GEN-END:MVDCABody
                // Insert pre-action code here
                exitMIDlet();//GEN-LINE:MVDCAAction15
                // Insert post-action code here
            }//GEN-BEGIN:MVDCACase15
        } else if (displayable == BTOffForm) {
            if (command == okCommand2) {//GEN-END:MVDCACase15
                // Insert pre-action code here
                exitMIDlet();//GEN-LINE:MVDCAAction18
                //Display.getDisplay(this).setCurrent(maincanvas);
            }//GEN-BEGIN:MVDCACase18
        } else if (displayable == StartFailedForm) {
            if (command == okCommand2) {//GEN-END:MVDCACase18
                // Insert pre-action code here
                exitMIDlet();//GEN-LINE:MVDCAAction21
                // Insert post-action code here
            }//GEN-BEGIN:MVDCACase21
        } else if (displayable == helpForm) {
            if (command == okCommand2) {//GEN-END:MVDCACase21
                Display.getDisplay(this).setCurrent(maincanvas);
                // Do nothing//GEN-LINE:MVDCAAction29
                // Insert post-action code here
            }//GEN-BEGIN:MVDCACase29
        } else if (displayable == NoBTAPIForm) {
            if (command == exitCommand) {//GEN-END:MVDCACase29
                // Insert pre-action code here
                exitMIDlet();//GEN-LINE:MVDCAAction45
                // Insert post-action code here
            }//GEN-BEGIN:MVDCACase45
        } else if (displayable == PrevNotClosedForm) {
            if (command == okCommand2) {//GEN-END:MVDCACase45
                // Insert pre-action code here
                exitMIDlet();//GEN-LINE:MVDCAAction48
                // Insert post-action code here
            }//GEN-BEGIN:MVDCACase48
        }//GEN-END:MVDCACase48
        // Insert global post-action code here
        
         if (command == exitCommand) { 
              exitMIDlet();
         }     
}//GEN-LINE:MVDCAEnd
    
    /**
     * This method should return an instance of the display.
     */
    public Display getDisplay() {//GEN-FIRST:MVDGetDisplay
        return Display.getDisplay(this);
    }//GEN-LAST:MVDGetDisplay
    
    /**
     * This method should exit the midlet.
     */
    public void exitMIDlet() {//GEN-FIRST:MVDExitMidlet
        getDisplay().setCurrent(null);
        destroyApp(true);
        curInstance = null;
        notifyDestroyed();        
    }//GEN-LAST:MVDExitMidlet
    
    
    
    /** This method returns instance for exitCommand component and should be called instead of accessing exitCommand field directly.//GEN-BEGIN:MVDGetBegin5
     * @return Instance for exitCommand component
     */
    public Command get_exitCommand() {
        if (exitCommand == null) {//GEN-END:MVDGetBegin5
            // Insert pre-init code here
            exitCommand = new Command("Exit", Command.EXIT, 1);//GEN-LINE:MVDGetInit5
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd5
        return exitCommand;
    }//GEN-END:MVDGetEnd5
    /** This method returns instance for NoBTForm component and should be called instead of accessing NoBTForm field directly.//GEN-BEGIN:MVDGetBegin11
     * @return Instance for NoBTForm component
     */
    public Form get_NoBTForm() {
        if (NoBTForm == null) {//GEN-END:MVDGetBegin11
            // Insert pre-init code here
            NoBTForm = new Form(null, new Item[] {get_stringItem1()});//GEN-BEGIN:MVDGetInit11
            NoBTForm.addCommand(get_okCommand2());
            NoBTForm.setCommandListener(this);//GEN-END:MVDGetInit11
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd11
        return NoBTForm;
    }//GEN-END:MVDGetEnd11

    /** This method returns instance for stringItem1 component and should be called instead of accessing stringItem1 field directly.//GEN-BEGIN:MVDGetBegin12
     * @return Instance for stringItem1 component
     */
    public StringItem get_stringItem1() {
        if (stringItem1 == null) {//GEN-END:MVDGetBegin12
            // Insert pre-init code here
            stringItem1 = new StringItem("Can\'t access Bluetooth", " You must press \"Yes\" (left softkey) when your phone asks for connectivity permission, to allow Bluetooth access for WMouseXP.\n\nPlease press OK to exit, then start WMouseXP again.");//GEN-LINE:MVDGetInit12
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd12
        return stringItem1;
    }//GEN-END:MVDGetEnd12

    /** This method returns instance for okCommand2 component and should be called instead of accessing okCommand2 field directly.//GEN-BEGIN:MVDGetBegin14
     * @return Instance for okCommand2 component
     */
    public Command get_okCommand2() {
        if (okCommand2 == null) {//GEN-END:MVDGetBegin14
            // Insert pre-init code here
            okCommand2 = new Command("OK", Command.BACK, 5);//GEN-LINE:MVDGetInit14
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd14
        return okCommand2;
    }//GEN-END:MVDGetEnd14

    /** This method returns instance for BTOffForm component and should be called instead of accessing BTOffForm field directly.//GEN-BEGIN:MVDGetBegin16
     * @return Instance for BTOffForm component
     */
    public Form get_BTOffForm() {
        if (BTOffForm == null) {//GEN-END:MVDGetBegin16
            // Insert pre-init code here
            BTOffForm = new Form(null, new Item[] {get_stringItem2()});//GEN-BEGIN:MVDGetInit16
            BTOffForm.addCommand(get_okCommand2());
            BTOffForm.setCommandListener(this);//GEN-END:MVDGetInit16
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd16
        return BTOffForm;
    }//GEN-END:MVDGetEnd16

    /** This method returns instance for stringItem2 component and should be called instead of accessing stringItem2 field directly.//GEN-BEGIN:MVDGetBegin17
     * @return Instance for stringItem2 component
     */
    public StringItem get_stringItem2() {
        if (stringItem2 == null) {//GEN-END:MVDGetBegin17
            // Insert pre-init code here
            stringItem2 = new StringItem("Bluetooth might be OFF", "Please turn ON your phone\'s Bluetooth.\n\nPlease press OK to exit, then start WMouseXP again.");//GEN-LINE:MVDGetInit17
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd17
        return stringItem2;
    }//GEN-END:MVDGetEnd17

    /** This method returns instance for StartFailedForm component and should be called instead of accessing StartFailedForm field directly.//GEN-BEGIN:MVDGetBegin19
     * @return Instance for StartFailedForm component
     */
    public Form get_StartFailedForm() {
        if (StartFailedForm == null) {//GEN-END:MVDGetBegin19
            // Insert pre-init code here
            StartFailedForm = new Form(null, new Item[] {get_stringItem3()});//GEN-BEGIN:MVDGetInit19
            StartFailedForm.addCommand(get_okCommand2());
            StartFailedForm.setCommandListener(this);//GEN-END:MVDGetInit19
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd19
        return StartFailedForm;
    }//GEN-END:MVDGetEnd19

    /** This method returns instance for stringItem3 component and should be called instead of accessing stringItem3 field directly.//GEN-BEGIN:MVDGetBegin20
     * @return Instance for stringItem3 component
     */
    public StringItem get_stringItem3() {
        if (stringItem3 == null) {//GEN-END:MVDGetBegin20
            // Insert pre-init code here
            stringItem3 = new StringItem("Start Failed", " You must press \"Yes\" (left softkey) when your phone asks for connectivity permission, to allow Bluetooth access for WMouseXP.\n\nPlease press OK to exit, then start WMouseXP again.");//GEN-LINE:MVDGetInit20
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd20
        return stringItem3;
    }//GEN-END:MVDGetEnd20

    /** This method returns instance for helpCommand component and should be called instead of accessing helpCommand field directly.//GEN-BEGIN:MVDGetBegin23
     * @return Instance for helpCommand component
     */
    public Command get_helpCommand() {
        if (helpCommand == null) {//GEN-END:MVDGetBegin23
            // Insert pre-init code here
            helpCommand = new Command("Help", Command.HELP, 6);//GEN-LINE:MVDGetInit23
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd23
        return helpCommand;
    }//GEN-END:MVDGetEnd23
 
    /** This method returns instance for helpForm component and should be called instead of accessing helpForm field directly.//GEN-BEGIN:MVDGetBegin25
     * @return Instance for helpForm component
     */
    public Form get_helpForm() {
        if (helpForm == null) {//GEN-END:MVDGetBegin25
            // Insert pre-init code here
            helpForm = new Form(null, new Item[] {get_stringItem4()});//GEN-BEGIN:MVDGetInit25
            helpForm.addCommand(get_okCommand2());
            helpForm.setCommandListener(this);//GEN-END:MVDGetInit25
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd25
        return helpForm;
    }//GEN-END:MVDGetEnd25
 
    /** This method returns instance for stringItem4 component and should be called instead of accessing stringItem4 field directly.//GEN-BEGIN:MVDGetBegin27
     * @return Instance for stringItem4 component
     */
    public StringItem get_stringItem4() {
        if (stringItem4 == null) {//GEN-END:MVDGetBegin27
            // Insert pre-init code here
            stringItem4 = new StringItem("Help", "Use your mobile phone to control your PC: Mouse, Presentation & Media Player.\n\nTo use WMouseXP:\n1. On your PC, download the PC-Side WMouseXP from www.ClearEvo.com, install it.\n2. Start this mobile program, allow it to access Bluetooth, until you see \"Awaiting PC Side\".\n3. Start the WMouseXP PC Side, press \"Connect\".\n\n\nJoystick: Use it to move your PC\'s mouse. Click joystick to click on PC (moves PowerPoint/Presentation to \"Next slide\" also).\n\nNumpad commands:\n1: Exit the presentation (Esc).\n2: Previous slide (Left Arrow).\n3: Start presentation (F5).\n4: Launch \"My Computer\".\n5: Launch Media Player.\n6: Mute \n7: Stop\n8: Play\n9: Increase Volume\n*: Previous Track\n0: Next Track\n#: Decrease Volume\n\nFor further info please visit: www.ClearEvo.com\n\nWMouseXP 3.0 Copyright (c) 2007 Kasidit Yusuf. All rights reserved. WMouseXP is FREE SOFTWARE - GNU GPL - visit website for source code.");//GEN-LINE:MVDGetInit27
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd27
        return stringItem4;
    }//GEN-END:MVDGetEnd27
   
 
   
    /** This method returns instance for NoBTAPIForm component and should be called instead of accessing NoBTAPIForm field directly.//GEN-BEGIN:MVDGetBegin43
     * @return Instance for NoBTAPIForm component
     */
    public Form get_NoBTAPIForm() {
        if (NoBTAPIForm == null) {//GEN-END:MVDGetBegin43
            // Insert pre-init code here
            NoBTAPIForm = new Form(null, new Item[] {get_stringItem8()});//GEN-BEGIN:MVDGetInit43
            NoBTAPIForm.addCommand(get_exitCommand());
            NoBTAPIForm.setCommandListener(this);//GEN-END:MVDGetInit43
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd43
        return NoBTAPIForm;
    }//GEN-END:MVDGetEnd43

    /** This method returns instance for stringItem8 component and should be called instead of accessing stringItem8 field directly.//GEN-BEGIN:MVDGetBegin44
     * @return Instance for stringItem8 component
     */
    public StringItem get_stringItem8() {
        if (stringItem8 == null) {//GEN-END:MVDGetBegin44
            // Insert pre-init code here
            stringItem8 = new StringItem("Not Compatible", "Unfortunately, this phone does not support the Java Bluetooth API (JSR-82). Therefore, you can\'t use WMouseXP on this phone.");//GEN-LINE:MVDGetInit44
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd44
        return stringItem8;
    }//GEN-END:MVDGetEnd44

    /** This method returns instance for PrevNotClosedForm component and should be called instead of accessing PrevNotClosedForm field directly.//GEN-BEGIN:MVDGetBegin46
     * @return Instance for PrevNotClosedForm component
     */
    public Form get_PrevNotClosedForm() {
        if (PrevNotClosedForm == null) {//GEN-END:MVDGetBegin46
            // Insert pre-init code here
            PrevNotClosedForm = new Form(null, new Item[] {get_stringItem5()});//GEN-BEGIN:MVDGetInit46
            PrevNotClosedForm.addCommand(get_okCommand2());
            PrevNotClosedForm.setCommandListener(this);//GEN-END:MVDGetInit46
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd46
        return PrevNotClosedForm;
    }//GEN-END:MVDGetEnd46

    /** This method returns instance for stringItem5 component and should be called instead of accessing stringItem5 field directly.//GEN-BEGIN:MVDGetBegin47
     * @return Instance for stringItem5 component
     */
    public StringItem get_stringItem5() {
        if (stringItem5 == null) {//GEN-END:MVDGetBegin47
            // Insert pre-init code here
            stringItem5 = new StringItem("Failed to close previous session", "Failed to close previous session. Please press OK to exit and try re-start your phone.");//GEN-LINE:MVDGetInit47
            // Insert post-init code here
        }//GEN-BEGIN:MVDGetEnd47
        return stringItem5;
    }//GEN-END:MVDGetEnd47
    
    public void startApp() {
        initialize();
    }
    
    public void pauseApp() {
    }
    
    public void destroyApp(boolean unconditional) {
        
        if(maincanvas!=null)
            maincanvas.exitCommandedFromApp();//cleanup and don't restart the service        
    }
   
    public static boolean checBTAPISupport()
    {
        //jpgsupport = false;
        try{
            Class c = null;
            c = Class.forName("javax.bluetooth.DiscoveryAgent");                        
            if(c!=null)           
            c = Class.forName("javax.bluetooth.LocalDevice");
            
            if(c==null)
                return false;                   
           }
        catch(Exception e)
            {
            return false;
            }
        
        return true;
    }
}
