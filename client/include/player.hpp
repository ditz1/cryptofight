#pragma once

#include <globals.hpp>
#include <networking.hpp>

typedef struct AnimData {
    Image img;
    Texture2D tex;
    int fc = 0;
} AnimData;

typedef class Player {
public:
    Player();
    ~Player();
    PlayerState State();
    PlayerState RequestedState();
    void inline SetRequestedState(PlayerState state) { _requested_state = uint8_t(state); }
    void inline SetIsAnimating(bool is_animating) { _is_animating = is_animating; }
    void PollInput();
    void Update(GameState& game);
    void Draw();
    bool inline IsAnimating() { return _is_animating; }
    uint8_t Id();
    void SetId(int id);
    bool Ready();
    int inline FaceDir() { return _player_face_dir; }
    Vector2int Position();
    void inline SetPosition(Vector2int position) { _position = position; }
    void inline SetState(PlayerState state) { _state = state; }
    int inline Hp() { return _hp; }
    void inline SetHp(int hp) { _hp = hp; }
    void ProcessPlayerAnimLogic();    
    void SetTexture(int texture_id);
    void LoadTextures();
    void PlayAnimOnce(PlayerState state);
    void AssignTexture(PlayerState state);

    // anim stuff, supposed to replicate "dummy" from anim_testing
    int fc; // frame counter total, mainly needed for delay
    int anim_current_frame;
    unsigned int buffer_offset; // offset to move pointer in img data
    int current_anim;
    int fc_delay; // this should be global
    Texture2D* tex; // this should be loaded from img anyway
    Image* img;
    std::array<AnimData, 7> texs;

private:
    void PollAttackInput();
    bool _textures_loaded;
    uint8_t _id;
    Vector2int _position;
    int _hp;
    bool _is_animating;
    int _player_face_dir;
    bool _ready;
    PlayerState _state;
    uint8_t _requested_state;
    Color _color;

    // image stuff, these will be assigned from characters later, for now just a placeholder

} Player;