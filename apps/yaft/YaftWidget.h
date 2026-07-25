#pragma once

#include "config.h"
#include "keyboard.h"
#include "layout.h"
#include "screen.h"

#include <yaft.h>

#include <UI/Rotate.h>
#include <UI/StatefulWidget.h>

#include <filesystem>

class YaftState;

class Yaft : public rmlib::StatefulWidget<Yaft> {
public:
  Yaft(const char* cmd, char* const argv[], YaftConfigAndError config)
    : configOrPath(std::move(config)), cmd(cmd), argv(argv) {}

  Yaft(const char* cmd, char* const argv[], std::filesystem::path configPath)
    : configOrPath(std::move(configPath)), cmd(cmd), argv(argv) {}

  static YaftState createState();

  YaftConfigAndError getConfigAndError() const;

private:
  friend class YaftState;

  std::variant<YaftConfigAndError, std::filesystem::path> configOrPath;

  const char* cmd;
  char* const* argv;
};

class YaftState : public rmlib::StateBase<Yaft> {
public:
  ~YaftState();

  /// Logs the given string to the terminal console.
  void logTerm(std::string_view str);

  void init(rmlib::AppContext& ctx, const rmlib::BuildContext& /*unused*/);

  auto build(rmlib::AppContext& ctx,
             const rmlib::BuildContext& buildCtx) const {
    using namespace rmlib;

    static int buildCount = 0;
    std::cerr << "yaft: build #" << ++buildCount << " rotation=" << static_cast<int>(rotation) << " hideKeyboard=" << hideKeyboard << "\n";

    const auto& layout = [this]() -> const Layout& {
      if (hideKeyboard) {
        return empty_layout;
      }

      if (smallKeyboard) {
        return hidden_layout;
      }

      return *config.layout;
    }();

    const auto repeatRateMs =
      std::chrono::milliseconds(1000) / config.repeatRate;
    // Use Screen::isLandscape for rotation instead of the Rotated widget
    // wrapper. The Rotated widget rotates a portrait-sized canvas, producing a
    // thin column of text on a mostly-blank landscape screen. isLandscape=true
    // makes Screen size the terminal in landscape dimensions AND draw pixels
    // at swapped coordinates, filling the whole screen.
    const bool isLandscape = rotation != Rotation::None;
    return Column(
      Expanded(Screen(term.get(), isLandscape, config.autoRefresh)),
      Keyboard(term.get(),
               KeyboardParams{
                 .layout = layout,
                 .keymap = *config.keymap,
                 .repeatDelay = std::chrono::milliseconds(config.repeatDelay),
                 .repeatTime = repeatRateMs,
               },
               [this](int num) {
                 setState([](auto& self) {
                   self.smallKeyboard = !self.smallKeyboard;
                 });
               }));
  }

private:
  void checkLandscape(rmlib::AppContext& ctx);
  void readInotify(rmlib::AppContext& ctx) const;

  std::unique_ptr<terminal_t> term;
  rmlib::TimerHandle pogoTimer;

  std::filesystem::path watchPath;
  unistdpp::FD inotifyFd;
  int inotifyWd;

  YaftConfig config;
  rmlib::Rotation rotation = rmlib::Rotation::None;
  bool smallKeyboard = false;
  bool hideKeyboard = false;
};
