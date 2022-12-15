#pragma once
#include <vector>
#include "Process.hpp"
#include "Frame.hpp"
#include "PTE.hpp"
#include <sstream>
#include "StatisticsTracker.hpp"

class Summary {
    public:
    Summary(std::vector<Frame*> f_t, std::vector<Process*> p) {
        frame_table = f_t;
        processes = p;
        inst_count = 0;
        ctx_switches = 0;
        process_exits = 0;
        pte_size = 0;
        cost = 0;
    }

    std::string print_frame_table() {
        std::stringstream ss;
        ss << "FT: ";
        for (int i = 0; i <  frame_table.size(); i++) {
            if (frame_table.at(i)->pte == nullptr) {
                ss << "*" << " ";
            } else {
                ss << frame_table.at(i)->process->get_id() << ":" 
                << frame_table.at(i)->vpage << " ";
            }
        }
        return ss.str();
    }

    std::string print_page_table() {
        std::stringstream ss;
        for(int j = 0; j < processes.size(); j++) {
            Process* p = processes.at(j);
            ss << "PT[" << p->get_id() << "]: ";
            for (int i = 0; i < MAX_VPAGE; i++) {
                PTE* pte = &(p->page_table.at(i));
                std::string output = print_pte_info(pte);
                if(pte->present) {
                    ss << i << ":" << output << " ";
                } else {
                    ss << output << " ";
                }
            }
            if (j+1 < processes.size()) {
                ss << '\n';
            }
        }
        return ss.str();
    }

    std::string print_pte_info(PTE* pte) {
        std::stringstream ss;
        
        if(pte->present) {
            if (pte->referenced) {
                ss << "R";
            } else {
                ss << "-";
            }

            if (pte->modified) {
                ss << "M";
            } else {
                ss << "-";
            }

            if (pte->pagedout) {
                ss << "S";
            } else {
                ss << "-";
            }
        } else {
            if (pte->pagedout) {
                // swapped out
                ss << "#";
            } else {
                ss << "*";
            }
        }
        return ss.str();
    }

    void calculate_total_cost(std::vector<StatisticsTracker> &stats) {
        unsigned long long total_unmaps = 0; 
        unsigned long long total_maps = 0; 
        unsigned long long total_ins = 0; 
        unsigned long long total_outs = 0;
        unsigned long long total_fins = 0; 
        unsigned long long total_fouts = 0; 
        unsigned long long total_zeros = 0;
        unsigned long long total_segv = 0; 
        unsigned long long total_segprot = 0;

        for (int i = 0; i < stats.size(); i++) {
            StatisticsTracker st = stats.at(i);
            total_unmaps += st.get_unmaps();
            total_maps += st.get_maps();
            total_ins += st.get_ins();
            total_outs += st.get_outs();
            total_fins += st.get_fins();
            total_fouts += st.get_fouts();
            total_zeros += st.get_zeros();
            total_segv += st.get_segv();
            total_segprot += st.get_segprot();
        }

        cost += 300 * total_maps + 400 * total_unmaps + 3100 * total_ins + 
                2700 * total_outs + 2800 * total_fins + 2400 * total_fouts + 140 * total_zeros + 
                340 * total_segv + 420 * total_segprot +
                1 * inst_count + 130 * ctx_switches + 1250 * process_exits;

    }

    std::string print_total_cost() {

        std::stringstream ss;
        ss << "TOTALCOST " << inst_count + ctx_switches + process_exits << " " 
        << ctx_switches << " " << process_exits << " " << cost << " " 
        << pte_size;
        return ss.str();
    }

    void increment_ctx_switches() {
        ctx_switches++;
    }

    void increment_inst_count() {
        inst_count++;
    }

    void increment_process_exits() {
        process_exits++;
    }

    void set_pte_size(PTE pte) {
        pte_size = sizeof(pte);
    }


    private:
    std::vector<Frame*> frame_table;
    std::vector<Process*> processes;
    int MAX_VPAGE = 64;
    unsigned long long cost = 0;
    unsigned long long inst_count = 0;
    unsigned long long ctx_switches = 0; 
    unsigned long long process_exits = 0;
    unsigned long pte_size = 0;

};