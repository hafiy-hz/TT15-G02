#include <iostream>
#include <string>
#include <fstream>

// Virtual Machine and Assembly Language Interpreter
// Combined Workspace: Adam Syafiq, Adam Harith, Airell Aidan, & Hafiy

// Written by: Adam Syafiq
template <typename T>
class MyArray { 
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
    MyArray() {
        capacity = 4;
        current_size = 0;
        arr = new T[capacity];
    }
    ~MyArray() { delete[] arr; }

    void push_back(const T& data) {
        if (current_size == capacity) resize();
        arr[current_size++] = data;
    }

    T& operator[](int index) { return arr[index]; }
    int size() const { return current_size; }
};

// Written by: Hafiy 
// Custom Queue class template to replace the forbidden 'std::queue'
template <typename T>
class MyQueue {
private:
    T arr[200];        // Fixed-size array mimicking a circular buffer
    int frontIndex;    // Tracks the index of the front element
    int rearIndex;     // Tracks where the next incoming element lands
    int count;         // Current total element count inside the queue
public:
    // Initializes an empty queue structure
    MyQueue() : frontIndex(0), rearIndex(0), count(0) {}
    
    // Enqueues an item at the back using circular tracking logic
    void enqueue(T val) {
        if (count < 200) {
            arr[rearIndex] = val;
            rearIndex = (rearIndex + 1) % 200; // Loops back to index 0 if it hits 200
            count++;
        }
    }
    
    // Dequeues and returns the front item, terminates if empty
    T dequeue() {
        if (count > 0) {
            T val = arr[frontIndex];
            frontIndex = (frontIndex + 1) % 200; // Moves front pointer circularly
            count--;
            return val;
        }
        exit(1); // Terminates the program cleanly if unexpected empty states happen
    }
    
    // Checks if the container has elements left to pull
    bool isEmpty() const { return count == 0; }
};

// Written by: Adam Syafiq 
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

// ============================================================================
// CORE ARCHITECTURE CLASSES
// ============================================================================

// Written by: Adam Syafiq
class Register {
protected:
    signed char value;
public:
    Register() : value(0) {}
    virtual ~Register() {}
    signed char getValue() const { return value; }
    void setValue(signed char val) { value = val; }
};

// Written by: Adam Syafiq
class GeneralRegister : public Register {
private:
    int id;
public:
    GeneralRegister() : id(0) {}
    void setId(int rId) { id = rId; }
    int getId() const { return id; }
};

// Written by: Adam Syafiq
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

// Written by: Adam Syafiq
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

// Written by: Adam Syafiq & Aiden
class CPU {
private:
    GeneralRegister registers[8];
    FlagRegister flags;
    Memory memory;
    int pc;
    int si;
    MyStack<signed char> systemStack;

    
    void printPadded(int val) const {
        if (val < 0) {
            std::cout << "-";
            val = -val;
        }
        if (val < 10) std::cout << "000";
        else if (val < 100) std::cout << "00";
        else if (val < 1000) std::cout << "0";
        std::cout << val;
    }

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
    void incrementSI() { si++; }
    void decrementSI() { si--; }
    int getSI() const { return si; }
    MyStack<signed char>& getStack() { return systemStack; }

    // FIXED: Moved to CPU class and renamed to dump() 
    void dump() {
        std::cout << "#Begin#" << std::endl;
        std::cout << "#Registers#";
        for (int i = 0; i < 8; i++) {
            printPadded(registers[i].getValue());
            std::cout << "#";
        }
        std::cout << std::endl;
        std::cout << "#Flags#OF#" << flags.getOF() << "#UF#" << flags.getUF()
                  << "#CF#" << flags.getCF() << "#ZF#" << flags.getZF() << "#" << std::endl;
        std::cout << "#PC#";
        printPadded(pc);
        std::cout << "#" << std::endl;
        
        std::cout << "#Memory#" << std::endl;
        for (int row = 0; row < 8; row++) {
            std::cout << "#";
            for (int col = 0; col < 8; col++) {
                printPadded(memory.read(row * 8 + col));
                std::cout << "#";
            }
            std::cout << std::endl;
        }
        std::cout << "#End#" << std::endl;
    }
};

// ============================================================================
// INSTRUCTION BASE & POLYMORPHIC SUBCLASSES
// ============================================================================

// Written by: Adam Syafiq
class Instruction {
public:
    virtual ~Instruction() {}
    virtual void execute(CPU& cpu) = 0;
};

// Written by: Adam Syafiq
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

