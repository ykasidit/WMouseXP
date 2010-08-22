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


import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import javax.bluetooth.BluetoothStateException;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.LocalDevice;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.StreamConnectionNotifier;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
/*
 * MainCanvas.java
 *
 * Created on June 20, 2007, 8:55 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author Administrator
 */
public class MainCanvas extends GameCanvas {
    
    /** Creates a new instance of MainCanvas */
    String status;
    Font font,sfont,hintfont;
    
    StreamConnectionNotifier notifier;
    OutputStream iOs; 
    //InputStream iIs;
        
    byte[] sendbuf;
    
    
    public final int KStateIdle = 0;
    public final int KStateOponingReceiver = 1;
    public final int KStateAwaitingConnection = 2;
    public final int KStateConnected = 3;
    
    public final int KMyBirthdayDMMYYYY = 6051983;
    
    //not used yet////////////////////////////////
    public final int KCommandModeMediaPresenter = 1;
    public final int KCommandModeNumpadMode = 2;
    
    
    /*public final int KAlpha=3;
    public final int KNumeric=5;
    public final int KBrowser=7;*/
    public final int KMedia=11;
    //public final int KLauncher=13;
    
    
    
    public final int KE61SHIFT_DIFF=32;
    
    public final int KE61KEY_NUM1=114;//r
    public final int KE61KEY_NUM2=116;//t
    public final int KE61KEY_NUM3=121;//y
    public final int KE61KEY_NUM4=102;//f
    public final int KE61KEY_NUM5=103;//g
    public final int KE61KEY_NUM6=104;//h
    public final int KE61KEY_NUM7=118;//v
    public final int KE61KEY_NUM8=98;//b
    public final int KE61KEY_NUM9=110;//n
    
    public final int KE61KEY_STAR=117;//u    
    public final int KE61KEY_NUM0=109;//m
    public final int KE61KEY_POUND=106;//j //#
    
    public final int KE71KEY_NUM3_DE=122; //german num 3 is 'z'
    
    /////////////////////////////////////////////
    //future add scroller mode
    
//WMouseXPOpcodeMultipliers
public final int KKeyPressedCode=1;
public final int KKeyRepeatedCode=2;
public final int KKeyReleasedCode=4;
public final int KRequestSSPNG=5; //next 2 bytes tell width, next 2 tell height, next 1 tell zoomout
public final int KRequestSSJPG=6; //next 2 bytes tell width, next 2 tell height, next 1 tell zoomout


public final int KInternalCommandDontSendToPC = -1;		
public final int KUnknownCommand = 0;		
public final int KJoyUp = 1;		
public final int KJoyDown = 2;		
public final int KJoyLeft = 3;		
public final int KJoyRight = 4;
public final int KJoyIn = 5;
public final int KJoyLong = 6; //right click

public final int KEsc = 7; 
public final int KPrevSlide = 8;
public final int KStartPresentation = 9;

public final int KPrevTrack = 10; 
public final int KIncreaseVolume = 11;
public final int KNextTrack = 12;

public final int KStop = 13; 
public final int KDecreaseVolume = 14;
public final int KPlay = 15;

public final int KMyComputer = 16; 
public final int KMediaPlayer = 17;
public final int KMute = 18;
int spaceremaining;
Image buttonMap;

public final char[] activityChars = {' ','/','-','\\','|'};
int curActivityChar;

public final char[] clearevoweb = {'G','e','t',' ','o','n',' ','P','C',':',' ','w','w','w','.','C','l','e','a','r','E','v','o','.','c','o','m'};

public final char[] helpstr = {'H','e','l','p'};
public final char[] exitstr = {'E','x','i','t'};
public final char[] stopstr = {'S','t','o','p'};

public final char[] hintConnectFromPCSide = {'P','r','e','s','s',' ','C','o','n','n','e','c','t',' ','o','n',' ','P','C','-','S','i','d','e'};
public final char[] hintConnected = {'C','o','n','n','e','c','t','e','d'};

public final char[] hintStarting = {'P','l','e','a','s','e',' ','W','a','i','t'};
public final char[] hintDisconnected = {'D','i','s','c','o','n','n','e','c','t','e','d'};
public char[] leftsoftkey;
public char[] rightsoftkey;

public char[] hinttext;

static final int NokiaKEY_SoftKey_Left	= -6;
static final int NokiaKEY_SoftKey_Right	= -7;
static final int MotoKEY_SoftKey_Left	= -21;
static final int MotoKEY_SoftKey_Right	= -22;

boolean exitCommanded;
int commandCode;
int InputMode;

int help_tl_x;
int help_tl_y;
int help_width;
int help_height;

int exit_tl_x;
int exit_tl_y;
int exit_width;
int exit_height;


Thread service;
//Thread iReaderThread;

////////////screenshot of computer
/*byte[] iPicReadBuff;
Image iPicImg;
boolean iShowScreen;
boolean iSSReqeustSent;*/
//////////////////

String iDebugHint; //if not null then use as hint - for debug
    
