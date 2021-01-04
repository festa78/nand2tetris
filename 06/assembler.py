#!/usr/bin/python3 -B

import argparse
import os

import asmparser
import code
import symbol_table

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("asmpath", help="Path to .asm file to compile",
                        type=str)
    args = parser.parse_args()

    if not args.asmpath.endswith('.asm'):
        raise ValueError('Not an .asm file but {}'.format(args.asmpath))

    file_basename = os.path.splitext(args.asmpath)[0]

    print('Resolve symbols.')
    print('First path: make a symbol table for labels.')
    asm_symbol_table = symbol_table.SymbolTable()
    asmparse = asmparser.Parser(args.asmpath)
    address_num = 0
    while(True):
        if asmparse.commandType() == asmparse.COMMAND_TYPES[2]:
            # L_COMMAND.
            l_command_symbol = asmparse.symbol()
            if not asm_symbol_table.contains(l_command_symbol):
                asm_symbol_table.addEntry(l_command_symbol, address_num)
                asmparse.advance()
                continue

        if not asmparse.hasMoreCommands():
            break
        asmparse.advance()
        address_num += 1

    print('Second path: make a symbol table for variables and',
        'convert symbols to addresses.')
    asmparse = asmparser.Parser(args.asmpath)
    variable_address = 16
    with open(file_basename + '_symless.asm', 'w') as outfile:
        while(True):
            if asmparse.commandType() == asmparse.COMMAND_TYPES[2]:
                # L_COMMAND. Already processed thus can skip.
                l_command_symbol = asmparse.symbol()
                print(asmparse.commands[asmparse.index], 'L_COMMAND (skip)', l_command_symbol)
                assert asm_symbol_table.contains(l_command_symbol), 'unknown label symbol'
            elif asmparse.commandType() == asmparse.COMMAND_TYPES[0]:
                # A_COMMAND.
                a_command_symbol = asmparse.symbol()
                if a_command_symbol.isdigit():
                    converted_command = '@' + a_command_symbol
                else:
                    if not asm_symbol_table.contains(a_command_symbol):
                        asm_symbol_table.addEntry(a_command_symbol, variable_address)
                        variable_address += 1
                    converted_command = '@' + str(asm_symbol_table.getAddress(a_command_symbol))

                print(asmparse.commands[asmparse.index], 'A_COMMAND', converted_command)
                outfile.write(converted_command + '\n')
            else:
                # C_COMMAND, process as is.
                converted_command = asmparse.commands[asmparse.index]
                print(asmparse.commands[asmparse.index], 'C_COMMAND', converted_command)
                outfile.write(converted_command + '\n')

            if not asmparse.hasMoreCommands():
                break
            asmparse.advance()

    print('Compile no symbol asm file.')
    asmparse = asmparser.Parser(file_basename + '_symless.asm')
    with open(file_basename + '.hack', 'w') as outfile:
        while(True):
            if asmparse.commandType() == asmparse.COMMAND_TYPES[0]:
                # A_COMMAND.
                a_command_binary = bin(int(asmparse.symbol()))
                binary = '{}\n'.format(a_command_binary[2:].zfill(16))
                outfile.write(binary)
            elif asmparse.commandType() == asmparse.COMMAND_TYPES[1]:
                # C_COMMAND.
                comp = code.comp(asmparse.comp())
                dest = code.dest(asmparse.dest())
                jump = code.jump(asmparse.jump())
                binary = '111{}{}{}\n'.format(comp, dest, jump)
                outfile.write(binary)
            else:
                raise ValueError('command type not supported')

            print(asmparse.commands[asmparse.index], binary)

            if not asmparse.hasMoreCommands():
                break
            asmparse.advance()