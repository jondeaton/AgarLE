#pragma once

#include "renderer.hpp"
#include "renderables.hpp"

#include <string>
#include <ctime>

class AgarioClient {
public:

  void connect() { }


  void game_loop(){
    renderer.draw_grid();
    renderer.draw_foods(_player, foods);
    renderer.draw_pellets(_player, pellets);
    renderer.draw_viruses(_player, viruses);

    renderer.draw_border();

    renderer.draw_players(_player, players);

//    socket.emit('0', window.canvas.target); // playerSendTarget "Heartbeat".
  }


  // socket.on('serverTellPlayerMove', function (userData, foodsList, massList, virusList) {
  void server_tell_player_move() {
    // todo: yeah... gotta figure this one out
  }

private:
  AgarioRenderer renderer;

  std::clock_t g_PreviousTicks;
  std::clock_t g_CurrentTicks;

  unsigned int player_id;

  player _player;
  std::vector<player> players;
  std::vector<food> foods;
  std::vector<pellet> pellets;
  std::vector<virus> viruses;
};