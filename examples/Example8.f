! -*- F90 -*-

!-------------------------------------------------------------------
!--   Example program to demonstrate usage of the MSTW 2008 PDFs. --
!--   LHAPDF version by Graeme Watt <watt(at)hep.ucl.ac.uk>.      --
!-------------------------------------------------------------------

PROGRAM Example8
  
  IMPLICIT NONE
  INTEGER iset,ieigen,neigen,ix,nx,iq,nq,flav,nhess
  DOUBLE PRECISION x,q,xmin,xmax,q2min,q2max, &
       &     xf(-6:6),xfp(-6:6),xfm(-6:6), &
       &     summin(-6:6),summax(-6:6),sum(-6:6), &
       &     mCharm,mBottom,qcdl4,qcdl5,alphasPDF,MZ
  CHARACTER flavours(-5:5)*10,xfilename(-5:5)*50,qfilename(-5:5)*50
  DATA flavours /"bbar","cbar","sbar","ubar","dbar","glu", &
       &     "dn","up","str","chm","bot"/
  
  call SetLHAPARM('EXTRAPOLATE') ! extrapolate beyond grids

  !-- Two sets of error PDFs are provided for each fit, corresponding to
  !-- an estimated 90% C.L. (as for previous MRST and CTEQ sets) or to
  !-- a 68% (one-sigma) C.L.: see Section 6 of arXiv:0901.0002.
  !-- The central PDF set is the same in both cases.

  call InitPDFSetByNameM(1,"MSTW2008lo90cl.LHgrid") ! 90% C.L.
  call InitPDFSetByNameM(2,"MSTW2008nlo90cl.LHgrid") ! 90% C.L.
  call InitPDFSetByNameM(3,"MSTW2008nnlo90cl.LHgrid") ! 90% C.L.

  !call InitPDFSetByNameM(1, "MSTW2008lo68cl.LHgrid") ! 68% C.L.
  !call InitPDFSetByNameM(2, "MSTW2008nlo68cl.LHgrid") ! 68% C.L.
  !call InitPDFSetByNameM(3, "MSTW2008nnlo68cl.LHgrid") ! 68% C.L.

  ! Print out values of mCharm and mBottom.
  call GetQmassM(3,4,mCharm)
  call GetQmassM(3,5,mBottom)
  write(6,*) "mCharm,mBottom = ",mCharm,mBottom

  ! Print out values of alphaS(MZ) for each fit.
  MZ = 91.1876D0
  Call InitPDFM(1,0) ! central set
  WRITE(6,'("LO: alphaS(MZ) = ",F7.5)') alphasPDF(MZ)
  Call InitPDFM(2,0) ! central set
  WRITE(6,'("NLO: alphaS(MZ) = ",F7.5)') alphasPDF(MZ)
  Call InitPDFM(3,0) ! central set
  WRITE(6,'("NNLO: alphaS(MZ) = ",F7.5)') alphasPDF(MZ)

  ! Warning: values of qcdl4 and qcdl5 set to zero since no longer used.
  call GetLam4M(3,0,qcdl4)
  call GetLam5M(3,0,qcdl5)
  write(6,*) "qcdl4,qcdl5 = ",qcdl4,qcdl5

  !----------------------------------------------------------------------

  !--   Calculate the uncertainty on the parton distributions using both
  !--   the formula for asymmetric errors [eqs.(51,52) of arXiv:0901.0002]
  !--   and the formula for symmetric errors [eq.(50) of same paper].

  !-- Select either the LO, NLO or NNLO fits.
  !  iset = 1 ! LO
  !  iset = 2 ! NLO
  iset = 3 ! NNLO

  !--   First get xf as a function of x at a fixed value of q.
  !--   Extrapolation will be used for x < 10^-6.
  q = 1.d1
  nx = 100
  xmin = 1.d-7
  xmax = 0.99d0
  DO flav = -5, 5
     xfilename(flav) = "x"//flavours(flav)(1:len_trim(flavours(flav))) &
          &        //"_vs_x.dat"
     OPEN(UNIT=20+flav,FILE=xfilename(flav))
     WRITE(20+flav,'(" # q = ",1PE12.4)') q
     WRITE(20+flav,'(A2,A10,4A12)') " #","x","x"// &
          &        flavours(flav)(1:len_trim(flavours(flav))), &
          &        "error+","error-","error"
  END DO ! flav
  DO ix = 1, nx
     !--   Assign x values distributed logarithmically.
     x = 10.d0**(log10(xmin) + (ix-1.D0)/(nx-1.D0)* &
          & (log10(xmax)-log10(xmin)))
     Call initPDFM(iset,0) ! central set
     Call evolvePDFM(iset,x,q,xf)
     DO flav = -5, 5
        summax(flav) = 0.d0
        summin(flav) = 0.d0
        sum(flav) = 0.d0
     END DO
     Call numberPDFM(iset,nhess)  ! number of eigenvector PDF sets
     neigen = nhess/2
     DO ieigen = 1, neigen ! loop over eigenvector sets
        Call initPDFM(iset,2*ieigen-1) ! "+" direction
        call evolvePDFM(iset,x,q,xfp)
        Call initPDFM(iset,2*ieigen) ! "-" direction
        call evolvePDFM(iset,x,q,xfm)
        DO flav = -5, 5
           summax(flav) = summax(flav) + &
                & (max(xfp(flav)-xf(flav),xfm(flav)-xf(flav),0.d0))**2
           summin(flav) = summin(flav) + &
                & (max(xf(flav)-xfp(flav),xf(flav)-xfm(flav),0.d0))**2
           sum(flav) = sum(flav)+(xfp(flav)-xfm(flav))**2
        END DO ! flav
     END DO ! ieigen
     DO flav = -5, 5
        WRITE(20+flav,'(1P5E12.4)') x,xf(flav), &
             & sqrt(summax(flav)),sqrt(summin(flav)),0.5d0*sqrt(sum(flav))
     END DO ! flav
  END DO ! ix
  DO flav = -5, 5
     CLOSE(UNIT=20+flav)
     WRITE(6,*) "x"//flavours(flav)(1:len_trim(flavours(flav))), &
          &        " vs. x for q2 = ",q**2," written to ", &
          &        xfilename(flav)(1:len_trim(xfilename(flav)))
  END DO ! flav

  !--   Now get xf as a function of q^2 at a fixed value of x.
  !--   Extrapolation will be used for q^2 < 1 GeV^2 and for q^2 > 10^9 GeV^2.
  x = 1.d-3
  nq = 100
  q2min = 5.d-1
  q2max = 1.d10
  DO flav = -5, 5
     qfilename(flav) = "x"//flavours(flav)(1:len_trim(flavours(flav))) &
          &        //"_vs_q2.dat"
     OPEN(UNIT=20+flav,FILE=qfilename(flav))
     WRITE(20+flav,'(" # x = ",1PE12.4)') x
     WRITE(20+flav,'(A2,A10,4A12)') " #","q2","x"// &
          &        flavours(flav)(1:len_trim(flavours(flav))), &
          &        "error+","error-","error"
  END DO ! flav
  DO iq = 1, nq
     !--   Assign q2 values distributed logarithmically.
     q = sqrt(10.d0**(log10(q2min) + (iq-1.D0)/(nq-1.D0)* &
          &           (log10(q2max)-log10(q2min))))
     Call initPDFM(iset,0) ! central set
     Call evolvePDFM(iset,x,q,xf)
     DO flav = -5, 5
        summax(flav) = 0.d0
        summin(flav) = 0.d0
        sum(flav) = 0.d0
     END DO
     Call numberPDFM(iset,nhess)  ! number of eigenvector PDF sets
     neigen = nhess/2
     DO ieigen = 1, neigen ! loop over eigenvector sets
        Call initPDFM(iset,2*ieigen-1) ! "+" direction
        call evolvePDFM(iset,x,q,xfp)
        Call initPDFM(iset,2*ieigen) ! "-" direction
        call evolvePDFM(iset,x,q,xfm)
        DO flav = -5, 5
           summax(flav) = summax(flav) + &
                & (max(xfp(flav)-xf(flav),xfm(flav)-xf(flav),0.d0))**2
           summin(flav) = summin(flav) + &
                & (max(xf(flav)-xfp(flav),xf(flav)-xfm(flav),0.d0))**2
           sum(flav) = sum(flav)+(xfp(flav)-xfm(flav))**2
        END DO ! flav
     END DO ! ieigen        
     DO flav = -5, 5     
        WRITE(20+flav,'(1P5E12.4)') q**2,xf(flav), &
             & sqrt(summax(flav)),sqrt(summin(flav)),0.5d0*sqrt(sum(flav))
     END DO ! flav
  END DO ! iq
  DO flav = -5, 5
     CLOSE(UNIT=20+flav)
     WRITE(6,*) "x"//flavours(flav)(1:len_trim(flavours(flav))), &
          &        " vs. q2 for x = ",x," written to ", &
          &        qfilename(flav)(1:len_trim(qfilename(flav)))
  END DO ! flav

  STOP
END PROGRAM Example8
!----------------------------------------------------------------------
