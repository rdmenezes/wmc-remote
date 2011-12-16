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

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;

public class LocationDialog extends AlertDialog implements TextWatcher {

   public final static int MIN_PORT = 1024;
   public final static int MAX_PORT = 65535;

   private final DialogInterface.OnClickListener _listener;
   private Location _location;
   private View _view;
   private EditText _nameEdit;
   private EditText _addressEdit;
   private EditText _portEdit;

   public Location getLocation() {

      Location location;

      if (_location == null) {
         location = new Location();
      } else {
         location = _location;
      }

      if (_nameEdit != null) {
         location.setName(_nameEdit.getText().toString());
      }

      if (_addressEdit != null) {
         location.setAddress(_addressEdit.getText().toString());
      }

      if (_portEdit != null) {
         try {
            int port = Integer.parseInt(_portEdit.getText().toString());
            location.setPort(port);
         } catch (NumberFormatException e) {
         }
      }

      return location;
   }

   public boolean edit() {
      return _location != null;
   }

   protected LocationDialog(Context context,
         DialogInterface.OnClickListener listener, Location location) {
      super(context);
      _listener = listener;
      _location = location;
   }

   @Override
   protected void onCreate(Bundle savedInstanceState) {

      _view = getLayoutInflater().inflate(R.layout.location_dialog, null);

      setView(_view);
      setInverseBackgroundForced(true);
      if (_location == null) {
         setTitle("Add new location");
      } else
      {
         setTitle("Edit location");
      }

      _nameEdit = (EditText)_view.findViewById(R.id.location_name_edit);
      _addressEdit = (EditText)_view.findViewById(R.id.location_address_edit);
      _portEdit = (EditText)_view.findViewById(R.id.location_port_edit);

      if (_location != null) {
         _nameEdit.setText(_location.getName());
         _addressEdit.setText(_location.getAddress());
         if (_location.getPort() != 0) {
            _portEdit.setText(Integer.toString(_location.getPort()));
         }
      }

      _nameEdit.addTextChangedListener(this);
      _addressEdit.addTextChangedListener(this);
      _portEdit.addTextChangedListener(this);

      setButton(DialogInterface.BUTTON_POSITIVE, "Save", _listener);
      setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel", _listener);

      super.onCreate(savedInstanceState);

      validate();
   }

   private void validate() {

      boolean enableSave = true;

      if (_nameEdit != null && _nameEdit.length() > 0) {
         String name = _nameEdit.getText().toString().trim();
         if (name.length() == 0) {
            enableSave = false;
         }
      } else {
         enableSave = false;
      }

      if (_addressEdit != null && _addressEdit.length() > 0) {
         String address = _addressEdit.getText().toString().trim();
         if (address.length() == 0) {
            enableSave = false;
         }
      } else {
         enableSave = false;
      }

      if (_portEdit != null && _portEdit.length() > 0) {
         try {
            int port = Integer.parseInt(_portEdit.getText().toString());
            if (port < MIN_PORT || port > MAX_PORT) {
               enableSave = false;
            }
         } catch (NumberFormatException e) {
            enableSave = false;
         }
      } else {
         enableSave = false;
      }

      getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(enableSave);
   }

   public void afterTextChanged(Editable e) {
      validate();
   }

   public void
         beforeTextChanged(CharSequence s, int start, int count, int after) {
   }

   public void onTextChanged(CharSequence s, int start, int before, int count) {
   }
}
