#include <helpers.hpp>
#include <player.hpp>

void DrawDebugInfo(GameState game, Player& client_player, std::array<Player, 4> all_players) {
            int font_size = 16;
            int y_spacing = 25;
            int x_start = 30;
            int y_start = 30;
            int y_start_other = 30;
            int x_start_other = GetScreenWidth() - 120;
            unsigned int frame_gap = 0;

            // right side
            Color grey1 = BEIGE;
            grey1.a = 150.0f;
            Rectangle bg1 = {(float)x_start_other - 10, (float)y_start_other - 10, 150.0f, 200.0f};
            //DrawRectangleRounded(bg1, 0.1f, 10, grey1);
            DrawFPS(x_start_other, y_start_other - y_spacing);            
            DrawText(TextFormat("Stage: %d", current_game_stage), x_start_other, y_start_other, font_size, RAYWHITE);
            DrawText(TextFormat("Players [ ]"), x_start_other, y_start_other + y_spacing, font_size, RAYWHITE);
            DrawText(TextFormat("P0: %d | %s", all_players[0].anim_current_frame, PlayerStateToString(all_players[0].State()).c_str()), x_start_other, y_start_other + y_spacing * 2, font_size, RAYWHITE);
            DrawText(TextFormat("P1: %d | %s", all_players[1].anim_current_frame, PlayerStateToString(all_players[1].State()).c_str()), x_start_other, y_start_other + y_spacing * 3, font_size, RAYWHITE);
            DrawText(TextFormat("P2: %d | %s", all_players[2].anim_current_frame, PlayerStateToString(all_players[2].State()).c_str()), x_start_other, y_start_other + y_spacing * 4, font_size, RAYWHITE);
            DrawText(TextFormat("P3: %d | %s", all_players[3].anim_current_frame, PlayerStateToString(all_players[3].State()).c_str()), x_start_other, y_start_other + y_spacing * 5, font_size, RAYWHITE);
            
            frame_gap = std::abs(all_players[0].anim_current_frame - all_players[1].anim_current_frame);
            if (all_players[0].State() == all_players[1].State()) {
                DrawText(TextFormat("Frame Gap: %d", frame_gap), x_start_other, y_start_other + y_spacing * 6, font_size - 4, RED);
            }
            DrawText(TextFormat("# failed pings: %d", num_failed_pings), x_start_other, y_start_other + y_spacing * 7, font_size - 4, RAYWHITE);

            Rectangle bg2 = {(float)x_start - 10, (float)y_start - 10, 195.0f, 200.0f};
            //DrawRectangleRounded(bg2, 0.1f, 10, grey1);
            // left side
            DrawText("Client", x_start, y_start, font_size, RAYWHITE);
            DrawText(TextFormat("- Player %d -", game.player_ids[this_client_id]), x_start, y_start + y_spacing, font_size, RAYWHITE);
            DrawText(TextFormat("State: %s, %s", PlayerStateToString(PlayerState(game.player_states[this_client_id])).c_str(), PlayerStateToString(client_player.State()).c_str()) , x_start, y_start + y_spacing * 2, font_size, RAYWHITE);
            DrawText(TextFormat("RequestedState: %s", PlayerStateToString(client_player.RequestedState()).c_str()) , x_start, y_start + y_spacing * 3, font_size, RAYWHITE);
            DrawText(TextFormat("CurrentAnimFrame: %d", client_player.texs[client_player.current_anim].fc), x_start, y_start + y_spacing * 4, font_size, RAYWHITE);
            DrawText(TextFormat("IsAnimating: %d", client_player.IsAnimating()), x_start, y_start + y_spacing * 5, font_size, RAYWHITE);
            DrawText(TextFormat("Current Anim FC: %d", client_player.anim_current_frame), x_start, y_start + y_spacing * 6, font_size, RAYWHITE);
            DrawText(TextFormat("FaceDir: %d", client_player.FaceDir()), x_start, y_start + y_spacing * 7, font_size, RAYWHITE);
}

void LogGameState(GameState game, Connection* conn) {
    printf("Game State: %d\n", int(sizeof(game.ToBytes())));
    std::array<uint8_t, 28> bytes = game.ToBytes();
    for (int i = 0; i < sizeof(game.ToBytes()); i++) {
        printf(" %x |", bytes[i]);
    }
    printf("\n");
}

void StripStageString(std::string& stage_str) {

    char reserved_chars[] = {'!',' ','\n','\r', '\t'};

    for (char c : reserved_chars) {
        for (int i = 0; i < stage_str.size(); i++) {
            if (stage_str.at(i) == c) {
                stage_str.erase(i, 1);
            } else {
                stage_str.erase(remove_if(stage_str.begin(), stage_str.end(), isspace));
            }
        }
    }
}
