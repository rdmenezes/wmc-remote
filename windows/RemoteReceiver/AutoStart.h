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

