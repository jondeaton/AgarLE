#include <iostream>
#include <sstream>
#include <map>

#include <engine/Engine.hpp>
#include <bots/bots.hpp>

#include <utils/thread-pool.h>
#include <utils/ostreamlock.h>
#include <mutex>

#define RENDERABLE false

#define NUM_GAMES 10
#define NUM_BOTS 7
#define GAME_DURATION 3.0 // minutes

#define TICK_FREQUENCY 30 // ticks per second
#define TICK_DURATION (1.0 / TICK_FREQUENCY)

#define TICKS_PER_GAME static_cast<int>(60 * GAME_DURATION * TICK_FREQUENCY)

#define NUM_THREADS 4

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

      // fin the longest name
      std::vector<unsigned long> lens;
      std::transform(names.begin(), names.end(), std::back_inserter(lens),
        [](const std::string &name){ return name.length(); });
      auto longest = *std::max_element(lens.begin(), lens.end());

      os << std::setw(longest) << "Name";
      os << std::setw(5) << "Avg.";
      os << std::setw(5) << "Max.";
      os << std::setw(5) << "Min." << std::endl;
      os << std::setfill('=') << std::setw(40) << "" << std::setfill(' ') << std::endl;

      for (auto &name : names) {
        os << std::setw(longest) << name;
        os << std::setw(5) << (int) recaps.macro_average(name);
        os << std::setw(5) << (int) recaps.macro_maximum(name);
        os << std::setw(5) << (int) recaps.macro_minimum(name) << std::endl;
      }
      return os;
    }

    template<typename ...Ts>
    class BotEvaluator {
    public:
      explicit BotEvaluator(int num_bots) : _num_bots(num_bots),
      pool(NUM_THREADS) {}

      /**
       * Executes `num_games` number of games, recording results
       * after each of the games
       * @param num_games the number of games to play
       */
      void run(int num_games) {
        for (int game = 0; game < num_games; game++) {
          pool.schedule([=](){
            std::cout << oslock << "Playing game " << game << std::endl << osunlock;
            agario::Engine<RENDERABLE> engine;
            engine.reset();
            add_bots<Ts...>(engine);

            agario::time_delta dt(TICK_DURATION);
            for (int t = 0; t < TICKS_PER_GAME; t++)
              engine.tick(dt);

            m.lock();
            record_scores(engine.get_game_state());
            m.unlock();
          });

        }

        pool.wait();
      }

      const GamesRecap &stats() { return recaps; }

    private:
      GamesRecap recaps;
      int _num_bots;

      ThreadPool pool;
      std::mutex m;

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
      void add_bots(agario::Engine<RENDERABLE> &engine) { _add_bots<Us...>(engine); }

      template<> void add_bots<>(agario::Engine<RENDERABLE> &engine) {} // recursive base case

      /* add multiple kinds of bots, using the typeid as name */
      template <typename U, typename ...Us>
      void _add_bots(agario::Engine<RENDERABLE> &engine) {
        for (int i = 0; i < _num_bots; i++) {
          engine.add_player<U>();
        }
        add_bots<Us...>(engine);
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