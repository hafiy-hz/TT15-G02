#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Virtual Machine and Assembly Language Interpreter
// Combined Workspace: Adam Syafiq, Adam Harith, Airell Aidan, & Hafiy (Member 4)

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

// FIXED: Added Custom Queue Class directly here for single-file assignment rules
template <typename T>
class MyQueue {
private:
    T arr[200];
    int frontIndex;
    int rearIndex;
    int count;
public:
    MyQueue() : frontIndex(0), rearIndex(0), count(0) {}
    
    void enqueue(T val) {
        if (count < 200) {
            arr[rearIndex] = val;
            rearIndex = (rearIndex + 1) % 200;
            count++;
        }
    }
    
    T dequeue() {
        if (count > 0) {
            T val = arr[frontIndex];
            frontIndex = (frontIndex + 1) % 200;
            count--;
            return val;
        }
        exit(1);
    }
    
    bool isEmpty() const { return count == 0; }
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
    
    // FIXED: Added missing stack index methods
    void incrementSI() { si++; }
    void decrementSI() { si--; }
    int getSI() const { return si; }

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

// Polymorphic Instructions - Adam Syafiq

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
// MEMBER 2 ARITHMETIC INSTRUCTIONS
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

// FIXED: Renamed class to differentiate Register clearing vs Flag resetting
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

// INSERTED: MUL with immediate value (e.g. MUL R3, 4)
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

// INSERTED: DIV with immediate value (e.g. DIV R3, 2)
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
// MEMBER 3 SHIFT, ROTATE, MEMORY ACCESS, STACK INSTRUCTIONS
// ============================================================================
// Written by: Member 3 (Hafiy)
 
void toBinary(signed char value, int bits[8]) {
    unsigned char uval = (unsigned char)value;
    for (int i = 0; i < 8; i++) bits[7 - i] = (uval >> i) & 1;
}
 
signed char toDecimal(int bits[8]) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) result = (unsigned char)((result << 1) | bits[i]);
    return (signed char)result;
}
 
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
        
        // FIXED: Added missing assignment flag status checks
        cpu.getFlags().resetAll();
        if (resVal == 0) cpu.getFlags().setZF(true);
        cpu.incrementPC();
    }
};
 
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
 
class PushInstruction : public Instruction {
private:
    int srcReg;
public:
    PushInstruction(int reg) : srcReg(reg) {}
    void execute(CPU& cpu) override {
        cpu.getStack().push(cpu.getRegister(srcReg).getValue());
        cpu.incrementSI();
        cpu.incrementPC();
    }
};
 
class PopInstruction : public Instruction {
private:
    int destReg;
public:
    PopInstruction(int reg) : destReg(reg) {}
    void execute(CPU& cpu) override {
        signed char val = cpu.getStack().pop();
        cpu.decrementSI();
        cpu.getRegister(destReg).setValue(val);
        cpu.incrementPC();
    }
};

// FIXED: Named explicitly as ResetFlagInstruction to avoid constructor type clashing
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

// Assembly Runner - Adam Syafiq
class Runner {
private:
    MyVector<Instruction*> program; 
    CPU cpu;                        

    int getRegisterId(std::string regStr) {
        for (char c : regStr) {
            if (c >= '0' && c <= '7') {
                return c - '0';
            }
        }
        return 0; 
    }

    void stripComma(std::string& s) {
        if (!s.empty() && s.back() == ',') s.pop_back();
    }

    bool parseMovInput(const std::string& op, int r1, std::stringstream& ss) {
        if (op == "INPUT") {
            program.push_back(new InputInstruction(r1));
            return true;
        }
        if (op == "MOV") {
            std::string arg2; ss >> arg2;
            if (arg2[0] == 'R') {
                program.push_back(new MovRegisterInstruction(r1, getRegisterId(arg2)));
            } else if (arg2[0] == '[') {
                program.push_back(new MovIndirectInstruction(r1, getRegisterId(arg2)));
            } else {
                int val = 0, sign = (arg2[0] == '-') ? -1 : 1;
                for (char c : arg2) if (c >= '0' && c <= '9') val = val * 10 + (c - '0');
                program.push_back(new MovImmediateInstruction(r1, val * sign));
            }
            return true;
        }
        return false;
    }

    bool parseShiftRotate(const std::string& op, int r1, std::stringstream& ss) {
        if (op != "ROL" && op != "ROR" && op != "SHL" && op != "SHR") return false;
        std::string arg2; ss >> arg2;
        stripComma(arg2);
        int count = parseInt(arg2);
        if (op == "ROL") program.push_back(new RotateLeftInstruction(r1, count));
        else if (op == "ROR") program.push_back(new RotateRightInstruction(r1, count));
        else if (op == "SHL") program.push_back(new ShiftLeftInstruction(r1, count));
        else program.push_back(new ShiftRightInstruction(r1, count));
        return true;
    }

