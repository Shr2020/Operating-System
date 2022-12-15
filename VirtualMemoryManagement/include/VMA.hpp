#pragma once
class VMA {
    private:
    int start_vpage;
    int end_vpage;
    int write_protected;
    int file_mapped;

    public:
    VMA(int sv, int ev, int wp, int fm) : start_vpage(sv), end_vpage(ev), write_protected(wp), file_mapped(fm){}
    int get_start_vpage() { return start_vpage; }
    int get_end_vpage() { return end_vpage; }
    int get_write_protected() { return write_protected; }
    int get_file_mapped() { return file_mapped; }

    void set_start_vpage(int p) {
        start_vpage = p;
    }

    void set_end_vpage(int p) {
        end_vpage = p;
    }

    void set_write_protected(int p) {
        write_protected = p;
    }

    void set_file_mapped(int p) {
        file_mapped = p;
    }

    void print_vma() {
        std::cout << start_vpage << " " << end_vpage << " " 
        << write_protected << " " << file_mapped << std::endl;
    }

};