    private int g_img_tl_x;
    private int g_img_tl_y;
    private final int g_button_h;
    private final int g_button_w;

    public MainCanvas(boolean suppress) {super(suppress);
        this.setFullScreenMode(true);
        
        leftsoftkey = helpstr;
        rightsoftkey = exitstr;
        hinttext = hintStarting;
        
        font =  Font.getFont(Font.FACE_PROPORTIONAL,Font.STYLE_BOLD,Font.SIZE_MEDIUM);
        sfont = Font.getFont(Font.FACE_PROPORTIONAL,Font.STYLE_PLAIN,Font.SIZE_SMALL);
        hintfont = Font.getFont(Font.FACE_PROPORTIONAL,Font.STYLE_PLAIN,Font.SIZE_SMALL);
         //addCommand(WMouseXP.curInstance.get_cancelCommand1());
         //addCommand(WMouseXP.curInstance.get_okCommand1());
         
         //addCommand(WMouseXP.curInstance.get_exitCommand());                
         //addCommand(WMouseXP.curInstance.get_helpCommand());

        //addCommand(WMouseXP.curInstance.get_aboutCommand());
         curActivityChar = 0;
         
         //setCommandListener(this);
        
        sendbuf =  new byte[6];//max is 6 - request ss 5 bytes + opcode 1 byte
        InputMode = KMedia;
        status = "Starting Mobile Side";
        
        try {
            if(getWidth()>290 && getHeight() > 290)
            buttonMap = Image.createImage("/combtnsbiggest.png");
            else
            if(getWidth()>190 && getHeight() > 190)
            buttonMap = Image.createImage("/combtnsbigger.png");
            else
            if(getWidth()>170 && getHeight() > 170)
            buttonMap = Image.createImage("/combtnsbig.png");
            else
            buttonMap = Image.createImage("/combtns.png");
        } catch (IOException ex) {
            //ex.printStackTrace();
        }

        g_button_w = (buttonMap.getWidth()-6/*borders*/)/3;
        g_button_h = (buttonMap.getHeight()-8/*borders*/)/4;

        
        service = new Thread()
        {
            public void run()
            {
                startservice();
            }
        };
        service.start();
        
        
        /*try{
        iPicReadBuff = new byte[100000];
        }
        catch(Exception e)
        {
        iPicReadBuff = null; //flag that this phone doesn't support enough ram to use the show screen on phone
        }*/
        
        
        
    }

    public void startservice()
    {
                            try
                            {
                                StartReceiver();
                            }
                            catch(Exception e)
                            {
                                if(exitCommanded)
                                    return;                              
      
                                if(e instanceof SecurityException)
                                {
                                   addLog("Can't access Bluetooth");
                                   Display.getDisplay(WMouseXP.curInstance).setCurrent(WMouseXP.curInstance.get_NoBTForm());
                                }
                                else
                                if(e instanceof javax.bluetooth.BluetoothStateException)
                                {
                                    addLog("Can't get Bluetooth");                        
                                    Display.getDisplay(WMouseXP.curInstance).setCurrent(WMouseXP.curInstance.get_BTOffForm());
                                }
                                else 
                                {                                    
                                    addLog("Start Failed, exit and retry");
                                    WMouseXP.curInstance.get_StartFailedForm().append(" Error: "+e.toString());
                                    Display.getDisplay(WMouseXP.curInstance).setCurrent(WMouseXP.curInstance.get_StartFailedForm());
                                }                                   
                            }                                                                        
                            
    }
    
