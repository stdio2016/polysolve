function pairCmp(a, b) {
    if (a[0] > b[0]) return 1;
    if (a[0] < b[0]) return -1;
    if (a[1] > b[1]) return 1;
    if (a[1] < b[1]) return -1;
    return 0;
}

function rotate(p) {
    var maxx = 0;
    p = p.slice();
    for (var i = 0; i < 5; i++)
        maxx = Math.max(maxx, p[i][0]);
    for (var i = 0; i < 5; i++) {
        p[i] = [p[i][1], maxx-p[i][0]];
    }
    p.sort(pairCmp);
    return p;
}

function mirror(p) {
    var maxx = 0;
    p = p.slice();
    for (var i = 0; i < 5; i++)
        maxx = Math.max(maxx, p[i][0]);
    for (var i = 0; i < 5; i++) {
        p[i] = [maxx-p[i][0], p[i][1]];
    }
    p.sort(pairCmp);
    return p;
}

var pentominos = [
    {name: 'I', coords: [[0,0],[1,0],[2,0],[3,0],[4,0]]},
    {name: 'L', coords: [[0,0],[1,0],[2,0],[3,0],[3,1]]},
    {name: 'L2', coords: [[3,0],[2,0],[1,0],[0,0],[0,1]]},
    {name: 'N', coords: [[0,0],[1,0],[2,0],[2,1],[3,1]]},
    {name: 'N2', coords: [[3,0],[2,0],[1,0],[1,1],[0,1]]},
    {name: 'Y', coords: [[0,0],[1,0],[2,0],[2,1],[3,0]]},
    {name: 'Y2', coords: [[3,0],[2,0],[1,0],[1,1],[0,0]]},
    {name: 'P', coords: [[0,0],[1,0],[1,1],[2,0],[2,1]]},
    {name: 'P2', coords: [[2,0],[1,0],[1,1],[0,0],[0,1]]},
    {name: 'U', coords: [[0,0],[0,1],[1,0],[2,0],[2,1]]},

    {name: 'V', coords: [[0,0],[1,0],[2,0],[2,1],[2,2]]},
    {name: 'W', coords: [[0,0],[1,0],[1,1],[2,1],[2,2]]},
    {name: 'Z', coords: [[0,0],[1,0],[1,1],[1,2],[2,2]]},
    {name: 'Z2', coords: [[2,0],[1,0],[1,1],[1,2],[0,2]]},
    {name: 'T', coords: [[0,0],[1,0],[1,1],[1,2],[2,0]]},
    {name: 'F', coords: [[0,1],[1,0],[1,1],[1,2],[2,0]]},
    {name: 'F2', coords: [[2,1],[1,0],[1,1],[1,2],[0,0]]},
    {name: 'X', coords: [[0,1],[1,0],[1,1],[1,2],[2,1]]}
];

var height = 18, width = 5;
console.log(pentominos.length, height * width);
for (var sh of pentominos) {
    var coord = sh.coords;
    var set = new Map();
    //for (var r = 0; r < 2; r++) {
        for (var i = 0; i < 4; i++) {
            coord = rotate(coord);
            set.set(JSON.stringify(coord), coord);
        }
        coord = mirror(coord);
    //}
    var imgs = [];
    for (var ori of set.values()) {
        var mx = 0, my = 0;
        for (var [ox, oy] of ori) {
            my = Math.max(my, oy);
            mx = Math.max(mx, ox);
        }
        for (var y = 0; y < height - my; y++) {
            for (var x = 0; x < width - mx; x++) {
                var imgarr = new Int32Array(width*height);
                for (var [ox, oy] of ori) {
                    var idx = (y + oy) * width + (x + ox);
                    imgarr[idx]=1;
                }
                imgs.push(imgarr.join(''));
            }
        }
    }
    console.log(imgs.length);
    for (var img of imgs) {
        console.log(img);
    }
}
