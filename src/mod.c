#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

//I'm not finding a better solution for when you run out of magic when giant and transformed so for now, you'll just have some magic left.
#define MIN_MAGIC_WHILE_GIANT_TRANSFORMED 3

static Player* s_playerPtr_interface = NULL;
static PlayerMask s_originalMask = PLAYER_MASK_NONE;
int should_be_giant = 0;
static Player* s_playerPtr_use = NULL;
static PlayState* s_playPtr = NULL;
static ItemId s_item = 0;
extern PlayerItemAction Player_ItemToItemAction(Player* this, ItemId item);
extern void Player_Init(Actor* thisx, PlayState* play);

RECOMP_HOOK("Player_UseItem") void GiantMask_logic(PlayState* play, Player* this, ItemId item) {

    s_playPtr = play;
    s_playerPtr_use = this;
    s_item = item;

}

RECOMP_HOOK_RETURN("Player_UseItem") void GiantMask_logic_return() {

    PlayerItemAction itemAction = Player_ItemToItemAction(s_playerPtr_use, s_item);

    if (s_playerPtr_use->transformation == PLAYER_FORM_HUMAN && gSaveContext.save.saveInfo.playerData.magic > 0 &&
            (s_playerPtr_use->currentMask == PLAYER_MASK_GIANT || s_playerPtr_use->currentMask == PLAYER_MASK_NONE)) {
        if (itemAction == PLAYER_IA_MASK_GIANT) {
            should_be_giant ^= 1;
        }
    }

    if (should_be_giant) {
        Magic_Consume(s_playPtr, 0, MAGIC_CONSUME_GIANTS_MASK);
    }

}

RECOMP_HOOK("Player_UpdateCommon") void GiantMask_logic_make_sure(Player* this, PlayState* play, Input* input) {

    if (should_be_giant && this->transformation != PLAYER_FORM_HUMAN && 
            gSaveContext.save.saveInfo.playerData.magic < MIN_MAGIC_WHILE_GIANT_TRANSFORMED) {
        gSaveContext.save.saveInfo.playerData.magic = MIN_MAGIC_WHILE_GIANT_TRANSFORMED;
    }
    if (should_be_giant && this->transformation == PLAYER_FORM_HUMAN) {
        this->currentBoots = PLAYER_BOOTS_GIANT;
        this->prevBoots = PLAYER_BOOTS_GIANT;
    }

    if (should_be_giant && this->currentMask == PLAYER_MASK_NONE) {
        this->currentMask = PLAYER_MASK_GIANT;
    } else if (!should_be_giant) {
        if (this->currentMask == PLAYER_MASK_GIANT) {
            this->currentMask = PLAYER_MASK_NONE;
        }
    }


    if (should_be_giant && gSaveContext.save.saveInfo.playerData.magic == 0) {
        this->currentMask = PLAYER_MASK_GIANT;
        should_be_giant = 0;
    }
}



RECOMP_HOOK("Interface_UpdateButtonsPart2") void GiantMask_ButtonHook_Entry(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->transformation == PLAYER_FORM_HUMAN) { 

        if (player->currentMask == PLAYER_MASK_GIANT) {
            s_playerPtr_interface = player;
            s_originalMask = PLAYER_MASK_GIANT;
            player->currentMask = PLAYER_MASK_NONE;
        }
    }
}


RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart2") void GiantMask_ButtonHook_Return() {
    if (s_playerPtr_interface != NULL) {
        s_playerPtr_interface->currentMask = s_originalMask;
        s_playerPtr_interface = NULL;
    }
}


RECOMP_HOOK("Play_SetupTransition") void Clear_GiantMask_on_Warp(PlayState* this, s32 transitionType) {
    should_be_giant = 0;

    Player* player = GET_PLAYER(this);

    if (player->currentMask == PLAYER_MASK_GIANT) {
        player->currentMask = PLAYER_MASK_NONE;
    }
    Magic_Reset(this);
}