    public void paint(Graphics g) {
        
        /*if(iPicImg!=null)
            {
                g.drawImage(iPicImg,0,0,Graphics.TOP|Graphics.LEFT);     
                return;
            }
        */
        /*if(iShowScreen)
        {   
            if(iPicImg!=null)
            {
                g.drawImage(iPicImg,0,0,Graphics.TOP|Graphics.LEFT);                    
            }
            else
            {
                //TODO: draw loading buffering %
                g.setColor(0xffffff);        
                g.fillRect(0,0, this.getWidth(), this.getHeight());                    
                g.setColor(0x1122aa);                            
                g.setFont(hintfont);
                g.drawString("Loading PC Screen...",getWidth()/2,getHeight()/2,Graphics.BASELINE|Graphics.HCENTER);                
            }                
            return;
        }*/
        
        g.setColor(0xffffff);        
        g.fillRect(0,0, this.getWidth(), this.getHeight());        
        spaceremaining = getHeight()-buttonMap.getHeight();                
        /*spaceremaining -= font.getHeight();
        spaceremaining -= 2*sfont.getHeight();
        */       
        spaceremaining/=2; //the only thing well use from now
        
        
        int curDrawY = getHeight()-(spaceremaining/2);        
        
        g.setFont(hintfont);
        g.setColor(0xaaaaaa);      
        
        if(iDebugHint==null)
            g.drawChars(hinttext,0,hinttext.length,getWidth()/2,curDrawY/*+(sfont.getHeight()/2)*/,Graphics.BASELINE|Graphics.HCENTER);        
        else
            g.drawString(iDebugHint,getWidth()/2,curDrawY/*+(sfont.getHeight()/2)*/,Graphics.BASELINE|Graphics.HCENTER);        
        
        curDrawY -= spaceremaining/2;
                
        g.drawImage(buttonMap,getWidth()/2,curDrawY,Graphics.BOTTOM|Graphics.HCENTER);        

        g_img_tl_x = (getWidth()/2) - buttonMap.getWidth()/2;
        g_img_tl_y = curDrawY - buttonMap.getHeight();

      
        
        //jump across middle pic
        curDrawY -= buttonMap.getHeight();
        ///////////////////////
        
        //change text color
        g.setColor(0x1122aa);                       
        ////////////////////////
        
        if(iOs!=null)
        {
        //set y position
        curDrawY -= spaceremaining/2;   
        curDrawY += font.getHeight()/2;
        ////////////////////            
        }
        else
        {
        //draw clearevo web url        
        curDrawY -= spaceremaining/5;
        g.setFont(hintfont);
        g.drawChars(clearevoweb,0,clearevoweb.length,getWidth()/2,curDrawY/*+(sfont.getHeight()/2)*/,Graphics.BASELINE|Graphics.HCENTER);        
        /////////////////////////        
        
        //set y position
        curDrawY -= (spaceremaining*2)/5;   
        ////////////////////
        }
        
        //draw command txt
        g.setFont(font);
        
        g.drawString(status,getWidth()/2, curDrawY ,Graphics.BASELINE|Graphics.HCENTER);               
        /////////////////////
        
        //draw activity char
        g.drawChar(activityChars[curActivityChar],getWidth()/2 + (font.stringWidth(status)/2)+3,curDrawY,Graphics.BOTTOM|Graphics.LEFT);
        ///////////////////        
        
        g.setFont(sfont);
        //g.setColor(0xaaaaaa);         
        //g.drawChars(clearevoweb,0,clearevoweb.length,getWidth()/2,getHeight(),Graphics.BOTTOM|Graphics.HCENTER);
        //g.setColor(0x1122aa);
        help_tl_x = 1;
        help_tl_y = getHeight()-((font.getHeight()*6)/5);
        help_width = font.charsWidth(leftsoftkey,0,leftsoftkey.length);
        help_height = font.getHeight();
        
        exit_tl_x = getWidth() - (1 + font.charsWidth(rightsoftkey,0,rightsoftkey.length));
        exit_tl_y = help_tl_y;
        exit_width = font.charsWidth(rightsoftkey,0,rightsoftkey.length);
        exit_height = help_height;

        g.drawChars(leftsoftkey,0,leftsoftkey.length,1,getHeight()-(font.getHeight()/5),Graphics.BOTTOM|Graphics.LEFT);
        g.drawChars(rightsoftkey,0,rightsoftkey.length,getWidth()-1,getHeight()-(font.getHeight()/5),Graphics.BOTTOM|Graphics.RIGHT);

        
        
    }
    
