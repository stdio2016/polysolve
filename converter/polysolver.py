import argparse
import re
import json
import itertools
import sys

import gridtypes

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('--input', '-i', type=str, help='PolySolver file name')
arg_parser.add_argument('--output', '-o', type=str, help='output file name')
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

def parse_coord(scanner, oriented=False, old=False):
    x = int(scanner.next())
    y = int(scanner.next())
    nxt = scanner.next()
    z = 0
    if nxt != ')' and nxt != ':' and nxt != '|' and nxt != ';':
        z = int(nxt)
        nxt = scanner.next()
    w = 0
    if nxt != ')' and nxt != '|' and nxt != ';':
        if nxt == ':': nxt = scanner.next()
        w = int(nxt)
        nxt = scanner.next()
    if oriented:
        ori = 0
        if nxt == '|':
            ori = int(scanner.next())
            nxt = scanner.next()
    if old:
        scanner.back()
    elif nxt != ')':
        raise SyntaxError('Missing ) at end of coord')
    if oriented: return (x, y, z, w, ori)
    return (x, y, z, w)

def parse_solution(scanner, shapes, shapeId, which='solution'):
    nxt = scanner.next()
    if nxt != '(': raise SyntaxError('Missing ( at ' + which)
    nxt = scanner.next()
    ans = []
    while nxt != ')':
        id = int(nxt)
        id = shapeId[id]
        nxt = scanner.next()
        if nxt != '(': raise SyntaxError('Missing ( at coord')
        coordori = parse_coord(scanner, oriented=True)
        coord = coordori[:-1]
        ori = coordori[-1]
        shape = shapes[id-1]
        obj = [id, coord, *shape['morphId'][ori]]
        nxt = scanner.next()
        ans.append(obj)
    return ans

def convert_coord(coord, dim, tileN, old):
    if old:
        if tileN > 1:
            return (coord[dim],) + coord[:dim]
        else:
            return coord[:dim]
    return (coord[3],) + coord[:dim] if tileN > 1 else coord[:dim]

def compact_coord(coord):
    return coord

def compact_coord2(coord):
    return coord[:dim]

if args.input:
    ff = open(args.input, 'r')
else:
    ff = sys.stdin
if args.output:
    fout = open(args.output, 'w')
else:
    fout = sys.stdout
