#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Virtual Machine and Assembly Language Interpreter
// Adam Syafiq

template <typename T>
class MyVector {
private:
    T* arr;
    int capacity;
    int current_size;

    void resize() {
        capacity *= 2;
        T* temp = new T[capacity];
        for (int i = 0; i < current_size; i++) {
            temp[i] = arr[i];
        }
        delete[] arr;
        arr = temp;
    }

public:
    MyVector() {
        capacity = 4;
        current_size = 0;
        arr = new T[capacity];
    }
    ~MyVector() { delete[] arr; }

    void push_back(const T& data) {
        if (current_size == capacity) resize();
        arr[current_size++] = data;
    }

    T& operator[](int index) { return arr[index]; }
    int size() const { return current_size; }
};

template <typename T>
class MyStack {
private:
    T arr[100];
    int topIndex;

public:
    MyStack() : topIndex(-1) {}
    
    void push(T val) {
        if (topIndex < 99) arr[++topIndex] = val;
    }

    T pop() {
        if (topIndex >= 0) return arr[topIndex--];
        std::cout << "CRASH: Stack Underflow!" << std::endl;
        exit(1);
    }

    bool isEmpty() const { return topIndex == -1; }
    int size() const { return topIndex + 1; }
};

// Core Classes - Adam Syafiq

class Register {
protected:
    signed char value;
public:
    Register() : value(0) {}
    virtual ~Register() {}
    signed char getValue() const { return value; }
    void setValue(signed char val) { value = val; }
};

class GeneralRegister : public Register {
private:
    int id;
public:
    GeneralRegister() : id(0) {}
    void setId(int rId) { id = rId; }
    int getId() const { return id; }
};

class FlagRegister {
private:
    bool cf; // Carry Flag
    bool of; // Overflow Flag
    bool uf; // Underflow Flag
    bool zf; // Zero Flag
public:
    FlagRegister() : cf(false), of(false), uf(false), zf(false) {}
    bool getCF() const { return cf; }
    bool getOF() const { return of; }
    bool getUF() const { return uf; }
    bool getZF() const { return zf; }
    void setCF(bool b) { cf = b; }
    void setOF(bool b) { of = b; }
    void setUF(bool b) { uf = b; }
    void setZF(bool b) { zf = b; }
    void resetAll() { cf = of = uf = zf = false; }
};

class Memory {
private:
    signed char data[64];
public:
    Memory() {
        for (int i = 0; i < 64; i++) data[i] = 0;
    }
    signed char read(int address) const {
        if (address >= 0 && address < 64) return data[address];
        return 0;
    }
    void write(int address, signed char val) {
        if (address >= 0 && address < 64) data[address] = val;
    }
};

class CPU {
private:
    GeneralRegister registers[8];
    FlagRegister flags;
    Memory memory;
    int pc;
    int si;
    MyStack<signed char> systemStack;
public:
    CPU() : pc(0), si(0) {
        for (int i = 0; i < 8; i++) registers[i].setId(i);
    }
    GeneralRegister& getRegister(int idx) { return registers[idx]; }
    FlagRegister& getFlags() { return flags; }
    Memory& getMemory() { return memory; }
    int getPC() const { return pc; }
    void setPC(int val) { pc = val; }
    void incrementPC() { pc++; }
    MyStack<signed char>& getStack() { return systemStack; }
};


// Formatting and Output - Adam Syafiq

class OutputFormatter {
public:
    static void printPadded(int val) {
        if (val < 0) {
            std::cout << "-";
            val = -val;
        }
        if (val < 10) std::cout << "000";
        else if (val < 100) std::cout << "00";
        else if (val < 1000) std::cout << "0";
        std::cout << val;
    }

    static void printState(CPU& cpu) {
        std::cout << "#Begin#" << std::endl;
        std::cout << "#Registers#";
        for (int i = 0; i < 8; i++) {
            printPadded(cpu.getRegister(i).getValue());
            std::cout << "#";
        }
        std::cout << std::endl;
        std::cout << "#Flags#OF#" << cpu.getFlags().getOF() << "#UF#" << cpu.getFlags().getUF()
                  << "#CF#" << cpu.getFlags().getCF() << "#ZF#" << cpu.getFlags().getZF() << "#" << std::endl;
        std::cout << "#PC#";
        printPadded(cpu.getPC());
        std::cout << "#" << std::endl;
        printMemoryDump(cpu);
        std::cout << "#End#" << std::endl;
    }

    static void printMemoryDump(CPU& cpu) {
        std::cout << "#Memory#" << std::endl;
        for (int row = 0; row < 8; row++) {
            std::cout << "#";
            for (int col = 0; col < 8; col++) {
                printPadded(cpu.getMemory().read(row * 8 + col));
                std::cout << "#";
            }
            std::cout << std::endl;
        }
    }
};


// Polymorphic Instructions -Adam Syafiq

