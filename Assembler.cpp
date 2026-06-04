#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <cctype>
using namespace std;
enum CommandType{
        A_COMMAND =0,
        C_COMMAND ,
        L_COMMAND =2
        };
class Parser {

    private:
        std::string dest_str;
        std::string comp_str;
        std::string jump_str;
        std::string symbol_str;
        std::string command;
        std::ifstream inFile;
        
        int currentIndex;
       

    public:
        std::vector<std::string> commands;  // 存储所有清洗后的命令
        Parser(std::string &filename) :currentIndex(-1){ 
            std::ifstream input(filename);
            std::string line;
        while(std::getline(input,line)){
            size_t pos = line.find("//");
            if (pos != std::string::npos)
            {
                line.erase(pos);
            }
        
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
            if (!line.empty()) {
                commands.push_back(line);
                }
            }
            input.close();
            }
  
        bool hasMoreCommands() {
           return currentIndex + 1 < commands.size();
        }


        bool advance(){
            if(!hasMoreCommands()){
                //遍历结束，重置索引！
                currentIndex = -1;
                return false;
            }
            currentIndex++;
            command = commands[currentIndex];
           
            return true;
        }



        CommandType commandType(){
            if (command.find('@') != std::string::npos){
            return A_COMMAND;
            }
            else if(command.find('(')!=std::string::npos||command.find(')')!=std::string::npos){
                return L_COMMAND;
            }
            else{return C_COMMAND;}

    }
        bool anlysis(){
            // 重置字符串
            dest_str = "";
            comp_str = "";
            jump_str = "";
            
            if(command.find(';')==std::string::npos&&command.find('=')==std::string::npos){
                //只有comp    
                 comp_str =command;
                    return true;
            }
            size_t start_pos=command.find('=');
            size_t end_pos= command.find(';');
            if(command.find(';')!=std::string::npos&&command.find('=')!=std::string::npos){
                // dest=comp;jump
                dest_str =command.substr(0,start_pos);
                jump_str =command.substr(end_pos+1,command.length()-end_pos-1);
                comp_str = command.substr(start_pos+1,end_pos-start_pos-1);
            }else if(command.find(';')!=std::string::npos){
                // comp;jump
                jump_str =command.substr(end_pos+1,command.length()-end_pos-1);
                comp_str =command.substr(0,end_pos);
            }else {
                // dest=comp
                comp_str= command.substr(start_pos+1,command.length()-start_pos-1);
               
                dest_str= command.substr(0,start_pos);
            }
            return  true;
        }
        std::string dest() { return dest_str; }
        std::string comp() { return comp_str; }
        std::string jump() { return jump_str; }
        std::string symbol(){ 
            symbol_str= command;
            symbol_str.erase(std::remove_if(symbol_str.begin(),symbol_str.end(),[](char c){return c=='('||c==')'||c=='@';}),symbol_str.end());
            return symbol_str;}
};

