class Parser:
    COMMAND_TYPES = (
        'C_ARITHMETIC',
        'C_PUSH',
        'C_POP',
        'C_LABEL',
        'C_GOTO',
        'C_IF',
        'C_FUNCTION',
        'C_RETURN',
        'C_CALL',
    )

    COMMAND_TO_TYPES = {
        'and': COMMAND_TYPES[0],
        'or': COMMAND_TYPES[0],
        'not': COMMAND_TYPES[0],
        'add': COMMAND_TYPES[0],
        'sub': COMMAND_TYPES[0],
        'neg': COMMAND_TYPES[0],
        'eq': COMMAND_TYPES[0],
        'lt': COMMAND_TYPES[0],
        'gt': COMMAND_TYPES[0],
        'push': COMMAND_TYPES[1],
        'pop': COMMAND_TYPES[2],
        'label': COMMAND_TYPES[3],
        'goto': COMMAND_TYPES[4],
        'if-goto': COMMAND_TYPES[5],
    }

    def __init__(self, vmpath):
        if not vmpath.endswith('.vm'):
            raise ValueError('Not an .vm file but {}'.format(vmpath))

        self.commands = []
        with open(vmpath, 'r') as f:
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
        command_name = current_command.split(' ')[0]
        if command_name not in self.COMMAND_TO_TYPES.keys():
            raise AttributeError('Unsupported command')

        return self.COMMAND_TO_TYPES[command_name]

    def arg1(self):
        command_type = self.commandType()
        assert command_type != self.COMMAND_TYPES[7], 'Not for C_RETURN'

        current_command = self.commands[self.index]
        if command_type in self.COMMAND_TYPES[0]:
            # C_ARITHMETIC.
            return current_command.split(' ')[0]

        return current_command.split(' ')[1]

    def arg2(self):
        command_type = self.commandType()
        current_command = self.commands[self.index]
        if command_type == self.COMMAND_TYPES[1]:
            # C_PUSH.
            return current_command.split(' ')[2]
        elif command_type == self.COMMAND_TYPES[2]:
            # C_POP.
            return current_command.split(' ')[2]
        elif command_type == self.COMMAND_TYPES[6]:
            # C_FUNCTION.
            return current_command.split(' ')[2]
        elif command_type == self.COMMAND_TYPES[8]:
            # C_CALL.
            return current_command.split(' ')[2]
        else:
            raise ValueError('Unsupported command type.')