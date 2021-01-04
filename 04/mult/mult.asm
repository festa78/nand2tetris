// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
@i
M=1
@R2
M=0

(LOOP)
@i  // for i = 1..R1
D=M
@R1
D=D-M  // i - R1 > 0 then end.
@END
D;JGT
@R0
D=M
@R2
M=D+M
@i
M=M+1
@LOOP
0;JMP

(END)
@END
0;JMP