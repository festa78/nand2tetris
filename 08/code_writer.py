import collections 

import vmparser

class CodeWriter:
    SEGMENT_BASE_ADDRESSES = {
        'STATIC': 16,
        'SP': 256,
    }

    SEGMENT_NAME_TO_SYMBOL = {
        'local': 'LCL',
        'argument': 'ARG',
        'this': 'THIS',
        'that': 'THAT',
        'pointer': 'R3',
        'temp': 'R5',
    }

    SYMBOL_TO_RAM_ADDRESS = {
        'SP': 0,
        'LCL': 1,
        'ARG': 2,
        'THIS': 3,
        'THAT': 4,
    }

    ANDORADDSUB_TO_OPERATOR = {
        'and': '&',
        'or': '|',
        'add': '+',
        'sub': '-',
    }

    EQLTGT_TO_JUMP_CODE = {
        'eq': 'JEQ',
        'lt': 'JLT',
        'gt': 'JGT',
    }

    NEGNOT_TO_OPERATOR = {
        'neg': '-',
        'not': '!',
    }

    def __init__(self, outFilePath):
        assert outFilePath.endswith('.asm'), 'Specify .asm file path'
        self.outFileStream = open(outFilePath, 'w')
        self.vmParser = None

        # Postfix ID to avoid duplicate label.
        self.label_count = 0

    def setFileName(self, vmFileName):
        self.vmFileName = vmFileName
        self.vmParser = vmparser.Parser(vmFileName)

    def writeLabel(self, label):
        self.outFileStream.write('({})\n'.format(label))

    def writeGoto(self, label):
        self.outFileStream.write('@{}\n'.format(label))
        self.outFileStream.write('0;JMP\n')

    def writeIf(self, label):
        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('D=M\n')
        # Jump to label if D is not equal to 0.
        self.outFileStream.write('@{}\n'.format(label))
        self.outFileStream.write('D;JNE\n')


    def writeArithmetic(self, command):
        """command is actual arithmetic command.
        """
        self.outFileStream.write('// {}\n'.format(command))
        assert self.vmParser.COMMAND_TO_TYPES[
            command] == self.vmParser.COMMAND_TYPES[
                0], 'Should be C_ARITHMETIC type'

        if command in self.ANDORADDSUB_TO_OPERATOR.keys():
            self._writeAndOrAddSub(command)
        elif command in self.EQLTGT_TO_JUMP_CODE.keys():
            self._writeEqLtGt(command)
        elif command in self.NEGNOT_TO_OPERATOR.keys():
            self._writeNegNot(command)
        else:
            print(command)
            raise AttributeError('Unsupported command')

    def _writeAndOrAddSub(self, command):
        assert command in self.ANDORADDSUB_TO_OPERATOR.keys(
            ), 'unknown command'

        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('D=M\n')
        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop and operate.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('M=M{}D\n'.format(
            self.ANDORADDSUB_TO_OPERATOR[command]))
        # Increment SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M+1\n')

    def _writeEqLtGt(self, command):
        assert command in self.EQLTGT_TO_JUMP_CODE.keys(
            ), 'unknown command'

        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('D=M\n')
        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop and compare.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('D=M-D\n')
        # If the condition satisfied, then return true (-1).
        self.outFileStream.write('@OUTPUT_TRUE{}\n'.format(self.label_count))
        self.outFileStream.write('D;{}\n'.format(self.EQLTGT_TO_JUMP_CODE[command]))
        # Otherwise, return false (0).
        self.outFileStream.write('D=0\n')
        self.outFileStream.write('@OUTPUT_D{}\n'.format(self.label_count))
        self.outFileStream.write('0;JMP\n')
        self.outFileStream.write('(OUTPUT_TRUE{})\n'.format(self.label_count))
        self.outFileStream.write('D=-1\n')
        self.outFileStream.write('@OUTPUT_D{}\n'.format(self.label_count))
        self.outFileStream.write('0;JMP\n')
        self.outFileStream.write('(OUTPUT_D{})\n'.format(self.label_count))
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('M=D\n')
        # Increment SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M+1\n')
        
        self.label_count += 1

    def _writeNegNot(self, command):
        assert command in self.NEGNOT_TO_OPERATOR.keys(
            ), 'unknown command'

        # Decrement SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M-1\n')
        # Pop and negate.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('A=M\n')
        self.outFileStream.write('M={}M\n'.format(
            self.NEGNOT_TO_OPERATOR[command]))
        # Increment SP.
        self.outFileStream.write('@SP\n')
        self.outFileStream.write('M=M+1\n')

    def writePushPop(self, command, segment, index):
        """command is one of C_PUSH or C_POP.
        """
        self.outFileStream.write('// {} {} {}\n'.format(command, segment, index))
        if command == self.vmParser.COMMAND_TYPES[1]:
            # C_PUSH.
            assert segment in list(self.SEGMENT_NAME_TO_SYMBOL.keys(
                )) + ['constant', 'static'], 'unknown segment'

            # Push to stack segment.
            if segment == 'constant':
                self.outFileStream.write('@{}\n'.format(index))
                self.outFileStream.write('D=A\n')
            elif segment == 'static':
                self.outFileStream.write('@{}.{}\n'.format(
                    self.vmFileName, index))
                self.outFileStream.write('D=M\n')
            elif segment in ('temp', 'pointer'):
                self.outFileStream.write('@{}\n'.format(
                    self.SEGMENT_NAME_TO_SYMBOL[segment]))
                for _ in range(int(index)):
                    self.outFileStream.write('A=A+1\n')
                self.outFileStream.write('D=M\n')
            elif segment in self.SEGMENT_NAME_TO_SYMBOL.keys():
                self.outFileStream.write('@{}\n'.format(
                    self.SEGMENT_NAME_TO_SYMBOL[segment]))
                self.outFileStream.write('A=M\n')
                for _ in range(int(index)):
                    self.outFileStream.write('A=A+1\n')
                self.outFileStream.write('D=M\n')
            else:
                raise AttributeError('unknown segment')

            self.outFileStream.write('@SP\n')
            self.outFileStream.write('A=M\n')
            self.outFileStream.write('M=D\n')

            # Increment SP.
            self.outFileStream.write('@SP\n')
            self.outFileStream.write('M=M+1\n')
        elif command == self.vmParser.COMMAND_TYPES[2]:
            # C_POP.
            assert segment in list(self.SEGMENT_NAME_TO_SYMBOL.keys(
                )) + ['static'], 'unknown segment'

            # Decrement SP.
            self.outFileStream.write('@SP\n')
            self.outFileStream.write('M=M-1\n')
            # Pop.
            self.outFileStream.write('@SP\n')
            self.outFileStream.write('A=M\n')
            self.outFileStream.write('D=M\n')
            # Assign to destination segment.
            if segment in ('temp', 'pointer'):
                self.outFileStream.write('@{}\n'.format(
                    self.SEGMENT_NAME_TO_SYMBOL[segment]))
                for _ in range(int(index)):
                    self.outFileStream.write('A=A+1\n')
                self.outFileStream.write('M=D\n')
            elif segment == 'static':
                self.outFileStream.write('@{}.{}\n'.format(
                    self.vmFileName, index))
                self.outFileStream.write('M=D\n')
            elif segment in self.SEGMENT_NAME_TO_SYMBOL.keys():
                self.outFileStream.write('@{}\n'.format(
                    self.SEGMENT_NAME_TO_SYMBOL[segment]))
                self.outFileStream.write('A=M\n')
                for _ in range(int(index)):
                    self.outFileStream.write('A=A+1\n')
                self.outFileStream.write('M=D\n')
            else:
                raise AttributeError('unknown segment')
        else:
            raise AttributeError('Unsupported command')

    def close(self):
        self.outFileStream.close()

    def __del__(self):
        self.close()