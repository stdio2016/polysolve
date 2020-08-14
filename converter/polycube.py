import argparse
import sys
import json
from collections import OrderedDict

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('-i', '--input', type=str, help='Polycube file name')
arg_parser.add_argument('-o', '--output', type=str, help='Output file name')

args = arg_parser.parse_args()

fin = sys.stdin
if args.input:
    fin = open(args.input, "r")

fout = sys.stdout
if args.output:
    fout = open(args.output, "w")

puzzleDef = None
pieces = None
lineno = 0
cury = 0
puzzleSetting = {"xDim", "yDim", "zDim", "oneSide"}
pieceSetting = {"name", "type", "layout"}
mode = ''
stationary = []
newdefined = []

def parseSettings(settings, names):
    out = {}
    for opt in settings:
        opt = [x.strip() for x in opt.split('=')]
        optname = opt[0]
        if optname == "":
            raise RuntimeError("Missing setting name".format())
        elif optname not in names:
            raise RuntimeError("Unknown setting '{}'.\nValid settings are: {}".format(optname, names))
        elif len(opt) > 2:
            raise RuntimeError("Multiple equal sign in setting '{}'".format(optname))
        elif optname in out:
            raise RuntimeError("Repeated setting '{}'".format(optname))
        elif len(opt) <= 1:
            if optname == "oneSide":
                out['oneSide'] = True
            else:
                raise RuntimeError("Setting '{}' requires a value.".format(optname))
        else:
            out[optname] = opt[1]
    return out

def parseD(opts):
    settings = parseSettings(opts, puzzleSetting)
    puzzleDef = {}
    for optname in settings:
        puzzleDef[optname] = int(settings[optname])
        if optname == "oneSide":
            if puzzleDef[optname] not in range(2):
                raise RuntimeError("Setting 'oneSide' can be either 0 or 1")
            puzzleDef[optname] = int(settings[optname]) == 1
    if 'xDim' not in puzzleDef:
        raise RuntimeError("Setting 'xDim' is required")
    if 'yDim' not in puzzleDef:
        raise RuntimeError("Setting 'yDim' is required")
    if 'zDim' not in puzzleDef:
        puzzleDef['zDim'] = 1
    if 'oneSide' not in puzzleDef:
        puzzleDef['oneSide'] = False
    if puzzleDef['xDim'] <= 0:
        raise RuntimeError('Puzzle x dimension must be > 0')
    if puzzleDef['yDim'] <= 0:
        raise RuntimeError('Puzzle y dimension must be > 0')
    if puzzleDef['zDim'] <= 0:
        raise RuntimeError('Puzzle z dimension must be > 0')
    if puzzleDef['zDim'] > 1 and puzzleDef['oneSide']:
        raise RuntimeError('Only 2D puzzle (zDim=1) can have one sided pieces')
    return puzzleDef

def parseC(opts, pieces):
    settings = parseSettings(opts, pieceSetting)
    piece = {
        'defined': True,
        'stationary': False,
        'layout': False,
        'coords': []
    }
    if 'name' not in settings or settings['name'] == "":
        raise RuntimeError("Setting 'name' is required")
    name = settings['name']
    if name in pieces:
        raise RuntimeError("Piece '{}' redefined".format(name))
    if 'type' in settings:
        if settings['type'] == 'M':
            piece['stationary'] = False
        elif settings['type'] == 'S':
            piece['stationary'] = True
        else:
            raise RuntimeError("Piece type must be either 'M' (Mobile) or 'S' (Stationary)")
    if 'layout' in settings and settings['layout'] != "":
        coords = settings['layout'].split(',')
        coordset = set()
        for i in range(len(coords)):
            coo = tuple([int(x) for x in coords[i].split(' ') if x])
            if len(coo) != 3:
                raise RuntimeError("Layout must be a list of x y z triplets")
            if coo in coordset:
                raise RuntimeError("Coord {} occured twice".format(coo))
            coordset.add(coo)
            coords[i] = coo
        piece['coords'] = coords
    else:
        raise RuntimeError("Setting 'layout' is required and cannot be empty")
    return name, piece

