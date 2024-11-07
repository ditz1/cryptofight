#include <game_manager.hpp>

void InitGameState(GameState* game){
    for (uint8_t& player_state : game->player_states) {
        player_state = 0xBB;
    }
    for (uint8_t& player_hp : game->player_hps){
        player_hp = 0xCC;
    }
    for (uint8_t& player_id : game->player_ids) {
        player_id = 0xAA;
    }
    for (Vector2int& player_position : game->player_positions) {
        player_position = Vector2int{0xDDDD, 0xEEEE};
    }
}

void RequestStateUpdate(GameState* game, Connection* conn, Player* player) {
    if (current_game_stage < 1) return;
    if ((int(player->RequestedState()) <= int(PlayerState::IDLE))) { 
        
        if (player->RequestedState() == PlayerState::IDLE 
            && (player->State() == PlayerState::IDLE)) return;
            
        game->player_states[int(player->Id())] = uint8_t(player->RequestedState());
        SendGameStateRequest(game, conn);
    }
}

void ParseAssignPlayerId(GameState* game, Connection* conn, Player* player){
    if (!conn->connected || data_from_server.size() == 0 || data_from_server[0] != msg_assign_id) return;
    if (current_game_stage > 0) return;
    player->SetId(data_from_server[1]);
    game->player_ids[data_from_server[1]] = data_from_server[1];
    game->player_positions[data_from_server[1]].x = 200;
    game->player_positions[data_from_server[1]].y = 200;
    this_client_id = int(data_from_server[1]);
}

void UpdateGameState(GameState* game, Connection* conn){
    if (data_from_server.size() < 32) return;
    std::array<uint8_t, 32> last_received_bytes;
    for (size_t i = 0; i < 32; i++) {
        last_received_bytes[i] = data_from_server[i];
    }
    data_from_server.clear();
    GameState new_game_state;
    new_game_state.FromBytes(last_received_bytes);
    for (int i = 0; i < 4; i++){
        game->player_states[i] = new_game_state.player_states[i];
        game->player_positions[i] = new_game_state.player_positions[i];
        game->player_hps[i] = new_game_state.player_hps[i];
    }
}

void ParseGameState(GameState* game, Connection* conn, Player* player) {
    if (!conn->connected) return;
    if (data_from_server.size() < 1) return;

    num_failed_pings <= 0 ? num_failed_pings = 0 : num_failed_pings--;
    

    switch (data_from_server[0]){
        case msg_connect:
            std::cout << "Connected To Server" << std::endl;
            break;
        case msg_disconnect:
            std::cout << "Disconnected From Server" << std::endl;
            break;
        case msg_assign_id:
            std::cout << "Assigned ID: " << int(data_from_server[1]) << std::endl;
            ParseAssignPlayerId(game, conn, player);
            break;
        case msg_update:
            //std::cout << "Update" << std::endl;
            UpdateGameState(game, conn);
            break;
    }
    conn->last_received->numBytes = 0;
    player->SetHp(game->player_hps[int(player->Id())]);
    player->SetState(PlayerState(game->player_states[int(player->Id())]));
    player->SetPosition((game->player_positions[int(player->Id())]));
    player->AssignTexture(player->State());
}

void SendReadyRequest(Player* player, Connection* conn){
    if (!conn->connected) return;
    std::array<uint8_t, 32> bytes_to_send;
    bytes_to_send[0] = msg_lobby;
    bytes_to_send[1] = msg_player_ready;
    bytes_to_send[2] = msg_signature;
    bytes_to_send[3] = player->Id();
    bytes_to_send[4] = msg_end;
    ClientSendBytes(conn, (void*)&bytes_to_send, 32);
}

void SendGameStateRequest(GameState* game, Connection* conn) {
    if (!conn->connected) return;
    num_failed_pings++;
    std::array<uint8_t, 32> bytes_to_send;
    bytes_to_send[0] = msg_update;
    std::array<uint8_t, 28> game_bytes = game->ToBytes();
    for (int i = 0; i < 28; i++) {
        bytes_to_send[i+1] = game_bytes[i];
    }
    bytes_to_send[29] = msg_signature;    
    bytes_to_send[30] = this_client_id;
    bytes_to_send[31] = msg_end;
    ClientSendBytes(conn, (void*)&bytes_to_send, 32);
}

