import argparse
import re
import json
import itertools

import gridtypes

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

def convert_coord(coord, dim, tileN=1):
    return (coord[3],) + coord[:dim] if tileN > 1 else coord[:dim]

def compact_coord(coord):
    return coord

def compact_coord2(coord):
    return coord[:dim]

with open(args.file, 'r') as ff:
    print('{')
    if not args.old:
        description = ff.readline()
        if description[-1] == '\n':
            description = description[:-1]
    print('  "description": ' + json.dumps(description) + ',')

    text = ff.read()
    scanner = Tokenizer(text)

    grid = scanner.next()
    if grid == 'board' or grid == 'tile':
        grid = 'square'
        scanner.back()
    if not grid in gridtypes.grids:
        raise NotImplementedError('Grid type "{}" is not yet supported'.format(grid))
    print('  "grid": ' + json.dumps(grid) + ',')
    grid = gridtypes.grids[grid]
    dim = grid['dimension']
    tileN = len(grid['orbits'])
    if len(grid['orbits']) == 1:
        compact_coord = compact_coord2
    
    fun = scanner.next()
    board = {}
    shapes = []
    while fun == 'board' or fun == 'tile':
        if fun == 'board':
            fun = scanner.next()
            if fun != '(': raise SyntaxError('Missing ( at board')
            fun = scanner.next()
            coords = []
            while fun == '(':
                coords.append(parse_coord(scanner))
                fun = scanner.next()
            if fun != ')':
                raise SyntaxError('Missing ) at end of board')
            xs,ys,zs,ws = coords[0]
            board['size'] = [xs+1,ys+1,zs+1][:dim]
            cells = set(itertools.product(
                    range(xs+1), range(ys+1), range(zs+1), range(ws+1)))
            for xyzw in coords[1:]:
                if xyzw in cells:
                    cells.remove(xyzw)
                else:
                    cells.add(xyzw)
            board['coords'] = [convert_coord(c, dim, tileN) for c in cells]
            board['coords'].sort()
        elif fun == 'tile':
            shape = {}
            fun = scanner.next()
            if fun == 'flip':
                type = 'mirror'
            elif fun == 'fixed':
                type = 'translate'
            else:
                type = 'rotate'
                scanner.back()
            shape['mobility'] = type
            
            fun = scanner.next()
            max_amount = 1
            min_amount = 0
            if fun != '(':
                max_amount = int(fun)
                fun = scanner.next()
                if fun != '(':
                    min_amount = int(fun)
                    fun = scanner.next()
            shape['amount'] = {"max": max_amount, "min": min_amount}
            if fun != '(':
                raise SyntaxError('Missing ( at tile')
            fun = scanner.next()
            morph = [[]]
            while fun == '(' or fun == '+':
                if fun == '+':
                    morph.append([])
                    fun = scanner.next()
                if fun == '(':
                    morph[-1].append(convert_coord(parse_coord(scanner), dim, tileN))
                    fun = scanner.next()
            if fun != ')':
                raise SyntaxError('Missing ) at end of tile')
            shape['morphs'] = morph
            fun = scanner.next()
            if fun == '(':
                shape['color'] = int(scanner.next()), int(scanner.next()), int(scanner.next())
                if scanner.next() != ')':
                    raise SyntaxError('Missing ) at end of tile color')
            else:
                scanner.back()
            shapes.append(shape)
        fun = scanner.next()
    
    print('  "board": {')
    print('    "size": %s,' % board['size'])
    print('    "coords": %s' % json.dumps([compact_coord(c) for c in board['coords']]))
    print('  },')
    print('  "shapes": [')
    i = 0
    for sh in shapes:
        i += 1
        print('    {')
        print('      "mobility": "%s",' % sh['mobility'])
        print('      "amount": %s,' % json.dumps(sh['amount']))
        if 'color' in sh:
            print('      "color": %s,' % json.dumps(sh['color']))
        print('      "morphs": [')
        j = len(sh['morphs'])
        for mo in sh['morphs']:
            j -= 1
            print('        {"coords": %s}%s' % (
                json.dumps([compact_coord(c) for c in mo]),
                '' if j == 0 else ','
            ))
        print('      ]')
        print('    }'+('' if i==len(shapes) else ','))
    print('  ],')
    print('  "placements": []')
    print('}')
