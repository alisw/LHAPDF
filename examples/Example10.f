! -*- F90 -*-

!--   31/05/2012 by Graeme Watt <Graeme.Watt(at)cern.ch>.
!--   Test automatic calculation of PDF uncertainties in LHAPDF.

!--   Compile with:
!--    gfortran -ffree-form testPDFunc.f `lhapdf-config --ldflags`



program testPDFunc

  implicit none
  integer nset,nmem,imem,MaxNumSets
  double precision x,q,xf(-6:6),xg(0:100),xu(0:100), &
       &     xf0,xfp,xfm,xfs,correlation
  logical lMonteCarlo,lSymmetric
  character*10 lhaversion

  !-- Get the LHAPDF version number.
  call getlhapdfversion(lhaversion)
  write(6,*) "LHAPDF Version = ",lhaversion

  !-- Get the maximum number of concurrent PDF sets.
  call GetMaxNumSets(MaxNumSets)
  write(6,*) "MaxNumSets = ",MaxNumSets
  write(6,*)

  !-- Test three PDF sets that have different uncertainty calculations.
  do nset = 1, MaxNumSets

     if (nset.eq.1) then
        call InitPDFSetByNameM(nset,"MSTW2008nnlo68cl.LHgrid")
     else if (nset.eq.2) then
        call InitPDFSetByNameM(nset,"abm11_5n_nnlo.LHgrid")
     else if (nset.eq.3) then
        call InitPDFSetByNameM(nset,"NNPDF21_nnlo_nf5_100.LHgrid")
     end if
     call numberPDFM(nset,nmem)
     write(6,*) "PDF set = ",nset
     write(6,*) "Number of PDF members = ",nmem

     !-- Check if Monte Carlo PDF set (NNPDF) or if
     !-- should compute symmetric errors (Alekhin).
     call GetPDFUncTypeM(nset,lMonteCarlo,lSymmetric)
     write(6,*) "lMonteCarlo = ",lMonteCarlo
     write(6,*) "lSymmetric = ",lSymmetric
     write(6,*)

     x = 0.1D0 ! momentum fraction
     q = 100.D0 ! scale in GeV

     !-- Fill arrays xg and xu using all PDF members.
     do imem = 0, nmem
        call InitPDFM(nset,imem)
        call evolvePDFM(nset,x,q,xf)
        xg(imem) = xf(0)    ! gluon distribution
        xu(imem) = xf(2)    ! up-quark distribution
     end do

     !-- Calculate PDF uncertainty on gluon distribution.
     write(6,*) "Gluon distribution at Q (GeV) = ",q
     write(6,'(A2,A10,4A12)') " #","x","xg","error+","error-","error"
     call GetPDFuncertaintyM(nset,xg,xf0,xfp,xfm,xfs)
     write(6,'(1P5E12.4)') x,xf0,xfp,xfm,xfs
     write(6,*)

     !-- Calculate PDF uncertainty on up-quark distribution.
     write(6,*) "Up-quark distribution at Q (GeV) = ",q
     write(6,'(A2,A10,4A12)') " #","x","xu","error+","error-","error"
     call GetPDFuncertaintyM(nset,xu,xf0,xfp,xfm,xfs)
     write(6,'(1P5E12.4)') x,xf0,xfp,xfm,xfs
     write(6,*)

     !-- Calculate PDF correlation between gluon and up-quark.
     call GetPDFcorrelationM(nset,xg,xu,correlation)
     write(6,*) "Correlation between xg and xu = ",correlation
     write(6,*)

  end do

  stop
end program testPDFunc
