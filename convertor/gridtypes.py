grids = {
    'square': {
        'dimension': 2,
        'orbits': [0],
        'rotation': [
            {'order': 4, 'transform': [
                [0, [0, 0, 1], [0, -1, 0]]
            ]}
        ],
        'reflection': [
            [0, [0, -1, 0], [0, 0, 1]]
        ]
    },
    'cube': {
        'dimension': 3,
        'orbits': [0],
        'rotation': [
            {'order': 4, 'transform': [
                [0, [0, 0, 1, 0], [0, -1, 0, 0], [0, 0, 0, 1]]
            ]},
            {'order': 2, 'transform': [
                [0, [0, 1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]]
            ]},
            {'order': 3, 'transform': [
                [0, [0, 0, 0, 1], [0, 1, 0, 0], [0, 0, 1, 0]]
            ]}
        ],
        'reflection': [
            [0, [0, -1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]
        ]
    },
    'triangle': {
        'dimension': 2,
        'orbits': [0, 0],
        'rotation': [
            {'order': 6, 'transform': [
                [1, [-1, 1, -1], [0, 1, 0]],
                [0, [0, 1, -1], [0, 1, 0]]
            ]}
        ],
        'reflection': [
            [0, [0, -1, 1], [0, 0, 1]],
            [1, [-1, -1, 1], [0, 0, 1]]
        ]
    },
    'hexagon': {
        'dimension': 2,
        'orbits': [0],
        'rotation': [
            {'order': 6, 'transform': [
                [0, [0, 1, -1], [0, 1, 0]]
            ]}
        ],
        'reflection': [
            [0, [0, 0, 1], [0, 1, 0]]
        ]
    }
}

def rotate(grid, coord, ori):
    dim = grid['dimension']
    orbits = len(grid['orbits'])
    steps = []
    for r in grid['rotation']:
        order = r['order']
        steps.append(ori % order)
        ori = ori // order
    for i in range(len(steps)-1, -1, -1):
        for t in range(steps[i]):
            how = grid['rotation'][i]['transform']
            how = how[coord[dim]]
            new_coord = []
            for j in range(dim):
                x = how[j+1][0]
                for k in range(dim):
                    x += how[j+1][k+1] * coord[k]
                new_coord.append(x)
            new_coord.append(how[0])
            coord = new_coord
    if ori == 1:
        how = grid['reflection'][i]
        how = how[coord[dim]]
        new_coord = []
        for j in range(dim):
            x = how[j+1][0]
            for k in range(dim):
                x += how[j+1][k+1] * coord[k]
            new_coord.append(x)
        new_coord.append(how[0])
        coord = new_coord
    return coord

def total_rotations(grid):
    total = 1
    for r in grid['rotation']:
        total *= r['order']
    return total
