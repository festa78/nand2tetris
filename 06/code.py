# d1, d2, d3
DEST_CODE = {
    'null': [0, 0, 0],
    'M': [0, 0, 1],
    'D': [0, 1, 0],
    'MD': [0, 1, 1],
    'A': [1, 0, 0],
    'AM': [1, 0, 1],
    'AD': [1, 1, 0],
    'AMD': [1, 1, 1],
}

# a, c1, c2, c3, c4, c5, c6
COMP_CODE = {
    '0': [0, 1, 0, 1, 0, 1, 0],
    '1': [0, 1, 1, 1, 1, 1, 1],
    '-1': [0, 1, 1, 1, 0, 1, 0],
    'D': [0, 0, 0, 1, 1, 0, 0],
    'A': [0, 1, 1, 0, 0, 0, 0],
    '!D': [0, 0, 0, 1, 1, 0, 1],
    '!A': [0, 1, 1, 0, 0, 0, 1],
    '-D': [0, 0, 0, 1, 1, 1, 1],
    '-A': [0, 1, 1, 0, 0, 1, 1],
    'D+1': [0, 0, 1, 1, 1, 1, 1],
    'A+1': [0, 1, 1, 0, 1, 1, 1],
    'D-1': [0, 0, 0, 1, 1, 1, 0],
    'A-1': [0, 1, 1, 0, 0, 1, 0],
    'D+A': [0, 0, 0, 0, 0, 1, 0],
    'D-A': [0, 0, 1, 0, 0, 1, 1],
    'A-D': [0, 0, 0, 0, 1, 1, 1],
    'D&A': [0, 0, 0, 0, 0, 0, 0],
    'D|A': [0, 0, 1, 0, 1, 0, 1],
    'M': [1, 1, 1, 0, 0, 0, 0],
    '!M': [1, 1, 1, 0, 0, 0, 1],
    '-M': [1, 1, 1, 0, 0, 1, 1],
    'M+1': [1, 1, 1, 0, 1, 1, 1],
    'M-1': [1, 1, 1, 0, 0, 1, 0],
    'D+M': [1, 0, 0, 0, 0, 1, 0],
    'D-M': [1, 0, 1, 0, 0, 1, 1],
    'M-D': [1, 0, 0, 0, 1, 1, 1],
    'D&M': [1, 0, 0, 0, 0, 0, 0],
    'D|M': [1, 0, 1, 0, 1, 0, 1],
}

# j1, j2, j3
JUMP_CODE = {
    'null': [0, 0, 0],
    'JGT': [0, 0, 1],
    'JEQ': [0, 1, 0],
    'JGE': [0, 1, 1],
    'JLT': [1, 0, 0],
    'JNE': [1, 0, 1],
    'JLE': [1, 1, 0],
    'JMP': [1, 1, 1],
}

def dest(mnemonic):
    """d1, d2, d3
    """
    if mnemonic not in DEST_CODE:
        raise ValueError('Unknown dest code {}'.format(mnemonic))
    return ''.join([str(i) for i in DEST_CODE[mnemonic]])

def comp(mnemonic):
    """a, c1, c2, c3, c4, c5, c6
    """
    if mnemonic not in COMP_CODE:
        raise ValueError('Unknown comp code {}'.format(mnemonic))
    return ''.join([str(i) for i in COMP_CODE[mnemonic]])

def jump(mnemonic):
    """j1, j2, j3
    """
    if mnemonic not in JUMP_CODE:
        raise ValueError('Unknown jump code {}'.format(mnemonic))
    return ''.join([str(i) for i in JUMP_CODE[mnemonic]])