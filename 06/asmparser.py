class Parser:
    COMMAND_TYPES = ('A_COMMAND', 'C_COMMAND', 'L_COMMAND')

    def __init__(self, asmpath):
        if not asmpath.endswith('.asm'):
            raise ValueError('Not an .asm file but {}'.format(asmpath))

        self.commands = []
        with open(asmpath, 'r') as f:
            for line in f.read().splitlines():
                line = line.strip()
                if not line:
                    continue
                if line.startswith('//'):
                    continue
                if '//' in line:
                    line = line.split('//')[0].strip()
                self.commands.append(line)

        self.index = 0
    
    def hasMoreCommands(self):
        return self.index + 1 < len(self.commands)

    def advance(self):
        assert self.hasMoreCommands()
        self.index += 1

    def commandType(self):
        current_command = self.commands[self.index]
        if current_command.startswith('@'):
            # A_COMMAND
            return self.COMMAND_TYPES[0]
        elif current_command.startswith('(') and current_command.endswith(')'):
            # L_COMMAND
            assert len(current_command) > 2, 'empty symbol'
            return self.COMMAND_TYPES[2]
        else:
            # C_COMMAND
            return self.COMMAND_TYPES[1]

    def symbol(self):
        command_type = self.commandType()
        current_command = self.commands[self.index]
        if command_type == self.COMMAND_TYPES[0]:
            # A_COMMAND
            return current_command[1:]
        elif command_type == self.COMMAND_TYPES[2]:
            # L_COMMAND
            return current_command[1:-1]
        else:
            raise AttributeError('Only A_/L_COMMAND are supported')

    def dest(self):
        assert self.commandType() == self.COMMAND_TYPES[1], 'Works only for C_COMMAND'

        current_command = self.commands[self.index]
        if '=' not in current_command:
            # Has no destination.
            return 'null'

        splits = current_command.split('=')
        assert splits[0], 'Empty destination.'

        return splits[0]

    def comp(self):
        assert self.commandType() == self.COMMAND_TYPES[1], 'Works only for C_COMMAND'

        current_command = self.commands[self.index]
        if '=' in current_command:
            current_command = current_command.split('=')[-1]
        if ';' in current_command:
            current_command = current_command.split(';')[0]

        assert current_command, 'Empty computation'
        return current_command

    def jump(self):
        assert self.commandType() == self.COMMAND_TYPES[1], 'Works only for C_COMMAND'

        current_command = self.commands[self.index]
        if ';' not in current_command:
            # Has no jump.
            return 'null'

        splits = current_command.split(';')

        assert splits[-1], 'Empty jump'
        return splits[-1]
