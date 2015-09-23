#! /usr/bin/env python

import math, numpy
import lhapdf
import matplotlib.pyplot as plt

q = math.sqrt(6400.0)
pdfsets = ["cteq6ll.LHpdf", "MSTW2008lo68cl.LHgrid", "MRST2001lo.LHgrid", "MRST2007lomod.LHgrid", "MRSTMCal.LHgrid"]
#pdfsets = ["cteq6ll.LHpdf", "MRST2001lo.LHgrid", "MRST2007lomod.LHgrid", "MRSTMCal.LHgrid"]
partons = { 0 : "gluon", 2 : "up" }
NPOINTS = 1000

xs = numpy.logspace(-4, -0.001, NPOINTS)
plt.figure(figsize=(13,7))
for n, parton in enumerate(sorted(partons.keys())):
    plt.subplot(1, len(partons), n+1)
    lines = []
    for pdfset in pdfsets:
        lhapdf.initPDFSetByName(pdfset)
        lhapdf.initPDF(0)
        xfxs = numpy.zeros([NPOINTS])
        for i, x in enumerate(xs):
            xfx = lhapdf.xfx(x, q, parton)
            xfxs[i] = xfx
        l = plt.plot(xs, xfxs)
        lines.append(l)

    plt.xscale("log")
    #plt.ylim(0.5, 3)
    plt.legend(lines, pdfsets)
    plt.title(partons[parton])
    plt.xlabel("$x$")
    if n == 0:
        plt.ylabel("$x f(x, Q^2)$")

plt.show()