// FIXED: Added missing DISPLAY instruction required by the specs
// Written by: Aiden
class DisplayInstruction : public Instruction {
private:
    int srcReg;
public:
    DisplayInstruction(int reg) : srcReg(reg) {}
    void execute(CPU& cpu) override {
        std::cout << static_cast<int>(cpu.getRegister(srcReg).getValue()) << std::endl;
        cpu.incrementPC();
    }
};

// Written by: Adam Syafiq
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

// Written by: Adam Syafiq
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

// Written by: Adam Syafiq
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
// ARITHMETIC INSTRUCTIONS & FLAGS
// ============================================================================
// Written by: Adam Harith

class FlagHelper {
public:
    static void updateFlags(CPU& cpu, int rawResult) {
        if (rawResult == 0) {
            cpu.getFlags().setZF(true);
        }
        if (rawResult > 127) {
            cpu.getFlags().setOF(true);
            cpu.getFlags().setCF(true); 
        }
        if (rawResult < -128) {
            cpu.getFlags().setUF(true);
            cpu.getFlags().setCF(true); 
        }
    }
};

class AddInstruction : public Instruction {
private:
    int destReg;
    int srcReg;
public:
    AddInstruction(int dest, int src) : destReg(dest), srcReg(src) {}
    void execute(CPU& cpu) override {
        signed char val1 = cpu.getRegister(destReg).getValue();
        signed char val2 = cpu.getRegister(srcReg).getValue();
        cpu.getFlags().resetAll();
        int rawResult = (int)val1 + (int)val2;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC(); 
    }
};

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
        int rawResult = (int)val1 - (int)val2;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC(); 
    }
};

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
        int rawResult = (int)val1 * (int)val2;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC(); 
    }
};

class IncInstruction : public Instruction {
private:
    int destReg;
public:
    IncInstruction(int dest) : destReg(dest) {}
    void execute(CPU& cpu) override {
        signed char val1 = cpu.getRegister(destReg).getValue();
        cpu.getFlags().resetAll();
        int rawResult = (int)val1 + 1;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC(); 
    }
};

class DecInstruction : public Instruction {
private:
    int destReg;
public:
    DecInstruction(int dest) : destReg(dest) {}
    void execute(CPU& cpu) override {
        signed char val1 = cpu.getRegister(destReg).getValue();
        cpu.getFlags().resetAll();
        int rawResult = (int)val1 - 1;
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC(); 
    }
};

class ResetRegisterInstruction : public Instruction {
private:
    int destReg;
public:
    ResetRegisterInstruction(int dest) : destReg(dest) {}
    void execute(CPU& cpu) override {
        cpu.getFlags().resetAll();
        FlagHelper::updateFlags(cpu, 0);
        cpu.getRegister(destReg).setValue(0);
        cpu.incrementPC(); 
    }
};

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

class MulImmediateInstruction : public Instruction {
private:
    int destReg, immediate;
public:
    MulImmediateInstruction(int dest, int imm) : destReg(dest), immediate(imm) {}
    void execute(CPU& cpu) override {
        int rawResult = (int)cpu.getRegister(destReg).getValue() * immediate;
        cpu.getFlags().resetAll();
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC();
    }
};

class DivImmediateInstruction : public Instruction {
private:
    int destReg, immediate;
public:
    DivImmediateInstruction(int dest, int imm) : destReg(dest), immediate(imm) {}
    void execute(CPU& cpu) override {
        if (immediate == 0) { 
            std::cout << "Error: Division by zero!" << std::endl; 
            cpu.incrementPC(); 
            return; 
        }
        int rawResult = (int)cpu.getRegister(destReg).getValue() / immediate;
        cpu.getFlags().resetAll();
        FlagHelper::updateFlags(cpu, rawResult);
        cpu.getRegister(destReg).setValue(static_cast<signed char>(rawResult));
        cpu.incrementPC();
    }
};

// ============================================================================
// BIT SHIFTS, ROTATES, MEMORY ACCESS, STACK INSTRUCTIONS
// ============================================================================
// Written by: Hafiy

// Utility converting a decimal byte into an explicit binary array tracking mapping bits
void toBinary(signed char value, int bits[8]) {
    unsigned char uval = (unsigned char)value;
    for (int i = 0; i < 8; i++) bits[7 - i] = (uval >> i) & 1;
}
 
// Utility converting a mapped binary tracking array back down to a decimal value
signed char toDecimal(int bits[8]) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) result = (unsigned char)((result << 1) | bits[i]);
    return (signed char)result;
}
 
