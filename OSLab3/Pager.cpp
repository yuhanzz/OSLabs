#include "Pager.h"

int FifoPager::select_victim(int inst_count)
{
    int victim_frame = hand;
    // increment hand
    hand = (hand + 1) % frame_count;
    return victim_frame;
}

int ClockPager::select_victim(int inst_count)
{
    while (process_table[frame_table[hand].first].page_table[frame_table[hand].second].referenced == 1)
    {
        process_table[frame_table[hand].first].page_table[frame_table[hand].second].referenced = 0;
        hand = (hand + 1) % frame_count;
    }
    int victim = hand;
    hand = (hand + 1) % frame_count;
    return victim;
}

int NruPager::select_victim(int inst_count)
{
    bool reset = false;
    if (inst_count - this->last_reset_instr_count >= 50)
    {
        reset = true;
        this->last_reset_instr_count = inst_count;
    }
    int class_hands[4];
    for (int i = 0; i < 4; i++)
    {
        class_hands[i] = -1;
    }
    for (int i = 0; i < frame_count; i++)
    {
        int index = (hand + i) % frame_count;
        PageTableEntry *pte = &process_table[frame_table[index].first].page_table[frame_table[index].second];
        int class_num = 2 * pte->referenced + pte->modified;
        if (class_hands[class_num] == -1)
        {
            class_hands[class_num] = index;
        }
        if (class_num == 0 && reset == false)
        {
            hand = (index + 1) % frame_count;
            return index;
        }
        if (reset)
        {
            pte->referenced = 0;
        }
    }
    int lowest_class = 3;
    for (int i = 0; i < 4; i++)
    {
        if (class_hands[i] != -1)
        {
            lowest_class = i;
            break;
        }
    }
    int victim = class_hands[lowest_class];
    hand = (victim + 1) % frame_count;
    return victim;
}