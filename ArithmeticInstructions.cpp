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