    bool parseLoad(int r1, std::stringstream& ss) {
        std::string arg2; ss >> arg2;
        if (arg2.size() > 1 && arg2[1] >= '0' && arg2[1] <= '9') {
            std::string numStr = arg2.substr(1, arg2.size() - 2);
            program.push_back(new LoadDirectInstruction(r1, parseInt(numStr)));
        } else {
            program.push_back(new LoadIndirectInstruction(r1, getRegisterId(arg2)));
        }
        return true;
    }

    // UPDATED: Replaced original implementation with flexible parsing paths
    bool parseStore(int r1, const std::string& arg1, std::stringstream& ss) {
        std::string arg2; ss >> arg2;
        stripComma(arg2);
        if (!arg1.empty() && arg1[0] == '[') {
            // STORE [R1], R2 — indirect mode
            program.push_back(new StoreIndirectInstruction(getRegisterId(arg1), getRegisterId(arg2)));
        } else if (!arg1.empty() && arg1[0] >= '0' && arg1[0] <= '9') {
            // STORE 20, R3 — address first (PDF sample format)
            program.push_back(new StoreDirectInstruction(getRegisterId(arg2), parseInt(arg1)));
        } else {
            // STORE R1, 43 — register first (spec section 3.9)
            program.push_back(new StoreDirectInstruction(r1, parseInt(arg2)));
        }
        return true;
    }

    bool parseStackAndFlags(const std::string& op, int r1, const std::string& arg1) {
        if (op == "PUSH") { program.push_back(new PushInstruction(r1)); return true; }
        if (op == "POP") { program.push_back(new PopInstruction(r1)); return true; }
        if (op == "RESET") { 
            // FIXED: Decides if parameter targets status bits or raw register indices
            if (arg1 == "CF" || arg1 == "OF" || arg1 == "UF" || arg1 == "ZF") {
                program.push_back(new ResetFlagInstruction(arg1));
                return true;
            }
        }
        return false;
    }

public:
    ~Runner() {
        for (int i = 0; i < program.size(); i++) {
            delete program[i];
        }
    }

    void loadFromFile(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            return;
        }
        MyQueue<std::string> lineQueue;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) lineQueue.enqueue(line);
        }
        file.close();

        while (!lineQueue.isEmpty()) {
            decodeAndBuild(lineQueue.dequeue());
        }
    }

    void decodeAndBuild(std::string line) {
        std::stringstream ss(line);
        std::string op, arg1, arg2; 
        ss >> op >> arg1;
        stripComma(arg1);
        int r1 = getRegisterId(arg1);

        if (parseMovInput(op, r1, ss)) return;
        if (parseShiftRotate(op, r1, ss)) return;
        if (op == "LOAD") { parseLoad(r1, ss); return; }
        if (op == "STORE") { parseStore(r1, arg1, ss); return; }
        if (parseStackAndFlags(op, r1, arg1)) return;
        
        // Member 2 Opcode Handlers
        if (op == "ADD") {
            ss >> arg2;
            program.push_back(new AddInstruction(r1, getRegisterId(arg2)));
        } else if (op == "SUB") {
            ss >> arg2;
            program.push_back(new SubInstruction(r1, getRegisterId(arg2)));
        } else if (op == "MUL") {
            // UPDATED: Direct immediate vs registration selection
            ss >> arg2; stripComma(arg2);
            if (!arg2.empty() && arg2[0] == 'R') {
                program.push_back(new MulInstruction(r1, getRegisterId(arg2)));
            } else {
                program.push_back(new MulImmediateInstruction(r1, parseInt(arg2)));
            }
        } else if (op == "DIV") {
            // UPDATED: Direct immediate vs registration selection
            ss >> arg2; stripComma(arg2);
            if (!arg2.empty() && arg2[0] == 'R') {
                program.push_back(new DivInstruction(r1, getRegisterId(arg2)));
            } else {
                program.push_back(new DivImmediateInstruction(r1, parseInt(arg2)));
            }
        } else if (op == "INC") {
            program.push_back(new IncInstruction(r1));
        } else if (op == "DEC") {
            program.push_back(new DecInstruction(r1));
        } else if (op == "RESET") {
            program.push_back(new ResetRegisterInstruction(r1));
        }
    }

    void runProgram() {
        while (cpu.getPC() < program.size()) {
            program[cpu.getPC()]->execute(cpu);
        }
        OutputFormatter::printState(cpu);
    }
};

int main() {
    Runner vmRunner;
    vmRunner.loadFromFile("program.asm");
    vmRunner.runProgram();
    return 0;
}
