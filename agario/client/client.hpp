#pragma once

#include "renderer.hpp"

#include <string>

// probably going to need something like this
//#include <netdb.h>                // for gethostbyname
//#include <sys/socket.h>           // for socket, AF_INET
//#include <sys/types.h>            // for SOCK_STREAM
//#include <unistd.h>               // for close


class AgarioClient {
public:


  void connect() { }


  void game_loop(){
    renderer.draw_grid();
    renderer.draw_foods();
    renderer.draw_pellets();
    renderer.draw_viruses();

    renderer.draw_border();

    renderer.draw_players();

//    socket.emit('0', window.canvas.target); // playerSendTarget "Heartbeat".
  }


  // socket.on('serverTellPlayerMove', function (userData, foodsList, massList, virusList) {
  void server_tell_player_move() {

  }

private:
  AgarioRenderer renderer;

  std::vector<
};