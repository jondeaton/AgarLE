
#ifndef __AgarLE_INTERFACE_HPP__
#define __AgarLE_INTERFACE_HPP__

#include "emucore/FSNode.hxx"
#include "emucore/OSystem.hxx"
#include "os_dependent/SettingsWin32.hxx"
#include "os_dependent/OSystemWin32.hxx"
#include "os_dependent/SettingsUNIX.hxx"
#include "os_dependent/OSystemUNIX.hxx"
#include "games/Roms.hpp"
#include "common/display_screen.h"
#include "environment/stella_environment.hpp"
#include "common/ScreenExporter.hpp"
#include "common/Log.hpp"


#include <string>
#include <memory>


class AgarLEInterface {
public:
  ALEInterface();
  ~ALEInterface();

  // Setting getters
  std::string getString(const std::string& key);
  int getInt(const std::string& key);
  bool getBool(const std::string& key);
  float getFloat(const std::string& key);

  // Set the value of a setting. loadRom() must be called before the
  // setting will take effect.
  void setString(const std::string& key, const std::string& value);
  void setInt(const std::string& key, const int value);
  void setBool(const std::string& key, const bool value);
  void setFloat(const std::string& key, const float value);

  void restartAgarLE();
  reward_t act(Action action);
  bool game_over() const;
  void reset_game();

  std::vector<Action> getLegalActoinSet();
  std::vector<Action>


};


#endif
