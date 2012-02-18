// wmc-remote -- An Android-based remote control for Windows Media Center
// Copyright (C) 2011-2012  http://wmc-remote.googlecode.com
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

import java.io.Serializable;

public class Location implements Serializable {

   private static final long serialVersionUID = 1L;

   private String _name;
   private String _address;
   private int _port;

   public String getName() {
      return _name;
   }

   public void setName(String name) {
      _name = name;
   }

   public String getAddress() {
      return _address;
   }

   public void setAddress(String address) {
      _address = address;
   }

   public int getPort() {
      return _port;
   }

   public void setPort(int port) {
      _port = port;
   }

   public String getSummary() {
      if (getAddress().length() == 0 || getPort() == 0) {
         return "";
      }
      return getAddress() + " : " + Integer.toString(getPort());
   }
}
