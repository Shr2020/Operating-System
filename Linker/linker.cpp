#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <ctype.h>
#include <list>
#include <algorithm>
#include <cmath>

using namespace std;

map<string,pair<int, string>> symbolMap;
vector<string> symList;
map<int, int> modAdd;
int baseAdd = 0;
int max_int = pow(2, 30);

// tokenizer vars
int linenum = 0;
int lineoffset = 1;
int tokenEndOffset = 1;
int FileEndOffset = 1;
bool newline = true;
string line;
char * token;
char * cstr;
fstream fi;


void __parseerror(int errcode) {
    static char * errstr[] = {
        "NUM_EXPECTED",      // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED",      // Symbol Expected
        "ADDR_EXPECTED",     // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",      // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE",    // > 16
        "TOO_MANY_USE_IN_MODULE",    // > 16
        "TOO_MANY_INSTR"           // total num_instr exceeds memory size (512)
    };
    std :: printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}

string getToken() {
    
    if(!newline) {
        token = strtok(NULL," \t\n");
        if (token == NULL) {
            // end of line reached. Get new line.
            newline = true;
            lineoffset = tokenEndOffset;
        }
    }
    if (newline && getline(fi, line)) {
        cstr = new char [line.length()+1];
        strcpy(cstr, line.c_str());
        token = strtok(cstr," \t\n");
        linenum = linenum + 1;
        FileEndOffset = line.length() + 1;
        while (token == NULL && getline(fi, line)) {
            //empty line detected. loop until non empty line.
            linenum = linenum + 1;
            FileEndOffset = line.length() + 1;
            lineoffset = FileEndOffset;
            cstr = new char [line.length()+1];
            strcpy(cstr, line.c_str());
            token = strtok(cstr," \t\n");            
        }
        newline = false;
    } 
    
    if(token == NULL) {
        return string();
    }
    lineoffset = token - cstr + 1;
    tokenEndOffset = lineoffset + strlen(token);
    FileEndOffset = line.length() + 1;
    return string(token);
}

int readInt(string p) {
    if(all_of(p.begin(), p.end(), ::isdigit) ){
        int i = stoi(p);
        if (i <= max_int) {
            return i;
        }
    } 
    __parseerror(0);
    exit(EXIT_FAILURE);
}

string readSymbol(string p) {
    if(isalpha(p[0]) && all_of(p.begin(), p.end(), ::isalnum)){
        if (p.length() < 17) {
           return p;
        } else {
            __parseerror(3);
            exit(EXIT_FAILURE);
        }
    }
    __parseerror(1);
    exit(EXIT_FAILURE);
}

string readIAER(string p) {
    if(p.size() == 1 && (p.compare("A") == 0 || p.compare("E") == 0 || 
                                p.compare("I") == 0 || p.compare("R") == 0)){
        return p;
    } 
    __parseerror(2);
    exit(EXIT_FAILURE);
}

string getStringForInt(int i, int isIndex) {
    string s = to_string(i);
    int size = s.length();
    if (size == 1) {
        s = "00" + s;
    } else if (size == 2) {
        s = "0" + s;
    } 
    if (isIndex == 0 && size < 4) {
        s = "0" + s;
    }
    return s;
}

void createMemMap(int key, string sym, int addr, map<int, pair<string, int>> &useMap, int modNum, int usecount, int numInst) {
    string err = "";
    int finAddr  = addr;
    if (sym.compare("A") == 0) {
        int num = addr % 1000;
        if (num > 512) {
            // Rule 8: "Error: Absolute address exceeds machine size; zero used"
            err = "Error: Absolute address exceeds machine size; zero used";
            finAddr = finAddr - num;
        }
    } else if (sym.compare("I") == 0) {
        if (addr > 10000) {
            // Rule 10: Error: Illegal immediate value; treated as 9999
            finAddr = 9999;
            err = "Error: Illegal immediate value; treated as 9999";
        }
    } else if (sym.compare("R") == 0) {
        if ((finAddr / 1000) >= 10) {
            // Rule 11: Error: Illegal opcode; treated as 9999
            finAddr = 9999;
            err = "Error: Illegal opcode; treated as 9999";
        } else {
            int bd = modAdd.find(modNum) -> second;
            int instRelAdd = finAddr % 1000;
            if (instRelAdd >= numInst) {
                // Rule 9: Error: Relative address exceeds module size; zero used
                finAddr = finAddr - instRelAdd + bd;
                err = "Error: Relative address exceeds module size; zero used";
            } else {
                finAddr = finAddr + bd;
            }
        }
    } else if (sym.compare("E") == 0) {
        int useSymNum = addr % 1000;
        finAddr = addr - useSymNum;
        string symbol = "";
        if (useSymNum > usecount - 1) {
            // Rule 6: "Error: External address exceeds length of uselist; treated as immediate"
            finAddr = addr;
            err = "Error: External address exceeds length of uselist; treated as immediate";

        } else {
            symbol = (useMap.find(useSymNum) -> second).first;
            useMap.at(useSymNum) = pair<string, int>(symbol, 1);
            if (symbolMap.count(symbol) <= 0) {
                // Rule 3: "Error: %s is not defined; zero used"
                err = "Error: " + symbol + " is not defined; zero used";

            } else {
                // All error checks passed!
                int md = (symbolMap.find(symbol) -> second).first;
                finAddr = finAddr + md;
            }
        }
    }
    std::cout << getStringForInt(key, 1) << ": " << getStringForInt(finAddr, 0) << " " << err << endl;
}