def writeJSON(puzzleDef, pieces, fout):
    movable = OrderedDict()
    fout.write('{\n')
    if puzzleDef['oneSide']:
        fout.write('  "grid": "square",\n')
    else:
        fout.write('  "grid": "cube",\n')
    fout.write('  "board": {\n    "tiles": [\n')
    if puzzleDef['oneSide']:
        for i in range(puzzleDef['yDim']):
            arr = [1] * puzzleDef['xDim']
            fout.write('      %s' % arr)
            if i < puzzleDef['yDim']-1: fout.write(',')
            fout.write('\n')
    else:
        for i in range(puzzleDef['zDim']):
            arr = [[1] * puzzleDef['xDim'] for y in range(puzzleDef['yDim'])]
            fout.write('      %s' % arr)
            if i < puzzleDef['zDim']-1: fout.write(',')
            fout.write('\n')
    fout.write('    ]\n  },\n')
    for name in pieces:
        piece = pieces[name]
        if piece['stationary']:
            piece['name'] = name
            movable[name] = piece
        else:
            x0,y0,z0 = piece['coords'][0]
            x1,y1,z1 = piece['coords'][0]
            for x,y,z in piece['coords']:
                x0 = min(x0, x)
                y0 = min(y0, y)
                z0 = min(z0, z)
                x1 = max(x1, x)
                y1 = max(y1, y)
                z1 = max(z1, z)
            piece['range'] = (x0,y0,z0,x1,y1,z1)
            normalized = []
            for x,y,z in piece['coords']:
                normalized.append((x-x0,y-y0,z-z0))
            normalized.sort()
            normalized = tuple(normalized)
            if normalized in movable:
                movable[normalized]['name'].append(name)
            else:
                piece['name'] = [name]
                movable[normalized] = piece
    fout.write('  "shapes": [\n')
    ide = 0
    for piece in movable.values():
        ide += 1
        fout.write('    {\n')
        fout.write('      "name": %s,\n' % json.dumps(piece['name']))
        mobility = 'stationary' if piece['stationary'] else 'rotate'
        fout.write('      "mobility": "%s",\n' % (mobility))
        if len(piece['name']) > 1:
            fout.write('      "amount": %d,\n' % len(piece['name']))
        fout.write('      "morphs": [{\n')
        if piece['layout'] and not piece['stationary']:
            x0,y0,z0,x1,y1,z1 = piece['range']
            arr = []
            for i in range(z1-z0+1):
                arr.append([])
                for j in range(y1-y0+1):
                    arr[i].append([0] * (x1-x0+1))
            for x,y,z in piece['coords']:
                arr[z-z0][y-y0][x-x0] = 1
            fout.write(' '*8+'"tiles": [\n')
            if puzzleDef['oneSide']:
                for y in range(y1-y0+1):
                    fout.write(' '*10 + json.dumps(arr[0][y]))
                    if y != y1-y0: fout.write(',')
                    fout.write('\n')
            else:
                for z in range(z1-z0+1):
                    fout.write(' '*10 + json.dumps(arr[z]))
                    if z != z1-z0: fout.write(',')
                    fout.write('\n')
            fout.write(' '*8+']\n')
        else:
            if puzzleDef['oneSide']:
                coords = [x[0:2] for x in piece['coords']]
            else:
                coords = piece['coords']
            fout.write(' '*8+'"coords": {}\n'.format(json.dumps(coords)))
        fout.write('      }]\n')
        fout.write('    }')
        if ide != len(movable): fout.write(',')
        fout.write('\n')
    fout.write("  ]\n}\n")

try:
    while True:
        line = fin.readline()
        lineno += 1
        # reach end of file
        if not line:
            break
        # strip whitespaces
        line = line.strip()
        # strip comment
        pos = line.find("#")
        if pos != -1:
            s = line[:pos]
        else:
            s = line
        if s == "":
            continue
        
        d = [x.strip() for x in s.split(":")]
        if mode == 'L':
            cury -= 1
            pcs = [[y for y in x.split(' ') if y] for x in s.split(',')]
            if pcs[0][0] == '~L':
                if len(stationary) > 0:
                    raise RuntimeError("Pieces '{}' do not occur in layout".format(stationary))
                for nd in newdefined:
                    pieces[nd]['defined'] = True
                mode = ''
            else:
                for curz, row in enumerate(pcs):
                    for curx, pc in enumerate(row):
                        if pc == '.':
                            # dot represents a space, not a piece
                            continue
                        if pc not in pieces:
                            pieces[pc] = {
                                'defined': False,
                                'stationary': False,
                                'layout': True,
                                'coords': []
                            }
                            newdefined.append(pc)
                        elif pieces[pc]['defined']:
                            raise RuntimeError("Piece '{}' redefined".format(pc))
                        if pc in stationary:
                            pieces[pc]['stationary'] = True
                            stationary.remove(pc)
                        pieces[pc]['coords'].append((curx, cury, curz))
        elif d[0] == "D":
            if puzzleDef is not None:
                raise RuntimeError("Multiple puzzle definition")
            puzzleDef = parseD(d[1:])
            pieces = OrderedDict()
        elif d[0] == "C":
            if puzzleDef is None:
                raise RuntimeError("The first puzzle directive must be D")
            name, piece = parseC(d[1:], pieces)
            pieces[name] = piece
        elif d[0] == "L":
            if puzzleDef is None:
                raise RuntimeError("The first puzzle directive must be D")
            cury = puzzleDef['yDim']
            settings = parseSettings(d[1:], {"stationary"})
            newdefined = []
            if 'stationary' in settings:
                stationary = {x for x in settings['stationary'].split(' ') if x}
                for piece in pieces:
                    if piece in stationary:
                        raise RuntimeError("Piece '{}' redefined".format(piece))
            mode = 'L'
        elif d[0] == "~D":
            if puzzleDef is None:
                raise RuntimeError("Closing directive ~D occured before D directive")
            writeJSON(puzzleDef, pieces, fout)
            puzzleDef = None
            pieces = None
        else:
            raise RuntimeError("Unknown directive type: '{}'".format(d[0]))
    if puzzleDef:
        raise RuntimeError('Unexpected end of file')
except (RuntimeError,ValueError) as x:
    print("<Error> found at line {}: {}".format(lineno, x), file=sys.stderr)

if args.input:
    fin.close()

if args.output:
    fout.close()
