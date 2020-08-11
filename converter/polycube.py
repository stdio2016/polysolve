import argparse
import sys
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
            raise RuntimeError('Unknown setting "{}".\nValid settings are: {}'.format(optname, names))
        elif len(opt) > 2:
            raise RuntimeError('Multiple equal sign in setting "{}"'.format(optname))
        elif optname in out:
            raise RuntimeError('Repeated setting "{}"'.format(optname))
        elif len(opt) <= 1:
            if optname == "oneSide":
                out['oneSide'] = True
            else:
                raise RuntimeError('Setting "{}" requires a value.'.format(optname))
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
                raise RuntimeError('Setting "oneSide" can be either 0 or 1')
            puzzleDef[optname] = int(settings[1]) == 1
    if 'xDim' not in puzzleDef:
        raise RuntimeError('Setting "xDim" is required')
    if 'yDim' not in puzzleDef:
        raise RuntimeError('Setting "yDim" is required')
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
                    raise RuntimeError("Pieces {} do not occur in layout".format(stationary))
                for nd in newdefined:
                    pieces[nd]['defined'] = True
                mode = ''
            else:
                print("y={} {}".format(cury, pcs))
                for curz, row in enumerate(pcs):
                    print(row)
                    for curx, pc in enumerate(row):
                        if pc == '.':
                            # dot represents a space, not a piece
                            continue
                        if pc not in pieces:
                            pieces[pc] = {
                                'defined': False,
                                'stationary': False,
                                'coords': []
                            }
                            newdefined.append(pc)
                        elif pieces[pc]['defined']:
                            raise RuntimeError('Piece "{}" redefined'.format(pc))
                        if pc in stationary:
                            pieces[pc]['stationary'] = True
                            stationary.remove(pc)
                        pieces[pc]['coords'].append((curx, cury, curz))
        elif d[0] == "D":
            if puzzleDef is not None:
                raise RuntimeError("Multiple puzzle definition")
            puzzleDef = parseD(d[1:])
            pieces = OrderedDict()
            print(puzzleDef)
        elif d[0] == "C":
            if puzzleDef is None:
                raise RuntimeError("The first puzzle directive must be D")
            print(d)
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
                        raise RuntimeError("Piece {} redefined".format(piece))
            print(stationary)
            mode = 'L'
        elif d[0] == "~D":
            if puzzleDef is None:
                raise RuntimeError("Closing directive ~D occured before D directive")
            for name in pieces:
                print(name, pieces[name])
            puzzleDef = None
            pieces = None
        else:
            raise RuntimeError('Unknown directive type: "{}"'.format(d[0]))
    if puzzleDef:
        raise RuntimeError('Unexpected end of file')
except (RuntimeError,ValueError) as x:
    print("<Error> found at line {}: {}".format(lineno, x), file=sys.stderr)

if args.input:
    fin.close()

if args.output:
    fout.close()
