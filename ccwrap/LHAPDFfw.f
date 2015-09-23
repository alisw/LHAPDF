! -*- F90 -*-
!
! A simple Fortran wrapper around LHAPDF for interfacing to C++
! By Stefan Gieseke 2004
! Adapted for LHAPDFv4 by Mike Whalley
! Adapted for LHAPDFv5 by Craig Group/Mike Whalley
! Improved string passing by Andy Buckley

subroutine finitpdfset(name)
  character name*(*)
  ! print *, "@@@", name, "@@@"
  ! print *, len_trim(name)
  ! print *, len(name)
  call InitPDFset(name)
end subroutine finitpdfset

subroutine finitpdfsetm(nset,name)
  character name*(*)
  integer nset
  call InitPDFsetM(nset,name)
end subroutine finitpdfsetm

subroutine finitpdfsetbyname(name)
  character name*(*)
  ! print *, "@@@", name, "@@@"
  ! print *, len_trim(name)
  ! print *, len(name)
  call InitPDFsetByName(name)
end subroutine finitpdfsetbyname

subroutine finitpdfsetbynameM(nset,name)
  character name*(*)
  integer nset
  call InitPDFsetByNameM(nset,name)
end subroutine finitpdfsetbynameM

! TODO: What's the point?!
subroutine finitpdf(mem)
  integer mem
  call InitPDF(mem)
end subroutine finitpdf

! TODO: What's the point?!
subroutine finitpdfM(nset,mem)
  integer nset,mem
  call InitPDFM(nset,mem)
end subroutine finitpdfM

subroutine fhasphoton(haspho)
  implicit none
  integer haspho
  logical has_photon
  haspho = 0
  if (has_photon()) then
     haspho = 1
  end if
end subroutine fhasphoton

! TODO: What's the point?!
subroutine fnumberpdf(set)
  integer set
  call numberPDF(set)
end subroutine fnumberpdf

! TODO: What's the point?!
subroutine fnumberpdfM(nset,set)
  integer nset,set
  call numberPDFM(nset,set)
end subroutine fnumberpdfM

! TODO: What's the point?!
subroutine fevolvepdf(x,Q,f)
  real*8 x,Q
  real*8 f(-6:6)
  call evolvePDF(x,Q,f)
end subroutine fevolvepdf

! TODO: What's the point?!
subroutine fevolvepdfm(nset,x,Q,f)
  real*8 x,Q
  real*8 f(-6:6)
  integer nset
  call evolvePDFM(nset,x,Q,f)
end subroutine fevolvepdfm

! TODO: What's the point?!
subroutine fevolvepdfp(x,Q,P2,ip,f)
  real*8 x,Q,P2
  real*8 f(-6:6)
  integer ip
  call evolvePDFp(x,Q,P2,ip,f)
end subroutine fevolvepdfp

! TODO: What's the point?!
subroutine fevolvepdfpM(nset,x,Q,P2,ip,f)
  real*8 x,Q,P2
  real*8 f(-6:6)
  integer ip,nset
  call evolvePDFpM(nset,x,Q,P2,ip,f)
end subroutine fevolvepdfpM

! TODO: What's the point?!
subroutine fevolvepdfa(x,Q,a,f)
  real*8 x,Q,a
  real*8 f(-6:6)
  call evolvePDFa(x,Q,a,f)
end subroutine fevolvepdfa

! TODO: What's the point?!
subroutine fevolvepdfaM(nset,x,Q,a,f)
  real*8 x,Q,a
  real*8 f(-6:6)
  integer nset
  call evolvePDFaM(nset,x,Q,a,f)
end subroutine fevolvepdfaM

! TODO: What's the point?!
subroutine fevolvepdfphoton(x,Q,f,photon)
  real*8 x,Q,photon
  real*8 f(-6:6)
  call evolvePDFphoton(x,Q,f,photon)
end subroutine fevolvepdfphoton

! TODO: What's the point?!
subroutine fevolvepdfphotonM(nset,x,Q,f,photon)
  real*8 x,Q,photon
  real*8 f(-6:6)
  integer nset
  call evolvePDFphotonM(nset,x,Q,f,photon)
end subroutine fevolvepdfphotonM

! TODO: What's the point?!
subroutine falphaspdfm(nset,Q,ans)
  real*8 ans,Q,alphasPDFM
  integer nset
  ans=alphasPDFM(nset,Q)
end subroutine falphaspdfm

! TODO: What's the point?!
subroutine falphaspdf(Q,ans)
  real*8 ans,Q,alphasPDF
  ans=alphasPDF(Q)
end subroutine falphaspdf

! TODO: What's the point?!
subroutine fgetorderpdf(order)
  integer order
  call getorderpdf(order)
end subroutine fgetorderpdf

! TODO: What's the point?!
subroutine fgetorderpdfm(nset,order)
  integer nset,order
  call getorderpdfM(nset,order)
end subroutine fgetorderpdfm

! TODO: What's the point?!
subroutine fgetorderas(order)
  integer order
  call getorderas(order)
end subroutine fgetorderas

! TODO: What's the point?!
subroutine fgetorderasm(nset,order)
  integer nset,order
  call getorderasm(nset,order)
