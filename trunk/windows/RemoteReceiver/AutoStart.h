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

#pragma once

#include <string>
#include <boost/utility.hpp>


class AutoStart : boost::noncopyable
{
public:
   AutoStart(const wchar_t* shortcutName, const wchar_t* filePath,
         const wchar_t* workingDir, const wchar_t* description);
   ~AutoStart();

   void Enable();
   void Disable();
   bool IsEnabled() const;

   static std::wstring GetAppPath();
   static std::wstring GetWorkingDir();
   static std::wstring GetShortcutPath(const wchar_t* shortcutName);

private:
   std::wstring filePath_;
   std::wstring workingDir_;
   std::wstring description_;
   std::wstring shortcutPath_;

   static void CheckHR(HRESULT hr, bool throwOnFailure);
};

