program example4
  ! Using LHAglue to do example 1.
  implicit double precision (a-h,o-z)
  character*20 parm(20)
  double precision value(20)

  parm(1)='DEFAULT'
  call SetLHAPARM('SILENT')
  val = 10800
  do i=0,40
     value(1)=val+i
     write(*,*) '---------------------------------------------'
     call pdfset(parm,value)
     if (i.eq.0) call getdesc()
     qmz = 91.18d0
     a = alphasPDF(qmz)
     print *,'Alpha_s(Mz)=',a
     write(*,*) 'x*up'
     write(*,*) '   x     Q2=10 GeV     Q=100 GeV    Q=1000 GeV'
     do ix=10,95,10
        x = dfloat(ix)*1.0d-03
        Q=10.0d0
        call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
        g1=upv+usea
        Q=100d0
        call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
        g2=upv+usea
        Q=1000d0
        call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
        g3=upv+usea
        write(*,*) x,g1,g2,g3
     enddo
  enddo

end program example4
