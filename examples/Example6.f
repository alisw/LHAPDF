program example6

  ! Using lhaglue to do example 1
  implicit double precision (a-h,o-z)
  character*20 parm(20)
  !character name*64
  double precision value(20),df(-6:6)
  integer inset(3)
  !data inset/20200,391,231/ 
  data inset/20200,20201,231/ 
  character*20 lhaparm(20)
  double precision lhavalue(20)
  common/lhacontrol/lhaparm,lhavalue
  common/W50513/xmin,xmax,q2min,q2max
  !open(7,file='/home/whalley/lhapdf/lhanames')
  parm(1)='DEFAULT'
  lhaparm(19)='SILENT'
      
  ip2 = 0
  p2 = 0.0d0
  do ntimes=1,3
     do j=1,3
        value(1)=inset(j)
        write(*,*) '---------------------------------------------'
        call pdfset(parm,value)
        qmz = 91.187d0
        a = alphasPDF(qmz)
        call getlam4m(j,0,xlam4)
        call getlam5m(j,0,xlam5)
        print *,value(1),a,xmin,xmax,q2min,q2max,xlam4,xlam5
        !write(*,*) 'x*Gluon'
        !write(*,*) '   x     Q2=10 GeV     Q=100 GeV    Q=1000 GeV'
        do ix=10,95,10
           x = dfloat(ix)*1.0d-03
           Q=100.0d0
           if (value(1).ge.300.and.value(1).le.399) then
              call structp(x,q,p2,ip2,upv,dnv,usea,dsea,str,chm,bot,top,glu)
              g1=glu
           else
              ! call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
              call pftopdg(x,q,df)
              g1=df(0)
           endif
           
           write(*,*) x,g1
        enddo
     enddo
  enddo

end program example6