int ParseLobbyState(GameState* game, std::array<Player, 4>& all_players){
    if (data_from_server.size() < 32) return 0;
    if (data_from_server[0] != msg_lobby) return 0;
    std::array<uint8_t, 32> last_received_bytes;
    std::copy(data_from_server.begin(), data_from_server.end(), last_received_bytes.begin());
    data_from_server.clear();
    if (last_received_bytes[1] == msg_load_stage_grid){
        std::cout << "received load stage grid" << std::endl;
        if (last_received_bytes[2] == 0 && last_received_bytes[3] == msg_from_server){
            std::cout << "starting to load stage data" << std::endl;
            return 1;        
        } else if (last_received_bytes[2] == msg_switch_to_game){
            current_game_stage = 1;
            return 1;
        }
    }
    int players_connected = last_received_bytes[1];
    num_players_connected = players_connected;
    for (int i = 2; i < 6; i++){
        last_received_bytes[i] > 0 ? player_ready[i-2] = true : player_ready[i-2] = false;
    }
    return 0;
}

std::vector<uint8_t> SerializeStageData(std::vector<Rectangle>& stage_cells, std::vector<Rectangle>& player_cells){
    std::vector<uint8_t> data;
    Rectangle buffer_rect = {-1.0f, -1.0f, -1.0f, -1.0f};


    return data;
}

std::vector<std::array<uint8_t, 32>> CreateStageMessage(std::vector<uint8_t> serialized_data){
    std::vector<std::array<uint8_t, 32>> messages;
    std::array<uint8_t, 32> msg;
    msg[0] = msg_stage_data;
    msg[1] = 1;
    msg[2] = 2;
    msg[3] = 3;
    msg[4] = 4;
    msg[5] = msg_end;

    messages.push_back(msg);

    return messages;
}

void ListenStageData(Connection* conn, Player& client, std::array<Player, 4>& players, Stage& stage){
    if (data_from_server.size() < 32) return;
    if (data_from_server[0] != msg_stage_data) return;
    std::array<uint8_t, 32> last_received_bytes;
    std::copy(data_from_server.begin(), data_from_server.end(), last_received_bytes.begin());
    data_from_server.clear();
    if (last_received_bytes[1] == msg_end_stage_data && last_received_bytes[2] == msg_switch_to_game){
        std::cout << "recieved end stage data" << std::endl;
        current_game_stage = 1;
        in_loading_screen = false;
        return;
    }
}

void StartSendStageData(Connection* conn, std::array<Player, 4>& players, Stage& stage){
    if (this_client_id != 0) return;
    std::cout << "starting to send stage data" << std::endl;
    std::array<uint8_t, 32> message;
    message[0] = msg_lobby;
    message[1] = msg_load_stage_grid;
    message[2] = 0;
    message[3] = msg_signature;
    message[4] = msg_end;
    ClientSendBytes(conn, (void*)&message, 32);
}

void EndSendStageData(Connection* conn, std::array<Player, 4>& players, Stage& stage){
    if (this_client_id != 0) return;
    std::array<uint8_t, 32> message;
    message[0] = msg_stage_data;
    message[1] = msg_end_stage_data;
    message[2] = msg_signature;
    message[3] = msg_end;
    ClientSendBytes(conn, (void*)&message, 32);
}

void SendStageData(Connection* conn, Player& client, std::array<Player, 4>& players, Stage& stage){
    if (stage_sent) {
        in_loading_screen = true;
        StartSendStageData(conn, players, stage);
        stage_sent = false;
        return;
    }
    if (!in_loading_screen) return;
    if (this_client_id != 0) return;
    std::vector<Rectangle> stage_rects;
    std::vector<Rectangle> player_rects;
    for (StageCell cell : stage.cells){
        if (cell.collidable) stage_rects.push_back(cell.rect);
    }
    for (Player player : players){
        player_rects.push_back(player.Bounds());
    }
    std::vector<uint8_t> serial_data = SerializeStageData(stage_rects, player_rects);
    std::vector<std::array<uint8_t, 32>> messages = CreateStageMessage(serial_data);

    for (std::array<uint8_t, 32> message : messages){
        ClientSendBytes(conn, (void*)&message, 32);
    }
    std::cout << "sent " << messages.size() << " messages" << std::endl;
    EndSendStageData(conn, players, stage);
}


