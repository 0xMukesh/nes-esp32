# CPU

NES uses a modified version of [6502 chip](https://en.wikipedia.org/wiki/MOS_Technology_6502). it is commonly known as [NES 2a03](https://en.wikipedia.org/wiki/Ricoh_2A03). it is a 8-bit processor.

## specs

- uses a 16-bit address bus i.e. it can address 65536 different cells present in the [memory map](#memory-map)
- 2 KiB internal RAM
- follows little endian format for packing bytes together i.e. LSB is at the lower memory address

## memory map

ref: https://bugzmanov.github.io/nes_ebook/images/ch3/cpu_registers_memory.png

- **[0x0000..0x2000]** - this address space is used to access RAM. CPU has 2 KiB of internal RAM but 0x2000 = 8 KiB, so why is the RAM address space 4 times the actual designated RAM to CPU? it is because of [memory mirroring](#memory-mirroring). this space is subdivided into other chunks:
  - **[0x0000..0x0100]** - it is known as "zero page", first 256 bytes
  - **[0x0100..0x0200]** - used for storing the stack
  - **[0x0200..0x0800]** - the actual RAM is stored over here
  - **[0x0800..0x2000]** - mirrors of 0x0000 to 0x07FF address space
- **[0x2000..0x4020]** - to be used by other hardware modules such as PPU, APU and gamepads
- **[0x4020..0x6000]** - this space's purposed changed quite a lot across multiple different generations. within the scope of building the emulator, we'll ignore this space
- **[0x6000..0x8000]** - RAM space for the cartridge
- **[0x8000..0xFFFF]** - it is mapped to the PRG ROM (program ROM) space present on the cartridge -->

## registers

ref: https://www.nesdev.org/obelisk-6502-guide/registers.html

NES 2a03 has 6 registers in total. apart from PC, remaining all register are 8-bit. PC is 16-bit

- **program counter (PC)** - it is used to hold the address of the next instruction which is to be executed
- **stack pointer (SP)** - it is used as an index for the stack. within the RAM, **[0x100..0x1FF]** is used for the stack and **[0x200..0x7FF]** is used for general purpose RAM. SP's initial value is 0x1FF. when a byte is pushed to the stack, SP value is decreased. when a byte is popped from the stack, SP value is increased. the stack in NES increases from top to bottom.
- **accumulator (A)** - it is used for storing the results for different operations
- **index register X (X)** - it can be used for additional storage needs (acting like a counter, storing temp. values etc.). it is also used an offset while accesssing memory in few particular [addressing modes](#addressing-modes)
- **index register Y (Y)** - same as index register X
- **program status (P)** - it holds a 8-bit value contains 7 flags

  ```
  7 6 5 4 3 2 1 0
  N V - B D I Z C
  ```

  - **bit 7 (negative, N)** - set if the result of an operation is negative i.e. bit 7 of the result is 1
  - **bit 6 (overflow, V)** - set if result of a signed arithmetic operation overflowed i.e. result is too large for a signed byte. it is determined by looking at the looking at the carry between bits 6 and 7 and between bit 7 and the carry flag i.e. whether the carry in bit and carry out bit for bit 7 are different or not
  - **bit 5 (unused)** - always set to 1
  - **bit 4 (break, B)** - it acts like a transient signal in the CPU. if the flags were pushed while processing an interrupt, then it is 0 and 1 when it is pushed by instructions (`BRK`, `PHP`)
  - **bit 3 (decimal mode, D)** - when decimal mode flag is set, processor will obey the rules of [binary coded decimal](https://en.wikipedia.org/wiki/Binary-coded_decimal) during arithmetic operations
  - **bit 2 (interrupt disable, I)** - set when [SEI](https://www.nesdev.org/obelisk-6502-guide/reference.html#SEI) instruction is executed
  - **bit 1 (zero, Z)** - set if the result of the operation was 0
  - **bit 0 (carry, C)** - set if an operation produced a carry/borrow

## addressing modes

ref: https://www.nesdev.org/obelisk-6502-guide/addressing.html

6502 provides multiple different ways on how to address particular locations present in the memory

- **absolute** - takes a full 16-bit address as the argument for the instruction
- **absolute, X** - it is similar to absolute addressing mode but contents of `X` register is added to 16-bit address to obtain the final address.
- **absolute, Y** - same as using X register indexed absolute addressing mode (absolute, X) but with `Y` register instead
- **zero page** - all the instructions which support absolute addressing also support zero page addressing, where a single-byte address is passed i.e. only the first page (first 256 bytes) of memorty is accessible.
- **zero page, X** - it is similar to zero page addressing mode but contents of `X` register is added to the 8-bit address to obtain the final address. it performs a wrapping addition operation.
- **zero page, Y** - same as using X register indexed zero page addressing mode (zero page, X) but with `Y` register instead
- **immediate** - it doesn't have anything related to memory addresses. it allows instruction to accept a 8-bit constant as a value argument.

  ```
  LDX #$01  ;loads the value $01 into the X register
  LDX $01   ;loads the value at memory location $01 into the X register
  ```

- **relative** - it is generally used by branching instructions (instructions which are used to get same behaviour as if-else condition), where it takes in a 8-bit signed offset which is added to PC if condition is true
- **indirect** - it uses 16-bit address to look up another 16-bit address. the value stored in the address which was passed gives the LSB of the final address and the following byte gives the MSB of the final address. ex: if location $0120 contains $FC and $0121 contains $BA then JMP ($0120) will cause the next instruction execution to occur at $BAFC
- **indexed indirect** - it is like a combination of indirect and zero page, X addressing mode. it takes in a zero-page address (8-bit address), add the value of `X` register to value present at that zero-page address and then use that to look up a two-byte address in a similar way as how it is done in indirect addressing mode
- **indirect indexed** - over here, first the two-byte address is look up in a similar fashion as how it was done in indirect addressing mode and then the at the end value stored in `Y` register is added

## instructions

ref: https://www.nesdev.org/obelisk-6502-guide/instructions.html

- [ ] [ADC](https://www.nesdev.org/obelisk-6502-guide/reference.html#ADC)
- [x] [AND](https://www.nesdev.org/obelisk-6502-guide/reference.html#AND)
- [x] [ASL](https://www.nesdev.org/obelisk-6502-guide/reference.html#ASL)
- [ ] [BCC](https://www.nesdev.org/obelisk-6502-guide/reference.html#BCC)
- [ ] [BCS](https://www.nesdev.org/obelisk-6502-guide/reference.html#BCS)
- [ ] [BEQ](https://www.nesdev.org/obelisk-6502-guide/reference.html#BEQ)
- [x] [BIT](https://www.nesdev.org/obelisk-6502-guide/reference.html#BIT)
- [ ] [BMI](https://www.nesdev.org/obelisk-6502-guide/reference.html#BMI)
- [ ] [BNE](https://www.nesdev.org/obelisk-6502-guide/reference.html#BNE)
- [ ] [BPL](https://www.nesdev.org/obelisk-6502-guide/reference.html#BPL)
- [x] [BRK](https://www.nesdev.org/obelisk-6502-guide/reference.html#BRK)
- [ ] [BVC](https://www.nesdev.org/obelisk-6502-guide/reference.html#BVC)
- [ ] [BVS](https://www.nesdev.org/obelisk-6502-guide/reference.html#BVS)
- [x] [CLC](https://www.nesdev.org/obelisk-6502-guide/reference.html#CLC)
- [x] [CLD](https://www.nesdev.org/obelisk-6502-guide/reference.html#CLD)
- [x] [CLI](https://www.nesdev.org/obelisk-6502-guide/reference.html#CLI)
- [x] [CLV](https://www.nesdev.org/obelisk-6502-guide/reference.html#CLV)
- [ ] [CMP](https://www.nesdev.org/obelisk-6502-guide/reference.html#CMP)
- [ ] [CPX](https://www.nesdev.org/obelisk-6502-guide/reference.html#CPX)
- [ ] [CPY](https://www.nesdev.org/obelisk-6502-guide/reference.html#CPY)
- [x] [DEC](https://www.nesdev.org/obelisk-6502-guide/reference.html#DEC)
- [x] [DEX](https://www.nesdev.org/obelisk-6502-guide/reference.html#DEX)
- [x] [DEY](https://www.nesdev.org/obelisk-6502-guide/reference.html#DEY)
- [x] [EOR](https://www.nesdev.org/obelisk-6502-guide/reference.html#EOR)
- [x] [INC](https://www.nesdev.org/obelisk-6502-guide/reference.html#INC)
- [x] [INX](https://www.nesdev.org/obelisk-6502-guide/reference.html#INX)
- [x] [INY](https://www.nesdev.org/obelisk-6502-guide/reference.html#INY)
- [ ] [JMP](https://www.nesdev.org/obelisk-6502-guide/reference.html#JMP)
- [ ] [JSR](https://www.nesdev.org/obelisk-6502-guide/reference.html#JSR)
- [x] [LDA](https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA)
- [x] [LDX](https://www.nesdev.org/obelisk-6502-guide/reference.html#LDX)
- [x] [LDY](https://www.nesdev.org/obelisk-6502-guide/reference.html#LDY)
- [x] [LSR](https://www.nesdev.org/obelisk-6502-guide/reference.html#LSR)
- [x] [NOP](https://www.nesdev.org/obelisk-6502-guide/reference.html#NOP)
- [x] [ORA](https://www.nesdev.org/obelisk-6502-guide/reference.html#ORA)
- [x] [PHA](https://www.nesdev.org/obelisk-6502-guide/reference.html#PHA)
- [ ] [PHP](https://www.nesdev.org/obelisk-6502-guide/reference.html#PHP)
- [x] [PLA](https://www.nesdev.org/obelisk-6502-guide/reference.html#PLA)
- [x] [PLP](https://www.nesdev.org/obelisk-6502-guide/reference.html#PLP)
- [x] [ROL](https://www.nesdev.org/obelisk-6502-guide/reference.html#ROL)
- [x] [ROR](https://www.nesdev.org/obelisk-6502-guide/reference.html#ROR)
- [x] [RTI](https://www.nesdev.org/obelisk-6502-guide/reference.html#RTI)
- [ ] [RTS](https://www.nesdev.org/obelisk-6502-guide/reference.html#RTS)
- [ ] [SBC](https://www.nesdev.org/obelisk-6502-guide/reference.html#SBC)
- [x] [SEC](https://www.nesdev.org/obelisk-6502-guide/reference.html#SEC)
- [x] [SED](https://www.nesdev.org/obelisk-6502-guide/reference.html#SED)
- [x] [SEI](https://www.nesdev.org/obelisk-6502-guide/reference.html#SEI)
- [x] [STA](https://www.nesdev.org/obelisk-6502-guide/reference.html#STA)
- [x] [STX](https://www.nesdev.org/obelisk-6502-guide/reference.html#STX)
- [x] [STY](https://www.nesdev.org/obelisk-6502-guide/reference.html#STY)
- [x] [TAX](https://www.nesdev.org/obelisk-6502-guide/reference.html#TAX)
- [x] [TAY](https://www.nesdev.org/obelisk-6502-guide/reference.html#TAY)
- [x] [TSX](https://www.nesdev.org/obelisk-6502-guide/reference.html#TSX)
- [x] [TXA](https://www.nesdev.org/obelisk-6502-guide/reference.html#TXA)
- [x] [TXS](https://www.nesdev.org/obelisk-6502-guide/reference.html#TXS)
- [x] [TYA](https://www.nesdev.org/obelisk-6502-guide/reference.html#TYA)

## quirks

### memory mirroring

within the CPU's memory map, the 2 KiB of physical RAM is mirrored 4 times across the 8 KiB address space (0x0000..0x2000). this means that the memory stored within [0x0800..0x1000] is equivalent to the memory stored within [0x0000..0x0800]. this means that there is no difference between reading/writing data at [0x0000..0x0800] and [0x0800..0x1000].

if we write represent 0x0000 and 0x1FFF in binary, we can notice something interesting:

- 0x0000 = 0000 0000 0000 0000
- 0x1FFF = 0001 1111 1111 1111

the address decoding logic to know where RAM was being accessed was just a simple 3-input NOR gate to check whether A13, A14 and A15 are all zero within the 16-bit address present on the bus. this leaves with more 13 bits and 2^13 = 8192 = 8 KiB and CPU has 2 KiB internal RAM. the mirroring happens because A11 and A12 are ignored.

if [0x0000...0x07FF] was selected to be the RAM within CPU's memory map (which is exactlty 2 KiB) then the decoder neeeds to check whether A15, A14, A13, A12 and A11 are all zero and at the time when NES was launched 3-input and 4-input NOR gates were much more common. incomplete address decoding was a common practice in game consoles at that time, it was present in [atari 2600](https://www.computerarcheology.com/Atari2600/Stella.html)

> "The address decoding for the 2600 is very simple. The TIA is accessed if A12 is 0 and A7 is 0. The TIA is selected if (address&1080 == 0000).
> The TIA chip uses the 6 lower address lines A0-A5. **The reads seem to ignore A4 and A5.**
> The addresses have different read/write definitions and many of them are strobes that are triggered by writing any value."

## additional resources

- https://www.emulationonline.com/systems/nes/6502-emulation-tips/
- https://github.com/mnaberez/py65
