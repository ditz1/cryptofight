#include <game_manager.hpp>

void InitGameState(GameState* game){
    for (int& player_state : game->player_states) {
        player_state = int(PlayerState::UNREGISTERED);
    }
    for (int& player_id : game->player_ids) {
        player_id = -1;
    }
    for (Vector2& player_position : game->player_positions) {
        player_position = Vector2{-200.0f -200.0f};
    }
}

void ParseGameState(GameState* game, Connection* conn, Player* player) {
    if (!conn->connected) return;
    if (conn->last_received->numBytes < 1) return;

    std::cout << "num bytes from server: " << int(conn->last_received->numBytes) << std::endl;

    for (int i = 0; i < conn->last_received->numBytes; i++) {
        printf("%x | ", conn->last_received->data[i]);
    }
    printf("\n");

    switch (conn->last_received->data[0]){
        case xCONNECT:
            std::cout << "Connected To Server" << std::endl;
            break;
        case xDISCONNECT:
            std::cout << "Disconnected From Server" << std::endl;
            break;
        case xASSIGN_ID:
            std::cout << "Assigned ID: " << int(conn->last_received->data[1]) << std::endl;
            player->SetId(int(conn->last_received->data[1]));
            break;
        case xUPDATE:
            std::cout << "Update" << std::endl;
            break;
    }

    std::cout << "Game State" << std::endl;
    for (int i = 0; i < sizeof(game->ToBytes().c_str()); i++) {
        printf("%x |", game->ToBytes().c_str()[i]);
    }

    
    printf("\n");

    if (game->player_states[player->Id() != int(player->RequestedState())]) {
        game->player_states[player->Id()] = int(player->RequestedState());
        for (int i = 0; i < sizeof(game->ToBytes().c_str()); i++) {
            printf("%x |", game->ToBytes().c_str()[i]);
        }
    }
    conn->last_received->numBytes = 0;
}

void SendGameState(GameState* game, Connection* conn) {
    if (!conn->connected) return;
    std::string bytes = game->ToBytes();
    ClientSendBytes(conn, (void*)bytes.c_str(), uint32_t(bytes.size()));
}