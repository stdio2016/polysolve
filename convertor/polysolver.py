import argparse
import re

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('file', type=str, help='PolySolver file name')
arg_parser.add_argument('-old', action='store_true', help='use this flag if the file is created in version 1.0')

args = arg_parser.parse_args()

description = ""

class Tokenizer(object):
    def __init__(self, text):
        super(Tokenizer, self).__init__()
        token_pattern = r"(\#.*)|([\x00-\x20,]+)|([-A-Za-z0-9^_'.\\/]+|.)"
        token_re = re.compile(token_pattern, re.VERBOSE)
        self.scanner = token_re.scanner(text)
        self.prev = None
        self.current = None
    def next(self):
        if self.prev is not None:
            self.prev, self.current = None, self.current
            return self.current
        while True:
            m = self.scanner.match()
            if not m:
                self.current = None
                return
            if m.lastindex == 3:
                self.current = m.group(3).lower()
                return self.current
    def back(self):
        self.prev = self.current

def parse_coord(scanner, oriented=False):
    x = int(scanner.next())
    y = int(scanner.next())
    nxt = scanner.next()
    z = 0
    if nxt != ')' and nxt != ':' and nxt != '|':
        z = int(nxt)
        nxt = scanner.next()
    w = 0
    if nxt != ')' and nxt != '|':
        if nxt == ':': nxt = scanner.next()
        w = int(nxt)
        nxt = scanner.next()
    if oriented:
        ori = 0
        if nxt == '|':
            ori = int(scanner.next())
            nxt = scanner.next()
    if nxt != ')':
        raise SyntaxError('Missing ) at end of coord')
    if oriented: return (x, y, z, w, ori)
    return (x, y, z, w)

with open(args.file, 'r') as ff:
    if not args.old:
        description = ff.readline()
        if description[-1] == '\n':
            description = description[:-1]

    text = ff.read()
    scanner = Tokenizer(text)

    print('description = {}'.format(description))
    grid = scanner.next()
    print('grid = {}'.format(grid))
    if args.old and (grid == 'board' or grid == 'tile'):
        grid = 'square'
        scanner.back()
    if not grid in ['square', 'cube']:
        raise NotImplementedError('Grid type "{}" is not yet supported'.format(grid))
    
    fun = scanner.next()
    while fun == 'board' or fun == 'tile':
        if fun == 'board':
            print('board')
            fun = scanner.next()
            if fun != '(': raise SyntaxError('Missing ( at board')
            fun = scanner.next()
            coords = []
            while fun == '(':
                coords.append(parse_coord(scanner))
                fun = scanner.next()
            if fun != ')':
                raise SyntaxError('Missing ) at end of board')
            print('  size = {}'.format(coords[0]))
            print('  voids = {}'.format(coords[1:]))
        elif fun == 'tile':
            print('shape')
            fun = scanner.next()
            if fun == 'flip':
                type = 'mirror'
            elif fun == 'fixed':
                type = 'fixed'
            else:
                type = 'rotate'
                scanner.back()
            print("  type = {}".format(type))
            fun = scanner.next()
            max_amount = 1
            min_amount = 0
            if fun != '(':
                max_amount = int(fun)
                fun = scanner.next()
                if fun != '(':
                    min_amount = int(fun)
                    fun = scanner.next()
            if fun != '(':
                raise SyntaxError('Missing ( at tile')
            fun = scanner.next()
            morph = [[]]
            while fun == '(' or fun == '+':
                if fun == '+':
                    morph.append([])
                    fun = scanner.next()
                if fun == '(':
                    morph[-1].append(parse_coord(scanner))
                    fun = scanner.next()
            if fun != ')':
                raise SyntaxError('Missing ) at end of tile')
            for m in morph:
                print("  morph = {}".format(m))
            fun = scanner.next()
            rgb = None
            if fun == '(':
                rgb = int(scanner.next()), int(scanner.next()), int(scanner.next())
                if scanner.next() != ')':
                    raise SyntaxError('Missing ) at end of tile color')
            else:
                scanner.back()
            print("  color = {}".format(rgb))
        fun = scanner.next()
