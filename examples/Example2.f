program example2
  implicit double precision (a-h,o-z)
  character*64 name
  double precision f(-6:6),mom1(9),mom2(9)

  Q=100d0
  name='Botje_100.LHpdf'
  call InitPDFsetByName(name)
  
  call numberPDF(Nmem)
  print *,Nmem
  write(*,*)
  write(*,*) 'Calculating the gluon momentum PDF average <g>'
  write(*,*) 'and standard deviaton SD(g) for several x values'
  write(*,*) 'at Q=100 GeV'
  write(*,*)
  write(*,*) '1. The slow way:'
  write(*,*) 
  write(*,*) '   x       <g>         SD(g)'
  do ix=10,95,10
     x = dfloat(ix)*1.0d-03
     gmom1=0d0
     gmom2=0d0
     do i=1,Nmem
        call InitPDF(i)
        call evolvePDF(x,Q,f)
        gmom1=gmom1+f(0)
        gmom2=gmom2+f(0)**2
     enddo
     av=gmom1/Nmem
     sd=sqrt(gmom2/Nmem-av**2)
     write(*,*) x,av,sd
  enddo
  write(*,*) 
  write(*,*) '2. The fast way:'
  write(*,*) 
  write(*,*) '   x       <g>         SD(g)'
  do i=1,9
     mom1(i)=0d0
     mom2(i)=0d0
  enddo
  do i=1,Nmem
     call InitPDF(i)
     ic=0
     do ix=10,95,10
        x = dfloat(ix)*1.0d-03
        call evolvePDF(x,Q,f)
        ic=ic+1
        mom1(ic)=mom1(ic)+f(0)
        mom2(ic)=mom2(ic)+f(0)**2
     enddo
  enddo
  ic=0
  do ix=10,95,10
     x = dfloat(ix)*1.0d-03
     ic=ic+1
     av=mom1(ic)/Nmem
     sd=sqrt(mom2(ic)/Nmem-av**2)
     write(*,*) x,av,sd
  enddo
  
  Q=10d0
  x1=0.001d0
  x2=0.01d0
  write(*,*)
  write(*,*) 'Calculating the normalized correlation coefficient'
  write(*,*) '<g1g2> between g(x=0.001) and g(x=0.01) and'
  write(*,*) '<sAlpha> between the strange quark momentum PDF'
  write(*,*) 'at x=0.001 and alpha_S(Q) for Q=10 GeV'
  write(*,*)
  avg1=0d0
  avg2=0d0
  avs=0d0
  avAs=0d0
  sdg1=0d0
  sdg2=0d0
  sds=0d0
  sdAs=0d0
  Cg1g2=0d0
  CsAs=0d0
  j=3
  do i=1,Nmem
     call InitPDF(i)
     As=alphasPDF(Q)
     print *,Q,As
     call evolvePDF(x1,Q,f)
     g1=f(0)
     s=f(j)
     call evolvePDF(x2,Q,f)
     g2=f(0)
     avAs=avAs+As
     avg1=avg1+g1
     avg2=avg2+g2
     avs=avs+s
     sdAs=sdAs+As**2
     sdg1=sdg1+g1**2
     sdg2=sdg2+g2**2
     sds=sds+s**2
     CsAs=CsAs+s*As
     Cg1g2=Cg1g2+g1*g2
  enddo
  avAs=avAs/Nmem
  avs=avs/Nmem
  avg1=avg1/Nmem
  avg2=avg2/Nmem
  print *,sdAs,Nmem,avas
  sdAs=sdAs/Nmem-avAs**2
  sds=sds/Nmem-avs**2
  sdg1=sdg1/Nmem-avg1**2
  sdg2=sdg2/Nmem-avg2**2
  CsAs=CsAs/Nmem-avs*avAs
  Cg1g2=Cg1g2/Nmem-avg1*avg2
  write(*,*) '<g1g2>    = ',Cg1g2/sqrt(sdg1*sdg2)
  print *,CsAs,sds,sdAs
  write(*,*) '<sAlpha> = ',CsAs/sqrt(sds*sdAs)
end program example2