   public void UpdateStr(String s)
   {
       status = s;
       
       if(!exitCommanded)
        repaint();
   }
    //Called when a key is pressed. 
   public int getWMouseXPCommandCode(int keyCode)
   {
        int ret=KUnknownCommand;
        
                
                if(keyCode == KEY_NUM1 || keyCode == KE61KEY_NUM1 || keyCode == (KE61KEY_NUM1-KE61SHIFT_DIFF))                  
                {    ret = (byte)KEsc;addLog("Escape key");
                     
                }                
                else
                if(keyCode == KEY_NUM2 || keyCode == KE61KEY_NUM2 || keyCode == (KE61KEY_NUM2-KE61SHIFT_DIFF))                                    
                {    ret = (byte)KPrevSlide;addLog("Previous Slide");
                }
                else
                if(keyCode == KEY_NUM3 || keyCode == KE61KEY_NUM3 || keyCode == KE71KEY_NUM3_DE || keyCode == (KE61KEY_NUM3-KE61SHIFT_DIFF))                                                      
                {    ret = (byte)KStartPresentation;addLog("Start Presentation");
                }
                
                else
                if(keyCode == KEY_NUM4 || keyCode == KE61KEY_NUM4 || keyCode == (KE61KEY_NUM4-KE61SHIFT_DIFF))                                                      
                {   ret = (byte)KMyComputer;addLog("My Computer");
                }
                else
                if(keyCode == KEY_NUM5 || keyCode == KE61KEY_NUM5 || keyCode == (KE61KEY_NUM5-KE61SHIFT_DIFF))                                                      
                {   ret = (byte)KMediaPlayer;addLog("Start Media Player");
                }
                else
                if(keyCode == KEY_NUM6 || keyCode == KE61KEY_NUM6 || keyCode == (KE61KEY_NUM6-KE61SHIFT_DIFF))                                                      
                {   ret = (byte)KMute;addLog("Mute/Unmute");
                }
        
                else
                if(keyCode == KEY_NUM7 || keyCode == KE61KEY_NUM7 || keyCode == (KE61KEY_NUM7-KE61SHIFT_DIFF))                                                      
                {   ret = (byte)KStop;addLog("Stop");
                }
                else
                if(keyCode == KEY_NUM8 || keyCode == KE61KEY_NUM8 || keyCode == (KE61KEY_NUM8-KE61SHIFT_DIFF))                                                      
                {   ret = (byte)KPlay;addLog("Play/Pause");
                }
                else
                if(keyCode == KEY_NUM9 || keyCode == KE61KEY_NUM9 || keyCode == (KE61KEY_NUM9-KE61SHIFT_DIFF))                                                      
                {   
                    ret = (byte)KIncreaseVolume;addLog("Increase Volume");
                }               
        
                else
                if(keyCode == KEY_STAR|| keyCode == KE61KEY_STAR || keyCode == (KE61KEY_STAR-KE61SHIFT_DIFF))                                                                        
                {   ret = (byte)KPrevTrack;addLog("Previous Track");
                }
                else
                if(keyCode == KEY_NUM0|| keyCode == KE61KEY_NUM0 || keyCode == (KE61KEY_NUM0-KE61SHIFT_DIFF))                                                                        
                {   
                    ret = (byte)KNextTrack;addLog("Next Track");
                }
                else
                if(keyCode == this.KEY_POUND|| keyCode == KE61KEY_POUND || keyCode == (KE61KEY_POUND-KE61SHIFT_DIFF))                                                                        
                {   ret = (byte)KDecreaseVolume;addLog("Decrease Volume");
                }        
                
                /////////////////////////////JOYSTICK..........
                else
                if(UP == getGameAction(keyCode))                                                        
                {   ret = (byte)KJoyUp;
                    addLog("Mouse Up");
                }
                else
                if(DOWN == getGameAction(keyCode))                                                                                                                
                {
                    ret = (byte)KJoyDown;
                    addLog("Mouse Down");
                }
                else
                if(LEFT == getGameAction(keyCode))                                                                                                           
                {
                    ret = (byte)KJoyLeft;
                    addLog("Mouse Left");
                }
                else
                if(RIGHT == getGameAction(keyCode))                                                        
                {
                    ret = (byte)KJoyRight;
                    addLog("Mouse Right");
                }   
                else
                if(FIRE == getGameAction(keyCode))                                                        
                {    
                    ret = (byte)KJoyIn;addLog("Mouse Click");
                }
                //////////////////////END JOYSTICK
        
                

                //////////////////////SOFTKEYS
                 else
                 {
                 if(!detectHelpOrExit(keyCode))
                    {
                        addLog("UnknownKey: "+keyCode);
                        ret = (byte)KUnknownCommand;//addLog("Unknown Key");
                    }
                 }               
                ///////////////////////END SOFTKEYS
        
        
        
              
        
        
        return ret;
        
   }
   
