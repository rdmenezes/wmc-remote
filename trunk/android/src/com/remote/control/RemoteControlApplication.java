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

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.StreamCorruptedException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import android.app.Application;
import android.content.Context;
import android.util.Log;

public class RemoteControlApplication extends Application {

   private static final String LOCATIONS_FILENAME = "locations";

   private List<Location> _locations = new ArrayList<Location>();
   private Location _curLoc;

   public Iterator<Location> getLocationsIterator() {
      return _locations.iterator();
   }

   public void addLocation(Location location) {
      _locations.add(location);
   }

   public void removeLocation(Location location) {
      if (getCurrentLocation() == location) {
         setCurrentLocation(null);
      }
      _locations.remove(location);
   }

   public Location getCurrentLocation() {
      return _curLoc;
   }

   public void setCurrentLocation(Location location) {
      if (_locations.contains(location)) {
         _curLoc = location;
      } else {
         _curLoc = null;
      }
   }

   @Override
   public void onCreate() {
      super.onCreate();
      loadLocations();
   }

   private void loadLocations() {
      try {
         FileInputStream fis = openFileInput(LOCATIONS_FILENAME);
         ObjectInputStream ois = new ObjectInputStream(fis);
         int count = ois.readInt();
         for (int index = 0; index < count; ++index) {
            _locations.add((Location)ois.readObject());
         }
         int selected = ois.readInt();
         if (selected != -1) {
            _curLoc = _locations.get(selected);
         } else {
            _curLoc = null;
         }
      } catch (FileNotFoundException e) {
         Log.d("RemoteControl", e.toString());
      } catch (StreamCorruptedException e) {
         Log.d("RemoteControl", e.toString());
      } catch (IOException e) {
         Log.d("RemoteControl", e.toString());
      } catch (ClassNotFoundException e) {
         Log.d("RemoteControl", e.toString());
      }
   }

   public void saveLocations() {
      try {
         FileOutputStream fos =
               openFileOutput(LOCATIONS_FILENAME, Context.MODE_PRIVATE);
         ObjectOutputStream oos = new ObjectOutputStream(fos);
         oos.writeInt(_locations.size());
         for (Location l : _locations) {
            oos.writeObject(l);
         }
         if (_curLoc != null) {
            oos.writeInt(_locations.indexOf(_curLoc));
         } else {
            oos.writeInt(-1);
         }
         oos.flush();
      } catch (FileNotFoundException e) {
         Log.d("RemoteControl", e.toString());
      } catch (IOException e) {
         Log.d("RemoteControl", e.toString());
      }
   }
}
