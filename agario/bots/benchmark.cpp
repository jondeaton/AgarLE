#include <iostream>
#include <sstream>
#include <map>

#include <engine/Engine.hpp>
#include <bots/bots.hpp>

#define RENDERABLE false

#define NUM_GAMES 1
#define NUM_BOTS 7
#define GAME_DURATION 1.0 // minutes

#define TICK_FREQUENCY 60 // ticks per second
#define TICK_DURATION (1.0 / TICK_FREQUENCY)

#define TICKS_PER_GAME static_cast<int>(60 * GAME_DURATION * TICK_FREQUENCY)

namespace agario {
  namespace bot {

    class GameRecap {
    public:

      /* records the score from a single player */
      void append(const std::string &name, agario::mass score) {
        scores[name].emplace_back(score);
      }

      /* return the names of all the bots being tested */
      std::vector<std::string> names() const {
        if (scores.empty()) return {};
        std::vector<std::string> names;
        for (auto &pair : scores)
          names.emplace_back(pair.first);
        return names;
      }

      float average(const std::string &name) const {
        float avg = 0;
        int count = 0;
        for (auto mass : scores.at(name)) {
          avg += mass;
          count++;
        }
        return avg / count;
      }

      float max(const std::string &name) const {
        float m = 0;
        for (auto &mass : scores.at(name))
          m = std::max<float>(m, mass);
        return m;
      }

      float min(const std::string &name) const {
        float m = std::numeric_limits<float>::max();
        for (auto &mass : scores.at(name))
          m = std::min<float>(m, mass);
        return m;
      }

    private:
      // UID => list of scores
      std::map<std::string, std::vector<agario::mass>> scores;
    };

    class GamesRecap {
    public:
      GameRecap &new_game_recap() {
        totals.emplace_back();
        return totals.back();
      }

      std::vector<std::string> names() const {
        if (totals.empty()) return {};
        else return totals.front().names();
      }

      float macro_average(const std::string &name) const {
        return macro_stat(&GameRecap::average, name);
      }

      float macro_maximum(const std::string &name) const {
        return macro_stat(&GameRecap::max, name);
      }

      float macro_minimum(const std::string &name) const {
        return macro_stat(&GameRecap::min, name);
      }

    private:
      std::vector<GameRecap> totals;

      template <typename F>
      float macro_stat(F get_stat, const std::string &name) const {
        float average = 0;
        int count = 0;
        for (auto &recap : totals) {
          average += (recap.*get_stat)(name);
          count++;
        }
        return average / count;
      }
    };

    std::ostream &operator<<(std::ostream &os, const GamesRecap& recaps) {
      auto names = recaps.names();
      for (auto &name : names) {
        os << name << "\t";
        os << recaps.macro_average(name) << "\t";
        os << recaps.macro_minimum(name) << "\t";
        os << recaps.macro_maximum(name) << std::endl;
      }
      return os;
    }

    template<typename ...Ts>
    class BotEvaluator {
    public:
      explicit BotEvaluator(int num_bots) : _num_bots(num_bots) {}

      /**
       * Executes `num_games` number of games, recording results
       * after each of the games
       * @param num_games the number of games to play
       */
      void run(int num_games) {
        for (int game = 0; game < num_games; game++) {
          std::cout << "Playing game " << game << std::endl;
          engine.reset();
          add_bots<Ts...>();

          agario::time_delta dt(TICK_DURATION);
          for (int t = 0; t < TICKS_PER_GAME; t++)
            engine.tick(dt);

          record_scores(engine.get_game_state());
        }
      }

      const GamesRecap &stats() { return recaps; }

    private:
      GamesRecap recaps;
      agario::Engine<RENDERABLE> engine;
      int _num_bots;

      /**
       * Records statistics about the scores after a single game
       * @param state The game state at the end of a single game
       */
      void record_scores(const agario::GameState<RENDERABLE> &state) {
        GameRecap &recap = recaps.new_game_recap();

        for (auto &pair : state.players) {
          auto &player = *pair.second;
          const std::string &name = player.name();
          recap.append(name, player.mass());
        }
      }

      template <typename ...Us>
      void add_bots() { _add_bots<Us...>(); }

      template<> void add_bots<>() {} // recursive base case

      /* add multiple kinds of bots, using the typeid as name */
      template <typename U, typename ...Us>
      void _add_bots() {
        for (int i = 0; i < _num_bots; i++) {
          std::string name = typeid(U).name();
          engine.add_player<U>(name);
        }
        add_bots<Us...>();
      }

    };
  }
}

int main(int argc, char *argv[]) {
  using namespace agario::bot;
  using HungryBot = HungryBot<RENDERABLE>;
  using HungryShyBot = HungryShyBot<RENDERABLE>;

  // configure which bots to evaluate in this template pack
  BotEvaluator<HungryBot, HungryShyBot> evaluator(NUM_BOTS);
  evaluator.run(NUM_GAMES);

  std::cout << evaluator.stats() << std::endl;

  return 0;
}