#include "Simulator.hpp"
#include <vector>
#include <Pager.hpp>
#include <memory>
#include "Process.hpp"
#include "StatisticsTracker.hpp"
#include "Summary.hpp"


using ProcessVec = std::vector<Process*>;
using InstructVec = std::vector<std::pair<std::string, int>>;
static const int MAX_VPAGES = 64;

//Constructor for Simulator
Simulator::Simulator(Pager *p, int num_f, std::vector<Frame*> frame_table_global) {
    num_frames = num_f;
    frame_table = frame_table_global;
    pager = p;
    for (int i = 0; i < num_frames; i++) {
        free_list.push(i);
    }    
} 

Frame* Simulator::get_frame() {
    int index = get_free_frame_from_list();
    Frame* frame = nullptr;

    // no free frames
    if (index == -1) {
        frame = pager->select_victim_frame();
        return frame;
    }
    return frame_table.at(index);
}

int Simulator::get_free_frame_from_list() {
    int i = -1;
    if (!free_list.empty()) {
        i = free_list.front();
        free_list.pop();
    }
    return i;
}

void Simulator::run(InstructVec insts, ProcessVec pv, std::vector<StatisticsTracker> &stats, 
                    Summary &summary) {

    int pid = 0;
    PTE *current_pte = nullptr;
    Process* current_process;
    Frame* new_frame = nullptr;
    unsigned long instr_count = 0;
    for (unsigned long i = 0; i < insts.size(); i++) {

        // num of instructions
        instr_count++;
        pager->set_instruction_count(instr_count);

        current_pte = nullptr;
        std::string inst_type = insts.at(i).first;
        int vpage = insts.at(i).second; 

        std::cout << i << ": ==> " << inst_type << " " << vpage << std::endl;

        // handle special case of “c” and “e” instruction
        if (inst_type == "c") {
            pid = vpage;
            current_process = pv.at(pid);
            summary.increment_ctx_switches();
            continue;
        } else if (inst_type == "e") {
            int k = 0;

            std::cout << "EXIT current process " << current_process->get_id() << std::endl;
            summary.increment_process_exits();

            while(k < MAX_VPAGES) {
                current_pte =  &current_process->page_table[k];
                if (current_pte->present) {
                    int frame_num = current_pte->frame_ind;

                    // unmap frames
                    std::cout << " UNMAP " << frame_table.at(frame_num)->process->get_id() << ":" 
                        << frame_table.at(frame_num)->vpage << std::endl;

                    stats.at(vpage).increment_unmaps();

                    if (current_pte->filemapped && current_pte->modified) {
                        std::cout << " FOUT" << std::endl;
                        stats.at(vpage).increment_fouts();
                    }

                    frame_table.at(frame_num)->process = nullptr;
                    frame_table.at(frame_num)->pte = nullptr;
                    frame_table.at(frame_num)->vpage = -1;
                    frame_table.at(frame_num)->age = 0;
                    frame_table.at(frame_num)->last_use_time = 0;
                    current_pte->reset_PTE();
                    current_pte->pagedout = 0;

                    free_list.push(frame_num);
                }
                current_pte->reset_PTE();
                current_pte->pagedout = 0;
                k++;
            }
            continue;
        }
        
        current_pte = &(current_process->page_table[vpage]);
        summary.increment_inst_count();
        summary.set_pte_size(current_process->page_table[vpage]);
        
        if (!current_pte->present) {
            //Check if valid page in VMA, otherwise throw error.
            std::vector<VMA> vma_list = current_process->get_vma_list();
            bool page_valid = false;
            for (int j = 0; j < vma_list.size(); j++) {
                VMA vma = vma_list.at(j);
                if (vpage >= vma.get_start_vpage() && vpage <= vma.get_end_vpage()) {
                    current_pte->present = 1;
                    current_pte->write_protect = vma.get_write_protected();
                    current_pte->filemapped = vma.get_file_mapped();
                    page_valid = true;
                    break;
                }
            }
            if (!page_valid) {
                // the virtual page does not exist in any VMA. Throw fault.
                current_pte = nullptr;
                std::cout << " SEGV" << std::endl;
                stats.at(current_process->get_id()).increment_segv();
                continue;
            }
            
            // If we reach here that means the page is valid and present in frame table;
            // get frame: will be a free frame or a victime frame.
            new_frame = get_frame();
            
            // the frame was mapped to a pte (i.e. it was not free). UNMAP the frame from previous page
            if (new_frame->pte != nullptr) {
                
                std::cout << " UNMAP " << new_frame->process->get_id() << ":" << new_frame->vpage << std::endl;
                stats.at(new_frame->process->get_id()).increment_unmaps();

                PTE* old_pte = &(new_frame->process->page_table[new_frame->vpage]);
                if (old_pte -> modified) {
                    if (old_pte -> filemapped) {
                        std::cout << " FOUT" << std::endl;
                        stats.at(new_frame->process->get_id()).increment_fouts();
                    } else {
                        std::cout << " OUT" << std::endl;
                        stats.at(new_frame->process->get_id()).increment_outs();
                        old_pte->pagedout = 1;
                    }       
                }

                // removing this pte from frame_table. reset the PTE
                old_pte->reset_PTE();
                
                // Clear the frame for new pte
                new_frame->process = nullptr;
                new_frame->pte = nullptr;
                new_frame->vpage = -1;
                new_frame->age = 0;
                new_frame->last_use_time = 0;
            }
            
            // set referenced bit and frame index in pte
            current_pte->referenced = 1;
            current_pte->frame_ind = new_frame->index;

            // map frame to current_pte
            new_frame->vpage = vpage;
            new_frame->process = current_process;
            new_frame->pte = current_pte;
            new_frame->age = 0;
            new_frame->last_use_time = instr_count;

            if (current_pte->filemapped) {
                std::cout << " FIN" << std::endl;
                stats.at(new_frame->process->get_id()).increment_fins();
            } else if (current_pte->pagedout) {
                std::cout << " IN" << std::endl;
                stats.at(new_frame->process->get_id()).increment_ins();
            } else {
                std::cout << " ZERO" << std::endl;
                stats.at(new_frame->process->get_id()).increment_zeros();
            }
            
            std::cout << " MAP " << new_frame->index << std::endl;
            stats.at(new_frame->process->get_id()).increment_maps();
        } else {
            current_pte -> referenced = 1;
        }

        if (inst_type == "w") {
            if (current_pte->write_protect) {
                std::cout << " SEGPROT" << std::endl;
                stats.at(current_process->get_id()).increment_segprot();
            } else {
                current_pte->modified = 1;
            }
        }
    }     
}
