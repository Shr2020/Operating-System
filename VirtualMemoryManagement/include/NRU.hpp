#pragma once
#include "Process.hpp"
#include "Frame.hpp"
#include "Pager.hpp"
#include <queue>
#include <vector>

// Class that models NRU Algorithm
class NRU : public Pager
{
public:
    NRU(std::vector<Frame *> f_q, int size)
    {
        frame_q = f_q;
        list_size = size;
        counter = 0;
        instr_count = 0;
        reset_tracker = 0;
        class_0_index = -1;
        class_1_index = -1;
        class_2_index = -1;
        class_3_index = -1;
    }

    Frame *select_victim_frame()
    {
        if (!has_next())
        {
            return nullptr;
        }

        //reset_indexes
        class_0_index = -1;
        class_1_index = -1;
        class_2_index = -1;
        class_3_index = -1;

        populate_class_index(counter);
        
        // set the counter to victime_index
        counter = get_victim_index();
        
        // victim frame
        Frame *p = frame_q.at(counter % list_size);

        // point to next frame
        counter++;

        if (instr_count - reset_tracker >= 50)
        {
            reset_reference();
            reset_tracker = instr_count;
        }

        return p;
    }

    void populate_class_index(int counter)
    {

        for (int i = 0; i < list_size; i++)
        {
            PTE *pte = frame_q.at(counter % list_size)->pte;
            if (pte->referenced == 0 && pte->modified == 0)
            {
                if (class_0_index == -1)
                {
                    class_0_index = counter % list_size;
                }
            }
            else if (pte->referenced == 0 && pte->modified == 1)
            {
                if (class_1_index == -1)
                {
                    class_1_index = counter % list_size;
                }
            }
            else if (pte->referenced == 1 && pte->modified == 0)
            {
                if (class_2_index == -1)
                {
                    class_2_index = counter % list_size;
                }
            }
            else if (pte->referenced == 1 && pte->modified == 1)
            {
                if (class_3_index == -1)
                {
                    class_3_index = counter % list_size;
                }
            }
            counter++;
        }
    }

    int get_victim_index() {
        int victim_index = -1;
        if (class_0_index != -1) {
            victim_index = class_0_index;
        } else if (class_1_index != -1) {
            victim_index = class_1_index;
        } else if (class_2_index != -1) {
            victim_index = class_2_index;
        } else if (class_3_index != -1) {
            victim_index = class_3_index;
        }
        return victim_index;
    }

    void set_instruction_count(unsigned long count)
    {
        instr_count = count;
    }

    void reset_reference()
    {
        for (int i = 0; i < list_size; i++)
        {
            PTE *pte = frame_q.at(i)->pte;
            pte->referenced = 0;
        }
    }

    bool has_next() { return !frame_q.empty(); }

private:
    std::vector<Frame *> frame_q;
    unsigned long list_size = 0;
    unsigned long counter = 0;
    unsigned long instr_count = 0;
    unsigned long reset_tracker = 0;
    int class_0_index = -1;
    int class_1_index = -1;
    int class_2_index = -1;
    int class_3_index = -1;
};