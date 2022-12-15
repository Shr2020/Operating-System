#include "Simulator.hpp"
#include <vector>
#include <IScheduler.hpp>
#include <memory>
#include "IORequest.hpp"
#include "StatisticsTracker.hpp"
#include "Summary.hpp"
#include <queue>

//Constructor for Simulator
Simulator::Simulator(IScheduler *isched) {
    sched = isched;
}

void Simulator::run(std::queue<IORequest> &ior, std::vector<StatisticsTracker> &stats, 
                    Summary &summary) {
    
    int time = 0;
    int total_movement = 0;
    int next_arrival = 0; // arrival of next io
    int io_next = 0;  // next io to arrive
    bool io_active = false;  // true when an io is active
    int io_end_time = 0; // end time of currently active io.
    int active_io_id = -1;  // id of currently running io; -1 if none active
    int current_head_at = 0;
    
    IORequest active_io;

    
    while (true) {
         if (ior.size() != 0) {
            next_arrival = ior.front().get_at();
            // if this is the arrival_time of a request;
            if (next_arrival == time) {
                sched->add_IORequest(ior.front());
                ior.pop();
            }
        }
        
        // if a request is active and getting completed
        if (io_active && current_head_at == active_io.get_track()) {
            // The IO has been completed
            (stats.at(active_io_id)).set_end_time(time);
            io_active = false;
        }

        // if no request active, ask scheduler
        if (!io_active) {
            if (sched -> has_next()) {

                // case when the hand resets (in CLOOK)
                int shift = sched->reset();

                active_io = sched->get_next_request();
                io_active = true;
                active_io_id = active_io.get_id();

                (stats.at(active_io_id)).set_start_time(time);

                if (shift > 0 ) {
                    int t = 1;
                    while (t <= shift) {
                        time++;
                        current_head_at--;
                        total_movement++;

                        // check if any new request is coming while the hand is getting reset
                        if (ior.size() != 0) {
                            next_arrival = ior.front().get_at();
                            // if this is the arrival_time of a request;
                            if (next_arrival == time) {
                                sched->add_IORequest(ior.front());
                                ior.pop();
                            }
                        }
                        t++;
                    }  
                }

                // case when it is already on its track when starting
                if (current_head_at == active_io.get_track()) {
                    (stats.at(active_io_id)).set_end_time(time);
                    io_active = false;
                    continue;
                }
            } 
            
            if (ior.size() == 0 && !(sched -> has_next()) && !io_active) {
                //all request are done. Exit sim
                summary.set_total_movement(total_movement);
                break;
            } 
        }

        if (io_active) {
            if (sched -> get_direction()) {
                current_head_at++;
            } else {
                current_head_at--;
            }
            total_movement++;
        }
        time = time + 1;
    }  
}
