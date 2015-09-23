# A brief example of how to use the Octave LHAPDF wrapper.
# Original author: Philip Ilten

# First let's read the helpfile
help lhapdf

# Now we print some information about the PDF set we select
dataset = 'cteq5l.LHgrid';
subset  = 0;
alphasorder = lhapdf('alphasorder',dataset,subset)
pdforder    = lhapdf('pdforder',dataset,subset)
xmin        = lhapdf('xmin',dataset,subset)
xmax        = lhapdf('xmax',dataset,subset)

# Next we plot the structure for a gluon in a nucleon at a momentum transfer
# of 20 GeV.
x = [0:.01:1];
Q = 20*ones(size(x));
f = zeros(size(x));
xfx = lhapdf(x,Q,f,dataset,subset);
semilogy(x,xfx);
xlabel('x');
ylabel('xf(x)');


# Finally we plot the running of the strong coupling constant
Q = [0:0.1:1000];
alphas = lhapdf('alphas',Q,dataset,subset);
figure(2)
loglog(Q,alphas)
xlabel('Q');
ylabel('\alpha_s(Q)');
ylim([0.05 1]);
