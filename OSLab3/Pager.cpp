#include "Pager.h"

int FifoPager::select_victim() {
    int victim_frame = hand;
    // increment hand
    hand = (hand + 1) % frame_count;
    return victim_frame;
}