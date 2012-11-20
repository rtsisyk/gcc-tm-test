#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
import pylab
import os, shutil
import matplotlib

DATA_PATH='./doc/tests/outputs/8core'
GRAPH_PATH='./doc/tests/graphs/8core';
TITLE="8 core configuration"

TYPES = ['onethread', 'mutex', 'tm-tiny'];
LINE_STYLES = {
    'onethread' : '-',
    'mutex': '-',
    'tm-tiny': '-',
    'tm-gnu': '-',
};

MARKER_STYLES = {
    'onethread' : 'o',
    'mutex': '^',
    'tm-tiny': '*',
    'tm-gnu': '.',
};

class GraphData:
    pass

def read_data():
    data = {};
    
    for typename in TYPES:
        f = open(DATA_PATH + '/tester-%s.txt' % typename, 'r');
        for line in f:
            if line[0] != '>':
                continue;
            line = line.split();
            # print line;
            if len(line) < 3:
                continue;
            
            testname=line[1];
            prefix=line[2];
            check=line[3];
            if check != "OK":
                continue;
            
            inputsize=float(line[4]);
            numthreads=int(line[5]);
            retrycount=int(line[6]);
            time=float(line[7]);
            ops=int(line[8]);
            
            if inputsize < 10000:
                continue;
            
            if typename == 'onethread':
                continue;
            
            if not numthreads in (2, 4, 8, 16):
                continue;
            
            if not testname in data:
                data[testname] = {};
            
            confname = "%s_%d" % (typename, numthreads);
            
            if not confname in data[testname]:
                testdata = GraphData();
                testdata.typename = typename;
                testdata.numthreads = numthreads;
                testdata.inputsize = [];
                testdata.title = "%s %d" % (typename, numthreads);
                testdata.ops = [];
                testdata.time = [];
                
                data[testname][confname] = testdata;
            else:
                testdata = data[testname][confname];
            
            testdata.time.append(time);
            testdata.ops.append(ops);
            testdata.inputsize.append(inputsize);
        f.close();

    return data;
    
def main():
    data = read_data();
    for (testname, testdata) in data.iteritems():
        print 'Test', testname
        plt.figure();
        params = [];
        
        print 'Confs count', len(testdata);

        confnames = sorted(testdata.iterkeys());
        for confname in confnames:
            confdata = testdata[confname];

            params.append(confdata.inputsize);
            params.append(confdata.ops);

        params = tuple(params);
        lines = plt.plot(*params);
        
        print 'Lines count', len(lines);
        for (confname, line) in zip(confnames, lines):
            confdata = testdata[confname];
            pylab.setp(line,
                antialiased = True,
                linewidth = 2,
                markersize = 1.5 * np.log2(confdata.numthreads),
                marker = MARKER_STYLES[confdata.typename],
                linestyle = LINE_STYLES[confdata.typename],
                label = u'%s' % confdata.title);
        
        plt.grid();
        plt.xlabel('Input size');
        plt.ylabel('Ops/s');
        plt.title('%s performance (%s)' % (testname, TITLE));
        plt.legend(loc = 'upper right');
        path = GRAPH_PATH + '/%s.pdf' % testname;
        path_cropped = GRAPH_PATH + '/%s-crop.pdf' % testname;
        plt.savefig(path);
        os.system('pdfcrop "%s"' % path);
        shutil.move(path_cropped, path);
        print
        print

if __name__ == "__main__":
    main();


