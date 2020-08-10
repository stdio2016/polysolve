import argparse
import sys

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
puzzleSetting = {"xDim", "yDim", "zDim", "oneSide"}

def parseD(settings):
    puzzleDef = {}
    for opt in settings:
        opt = [x.strip() for x in opt.split('=')]
        optname = opt[0]
        if optname == "":
            raise RuntimeError("Missing setting name".format())
        elif optname not in puzzleSetting:
            raise RuntimeError('Unknown setting "{}".\nValid settings are: {}'.format(optname, puzzleSetting))
        elif len(opt) > 2:
            raise RuntimeError('Multiple equal sign in setting "{}"'.format(optname))
        elif optname in puzzleDef:
            raise RuntimeError('Repeated setting "{}"'.format(optname))
        elif len(opt) <= 1:
            if optname == "oneSide":
                puzzleDef['oneSide'] = True
            else:
                raise RuntimeError('Setting "{}" requires a value.'.format(optname))
        else:
            puzzleDef[optname] = int(opt[1])
            if optname == "oneSide":
                if puzzleDef[optname] not in range(2):
                    raise RuntimeError('Setting "oneSide" can be either 0 or 1')
                puzzleDef[optname] = int(opt[1]) == 1
    if 'xDim' not in puzzleDef:
        raise RuntimeError('Setting "xDim" is required')
    if 'yDim' not in puzzleDef:
        raise RuntimeError('Setting "yDim" is required')
    if 'zDim' not in puzzleDef:
        puzzleDef['zDim'] = 1
    if 'oneSide' not in puzzleDef:
        puzzleDef['oneSide'] = False
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
        if d[0] == "D":
            if puzzleDef is not None:
                raise RuntimeError("Multiple puzzle definition")
            puzzleDef = parseD(d[1:])
            print(puzzleDef)
        elif d[0] == "C":
            if puzzleDef is None:
                raise RuntimeError("The first puzzle directive must be D")
            print(d)
        elif d[0] == "L":
            if puzzleDef is None:
                raise RuntimeError("The first puzzle directive must be D")
            print(d)
        elif d[0] == "~D":
            if puzzleDef is None:
                raise RuntimeError("Closing directive ~D occured before D directive")
            puzzleDef = None
        else:
            raise RuntimeError('Unknown directive type: "{}"'.format(d[0]))
except (RuntimeError,ValueError) as x:
    print("<Error> found at line {}: {}".format(lineno, x), file=sys.stderr)

if args.input:
    fin.close()

if args.output:
    fout.close()