  public void exitCommandedFromApp()
  {
      if(exitCommanded)
          return; //this call is from the app's DestroyApp func which was called by this class from  WMouseXP.curInstance.exitMIDlet() above...
      
         exitCommanded = true;
         cleanup();
  }
  
  public void cleanup()
  {
                    try{service.yield();}catch(Exception ee){}                    
                    //////////////////// if in wait state
                    try{
                        if(notifier!=null)
                        {                    
                            notifier.close();                                    
                        }
                    }
                    catch(Exception e){}
                    notifier=null;
                    //////////////////////                    
  }
  
  protected void setHintDebug(String str)     
  {
    iDebugHint = str;
    repaint();
    serviceRepaints();
  }

   public void pointerPressed (int x, int y) {

       if(y>=exit_tl_y && x>=exit_tl_x)
       {
           exitCommanded = true;
                    cleanup();
                    WMouseXP.curInstance.exitMIDlet();
                    return;
       }

        if(y>=help_tl_y && x>=help_tl_x && x<=(help_tl_x+help_width) )
       {
           Display.getDisplay(WMouseXP.curInstance).setCurrent(WMouseXP.curInstance.get_helpForm());
                    return;
       }

    if(iOs==null)
        return;
       
       
    if(x> g_img_tl_x && y > g_img_tl_y && x < g_img_tl_x+buttonMap.getWidth() && y < g_img_tl_y+buttonMap.getHeight() )
    {
        int c=-1;
        int r=-1;

        y = y - g_img_tl_y -1 /*upper border*/;
        x = x - g_img_tl_x -1 /*left border*/;

        //System.out.println("y "+y);
        //System.out.println("x "+x);

        for(int i=3;i>=0;i--)
        {
            
         //           System.out.println("y>"+(g_button_h*i + (2*(i-1))));
            if(y > g_button_h*i + (2*(i-1)) /* separator borders*/ )
            {
                r = i;
                break;
            }
        }

        for(int i=3;i>=0;i--)
        {
            if(x > g_button_w*i + (2*(i-1)) /* separator borders*/ )
            {
                c = i;
                break;
            }
        }

        if(r>=0 && c>=0)
        {
            if(r<3) //if not lowest * 0 # row...
            {
                int kc = (49+((r*(3))+c));
                //System.out.println(""+r+","+c+":"+kc);
                keyPressed(kc);
            }
            else
            {
                // r==3 lowest row
                switch(c)
                {
                    case 0:
                        keyPressed(KEY_STAR);
                        break;
                    case 1:
                        keyPressed(KEY_NUM0);
                        break;
                    case 2:
                        keyPressed(KEY_POUND);
                        break;
                    default:
                        break;//should never reach here though
                }

                
            }


        }

        //addLog("ptr "+x+","+y+"["+c+","+r+"]");
              

    }
    
  }
  protected boolean detectHelpOrExit(int keyCode)
    {
            if( keyCode!=49 /*strangely it would pass the rest in emulator*/ && (GAME_A == getGameAction(keyCode) || NokiaKEY_SoftKey_Left == keyCode || MotoKEY_SoftKey_Left == keyCode || Math.abs(MotoKEY_SoftKey_Left) == keyCode/*Motorola V600: 21 = lsk*/))
                {    
                    //addLog("Showing Help...");
                    //System.out.println("help "+keyCode);
                    
                    Display.getDisplay(WMouseXP.curInstance).setCurrent(WMouseXP.curInstance.get_helpForm());
                    return true;
                }
                else
                if( keyCode!=51 /*strangely it would pass the rest in emulator*/ && (GAME_B == getGameAction(keyCode) || NokiaKEY_SoftKey_Right == keyCode || MotoKEY_SoftKey_Right == keyCode || Math.abs(MotoKEY_SoftKey_Right) == keyCode/*Motorola V600: 22 = rsk*/))
                {    
                    //addLog("Preparing Exit...");
                    //System.out.println("exit"+keyCode);
                    
                    
                    exitCommanded = true;
                    cleanup();            
                    WMouseXP.curInstance.exitMIDlet();
                    return true;
                }

            return false;
  }

