#include "Pager.h"

int FifoPager::select_victim(unsigned long long inst_count)
{
    int victim_frame = hand;
    // increment hand
    hand = (hand + 1) % frame_count;
    return victim_frame;
}

int ClockPager::select_victim(unsigned long long inst_count)
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

int NruPager::select_victim(unsigned long long inst_count)
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

int WorkingSetPager::select_victim(unsigned long long inst_count)
{

    int index_oldest_last_time_used = hand;
    unsigned long long oldest_last_time_used = ULLONG_MAX;
    for (int i = 0; i < frame_count; i++)
    {
        int index = (hand + i) % frame_count;
        PageTableEntry *pte = &process_table[frame_table[index].first].page_table[frame_table[index].second];
        
        if (pte->referenced == 1)
        {
            time_last_used_table[index] = inst_count;
            pte->referenced = 0;
        }
        else
        {
            if (inst_count - time_last_used_table[index] >= 50)
            {
                hand = (index + 1) % frame_count;
                return index;
            }
        }
        if (time_last_used_table[index] < oldest_last_time_used)
        {
            oldest_last_time_used = time_last_used_table[index];
            index_oldest_last_time_used = index;
        }
    }
    hand = (index_oldest_last_time_used + 1) % frame_count;
    return index_oldest_last_time_used;
}

int AgingPager::select_victim(unsigned long long inst_count)
{
    int victim = hand;
    // be larger than the maximum of uint32_t
    uint64_t smallest_age = 0xFFFFFFFFFFFFFFFF;
    for (int i = 0; i< frame_count; i++)
    {
        int index = (hand + i) % frame_count;
        PageTableEntry *pte = &process_table[frame_table[index].first].page_table[frame_table[index].second];
        age_table[index] = age_table[index] >> 1;
        if (pte->referenced == 1) {
            age_table[index] = (age_table[index] | 0x80000000);
            pte->referenced = 0;
        }
        if (age_table[index] < smallest_age)
        {
            smallest_age = age_table[index];
            victim = index;
        }
    }
    hand = (victim + 1) % frame_count;
    return victim;
}

int RandomPager::select_victim(unsigned long long inst_count)
{
    return random_generator->get(frame_count);
}