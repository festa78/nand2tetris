#!/usr/bin/python -B

import argparse
import os

import vmparser
import code_writer

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("vmpath", help="Path to or folder of .vm file(s) to compile",
                        type=str)
    parser.add_argument("outpath", help="Path to compiled .asm file to output",
                        type=str)
    parser.add_argument("--do_bootstrap", help="Generate bootstrap code.",
                        action='store_true')
    args = parser.parse_args()

    if os.path.isdir(args.vmpath):
        vmpaths = [os.path.join(args.vmpath, p)
            for p in os.listdir(args.vmpath) if p.endswith('.vm')]
    else:
        vmpaths = [args.vmpath]

    codeWriter = code_writer.CodeWriter(args.outpath, args.do_bootstrap)

    for vmpath in vmpaths:
        print(vmpath)
        codeWriter.setFileName(vmpath)

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
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[3]:
                # C_LABEL.
                label = codeWriter.vmParser.arg1()
                assert not label[0].isdigit(
                    ), 'label name should not start from a digit.'

                codeWriter.writeLabel(label)
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[4]:
                # C_GOTO.
                label = codeWriter.vmParser.arg1()
                assert not label[0].isdigit(
                    ), 'label name should not start from a digit.'

                codeWriter.writeGoto(label)
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[5]:
                # C_IF.
                label = codeWriter.vmParser.arg1()
                assert not label[0].isdigit(
                    ), 'label name should not start from a digit.'

                codeWriter.writeIf(label)
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[6]:
                # C_FUNCTION.
                functionName = codeWriter.vmParser.arg1()
                numLocals = codeWriter.vmParser.arg2()
                assert numLocals.isdigit(
                    ), 'numLocals should be a digit.'

                codeWriter.writeFunction(functionName, int(numLocals))
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[7]:
                # C_RETURN.
                codeWriter.writeReturn()
            elif command_type in codeWriter.vmParser.COMMAND_TYPES[8]:
                # C_CALL.
                functionName = codeWriter.vmParser.arg1()
                numArgs = codeWriter.vmParser.arg2()
                assert numArgs.isdigit(
                    ), 'numArgs should be a digit.'

                codeWriter.writeCall(functionName, int(numArgs))
            else:
                AttributeError('Unsupported command type')

            if not codeWriter.vmParser.hasMoreCommands():
                break
            codeWriter.vmParser.advance()