class Instruction {
public:
    virtual ~Instruction() {}
    virtual void execute(CPU& cpu) = 0;
};

class InputInstruction : public Instruction {
private:
    int destReg;
public:
    InputInstruction(int reg) : destReg(reg) {}
    void execute(CPU& cpu) override {
        std::cout << "?" << std::endl;
        int val;
        std::cin >> val;
        cpu.getFlags().resetAll();
        if (val > 127) {
            cpu.getFlags().setOF(true);
            cpu.getRegister(destReg).setValue(127);
        } else if (val < -128) {
            cpu.getFlags().setUF(true);
            cpu.getRegister(destReg).setValue(-128);
        } else {
            cpu.getRegister(destReg).setValue(static_cast<signed char>(val));
        }
        if (cpu.getRegister(destReg).getValue() == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};

class MovImmediateInstruction : public Instruction {
private:
    int destReg;
    int immediate;
public:
    MovImmediateInstruction(int reg, int val) : destReg(reg), immediate(val) {}
    void execute(CPU& cpu) override {
        cpu.getRegister(destReg).setValue(static_cast<signed char>(immediate));
        cpu.getFlags().resetAll();
        if (immediate == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};

class MovRegisterInstruction : public Instruction {
private:
    int destReg;
    int srcReg;
public:
    MovRegisterInstruction(int dest, int src) : destReg(dest), srcReg(src) {}
    void execute(CPU& cpu) override {
        signed char val = cpu.getRegister(srcReg).getValue();
        cpu.getRegister(destReg).setValue(val);
        cpu.getFlags().resetAll();
        if (val == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};

class MovIndirectInstruction : public Instruction {
private:
    int destReg;
    int srcRegAddr;
public:
    MovIndirectInstruction(int dest, int src) : destReg(dest), srcRegAddr(src) {}
    void execute(CPU& cpu) override {
        signed char addr = cpu.getRegister(srcRegAddr).getValue();
        signed char val = cpu.getMemory().read(addr);
        cpu.getRegister(destReg).setValue(val);
        cpu.getFlags().resetAll();
        if (val == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};


// ============================================================================
// PLACEHOLDERS FOR MEMBER 2 & MEMBER 3 INSTRUCTIONS
// ============================================================================

// TODO: Member 2 adds ArithmeticInstruction (ADD, SUB, MUL, DIV, INC, DEC) here
// TODO: Member 3 adds ShiftInstruction (ROL, ROR, SHL, SHR), Load/Store, Stack (PUSH, POP) here



// Assembly Runner - Adam Syafiq
class Runner {
private:
    MyVector<Instruction*> program; // Custom vector tracking polymorphic instruction for pointers
    CPU cpu;                        // The virtual machine hardware chassis instance

    // Convert a string like "R3" or "[R1]" into a raw integer ID
    int getRegisterId(std::string regStr) {
        for (char c : regStr) {
            if (c >= '0' && c <= '7') {
                return c - '0';
            }
        }
        return 0; 
    }

public:
    // Destructor to safely clean up allocated heap memory 
    ~Runner() {
        for (int i = 0; i < program.size(); i++) {
            delete program[i];
        }
    }

    // Opens and reads external file line by line
    void loadFromFile(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                decodeAndBuild(line); 
            }
        }
        file.close();
    }

    // Parses raw text lines and maps them to Member 1 polymorphic class objects
    void decodeAndBuild(std::string line) {
        std::stringstream ss(line);
        std::string op, arg1, arg2;
        ss >> op >> arg1;
        
        if (!arg1.empty() && arg1.back() == ',') {
            arg1.pop_back(); // Strip off trailing commas (e.g., "R0," -> "R0")
        }
        int r1 = getRegisterId(arg1);

        if (op == "INPUT") {
            program.push_back(new InputInstruction(r1));
        } else if (op == "MOV") {
            ss >> arg2;
            if (arg2[0] == 'R') {
                program.push_back(new MovRegisterInstruction(r1, getRegisterId(arg2)));
            } else if (arg2[0] == '[') {
                program.push_back(new MovIndirectInstruction(r1, getRegisterId(arg2)));
            } else {
                // Manual string-to-integer conversion to bypass unlearned functions safely
                int val = 0, sign = (arg2[0] == '-') ? -1 : 1;
                for (char c : arg2) {
                    if (c >= '0' && c <= '9') val = val * 10 + (c - '0');
                }
                program.push_back(new MovImmediateInstruction(r1, val * sign));
            }
        }
    }

    // The core execution loop engine
    void runProgram() {
        while (cpu.getPC() < program.size()) {
            program[cpu.getPC()]->execute(cpu);
        }
        OutputFormatter::printState(cpu);
    }
};

int main() {
    Runner vmRunner;

    //vmRunner.loadAndParseMockFile(); testing without asm file

    vmRunner.loadFromFile("program.asm");

    vmRunner.runProgram();
    return 0;
}