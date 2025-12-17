#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"


static Player* s_playerPtr_interface = NULL;
static PlayerMask s_originalMask = PLAYER_MASK_NONE;
static int should_be_giant = 0;
static Player* s_playerPtr_use = NULL;
static ItemId s_item = 0;
extern PlayerItemAction Player_ItemToItemAction(Player* this, ItemId item);

RECOMP_HOOK("Player_UseItem") void GiantMask_logic(PlayState* play, Player* this, ItemId item) {


    s_playerPtr_use = this;
    s_item = item;

}

RECOMP_HOOK_RETURN("Player_UseItem") void GiantMask_logic_return() {

    PlayerItemAction itemAction = Player_ItemToItemAction(s_playerPtr_use, s_item);
    
    if (itemAction == PLAYER_IA_MASK_GIANT) {
        should_be_giant ^= 1;
    }
}

RECOMP_HOOK("Player_UpdateCommon") void GiantMask_logic_make_sure(Player* this, PlayState* play, Input* input) {


    if (should_be_giant && this->currentMask == PLAYER_MASK_NONE) {
        this->currentMask = PLAYER_MASK_GIANT;
        Magic_Consume(play, 0, MAGIC_CONSUME_GIANTS_MASK);
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


