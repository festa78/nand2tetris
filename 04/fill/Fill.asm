// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
@16384
D=A
@start
M=D
@24576
D=A
@end
M=D

(MAINLOOP)
@start
D=M
@i
M=D
@24576  // If 0, goto IFWHITE, else IFBLACK.
D=M
@WHITELOOP
D;JEQ
@BLACKLOOP
0;JMP

(WHITELOOP)
@i  // for i = 16384..24576
D=M
@end
D=D-M  // i - 24576 => 0 then end.
D=D+1
@MAINLOOP
D;JGT
@i
A=M
M=0  // White
@i
M=M+1
@WHITELOOP
0;JMP

(BLACKLOOP)
@i  // for i = 16384..24576
D=M
@end
D=D-M  // i - 24576 => 0 then end.
D=D+1
@MAINLOOP
D;JGT
@i
A=M
M=-1  // Black
@i
M=M+1
@BLACKLOOP
0;JMP
