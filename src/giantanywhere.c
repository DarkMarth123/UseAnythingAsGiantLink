#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "z64player.h"
#include "attributes.h"

#define SPEED_MODE_CURVED 0.018f

extern void func_8083F57C(Player* this, PlayState* play);
extern void func_8083CB58(Player* this, f32 arg1, s16 arg2);
extern s32 Player_TryActionHandlerList(PlayState* play, Player* this, s8* actionHandlerList, s32 updateUpperBody);
extern void func_80839E3C(Player* this, PlayState* play);
extern void func_8083C8E8(Player* this, PlayState* play);
extern void func_8083A794(Player* this, PlayState* play);
extern bool Player_IsZTargetingWithHostileUpdate(Player* this);
extern s32 Player_GetMovementSpeedAndYaw(Player* this, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode, PlayState* play);
extern s32 func_8083A4A4(Player* this, f32* speedTarget, s16* yawTarget, f32 decelerationRate);

extern s8 sActionHandlerList8[];

extern int should_be_giant;

static PlayState* s_playPtr_interface1 = NULL;
static s16 s_original_scene_id = -1;
static float modifier = 2.0f;


RECOMP_HOOK("Interface_UpdateButtonsPart2") void Fetch_PlayState_and_Scene_Id(PlayState* play) {
    s_playPtr_interface1 = play;
    s_original_scene_id = play->sceneId;
    play->sceneId = SCENE_INISIE_BS;

}


RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart2") void GiantMask_Restore_Scene_Id() {
    PlayState* play = s_playPtr_interface1;

    if (play == NULL) {
        return;
    }

    play->sceneId = s_original_scene_id;

}


RECOMP_HOOK("Player_UpdateCommon") void GiantMask_Outside_Boss(Player* player, PlayState* play) {
    if (play->sceneId != SCENE_INISIE_BS) {
        player->stateFlags1 &= ~PLAYER_STATE1_100;

        f32 scale_multiplier = should_be_giant ? modifier : 1.0f;

        if (player->transformation != PLAYER_FORM_FIERCE_DEITY) {
            Actor_SetScale(&player->actor, 0.01f * scale_multiplier); // giant
        }else {
            Actor_SetScale(&player->actor, 0.015f * scale_multiplier); // fd giant
        }
        
        if (should_be_giant) {
            player->cylinder.dim.radius = (s16)(18.0f * modifier);  // Default is ~18
            player->cylinder.dim.height = (s16)(44.0f * modifier);  // Default is ~44
            player->cylinder.dim.yShift = (s16)(0.0f * modifier);   // Adjust if needed
        }
    }
}


RECOMP_PATCH void Player_Action_13(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_20;
    func_8083F57C(this, play);
    if (Player_TryActionHandlerList(play, this, sActionHandlerList8, true)) {
        return;
    }

    if (Player_IsZTargetingWithHostileUpdate(this)) {
        func_8083A794(this, play);
        return;
    }

    Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    if (this->currentMask == PLAYER_MASK_BUNNY) {
        speedTarget *= 1.5f;
    }


    if (play->sceneId != SCENE_INISIE_BS) {
        if (should_be_giant) {
            speedTarget *= modifier;
        }
    }


    if (!func_8083A4A4(this, &speedTarget, &yawTarget, R_DECELERATE_RATE / 100.0f)) {
        func_8083CB58(this, speedTarget, yawTarget);
        func_8083C8E8(this, play);
        if ((this->speedXZ == 0.0f) && (speedTarget == 0.0f)) {
            func_80839E3C(this, play);
        }
    }
}