  protected void keyPressed(int keyCode)     
    {
          if(exitCommanded)
            return;
    
      try
        {  
            if(iOs!=null)
            { 
                commandCode = getWMouseXPCommandCode(keyCode);                    
                
                if(commandCode == KInternalCommandDontSendToPC) //left or right softkey
                    return;                
                
                int sendlen = 2;//default - except KRequestSSPNG => 6
                
                //if(iPicReadBuff != null)
                {
                 /*if(commandCode == KRequestSSPNG )
                    {
                        if(iShowScreen == false)
                        {
                        int w = getWidth();
                        int h = getHeight();                        
                        sendbuf[0] = (byte)(KRequestSSPNG*InputMode);
                        sendbuf[1] = (byte) (w%0xff);
                        sendbuf[2] = (byte) (w/0xff);
                        sendbuf[3] = (byte) (h%0xff);
                        sendbuf[4] = (byte) (h/0xff);
                        sendbuf[5] = (byte) 1;
                        sendlen = 6;
                        
                        iShowScreen = true;
                        setHintDebug("showscr true");
                        repaint();
                        
                        
                        }
                        else
                        {
                        iShowScreen = false;
                        setHintDebug("showscr false");
                        repaint();
                        
                        
                        }
                    
                    }
                 else*/
                 {
                sendbuf[0] = (byte)(KKeyPressedCode*InputMode);
                sendbuf[1] = (byte) commandCode;
                 }
                }
                /*else
                {
                    //TODO: show alert this phone doesn't have enough ram to use show screen feature
                }*/
                
                /////////////send
                iOs.write(sendbuf,0,sendlen);                
                iOs.flush();
                ////////////////
                
                ActivityCharNext();
                
                
                /*if(commandCode == KRequestSSPNG && iShowScreen == true)
                {
                    //TODO: notify() reader thread to start reading         
                    if(iReaderThread!=null && iReaderThread.isAlive())
                    {
                        iSSReqeustSent = true;                        
                        setHintDebug("TRUE set ss req");
                    }                 
                    else
                    {
                        setHintDebug("NOT set ss req");
                    }
                    
                    
                    //reader thread would then start a chain reaction: it would then send the request itself until iShowScreen is false again
                }   */
                
            }
            else //not connected
            {               
                detectHelpOrExit(keyCode);
            }
        }
        catch(Exception e)
        {
             if(exitCommanded)
                return;
            
            addLog("Connection Lost/Closed");
            
            
        }
    }
  
  protected void keyRepeated(int keyCode) 
     {

      
      if(exitCommanded)
            return;
      
        try
        {
            if(iOs!=null)
            {
                commandCode = getWMouseXPCommandCode(keyCode);                
                if(commandCode == KInternalCommandDontSendToPC) //left or right softkey
                    return;                
                sendbuf[0] = (byte)(KKeyRepeatedCode*InputMode);
                sendbuf[1] = (byte) commandCode;
                iOs.write(sendbuf,0,2);     
                iOs.flush();
                ActivityCharNext();
                
            }
        }
        catch(Exception e)
        {
             if(exitCommanded)
                return;
             
            
            addLog("Connection Lost/Closed");
            
        }
     }
  
  protected void keyReleased(int keyCode) 
     {
      
      if(exitCommanded)
            return;
      
        try
        {
            if(iOs!=null)
            {                
                commandCode = getWMouseXPCommandCode(keyCode);                
                if(commandCode == KInternalCommandDontSendToPC) //left or right softkey
                    return;                
                sendbuf[0] = (byte)(KKeyReleasedCode*InputMode);
                sendbuf[1] = (byte) commandCode;
                
               iOs.write(sendbuf,0,2);     
               iOs.flush();
               ActivityCharNext();
               
            }
        }
        catch(Exception e)
        {
             if(exitCommanded)
                return;            
             
            
            addLog("Connection Lost/Closed");
              
        }
        
     }
  
  
   public void hideNotify()
    {
       
       if(exitCommanded)
            return;
       
       //try stop the mover incase mouse was still moving... see TryHandleJoyStickEvent in PC side src
        try
        {
            if(iOs!=null)
            {                
               sendbuf[0] = (byte)(KKeyReleasedCode*InputMode);
               sendbuf[1] = KJoyUp; //any key with released type would stop the mover
               iOs.write(sendbuf,0,2);     
               iOs.flush();
               ActivityCharNext();
               
            }
        }
        catch(Exception e)
        {
             if(exitCommanded)
                 return;             
            
            addLog("Connection Lost/Closed");            
        }
    }
   