void LoadGameState(GameState* game, Player& client, std::array<Player, 4>& players){
    for (int i = 0; i < 4; i++){
        players[i].LoadTextures();        
    }
    AdjustPlayerDimensions(client, players);
}

void AdjustPlayerDimensions(Player& client, std::array<Player, 4>& all_players){
    int curr_width = client.tex->width * (client.draw_data.scale / 3.0f);
    while (curr_width >= stage.cell_size) {
        client.draw_data.scale -= 0.1f;
        curr_width = client.tex->width * (client.draw_data.scale / 3.0f);
    }

    for (auto& player : all_players){
        int curr_width = player.tex->width * (player.draw_data.scale / 3.0f);
        while (curr_width >= stage.cell_size) {
            player.draw_data.scale -= 0.1f;
            curr_width = player.tex->width * (player.draw_data.scale / 3.0f);
        }
    }
}

void ParseEndState(GameState* game, Connection* conn, Player* player){
    std::cout << "ERROR: End state not yet implemented" << std::endl;
}

void DrawGameState(std::array<Player, 4> players){
    for (int i = 0; i < 4; i++){
        players[i].Draw();
    }
}

// this is basically a paraphrased version of the checks in Player::ProcessPlayerAnimLogic
void UpdatePlayerCopyAnimInfo(Player& copy) {
    if (copy.Id() == this_client_id) return;
    if (copy.last_state != copy.State()) {
        copy.last_state = copy.State();
        copy.anim_current_frame = 0;
    }


    if (copy.State() == MOVE_LEFT && copy.FaceDir() == 1) {
        copy.SetFaceDir(-1);
    } else if (copy.State() == MOVE_RIGHT && copy.FaceDir() == -1) {
        copy.SetFaceDir(1);
    }

    copy.AssignTexture(copy.State());

    if (copy.IsAttacking() && (copy.anim_current_frame >= copy.texs[copy.current_anim].fc - 1)) {
        copy.SetAttacking(false);
    }
        
    // this is 4 because single byte per channel (RGBA)
    copy.buffer_offset = copy.img->width * copy.img->height * 4 * copy.anim_current_frame;
    
    copy.fc++; // total fc
    if (copy.fc >= copy.fc_delay) {
        // move to next frame
        copy.anim_current_frame++;
        if (copy.anim_current_frame >= copy.texs[copy.current_anim].fc) copy.anim_current_frame = 0; // if final frame is reached we return to first frame
        // get memory offset position for next frame data in image.data
        copy.buffer_offset = copy.img->width*copy.img->height*4*copy.anim_current_frame;
        // WARNING: data size (frame size) and pixel format must match already created texture
        // "void* pixels" is pointer to image raw data
        UpdateTexture(*(copy.tex), ((unsigned char *)copy.img->data) + copy.buffer_offset);
        copy.fc = 0;
    }
    
}

void UpdateClientPlayerCopies(std::array<Player, 4>& players, GameState* game){
    for (int i = 0; i < 4; i++){
        players[i].SetState(PlayerState(game->player_states[i]));
        players[i].SetHp(game->player_hps[i]);
        players[i].SetPosition(game->player_positions[i]);
        UpdatePlayerCopyAnimInfo(players[i]);
    }
}


void DrawLobbyState(GameState* game) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    DrawRectangle(screen_width/2 - 400, screen_height/2 - 400, 200, 200, player_ready[0] ? GREEN : RED);
    DrawRectangle(screen_width/2 + 200, screen_height/2 - 400, 200, 200, player_ready[1] ? GREEN : RED);
    DrawRectangle(screen_width/2 - 400, screen_height/2 + 200, 200, 200, player_ready[2] ? GREEN : RED);
    DrawRectangle(screen_width/2 + 200, screen_height/2 + 200, 200, 200, player_ready[3] ? GREEN : RED);
}