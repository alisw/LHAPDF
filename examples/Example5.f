program example5

! Using lhaglue for 3 different PDF sets proton/photon/pion 
! Using setpdfpath to define the path
! Using PDFsta for statistics
 
  implicit double precision (a-h,o-z)
  character*20 parm(20)
  double precision value(20),g(3)
  integer inset(3)
  data inset/20200,391,231/ 
  common/W50513/xmin,xmax,q2min,q2max

  parm(1)='DEFAULT'
  call setlhaparm('SILENT')
  !call setpdfpath('../PDFsets')
  qmz = 91.180d0      
  ip2 = 0
  p2 = 0.0d0
  do j=1,3  
     print *,'PDF set ',j,' is PDF set number ',inset(j)
     value(1)=inset(j)
     call pdfset(parm,value)
     call getdescm(j)
     call getlam4m(j,0,xlam4)
     call getlam5m(j,0,xlam5)
     a=alphasPDF(QMZ)
     print *,'PDF number, alpha_s(mz), xmin, xmax, q2min, q2max, lambda4, lambda5'
     print *,value(1),a,xmin,xmax,q2min,q2max,xlam4,xlam5
     write(*,*) '---------------------------------------------'
  enddo
  Q=100.0d0
  q2 = q*q
  print *,'x*gluon at Q = 100 GeV'
  print *,'PDF sets: ',(inset(j),j=1,3)
  write(*,*) '---------------------------------------------'
  do ix=10,95,10
     x = dfloat(ix)*1.0d-03
     do j=1,3
        value(1)=inset(j)
        call pdfset(parm,value)
        if(value(1).ge.300.and.value(1).le.399) then
           ! call structp(x,q2,p2,ip2,upv,dnv,usea,dsea,str,chm,bot,top,glu)
           call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
        else
           call structm(x,q,upv,dnv,usea,dsea,str,chm,bot,top,glu)
        endif
        g(j)=glu
     enddo
     write(*,*) x,(g(j),j=1,3)
  enddo

  call PDFsta
end program example5
