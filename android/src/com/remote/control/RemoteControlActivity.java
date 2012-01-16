// wmc-remote -- An Android-based remote control for Windows Media Center
// Copyright (C) 2011  http://wmc-remote.googlecode.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

package com.remote.control;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.HashMap;
import java.util.Map;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.LightingColorFilter;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.ImageButton;

import com.remote.control.R.id;

public class RemoteControlActivity extends Activity {

   private static final int REPEAT_DELAY = 333;

   private Map<Integer, Byte> _codes = new HashMap<Integer, Byte>();

   private OnClickListener _onClickListener = new OnClickListener() {

      public void onClick(View v) {
         Integer key = new Integer(v.getId());
         if (_codes.containsKey(key)) {
            sendByte(_codes.get(key));
         }
      }
      
   };

   private RepeatHandler _repeatHandler;

   private OnLongClickListener _onLongClickListener = new OnLongClickListener() {

      public boolean onLongClick(View v) {
         Integer key = new Integer(v.getId());
         if (_codes.containsKey(key)) {
            sendByte(_codes.get(key));
            _repeatHandler = new RepeatHandler(key);
            _repeatHandler.removeCallbacks(_repeatTask);
            _repeatHandler.postDelayed(_repeatTask, REPEAT_DELAY);
         }
         return false;
      }

   };

   private Runnable _repeatTask = new Runnable() {

      public void run() {
         sendByte(_codes.get(_repeatHandler.getKeycode()));
         _repeatHandler.postDelayed(_repeatTask, REPEAT_DELAY);
      }

   };

   private OnTouchListener _onTouchListener = new OnTouchListener() {

      public boolean onTouch(View button, MotionEvent motion) {
         if (motion.getAction() == MotionEvent.ACTION_UP) {
            if (_repeatHandler != null) {
               _repeatHandler.removeCallbacks(_repeatTask);
               _repeatHandler = null;
            }
         }
         return false;
      }

   };

   @Override
   public boolean onKeyUp(int keyCode, KeyEvent event) {
      if (keyCode == KeyEvent.KEYCODE_MENU) {
         Intent intent = new Intent(RemoteControlActivity.this,
               LocationPreferenceActivity.class);
         startActivity(intent);
      }
      return super.onKeyUp(keyCode, event);
   }

   private void setupLongClick(int id) {
      View v = findViewById(id);
      v.setOnLongClickListener(_onLongClickListener);
      v.setOnTouchListener(_onTouchListener);
   }

   @Override
   public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.main);

      setupCodes();

      findViewById(id.launch_media_center).setOnClickListener(_onClickListener);
      findViewById(id.recorded_tv).setOnClickListener(_onClickListener);
      findViewById(id.accept).setOnClickListener(_onClickListener);
      findViewById(id.go_back).setOnClickListener(_onClickListener);
      findViewById(id.left).setOnClickListener(_onClickListener);
      findViewById(id.up).setOnClickListener(_onClickListener);
      findViewById(id.down).setOnClickListener(_onClickListener);
      findViewById(id.right).setOnClickListener(_onClickListener);
      findViewById(id.mute).setOnClickListener(_onClickListener);
      findViewById(id.volume_down).setOnClickListener(_onClickListener);
      findViewById(id.volume_up).setOnClickListener(_onClickListener);
      findViewById(id.play).setOnClickListener(_onClickListener);
      findViewById(id.stop).setOnClickListener(_onClickListener);
      findViewById(id.skip_backward).setOnClickListener(_onClickListener);
      findViewById(id.skip_forward).setOnClickListener(_onClickListener);
      findViewById(id.rewind).setOnClickListener(_onClickListener);
      findViewById(id.fast_forward).setOnClickListener(_onClickListener);

      setupLongClick(id.left);
      setupLongClick(id.up);
      setupLongClick(id.down);
      setupLongClick(id.right);
      setupLongClick(id.volume_down);
      setupLongClick(id.volume_up);
      setupLongClick(id.skip_backward);
      setupLongClick(id.skip_forward);

      setBackground(id.play, 0x22222222, 0xdd00dd00);
      setBackground(id.stop, 0x22222222, 0xdddd0000);
      setBackground(id.skip_backward, 0x22222222, 0x00dd00dd);
      setBackground(id.skip_forward, 0x22222222, 0x00dddd00);
      setBackground(id.rewind, 0xcccccccc, 0x00dd00dd);
      setBackground(id.fast_forward, 0xcccccccc, 0x00dddd00);
      setBackground(id.accept, 0xcccccccc, 0xdd00dd00);
      setBackground(id.go_back, 0xcccccccc, 0xdddd0000);

      getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
   }

   @Override
   protected void onResume() {
      RemoteControlApplication app = (RemoteControlApplication) getApplication();
      Location location = app.getCurrentLocation();
      if (location != null) {
         String title = "Remote Control - " + location.getName();
         setTitle(title);
      } else {
         setTitle("Remote Control");
      }
      WifiManager wm = (WifiManager) getSystemService(Context.WIFI_SERVICE);
      wm.startScan();
      super.onResume();
   }

   private void sendByte(byte b) {
      RemoteControlApplication app = (RemoteControlApplication) getApplication();
      Location location = app.getCurrentLocation();
      if (location == null) {
         return;
      }
      String host = location.getAddress();
      int port = location.getPort();
      byte[] message = { b };
      try {
         InetAddress address = InetAddress.getByName(host);
         DatagramPacket packet = new DatagramPacket(message, message.length,
               address, port);
         DatagramSocket socket = new DatagramSocket();
         socket.send(packet);
         socket.close();
      } catch (Exception e) {
         Log.d("RemoteControl", e.toString());
      }
   }

   private void setupCodes() {
      _codes.put(new Integer(R.id.launch_media_center), new Byte((byte) 1));
      _codes.put(new Integer(R.id.recorded_tv), new Byte((byte) 2));
      _codes.put(new Integer(R.id.accept), new Byte((byte) 3));
      _codes.put(new Integer(R.id.go_back), new Byte((byte) 4));
      _codes.put(new Integer(R.id.left), new Byte((byte) 5));
      _codes.put(new Integer(R.id.up), new Byte((byte) 6));
      _codes.put(new Integer(R.id.down), new Byte((byte) 7));
      _codes.put(new Integer(R.id.right), new Byte((byte) 8));
      _codes.put(new Integer(R.id.mute), new Byte((byte) 9));
      _codes.put(new Integer(R.id.volume_down), new Byte((byte) 10));
      _codes.put(new Integer(R.id.volume_up), new Byte((byte) 11));
      _codes.put(new Integer(R.id.play), new Byte((byte) 12));
      _codes.put(new Integer(R.id.stop), new Byte((byte) 13));
      _codes.put(new Integer(R.id.skip_backward), new Byte((byte) 14));
      _codes.put(new Integer(R.id.skip_forward), new Byte((byte) 15));
      _codes.put(new Integer(R.id.rewind), new Byte((byte) 16));
      _codes.put(new Integer(R.id.fast_forward), new Byte((byte) 17));
   }

   private void setBackground(int buttonId, int mul, int add) {
      ImageButton button = (ImageButton) findViewById(buttonId);
      button.getBackground().setColorFilter(new LightingColorFilter(mul, add));
   }

}