end subroutine fgetorderasm

! TODO: What's the point?!
subroutine fgetdesc()
  call getdesc()
end subroutine fgetdesc

! TODO: What's the point?!
subroutine fgetdescm(nset)
  integer nset
  call getdescm(nset)
end subroutine fgetdescm

! TODO: What's the point?!
subroutine fgetqmass(nf,mass)
  integer nf
  real*8 mass
  call getqmass(nf,mass)
end subroutine fgetqmass

! TODO: What's the point?!
subroutine fgetqmassm(nset,nf,mass)
  integer nset,nf
  real*8 mass
  call getqmassm(nset,nf,mass)
end subroutine fgetqmassm

! TODO: What's the point?!
subroutine fgetthreshold(nf,Q)
  integer nf
  real*8 Q
  call getthreshold(nf,Q)
end subroutine fgetthreshold

! TODO: What's the point?!
subroutine fgetthresholdm(nset,nf,Q)
  integer nset,nf
  real*8 Q
  call getthresholdm(nset,nf,Q)
end subroutine fgetthresholdm

! TODO: What's the point?!
subroutine fgetnf(nfmax)
  integer nfmax
  call getnf(nfmax)
end subroutine fgetnf

! TODO: What's the point?!
subroutine fgetnfm(nset,nfmax)
  integer nset,nfmax
  call getnfm(nset,nfmax)
end subroutine fgetnfm

! TODO: What's the point?!
subroutine fgetlam4(mem,xlam4)
  integer mem
  real*8 xlam4
  call getlam4(mem,xlam4)
end subroutine fgetlam4

! TODO: What's the point?!
subroutine fgetlam4m(nset,mem,xlam4)
  integer nset,mem
  real*8 xlam4
  call getlam4m(nset,mem,xlam4)
end subroutine fgetlam4m

! TODO: What's the point?!
subroutine fgetlam5(mem,xlam5)
  integer mem
  real*8 xlam5
  call getlam5(mem,xlam5)
end subroutine fgetlam5

! TODO: What's the point?!
subroutine fgetlam5m(nset,mem,xlam5)
  integer nset,mem
  real*8 xlam5
  call getlam5m(nset,mem,xlam5)
end subroutine fgetlam5m

! TODO: What's the point?!
subroutine fgetXmin(mem,xmin)
  integer mem
  real*8 xmin
  call getXmin(mem,xmin)
end subroutine fgetXmin

! TODO: What's the point?!
subroutine fgetXmax(mem,xmax)
  integer mem
  real*8 xmax
  call getXmax(mem,xmax)
end subroutine fgetXmax

! TODO: What's the point?!
subroutine fgetQ2min(mem,q2min)
  integer mem
  real*8 q2min
  call getQ2min(mem,q2min)
end subroutine fgetQ2min

! TODO: What's the point?!
subroutine fgetQ2max(mem,q2max)
  integer mem
  real*8 q2max
  call getQ2max(mem,q2max)
end subroutine fgetQ2max

! TODO: What's the point?!
subroutine fgetXminM(nset,mem,xmin)
  integer nset,mem
  real*8 xmin
  call getXminM(nset,mem,xmin)
end subroutine fgetXminM

! TODO: What's the point?!
subroutine fgetXmaxM(nset,mem,xmax)
  integer nset,mem
  real*8 xmax
  call getXmaxM(nset,mem,xmax)
end subroutine fgetXmaxM

! TODO: What's the point?!
subroutine fgetQ2minM(nset,mem,q2min)
  integer nset,mem
  real*8 q2min
  call getQ2minM(nset,mem,q2min)
end subroutine fgetQ2minM

! TODO: What's the point?!
subroutine fgetQ2maxM(nset,mem,q2max)
  integer nset,mem
  real*8 q2max
  call getQ2maxM(nset,mem,q2max)
end subroutine fgetQ2maxM

! TODO: What's the point?!
subroutine fgetMinMax(mem,xmin,xmax,q2min,q2max)
  integer mem
  real*8 xmin,xmax,q2min,q2max
  call getMinMax(mem,xmin,xmax,q2min,q2max)
end subroutine fgetMinMax

! TODO: What's the point?!
subroutine fgetMinMaxM(nset,mem,xmin,xmax,q2min,q2max)
  integer nset,mem
  real*8 xmin,xmax,q2min,q2max
  call getMinMaxM(nset,mem,xmin,xmax,q2min,q2max)
end subroutine fgetMinMaxM

subroutine fextrapolateon()
  call SetLHAPARM('EXTRAPOLATE')
end subroutine fextrapolateon

subroutine fextrapolateoff()
  call SetLHAPARM('18')
end subroutine fextrapolateoff

subroutine fsilent()
  call SetLHAPARM('SILENT')
end subroutine fsilent

subroutine flowkey()
  call SetLHAPARM('LOWKEY')
end subroutine flowkey

subroutine fdefaultverb()
  call SetLHAPARM('19')
end subroutine fdefaultverb

subroutine fsetpdfpath(path)
  character path*(*)
  call SetPDFPath(path)
end subroutine fsetpdfpath