void Pass1() {
    int modNum = 1;
    string s;
    while(!fi.eof()) {
        string s = getToken();
        if (!s.empty()) {
            int defcount = readInt(s);
            map<string, int> defSymMap;
            if (defcount > 16) {
                __parseerror(4);
                exit(EXIT_FAILURE);
            }
            while (defcount > 0) {
                string sym = readSymbol(getToken());
                int symAdd = readInt(getToken());
                int relAdd = symAdd + baseAdd;
                defSymMap.insert(pair<string, int>(sym, symAdd));
                if (symbolMap.count(sym) > 0) {
                    // Rule 2: Error: This variable is multiple times defined; first value used
                    relAdd = (symbolMap.find(sym) -> second).first;
                    symbolMap.at(sym) = pair<int, string>(relAdd, "Error: This variable is multiple times defined; first value used");
                } else {
                    symbolMap.insert(pair<string, pair<int, string>> (sym, pair<int, string>(relAdd, "")));
                }
                defcount--;
            }

            int usecount = readInt(getToken());
            if (usecount > 16) {
                __parseerror(5);
                exit(EXIT_FAILURE);
            }
            while (usecount > 0) {
                string sym = readSymbol(getToken());
                usecount--;
            }

            int numInst = readInt(getToken());
            modAdd.insert(pair<int, int>(modNum, baseAdd));
            baseAdd = baseAdd + numInst;
            int modEnd = numInst - 1;

            for (auto itr = defSymMap.begin(); itr != defSymMap.end(); ++itr) {
                // Rule 5: Warning: Module %d: %s too big %d (max=%d) assume zero relative\n
                int addr = itr -> second;
                string s = itr -> first;
                if (addr > modEnd) {
                    pair<int, string> val = symbolMap.find(s) -> second;
                    if (val.second.compare("") == 0) {
                        cout << "Warning: Module " << modNum << ": " << itr -> first << " too big " << addr << " (max=" << modEnd << ") assume zero relative" << endl;
                        int newAdd = baseAdd - numInst;
                        symbolMap.at(s) = pair<int, string>(newAdd, val.second);
                    }

                }
            }
            
            if (baseAdd > 512) {
                __parseerror(6);
                exit(EXIT_FAILURE);
            }
            while (numInst > 0) {
                string sym = readIAER(getToken());
                int addr = readInt(getToken());
                numInst--;
            }
            modNum++;
        }        
    }

    std::cout << "Symbol Table" << endl;
    
    for(auto itr = symbolMap.begin(); itr != symbolMap.end(); ++itr) {
        std::cout << itr -> first << "=" << (itr -> second).first << " " << (itr -> second).second << '\n';
    }
    std::cout << endl;
}

void Pass2() {
    std::cout << "Memory Map" << endl;
    int index = 0;
    int modNum = 1;
    map<string, int> symVsModule;
    vector<string> useList;
    vector<string> errList;
    string s;
    while(!fi.eof()) {
        string s = getToken();
        if (!s.empty()) {
            int defcount = readInt(s);
            int defcountItr = defcount;
            while (defcountItr > 0) {
                string sym = readSymbol(getToken());
                int symAdd = readInt(getToken());
                if (symVsModule.count(sym) <= 0) {
                    symVsModule.insert(pair<string, int>(sym, modNum));
                }
                defcountItr--;
            }
            map<int,pair<string, int>> useMap;
            int i = 0;
            int usecount = readInt(getToken());
            int usecountItr = usecount;
            while (usecountItr > 0) {
                string sym = readSymbol(getToken());
                useMap.insert(pair<int, pair<string, int>>(i++, pair<string, int> (sym, 0)));
                int t = count(useList.begin(), useList.end(), sym);
                if (t <= 0) {
                    useList.push_back(sym);
                }
                usecountItr--;
            }

            int numInst = readInt(getToken());
            int num = numInst;
            while (numInst > 0) {
                string sym = readIAER(getToken());
                int addr = readInt(getToken());
                createMemMap(index, sym, addr, useMap, modNum, usecount, num);
                numInst--;
                index++;
            }

            for (auto itr = useMap.begin(); itr != useMap.end(); ++itr) {
                if ((itr -> second).second == 0) {
                    //Rule 7: Warning: Module %d: %s appeared in the uselist but was not actually used\n
                    cout << "Warning: Module " << modNum << ": " << (itr -> second).first << " appeared in the uselist but was not actually used" << endl;
                }
            }
            modNum++;
        }
    }
    
    for(int i = 0; i < useList.size(); i++) {
        string sym = useList.at(i);
        if (symVsModule.count(sym) > 0) {
            symVsModule.erase(sym);
        }
    }

    cout << endl;
    for (auto itr = symVsModule.begin(); itr != symVsModule.end(); ++itr) {
        // Rule 4: Warning: Module %d: %s was defined but never used\n
        cout << "Warning: Module " << itr -> second << ": " << itr -> first << " was defined but never used" << endl;
    }
    std::cout << endl;
    std::cout << '\n' << endl;
}

int main(int argc, char * argv[]) {
    string filename = argv[1];
    fi.open(filename);
    if (!fi.is_open()) {
        std::cout << "File Not open." << endl;
        return 0;
    }
    string line;
    Pass1();
    fi.close();

    fi.open(filename);
    if (!fi.is_open()) {
        std::cout << "File Not open." << endl;
        return 0;
    }
    Pass2();
    fi.close();
}