import argparse
import gzip
import xml.etree.ElementTree as ET
import re
import sys
import json

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('-i', '--input', type=str, help='Polycube file name')
arg_parser.add_argument('-o', '--output', type=str, help='Output file name')
args = arg_parser.parse_args()

inp = gzip.open(args.input, 'r')
root = ET.fromstring(inp.read().decode('1252'))

fout = sys.stdout
if args.output:
    fout = open(args.output, "w")

def convertGrid(grid, sx, sy, sz, voxel):
    ans = []
    if grid == 0 or grid == 2:
        ans = [[[voxel[x + sx*(y + sy*z)] for x in range(sx)]
                for y in range(sy)] for z in range(sz)]
    else:
        pass
    return ans

def writeGrid(fout, voxel, dim=3, indent=0):
    if dim <= 1:
        fout.write(json.dumps(voxel))
    else:
        fout.write('[\n')
        for i,x in enumerate(voxel):
            fout.write('  ' * (indent+1))
            writeGrid(fout, x, dim-1, indent+1)
            if i < len(voxel)-1: fout.write(',')
            fout.write('\n')
        fout.write('  ' * indent + ']')

def parseVoxel(tt, grid):
    sx,sy,sz = tt.attrib['x'], tt.attrib['y'], tt.attrib['z']
    sx = int(sx)
    sy = int(sy)
    sz = int(sz)
    print('\tvoxel size: %dx%dx%d'%(sx,sy,sz), end='')
    name = ''
    if 'name' in tt.attrib:
        name = tt.attrib['name']
        print(' name: %s' % name, end='')
    print('')
    #print('\t'+tt.text)
    cc = re.compile('[_#+][0-9]*')
    voxel = cc.findall(tt.text)
    ez = ''.join([x[0] for x in voxel])
    zcnt = max(80 // (sx+2), 1)
    curz = 0
    while curz < sz:
        for y in range(sy-1, -1, -1):
            for z in range(curz, min(zcnt+curz, sz)):
                print(ez[sx * (y + sy * z): sx * (1 + y + sy * z)], end='')
                if z < sz-1:
                    print(',', end='' if z==curz+zcnt-1 else ' ')
            print('')
        curz += zcnt
        print('')
    for i in range(sx*sy*sz):
        v = voxel[i]
        if v[0] == '_':
            voxel[i] = 0
        elif v[0] == '#':
            if len(v) == 1: voxel[i] = 1
            else: voxel[i] = "c" + v[1:]
        elif v[0] == '+':
            if len(v) == 2: voxel[i] = 2
            else: voxel[i] = "v" + v[1:]
    voxel = convertGrid(grid, sx, sy, sz, voxel)
    return {'name': name, 'x':sx, 'y':sy, 'z':sz, 'voxel':voxel}

def parseProblem(tt):
    name = ''
    maxHoles = -1
    print('\tproblem', end='')
    if 'name' in tt.attrib:
        name = tt.attrib['name']
        print(' name: %s' % tt.attrib['name'], end='')
    if 'assemblies' in tt.attrib:
        print(' assemblies: %s' % tt.attrib['assemblies'], end='')
    if 'time' in tt.attrib:
        print(' time: %s' % tt.attrib['time'], end='')
    if 'maxHoles' in tt.attrib:
        maxHoles = int(tt.attrib['maxHoles'])
        print(' max holes: %s' % tt.attrib['maxHoles'], end='')
    print('')
    
    shapes = []
    b = tt.find('shapes')
    if b is not None:
        for tu in b.findall('shape'):
            id = tu.attrib['id']
            id = int(id)
            shape = {'id': id, 'min': 1, 'max': 1}
            print('\t\tshape id: %s' % tu.attrib['id'], end='')
            if 'count' in tu.attrib:
                shape['min'] = shape['max'] = int(tu.attrib['count'])
                print(' count: %s' % tu.attrib['count'], end='')
            if 'min' in tu.attrib:
                shape['min'] = int(tu.attrib['min'])
                print(' min: %s' % tu.attrib['min'], end='')
            if 'max' in tu.attrib:
                shape['max'] = int(tu.attrib['max'])
                print(' max: %s' % tu.attrib['max'], end='')
            shapes.append(shape)
            print('')
    
    result = None
    b = tt.find('result')
    if b is not None:
        result = b.attrib['id']
        result = int(result)
        print('\t\tresult: %s' % b.attrib['id'])
    
    bitmap = []
    b = tt.find('bitmap')
    if b is not None:
        for tu in b.findall('pair'):
            pc, rs = tu.attrib['piece'], tu.attrib['result']
            pc = int(pc)
            rs = int(rs)
            bitmap.append((pc, rs))
            print('\t\tbitmap piece: %d result: %d' % (pc, rs))
    
    return {
        'name': name,
        'maxHoles': maxHoles,
        'shapes': shapes,
        'result': result,
        'bitmap': bitmap
    }

def writeProblem(fout, gridType, colors, shapes, problem, comment):
    fout.write('{\n')
    if gridType == 0 or gridType == 2:
        gridname = 'cube'
    else:
        raise RuntimeError('Grid type %d is not supported'%gridType)
    if problem['name']:
        fout.write('  "description": %s,\n' % json.dumps(problem['name']))
    fout.write('  "grid": "%s",\n' % gridname)
    board = problem['result']
    board = shapes[board]
    fout.write('  "board": {\n    "tiles": ')
    writeGrid(fout, board['voxel'], indent=2)
    fout.write('\n  },\n')
    if problem['maxHoles'] > -1:
        fout.write('  "maxHoles": %d,\n', problem['maxHoles'])
    fout.write('  "shapes": [\n')
    for i, shape in enumerate(problem['shapes']):
        voxel = shapes[shape['id']]
        fout.write('    {\n')
        if voxel['name']:
            fout.write('      "name": "%s",\n' % voxel['name'])
        if shape['max'] != 1 or shape['min'] != 1:
            fout.write('      "amount": {"min": %d, "max": %d},\n' % (shape['min'],shape['max']))
        fout.write('      "morphs": [{\n        "tiles": ')
        writeGrid(fout, voxel['voxel'], indent=4)
        fout.write('\n      }]\n    }')
        if i+1 < len(problem['shapes']): fout.write(',')
        fout.write('\n')
    if comment:
        fout.write('  ],\n')
        fout.write('  "notes": %s\n' % json.dumps(comment))
    else:
        fout.write('  ]\n')
    fout.write('}\n')

def parsePuzzle(tree):
    if tree.tag != 'puzzle':
        raise RuntimeError('root node is not puzzle')
    
    a = tree.find('gridType')
    gridType = 0
    if a is not None:
        gridType = int(a.attrib['type'])
        print('Grid type:', a.attrib['type'])
    
    a = tree.find('colors')
    colors = []
    if a is not None:
        print('Colors:')
        for tt in a.findall('color'):
            red,green,blue = tt.attrib['red'], tt.attrib['green'], tt.attrib['blue']
            colors.append({'red': red, 'green': green, 'blue': blue})
            print("\tcolor red: %s green: %s blue: %s" % (red,green,blue))
    
    a = tree.find('shapes')
    shapes = []
    if a is not None:
        print('Shapes:')
        for tt in a.findall('voxel'):
            shapes.append(parseVoxel(tt, gridType))
    
    a = tree.find('problems')
    problems = []
    if a is not None:
        print('Problems:')
        for tt in a.findall('problem'):
            problems.append(parseProblem(tt))
    print(problems)
    
    a = tree.find('comment')
    comment = ""
    if a is not None and a.text:
        comment = a.text
        print('Comment:\n' + a.text)
    
    for problem in problems:
        writeProblem(fout, gridType, colors, shapes, problem, comment)

parsePuzzle(root)

if args.output:
    fout.close()