class Code {
private:
    std::unordered_map<std::string, const char*> dest_map;
    std::unordered_map<std::string, const char*> comp_map;
    std::unordered_map<std::string, const char*> jump_map;
    
public:
    Code() {
        dest_map.reserve(8);
        dest_map = {
            {"", "000"}, {"M", "001"}, {"D", "010"}, {"MD", "011"},
            {"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}
        };
        
        comp_map.reserve(28);
        comp_map = {
            {"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"},
            {"D", "0001100"}, {"A", "0110000"}, {"M", "1110000"},
            {"!D", "0001101"}, {"!A", "0110001"}, {"!M", "1110001"},
            {"-D", "0001111"}, {"-A", "0110011"}, {"-M", "1110011"},
            {"D+1", "0011111"}, {"A+1", "0110111"}, {"M+1", "1110111"},
            {"D-1", "0001110"}, {"A-1", "0110010"}, {"M-1", "1110010"},
            {"D+A", "0000010"}, {"D+M", "1000010"}, {"D-A", "0010011"},
            {"D-M", "1010011"}, {"A-D", "0000111"}, {"M-D", "1000111"},
            {"D&A", "0000000"}, {"D&M", "1000000"}, {"D|A", "0010101"},
            {"D|M", "1010101"}
        };
        
        jump_map.reserve(8);
        jump_map = {
            {"", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
            {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
        };
    }
    
    std::string dest(const std::string& mnemonic) {
        auto it = dest_map.find(mnemonic);
        return it != dest_map.end() ? it->second : "000";
    }
    
    std::string comp(const std::string& mnemonic) {
        auto it = comp_map.find(mnemonic);
        return it != comp_map.end() ? it->second : "0000000";
    }
    
    std::string jump(const std::string& mnemonic) {
        auto it = jump_map.find(mnemonic);
        return it != jump_map.end() ? it->second : "000";
    }
};

class SymbolTable{
    public:
    std::unordered_map<std::string, int> symbol_map;
    //折中之计，造成成员变量封装性的破坏！！！！
    
    SymbolTable(){
        symbol_map.reserve(1000);
        symbol_map = {
            {"SP", 0},
            {"LCL", 1},
            {"ARG", 2},
            {"THIS", 3},
            {"THAT", 4},
            {"R0", 0},
            {"R1", 1},
            {"R2", 2},
            {"R3", 3},
            {"R4", 4},
            {"R5", 5},
            {"R6", 6},
            {"R7", 7},
            {"R8", 9},
            {"R9", 9},
            {"R10", 10},
            {"R11", 11},
            {"R12", 12},
            {"R13", 13},
            {"R14", 14},
            {"R15", 15},
            {"SCREEN", 16384},
            {"KBD", 24576}};
        }
        //将（symbol,address）配对加入符号表
        void addEntry(std::string &symbol, int& address){
            symbol_map[symbol] = address;
        }
        //符号表是否包含指定的symbol？
        bool contains(std::string &symbol){
            auto res = symbol_map.find(symbol);
            if(res !=symbol_map.end()){
                return true;
            }
            return false;
        }
};

int main(int argc, char* argv[]){
    if (argc < 1) {
        cerr << "Usage: " << argv[0] << " <input_file> " << endl;
        return 1;
    }
    std::string inFilename = argv[1];
    auto pos = inFilename.find(".asm");
    std::string outFilename = inFilename.substr(0,pos+1)+"hack";
    std::ofstream outFile(outFilename);
    Parser parser(inFilename);
    Code code;
    SymbolTable table;
    
    //第一次循环，symbolTable解读伪指令

    int pc = 0;
    while(parser.advance()){
        
        auto type = parser.commandType();
        if(type==L_COMMAND){
            table.symbol_map[parser.symbol()] = pc;
            continue;
        }
        pc++;
    }
    int variable_index=15;
    while (parser.advance())
    {
        string out = "";
        if (parser.commandType() == C_COMMAND)
        {   parser.anlysis();
      

            string d = parser.dest();
            string c = parser.comp();
            string j = parser.jump();
           
            out ="111"+code.comp(c)+code.dest(d)+code.jump(j);
            outFile << out << "\n";
        }
        else if(parser.commandType()==A_COMMAND)
        {   
            string s = parser.symbol();
            int address;
            bool isNumber =true;
            vector<char> vec;
            for (char ch:s){
                vec.push_back(ch);
            }
            for (char c:vec){
                if (!(c>='0'&&c<='9')){
                    isNumber = false;
                }
            }
            if (isNumber){
                address = stoi(s);
            }
                // 识别变量，完善哈希表
            if (!table.contains(s)&&!isNumber)
                {
                    variable_index++;
                    table.symbol_map[s] = variable_index;
                    address = table.symbol_map[s];
                }
            if (table.contains(s)&&!isNumber){
                address = table.symbol_map[s];
            }
            std::bitset<15> bs(address);
            out = "0" + bs.to_string();           
            outFile << out << "\n";
        }
   }

    return 0;
}


