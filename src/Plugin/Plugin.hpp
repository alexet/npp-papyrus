/*
This file is part of Papyrus Plugin for Notepad++.

Copyright (C) 2016 Tschilkroete <tschilkroete@gmail.com> (original author)
Copyright (C) 2021 blu3mania <blu3mania@hotmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Common\EnumUtil.hpp"
#include "Common\Game.hpp"
#include "Common\NotepadPlusPlus.hpp"
#include "Common\Resources.hpp"
#include "Common\Timer.hpp"
#include "CompilationErrorHandling\ErrorAnnotator.hpp"
#include "CompilationErrorHandling\ErrorsWindow.hpp"
#include "Compiler\Compiler.hpp"
#include "Compiler\CompilerSettings.hpp"
#include "Settings\Settings.hpp"
#include "Settings\SettingsDialog.hpp"
#include "UI\AboutDialog.hpp"

#include "..\external\npp\PluginInterface.h"

#include <memory>
#include <vector>

// Plugin constants
//
namespace papyrus {

  using Game = game::Game;

  class Plugin {
    public:
      Plugin();

      // DLL initialization/cleanup
      void onInit(HINSTANCE instance);
      void cleanUp();

      // Interface functions with Notepad++
      inline TCHAR* name() const { return const_cast<TCHAR*>(PLUGIN_NAME); }
      inline BOOL useUnicode() const { return USE_UNICODE; }
      inline int numFuncs() const { return utility::underlying(Menu::COUNT); }
      inline  FuncItem* getFuncs() { return funcs; }
      void setNppData(NppData nppData);
      void onNotification(SCNotification* notification);
      LRESULT handleNppMessage(UINT message, WPARAM wParam, LPARAM lParam);

    private:
      enum class Menu {
        Compile,
        Options,
        Seperator1,
        Advanced,
        Seperator2,
        About,
        COUNT
      };

      enum class AdvancedMenu {
        ShowLangID,
        AddAutoCompletion,
        AddFunctionList
      };

      void initializeComponents();

      // Check if lexer's config file exists, and attempt to fix it if not
      void checkLexerConfigFile(const std::wstring& configPath);

      // Notepad++ notification NPPN_BUFFERACTIVATED handler
      void handleBufferActivation(npp_buffer_t bufferID);

      // Handle setting changes
      void onSettingsUpdated();
      void updateLexerDataGameSettings(Game game, const CompilerSettings::GameSettings& gameSettings);

      // Find out langID assigned to Papyrus Script lexer
      void detectLangID();

      // Find out game type based on file path and settings
      std::pair<Game, bool> detectGameType(const std::wstring& filePath, const CompilerSettings& compilerSettings);

      // Clear cached active compilation request, so when buffer gets switched
      // in NPP it can be properly handled
      void clearActiveCompilation();

      // Plugin's own message handling
      static LRESULT CALLBACK messageHandleProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
      LRESULT handleOwnMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

      // Copy source file to destination (possibly overwrite). May invoke shell command to execute if privilege
      // elevation is needed. In that case, "waitFor" will be used to determien how long the process is going
      // to wait for the execution. By default it only waits for up to 3 seconds.
      bool copyFile(std::wstring sourceFile, std::wstring destinationFile, int waitFor = 3000);

      // Menu functions
      //
      void setupAdvancedMenu();
      static void advancedMenuFunc(); // Still need an empty func so NPP won't render the menu item as a separator
      void showLangID();
      void addAutoCompletion();
      void addFunctionList();

      static void compileMenuFunc();
      void compile();
      static void settingsMenuFunc();
      void showSettings();
      static void aboutMenuFunc();
      void showAbout();

      // Private members
      //
      FuncItem funcs[utility::underlying(Menu::COUNT)];

      UINT advancedMenuBaseCmdID {};

      HWND messageWindow {};

      HINSTANCE instance {};
      NppData nppData;

      Settings settings;
      SettingsStorage settingsStorage;
      SettingsDialog settingsDialog {settings};

      std::unique_ptr<Compiler> compiler;
      CompilationRequest activeCompilationRequest;
      bool isComplingCurrentFile {false};

      std::unique_ptr<ErrorAnnotator> errorAnnotator;
      std::unique_ptr<ErrorsWindow> errorsWindow;
      std::list<Error> activatedErrorsTrackingList;
      std::unique_ptr<utility::Timer> jumpToErrorLineTimer;

      npp_lang_type_t scriptLangID {0};

      AboutDialog aboutDialog;
  };

} // namespace
