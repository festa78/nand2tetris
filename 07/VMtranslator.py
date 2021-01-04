#!/usr/bin/python -B

import argparse
import os

import vmparser
import code_writer

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("vmpath", help="Path to .vm file to compile",
                        type=str)
    parser.add_argument("outpath", help="Path to compiled .asm file to output",
                        type=str)
    args = parser.parse_args()

    codeWriter = code_writer.CodeWriter(args.outpath)
    codeWriter.setFileName(args.vmpath)

    while True:
        command_type = codeWriter.vmParser.commandType()
        if command_type == codeWriter.vmParser.COMMAND_TYPES[0]:
            # C_ARITHMETIC.
            command = codeWriter.vmParser.arg1()
            codeWriter.writeArithmetic(command)
        elif command_type in codeWriter.vmParser.COMMAND_TYPES[1:3]:
            # C_PUSH or C_POP.
            segment = codeWriter.vmParser.arg1()
            value = codeWriter.vmParser.arg2()
            assert value.isdigit()

            codeWriter.writePushPop(command_type, segment, value)
        else:
            AttributeError('Unsupported command type')

        if not codeWriter.vmParser.hasMoreCommands():
            break
        codeWriter.vmParser.advance()