if True:
    fout.write('{\n')
    if not args.old:
        description = ff.readline()
        if description[-1] == '\n':
            description = description[:-1]
    fout.write('  "description": ' + json.dumps(description) + ',\n')

    text = ff.read()
    scanner = Tokenizer(text)

    grid = scanner.next().lower()
    if grid == 'board' or grid == 'tile':
        grid = 'square'
        scanner.back()
    if not grid in gridtypes.grids:
        raise NotImplementedError('Grid type "{}" is not yet supported'.format(grid))
    fout.write('  "grid": ' + json.dumps(grid) + ',\n')
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
            if args.old:
                scanner.back()
                coords.append(parse_coord(scanner, old=True))
                fun = scanner.next()
                while fun == ';':
                    coords.append(parse_coord(scanner, old=True))
                    fun = scanner.next()
            else:
                while fun == '(':
                    coords.append(parse_coord(scanner))
                    fun = scanner.next()
            if fun != ')':
                raise SyntaxError('Missing ) at end of board')
            xs,ys,zs,ws = coords[0]
            if args.old:
                xs,ys,zs,ws = max(0,xs-1),max(0,ys-1),max(0,zs-1),max(0,ws-1)
            board['size'] = [xs+1,ys+1,zs+1][:dim]
            cells = set(itertools.product(
                    range(xs+1), range(ys+1), range(zs+1), range(ws+1)))
            for xyzw in coords[1:]:
                if xyzw in cells:
                    cells.remove(xyzw)
                else:
                    cells.add(xyzw)
            board['coords'] = [convert_coord(c, dim, tileN, args.old) for c in cells]
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
            min_amount = 1 if args.old else 0
            if fun != '(':
                max_amount = int(fun)
                fun = scanner.next()
                if args.old:
                    min_amount = max_amount
                elif fun != '(':
                    min_amount = int(fun)
                    fun = scanner.next()
            shape['amount'] = {"max": max_amount, "min": min_amount}
            if fun != '(':
                raise SyntaxError('Missing ( at tile')
            fun = scanner.next()
            morph = [[]]
            # old version uses ; to separate orientations, new version uses +
            sep = ';' if args.old else '+'
            while fun == '(' or fun == sep:
                if fun == sep:
                    morph.append([])
                    fun = scanner.next()
                if fun == '(':
                    morph[-1].append(convert_coord(parse_coord(scanner), dim, tileN, args.old))
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
        
    fout.write('  "board": {\n')
    fout.write('    "size": %s,\n' % board['size'])
    fout.write('    "coords": %s\n' % json.dumps([compact_coord(c) for c in board['coords']]))
    fout.write('  },\n')
    fout.write('  "shapes": [\n')
    i = 0
    for sh in shapes:
        i += 1
        fout.write('    {\n')
        fout.write('      "mobility": "%s",\n' % sh['mobility'])
        fout.write('      "amount": %s,\n' % json.dumps(sh['amount']))
        if 'color' in sh:
            fout.write('      "color": %s,\n' % json.dumps(sh['color']))
        fout.write('      "morphs": [\n')
        j = len(sh['morphs'])
        for mo in sh['morphs']:
            j -= 1
            fout.write('        {"coords": %s}%s\n' % (
                json.dumps([compact_coord(c) for c in mo]),
                '' if j == 0 else ','
            ))
        fout.write('      ]\n')
        fout.write('    }'+('' if i==len(shapes) else ',')+'\n')
    fout.write('  ],\n')

    # build shape id mapping for place/solution
    shapeId = [0]
    for i, s in enumerate(shapes):
        for _i in range(s['amount']['max']):
            shapeId.append(i+1)
    # build orient id mapping for place/solution
    oriCount = gridtypes.total_rotations(grid)
    for s in shapes:
        mm = {}
        morphId = []
        oriId = 0
        if s['mobility'] == 'translate':
            myOriCount = 1
        elif s['mobility'] == 'rotate':
            myOriCount = oriCount
        else:
            myOriCount = oriCount * 2
        for i, m in enumerate(s['morphs']):
            for ori in range(myOriCount):
                rotated = tuple(gridtypes.rotate_all(grid, m, ori))
                if rotated not in mm:
                    oriId += 1
                    mm[rotated] = oriId
                    morphId.append((i+1, ori))
        s['morphId'] = morphId

    # place
    placement = []
    if fun == 'place':
        placement0 = parse_solution(scanner, shapes, shapeId, which='place')
        for pla in placement0:
            id, pos, morph, ori = pla
            pos = compact_coord(convert_coord(pos, dim, tileN, args.old))
            placement.append({'id':id, 'position':pos, 'morph':morph, 'orientation':ori})
        fun = scanner.next()
    fout.write('  "placement": [\n')
    for i, p in enumerate(placement):
        fout.write('    ' + json.dumps(p) + (',' if i < len(placement)-1 else '') + '\n')
    fout.write('  ],\n')

    # mode
    mode = ''
    if fun == 'mode':
        mode = scanner.next()
        fun = scanner.next()
    fout.write('  "mode": %s,\n' % (json.dumps(mode)))

    # solution
    solutions = []
    while fun == 'solution':
        sol = parse_solution(scanner, shapes, shapeId)
        for pla in sol:
            pla[1] = compact_coord(convert_coord(pla[1], dim, tileN, args.old))
        solutions.append(sol)
        fun = scanner.next()
    fout.write('  "solutions": [\n')
    for i, p in enumerate(solutions):
        fout.write('    ' + json.dumps(p) + (',' if i < len(solutions)-1 else '') + '\n')
    fout.write('  ],\n')
    
    # notes
    notes = ""
    if fun == 'notes':
        notes = scanner.next()
        # decode notes
        txt = []
        for i in range(len(notes) // 2):
            lo = (ord(notes[i*2+1])-1) & 15
            hi = (ord(notes[i*2])-1) & 15
            txt.append(chr(hi*16 + lo))
        notes = "".join(txt)
    fout.write('  "notes": %s\n' % (json.dumps(notes)))
    fout.write('}\n')
if args.input:
    ff.close()
if args.output:
    fout.close()