// Rotates bits inside targeted register directly towards the left side
class RotateLeftInstruction : public Instruction {
private:
    int destReg, count;
public:
    RotateLeftInstruction(int reg, int c) : destReg(reg), count(c) {}
    void execute(CPU& cpu) override {
        int bits[8], result[8];
        toBinary(cpu.getRegister(destReg).getValue(), bits);
        int c = count % 8;
        for (int i = 0; i < 8; i++) result[i] = bits[(i + c) % 8];
        signed char resVal = toDecimal(result);
        cpu.getRegister(destReg).setValue(resVal);
        cpu.getFlags().resetAll();
        if (resVal == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};
 
// Rotates bits inside targeted register directly towards the right side
class RotateRightInstruction : public Instruction {
private:
    int destReg, count;
public:
    RotateRightInstruction(int reg, int c) : destReg(reg), count(c) {}
    void execute(CPU& cpu) override {
        int bits[8], result[8];
        toBinary(cpu.getRegister(destReg).getValue(), bits);
        int c = count % 8;
        for (int i = 0; i < 8; i++) result[i] = bits[(i - c + 8) % 8];
        signed char resVal = toDecimal(result);
        cpu.getRegister(destReg).setValue(resVal);
        cpu.getFlags().resetAll();
        if (resVal == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};
 
// Shifts bits leftward, padding rightmost elements with zeros
class ShiftLeftInstruction : public Instruction {
private:
    int destReg, count;
public:
    ShiftLeftInstruction(int reg, int c) : destReg(reg), count(c) {}
    void execute(CPU& cpu) override {
        signed char resVal = 0;
        if (count < 8) {
            int bits[8], result[8];
            toBinary(cpu.getRegister(destReg).getValue(), bits);
            for (int i = 0; i < 8; i++) result[i] = (i + count < 8) ? bits[i + count] : 0;
            resVal = toDecimal(result);
        }
        cpu.getRegister(destReg).setValue(resVal);
        cpu.getFlags().resetAll();
        if (resVal == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};
 
// Shifts bits rightward, padding leftmost elements with zeros
class ShiftRightInstruction : public Instruction {
private:
    int destReg, count;
public:
    ShiftRightInstruction(int reg, int c) : destReg(reg), count(c) {}
    void execute(CPU& cpu) override {
        signed char resVal = 0;
        if (count < 8) {
            int bits[8], result[8];
            toBinary(cpu.getRegister(destReg).getValue(), bits);
            for (int i = 0; i < 8; i++) result[i] = (i - count >= 0) ? bits[i - count] : 0;
            resVal = toDecimal(result);
        }
        cpu.getRegister(destReg).setValue(resVal);
        cpu.getFlags().resetAll();
        if (resVal == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};
 
// Loads specific data straight from a explicit memory location address index
class LoadDirectInstruction : public Instruction {
private:
    int destReg, address;
public:
    LoadDirectInstruction(int reg, int addr) : destReg(reg), address(addr) {}
    void execute(CPU& cpu) override {
        signed char val = cpu.getMemory().read(address);
        cpu.getRegister(destReg).setValue(val);
        cpu.incrementPC();
    }
};
 
// Loads memory data indirectly based on address stored inside a register
class LoadIndirectInstruction : public Instruction {
private:
    int destReg, srcReg;
public:
    LoadIndirectInstruction(int dest, int src) : destReg(dest), srcReg(src) {}
    void execute(CPU& cpu) override {
        int address = cpu.getRegister(srcReg).getValue();
        signed char val = cpu.getMemory().read(address);
        cpu.getRegister(destReg).setValue(val);
        cpu.incrementPC();
    }
};
 
// Stores specific values from a register directly into a concrete memory cell
class StoreDirectInstruction : public Instruction {
private:
    int srcReg, address;
public:
    StoreDirectInstruction(int reg, int addr) : srcReg(reg), address(addr) {}
    void execute(CPU& cpu) override {
        signed char val = cpu.getRegister(srcReg).getValue();
        cpu.getMemory().write(address, val);
        cpu.incrementPC();
    }
};
 
// Stores specific values from a register indirectly into a memory cell mapped by a register address
class StoreIndirectInstruction : public Instruction {
private:
    int destReg, srcReg;
public:
    StoreIndirectInstruction(int dest, int src) : destReg(dest), srcReg(src) {}
    void execute(CPU& cpu) override {
        int address = cpu.getRegister(destReg).getValue();
        signed char val = cpu.getRegister(srcReg).getValue();
        cpu.getMemory().write(address, val);
        cpu.incrementPC();
    }
};
 
// Pushes value inside specified target register directly up onto stack allocation
class PushInstruction : public Instruction {
private:
    int srcReg;
public:
    PushInstruction(int reg) : srcReg(reg) {}
    void execute(CPU& cpu) override {
        cpu.getStack().push(cpu.getRegister(srcReg).getValue());
        cpu.incrementSI(); // Expand stack index boundaries
        cpu.incrementPC();
    }
};
 
// Pops top value off stack architecture allocation straight back down into a register variable
class PopInstruction : public Instruction {
private:
    int destReg;
public:
    PopInstruction(int reg) : destReg(reg) {}
    void execute(CPU& cpu) override {
        signed char val = cpu.getStack().pop();
        cpu.decrementSI(); // Shrink stack index parameters
        cpu.getRegister(destReg).setValue(val);
        cpu.incrementPC();
    }
};

// Targets single specific named hardware flag configurations and strips assertions away back to false
class ResetFlagInstruction : public Instruction {
private:
    std::string flagName;
public:
    ResetFlagInstruction(std::string flag) : flagName(flag) {}
    void execute(CPU& cpu) override {
        if (flagName == "CF") cpu.getFlags().setCF(false);
        else if (flagName == "OF") cpu.getFlags().setOF(false);
        else if (flagName == "UF") cpu.getFlags().setUF(false);
        else if (flagName == "ZF") cpu.getFlags().setZF(false);
        cpu.incrementPC();
    }
};

// ============================================================================
// FLEXIBLE LINE PARSER & INTEGRATED ASM RUNNER
// ============================================================================
// Written by: Adam Syafiq & Aiden

// Custom string parsing tool converting raw text parameters safely into base integer values
int parseInt(std::string s) {
    int val = 0;
    int sign = 1;
    int start = 0;
    if (!s.empty() && s[0] == '-') { sign = -1; start = 1; }
    for (int i = start; i < (int)s.size(); i++) {
        if (s[i] >= '0' && s[i] <= '9') val = val * 10 + (s[i] - '0');
    }
    return val * sign;
}

// System controller coordinator handling orchestration loops and initialization routines
class Runner {
private:
    MyArray<Instruction*> program; // Dynamic pointer compilation vector tracking parsed executable instruction blocks
    CPU cpu;                       // Concrete structural target instantiation mapping processor logic blocks

    // Pulls structural ID index definitions cleanly out from tracking literal names (ex: "R3" returns integer 3)
    int getRegisterId(std::string regStr) {
        for (char c : regStr) {
            if (c >= '0' && c <= '7') {
                return c - '0';
            }
        }
        return 0; 
    }

    // Drops syntactical instruction commas from text configurations
    void stripComma(std::string& s) {
        if (!s.empty() && s.back() == ',') s.pop_back();
    }

    // Custom string loop slice parsing tool replacing forbidden std::stringstream structures safely
    int tokenize(const std::string& line, std::string tokens[4]) {
        int tokenCount = 0;
        std::string currentToken = "";
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            // Split strings on spaces, tabs, or carriage returns
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                if (!currentToken.empty()) {
                    if (tokenCount < 4) tokens[tokenCount++] = currentToken;
                    currentToken = "";
                }
            } else {
                currentToken += c;
            }
        }
        if (!currentToken.empty() && tokenCount < 4) {
            tokens[tokenCount++] = currentToken;
        }
        return tokenCount; // Return total structural tokens captured
    }

public:
    // Destructor cleanly recycling polymorphic commands from heap loops memory spaces
    ~Runner() {
        for (int i = 0; i < program.size(); i++) {
            delete program[i];
        }
    }

    // File loading system streaming configuration information directly from text maps
    void loadFromFile(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            return;
        }
        MyQueue<std::string> lineQueue; // Custom queue instance processing text lines
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) lineQueue.enqueue(line);
        }
        file.close();

        // Process line maps sequentially from the queue
        while (!lineQueue.isEmpty()) {
            decodeAndBuild(lineQueue.dequeue());
        }
    }

    // Decodes assembly instructions and instantiates corresponding instruction objects
    void decodeAndBuild(std::string line) {
        std::string tokens[4] = {"", "", "", ""};
        int count = tokenize(line, tokens);
        if (count == 0) return; // Skip evaluation routines if parsing empty lines

        std::string op = tokens[0];     // Captured opcode string (ex: "MOV", "ADD")
        std::string arg1 = tokens[1];   // Target parameter element string
        stripComma(arg1);
        int r1 = getRegisterId(arg1);   // Target resolved reference ID assignment mappings

        // I/O Command Branch Handlers
        if (op == "INPUT") {
            program.push_back(new InputInstruction(r1));
            return;
        }
        if (op == "DISPLAY") {
            program.push_back(new DisplayInstruction(r1));
            return;
        }

        // MOV Variable Parser Branch Configuration
        if (op == "MOV") {
            std::string arg2 = tokens[2];
            if (arg2[0] == 'R') {
                program.push_back(new MovRegisterInstruction(r1, getRegisterId(arg2)));
            } else if (arg2[0] == '[') {
                program.push_back(new MovIndirectInstruction(r1, getRegisterId(arg2)));
            } else {
                program.push_back(new MovImmediateInstruction(r1, parseInt(arg2)));
            }
            return;
        }

        // Shifts & Rotates Branch Handlers
        if (op == "ROL" || op == "ROR" || op == "SHL" || op == "SHR") {
            std::string arg2 = tokens[2];
            int countVal = parseInt(arg2);
            if (op == "ROL") program.push_back(new RotateLeftInstruction(r1, countVal));
            else if (op == "ROR") program.push_back(new RotateRightInstruction(r1, countVal));
            else if (op == "SHL") program.push_back(new ShiftLeftInstruction(r1, countVal));
            else program.push_back(new ShiftRightInstruction(r1, countVal));
            return;
        }

        // Memory Command Branch Handlers
        if (op == "LOAD") {
            std::string arg2 = tokens[2];
            if (arg2.size() > 1 && arg2[1] >= '0' && arg2[1] <= '9') {
                std::string numStr = arg2.substr(1, arg2.size() - 2); // Drops flanking brackets
                program.push_back(new LoadDirectInstruction(r1, parseInt(numStr)));
            } else {
                program.push_back(new LoadIndirectInstruction(r1, getRegisterId(arg2)));
            }
            return;
        }
        if (op == "STORE") {
            std::string arg2 = tokens[2];
            stripComma(arg2);
            if (!arg1.empty() && arg1[0] == '[') {
                program.push_back(new StoreIndirectInstruction(getRegisterId(arg1), getRegisterId(arg2)));
            } else if (!arg1.empty() && arg1[0] >= '0' && arg1[0] <= '9') {
                program.push_back(new StoreDirectInstruction(getRegisterId(arg2), parseInt(arg1)));
            } else {
                program.push_back(new StoreDirectInstruction(r1, parseInt(arg2)));
            }
            return;
        }

        // Stack and Hardware State Reset Controls
        if (op == "PUSH") { program.push_back(new PushInstruction(r1)); return; }
        if (op == "POP") { program.push_back(new PopInstruction(r1)); return; }
        if (op == "RESET") { 
            if (arg1 == "CF" || arg1 == "OF" || arg1 == "UF" || arg1 == "ZF") {
                program.push_back(new ResetFlagInstruction(arg1));
                return;
            }
        }
        
        // Member 2 Arithmetic Instructions Processing Branch
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            std::string arg2 = tokens[2];
            stripComma(arg2);
            if (op == "ADD") program.push_back(new AddInstruction(r1, getRegisterId(arg2)));
            else if (op == "SUB") program.push_back(new SubInstruction(r1, getRegisterId(arg2)));
            else if (op == "MUL") {
                if (!arg2.empty() && arg2[0] == 'R') program.push_back(new MulInstruction(r1, getRegisterId(arg2)));
                else program.push_back(new MulImmediateInstruction(r1, parseInt(arg2)));
            } else if (op == "DIV") {
                if (!arg2.empty() && arg2[0] == 'R') program.push_back(new DivInstruction(r1, getRegisterId(arg2)));
                else program.push_back(new DivImmediateInstruction(r1, parseInt(arg2)));
            }
            return;
        } 
        
        if (op == "INC") program.push_back(new IncInstruction(r1));
        else if (op == "DEC") program.push_back(new DecInstruction(r1));
        else if (op == "RESET") program.push_back(new ResetRegisterInstruction(r1));
    }

    // Main execution driver loop processing commands sequence paths sequentially
    void runProgram() {
        while (cpu.getPC() < program.size()) {
            program[cpu.getPC()]->execute(cpu); // Fetch and execute instruction dynamically
        }
        cpu.dump(); // Prints register, flags, and memory dumps after execution completes
    }
};

// ============================================================================
// MAIN SYSTEM ENTRY POINT
// ============================================================================
// Written by: aiden
int main() {
    Runner vmRunner;
    vmRunner.loadFromFile("program.asm");
    vmRunner.runProgram();
    return 0;
}
