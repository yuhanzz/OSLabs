#include "Pager.h"

int FifoPager::select_victim()
{
    int victim_frame = hand;
    // increment hand
    hand = (hand + 1) % frame_count;
    return victim_frame;
}

int ClockPager::select_victim()
{
    while (process_table[frame_table[hand].first].page_table[frame_table[hand].second].referenced == 1) {
        process_table[frame_table[hand].first].page_table[frame_table[hand].second].referenced = 0;
        hand = (hand + 1) % frame_count;
    }
    int victim = hand;
    hand = (hand + 1) % frame_count;
    return victim;
}