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
// Written by: [Adam Harith]
#include <iostream>

// FLAGHELPER; using a static helper class so that we don't waste lines of code in the main instructions.
class FlagHelper {
public:
    static void updateFlags(CPU& cpu, int rawResult) {
        // 1. Check for Zero (ZF)
        if (rawResult == 0) {
            cpu.getFlags().setZF(true);
        }

        // 2. Check for Overflow (OF) and Carry (CF)
        // An 8-bit signed integer cannot go above 127.
        if (rawResult > 127) {
            cpu.getFlags().setOF(true);
            cpu.getFlags().setCF(true); 
        }

        // 3. Check for Underflow (UF) 
        // An 8-bit signed integer cannot drop below -128.
        if (rawResult < -128) {
            cpu.getFlags().setUF(true);
            cpu.getFlags().setCF(true); 
        }
    }
};

// AddInstruction or ADD (+)  <3333333
class AddInstruction : public Instruction {
private:
    int destReg;
    int srcReg;

public:
    // The constructor saves which registers are using (example: ADD R1, R2)
    AddInstruction(int dest, int src) : destReg(dest), srcReg(src) {}

    void execute(CPU& cpu) override {
        // 1. Get the current numbers stored in the two registers
        signed char val1 = cpu.getRegister(destReg).getValue();
        signed char val2 = cpu.getRegister(srcReg).getValue();

        // 2. Clear any old flags from previous commands
        cpu.getFlags().resetAll();

        // 3. Do the math using a standard 'int' so we can catch overflows
        int rawResult = (int)val1 + (int)val2;

        // 4. Send the answer to helper to set OF, UF, CF, or ZF
        FlagHelper::updateFlags(cpu, rawResult);

        // 5. Force the answer back into an 8-bit size and save it
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        // 6. Tell the CPU to move to the next line of the text file
        cpu.incrementPC(); 
    }
};

// SubInstruction or SUB (-)  <3333333, 
class SubInstruction : public Instruction {
private:
    int destReg;
    int srcReg;

public:
    SubInstruction(int dest, int src) : destReg(dest), srcReg(src) {}

    void execute(CPU& cpu) override {

        signed char val1 = cpu.getRegister(destReg).getValue();
        signed char val2 = cpu.getRegister(srcReg).getValue();

        cpu.getFlags().resetAll();
        
        // similar to ADD function except we sutract instead
        int rawResult = (int)val1 - (int)val2;

        FlagHelper::updateFlags(cpu, rawResult);

        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        cpu.incrementPC(); 
    }
};

// MulInstruction or MULL (*)  <3333333,
class MulInstruction : public Instruction {
private:
    int destReg;
    int srcReg;

public:
    MulInstruction(int dest, int src) : destReg(dest), srcReg(src) {}

    void execute(CPU& cpu) override {

        signed char val1 = cpu.getRegister(destReg).getValue();
        signed char val2 = cpu.getRegister(srcReg).getValue();

        cpu.getFlags().resetAll();
        
        // similar to the other function except we multiply instead
        int rawResult = (int)val1 * (int)val2;

        FlagHelper::updateFlags(cpu, rawResult);

        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        cpu.incrementPC(); 
    }
};

// IncInstruction or increment  (++)  wkwkwkwk
class IncInstruction : public Instruction {
private:
    int destReg;

public:
    IncInstruction(int dest) : destReg(dest) {}

    void execute(CPU& cpu) override {

        signed char val1 = cpu.getRegister(destReg).getValue();

        cpu.getFlags().resetAll();
        
        // only need one register for this, not two
        int rawResult = (int)val1 + 1;

        FlagHelper::updateFlags(cpu, rawResult);

        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        cpu.incrementPC(); 
    }
};

// DecInstruction or Decrement  (--)  HAHAHAHAHAHA
class DecInstruction : public Instruction {
private:
    int destReg;

public:
    DecInstruction(int dest) : destReg(dest) {}

    void execute(CPU& cpu) override {

        signed char val1 = cpu.getRegister(destReg).getValue();

        cpu.getFlags().resetAll();
        
        // only need one register for this, not two
        int rawResult = (int)val1 - 1;

        FlagHelper::updateFlags(cpu, rawResult);

        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        cpu.incrementPC(); 
    }
};

// ResetInstruction is reset (\\>.<//)
class ResetInstruction : public Instruction {
private:
    int destReg;

public:
    ResetInstruction(int dest) : destReg(dest) {}

    void execute(CPU& cpu) override {
        // 1. Clear any old flags from previous commands
        cpu.getFlags().resetAll();

        // 2. The result of a reset is always just 0
        int rawResult = 0;

        // 3. Send the answer to flag 
        // (Because the answer is 0, flag helper will automatically turn ON the Zero Flag)
        FlagHelper::updateFlags(cpu, rawResult);

        // 4. Force the 0 back into the register
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));

        // 5. Move to the next line of the text file
        cpu.incrementPC(); 
    }
};

// DivInstruction or DIV (/)
class DivInstruction : public Instruction {
private:
    int destReg;
    int srcReg;

public:
    DivInstruction(int dest, int src) : destReg(dest), srcReg(src) {}

    void execute(CPU& cpu) override {
        signed char val1 = cpu.getRegister(destReg).getValue();
        signed char val2 = cpu.getRegister(srcReg).getValue();
        cpu.getFlags().resetAll();

        // to prevent crashing
        if (val2 == 0) {
            std::cout << "Error: Division by zero!" << std::endl;
            cpu.incrementPC();
            return; 
        }

        int rawResult = (int)val1 / (int)val2;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC();
    }
};

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
        // ADDED BY Adam harith, for my part to trigger the math classes
        else if (op == "ADD") {
            ss >> arg2;
            program.push_back(new AddInstruction(r1, getRegisterId(arg2)));
        } else if (op == "SUB") {
            ss >> arg2;
            program.push_back(new SubInstruction(r1, getRegisterId(arg2)));
        } else if (op == "MUL") {
            ss >> arg2;
            program.push_back(new MulInstruction(r1, getRegisterId(arg2)));
        } else if (op == "DIV") {
            ss >> arg2;
            program.push_back(new DivInstruction(r1, getRegisterId(arg2)));
        } else if (op == "INC") {
            program.push_back(new IncInstruction(r1));
        } else if (op == "DEC") {
            program.push_back(new DecInstruction(r1));
        } else if (op == "RESET") {
            program.push_back(new ResetInstruction(r1));
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