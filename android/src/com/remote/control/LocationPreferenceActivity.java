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

import java.util.Iterator;

import com.remote.control.R;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.ListView;

public class LocationPreferenceActivity extends PreferenceActivity implements
      DialogInterface.OnClickListener {

   class LocationCheckBoxPreference extends CheckBoxPreference {

      private Location _location;

      public LocationCheckBoxPreference(Context context, Location location) {
         super(context);
         _location = location;
         refresh();
      }

      public Location getLocation() {
         return _location;
      }

      public void refresh() {
         setTitle(getLocation().getName());
         setSummary(getLocation().getSummary());
      }
   }

   private PreferenceCategory _locationCategory;
   private Preference _addNewLocation;
   private LocationCheckBoxPreference _contextPreference;

   private LocationDialog _dialog;

   private OnPreferenceClickListener _onNewLocation =
         new OnPreferenceClickListener() {

            public boolean onPreferenceClick(Preference preference) {
               _dialog =
                     new LocationDialog(LocationPreferenceActivity.this,
                           LocationPreferenceActivity.this, null);
               _dialog.show();
               return true;
            }
         };

   private OnPreferenceClickListener _onClickLocation =
         new OnPreferenceClickListener() {

            public boolean onPreferenceClick(Preference preference) {
               LocationCheckBoxPreference lcb =
                     (LocationCheckBoxPreference)preference;
               LocationPreferenceActivity.this.setActiveLocation(lcb);
               return true;
            }
         };

   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      addPreferencesFromResource(R.xml.location_preference);

      _locationCategory =
            (PreferenceCategory)findPreference("location_category");

      RemoteControlApplication app = (RemoteControlApplication)getApplication();
      Iterator<Location> locations = app.getLocationsIterator();

      while (locations.hasNext()) {
         Location location = locations.next();
         LocationCheckBoxPreference pref =
               new LocationCheckBoxPreference(this, location);
         pref.setTitle(location.getName());
         pref.setOnPreferenceClickListener(_onClickLocation);
         _locationCategory.addPreference(pref);
         if (app.getCurrentLocation() == location) {
            pref.setChecked(true);
         }
      }

      registerForContextMenu(getListView());

      _addNewLocation = (Preference)findPreference("add_new_location");
      _addNewLocation.setOnPreferenceClickListener(_onNewLocation);
   }

   public void onClick(DialogInterface dialog, int which) {
      if (which == DialogInterface.BUTTON_POSITIVE && _dialog != null) {
         Location location = _dialog.getLocation();
         if (!_dialog.edit()) {
            RemoteControlApplication app =
                  (RemoteControlApplication)getApplication();
            app.addLocation(location);
            LocationCheckBoxPreference pref =
                  new LocationCheckBoxPreference(this, location);
            pref.setOnPreferenceClickListener(_onClickLocation);
            _locationCategory.addPreference(pref);
            // the following calls saveLocations()
            setActiveLocation(pref);
         } else {
            if (_contextPreference != null) {
               _contextPreference.refresh();
               _contextPreference = null;
            }
            RemoteControlApplication app =
                  (RemoteControlApplication)getApplication();
            app.saveLocations();
         }
      }
   }

   @Override
   public void onCreateContextMenu(ContextMenu menu, View v,
         ContextMenuInfo menuInfo) {
      super.onCreateContextMenu(menu, v, menuInfo);
      AdapterContextMenuInfo info = (AdapterContextMenuInfo)menuInfo;
      int position = info.position;
      ListView listView = getListView();
      int count = listView.getChildCount();
      if ((position + 1) == count) {
         return;
      }
      int categoryPosition = position - 1;
      if (categoryPosition >= 0
            && categoryPosition < _locationCategory.getPreferenceCount()) {
         LocationCheckBoxPreference pref =
               (LocationCheckBoxPreference)_locationCategory
                     .getPreference(categoryPosition);
         getMenuInflater().inflate(R.menu.location_context, menu);
         menu.setHeaderTitle(pref.getLocation().getName());
         _contextPreference = pref;
      }
   }

   @Override
   public boolean onContextItemSelected(MenuItem item) {
      switch (item.getItemId()) {
      case R.id.edit_location:
         if (_contextPreference != null) {
            _dialog =
                  new LocationDialog(this, this,
                        _contextPreference.getLocation());
            _dialog.show();
         }
         return true;
      case R.id.delete_location:
         if (_contextPreference != null) {
            RemoteControlApplication app =
                  (RemoteControlApplication)getApplication();
            app.removeLocation(_contextPreference.getLocation());
            _locationCategory.removePreference(_contextPreference);
            _contextPreference = null;
            app.saveLocations();
         }
         return true;
      }
      return super.onContextItemSelected(item);
   }

   private void setActiveLocation(LocationCheckBoxPreference pref) {
      for (int i = 0; i < _locationCategory.getPreferenceCount(); ++i) {
         LocationCheckBoxPreference item =
               (LocationCheckBoxPreference)_locationCategory.getPreference(i);
         if (item == pref) {
            item.setChecked(true);
         } else {
            item.setChecked(false);
         }
      }
      RemoteControlApplication app = (RemoteControlApplication)getApplication();
      app.setCurrentLocation(pref.getLocation());
      app.saveLocations();
   }
}