    public static final int KMaxWaitStartServiceRetries = 30;
    public static final int KWaitStartServiceDurationMS = 1000;    

    
    public void StartReceiver() throws BluetoothStateException, IOException, Exception
     {
        
            LocalDevice local_device = LocalDevice.getLocalDevice();
            // Obtain discovery agent object
            DiscoveryAgent disc_agent = local_device.getDiscoveryAgent();
            // Set device into limited access mode. Inquiry scan
            // will listen only to LIAC.
            local_device.setDiscoverable( DiscoveryAgent.LIAC );
            
            
            // Obtain local device object
           
            // Do the service search on all found devices.
            // Note: don�t use this UUID in your own MIDlets.
            // You have to create an own UUID for each MIDlet that you write:
            //OLD String service_UUID = "27b8f85a1d7411dc83140800200c9a66";

            //String service_UUID = "2bc2b92e399211dc83140800200c9a66";
            String service_UUID = "2BC2B92E399211DC83140800200C9A66";

            // Retrieve players name
            // This could be a name that user has modified and stored to
            // non-volatile memory. As default (when game is first started)
            // the local friendly name could be chosen.




            // String player_name = local_device.getFriendlyName();
            // Open connection, note: name is attribute ID 0x0100
            String url = "btspp://localhost:" + service_UUID + ";name=WMouseXP" ;  
        
        while(true)
        {
                
            hinttext = hintStarting;
            addLog("Starting service");
            //StreamConnectionNotifier notifier = (StreamConnectionNotifier) Connector.open( url );
            notifier = (StreamConnectionNotifier) Connector.open( url, Connector.WRITE);
            
            // Wait on someone to connect (note: you can cancel this wait
            // only if you call notifier.close() from another thread.
            // This is important if you want to offer a UI for the user
            // to cancel connections setup.)
            hinttext = hintConnectFromPCSide;
            addLog("Awaiting PC-Side");

            //removeCommand(WMouseXP.curInstance.get_okCommand1());  
            
            //iSSReqeustSent = false;
            //iShowScreen = false;

            StreamConnection con = (StreamConnection) notifier.acceptAndOpen();   
          
            //////////////////// we're not free now so close the notifier
            try{
                if(notifier!=null)
                {                    
                    notifier.close();                                    
                }
            }
            catch(Exception e){}
            notifier=null;
            //////////////////////
            
            if(exitCommanded)
            {
                //try Close connection
                try{
                if(con!=null)
                {
                    con.close();                
                }
                }
                catch(Exception e){}
                con = null;
                ////////////////////    
            
                break;            
            }

            hinttext = hintConnected;
            addLog("Opening connection");


    //        addCommand(WMouseXP.curInstance.get_cancelCommand1());


            //// open input stream        
            //InputStream is = con.openInputStream();
            //iIs = is;

            // open output stream
            OutputStream os = con.openOutputStream();        
            iOs = os;
            addLog("Connected");  


            //TODO: Start reader thread - read from iOs
            
           /* iReaderThread = new Thread()
            {
                public void run()
                {
                    try
                    {                    
                        while(true)
                        {
            
                            try
                            {
                                setHintDebug("waiting frame");     
                                sleep(300);                                       
                                //setHintDebug("END wait"); 
                                //serviceRepaints();
                                repaint();
                            }
                            catch(Exception e)
                            {
                                setHintDebug("waitframe exp "+e.toString());     
                                return;
                            }
                            
                            if(iIs == null ) //connection closed so exit thread
                                return;
                            
                            setHintDebug("reader woke up");     
                            
                            if(iSSReqeustSent)
                            {
                                setHintDebug("iSSReqeustSent true");     
                                iSSReqeustSent = false;
                                int readlen = 0;
                                boolean bufffull = false;
                                //TODO: read from iIs and show it on screen                                
                                for(int i=0;i<19;i++)
                                {                                    
                                    int readinthisloop = 0;

                                    try{
                                    sleep(38);
                                    }catch(Exception e){}
                                    
                                       setHintDebug("f sleepend");     

                                    if(iIs == null)//connection closed so exit thread
                                        return;

                                        
                                    if(readlen == 0)
                                    {
                                           setHintDebug("start read 0");    
                                        readlen = iIs.read(iPicReadBuff);
                                    }
                                    else
                                    {
                                        if(iPicReadBuff.length-readlen>0)                                        
                                            bufffull = true;
                                        
                                        setHintDebug("start read cont");    
                                        
                                        if(!bufffull)
                                        {
                                        readinthisloop = iIs.read(iPicReadBuff,readlen,iPicReadBuff.length-readlen);
                                        readlen += readinthisloop;
                                        }
                                    }

                                    setHintDebug("readlen "+ readlen);     
                                    serviceRepaints();
                           
                                    if(readlen >0 && (readinthisloop==0 || bufffull))//buffer seems to be ready
                                    {
                                        try 
                                        {
                                            iPicImg = Image.createImage(iPicReadBuff,0,readlen);
                                            setHintDebug("got frame");
                                            break; //Okay - break from this for loop and enter main wait() loop again

                                        }catch(Exception ex)
                                        {
                                        iPicImg = null;
                                        setHintDebug("decode frame failed"+ readlen);
                                        }
                                        return ;
                                        //TODO: break;
                                    }
                                    else
                                    {
                                    
                                       // setHintDebug("readlen "+ readlen);     
                                        //serviceRepaints();
                                    }
                                }
                                
                            }
                            else
                            {
                            setHintDebug("iSSReqeustSent false");     
                            }
                            
                            //iIs.

                        }
                    }
                    catch(Exception e)
                    {
                        //a read error occured so exit thread
                        setHintDebug("reader exp: "+e.toString());
                        return;
                    }                    
                }
            };
            
            
            iReaderThread.start();
            setHintDebug("started reader");   */
            //iReaderThread = 


                       try
                        {  
                            while(true)
                            {     
                                if(exitCommanded)                                   
                                    break; //breaks from this inner while loop to execute cleanup
                                

                                sendbuf[0] = (byte)(KKeyPressedCode*InputMode);                              
                                sendbuf[1] = (byte) KUnknownCommand;                                
                         
                                os.write(sendbuf,0,2);                
                                ActivityCharNext();
                                os.flush();

                                Thread.currentThread().sleep(1000);
                            }

                        }
                        catch(Exception e)
                        {                       
                            //interrupted... go through to cleanup
                        }
            
            
            ///////////////cleanup             
                        
            // close output stream
            try{
            if(os!=null)
            {
                os.close();               
            }
            }
            catch(Exception e){}
            os=null;
            iOs=null;//IMPORTANT: this flags the main thread to know that we're not connected
            ////////////////////                      

            // close input stream
            /*try{
            if(is!=null)
            {
                is.close();               
            }
            }
            catch(Exception e){}
            is=null;
            iIs=null;//IMPORTANT: this shows iReaderThread that we've lost connection
            
                ///////// wait for reader Thread to see that iIs is null now
                if(iReaderThread!=null && iReaderThread.isAlive())
                {
                    Thread.currentThread().sleep(700);  //give some time to make sure if exits itself
                   iReaderThread = null;                   
                }            */
                //////////////////////
            
            ////////////////////
            
            // Close connection
            try{
            if(con!=null)
            {
                con.close();                
            }
            }
            catch(Exception e){}
            con = null;
            ////////////////////     
                
            
        //////////////////////////
            
            
            if(exitCommanded)
                break;
            
            curActivityChar = 0;
        
            UpdateStr("Stopped");
            addLog("Connection Lost/Closed");
            
        }//end of while loop         
             
            
            ////////////////////
            try{
                if(notifier!=null)
                {                    
                    notifier.close();                                    
                }
            }
            catch(Exception e){}
            notifier=null;
            //////////////////////                 
      
    }
    
     public void addLog(String s)
    {
        UpdateStr(s);            
    }
     
     



    
   void ActivityCharNext()
   {     
       curActivityChar%=4;
       curActivityChar++;       
       repaint();
   }
}
