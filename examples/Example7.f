!////////////////////////////////////////////////////////////////
!/ Program to demonstrate usage of the MRST 2006 NNLO PDFs.    //
!/ to calculate errors.   Fortan version                       //
!////////////////////////////////////////////////////////////////
! Main program
  Implicit Double Precision (a-h,o-z)
  Character*80 Name
  Double Precision plus,minus,diff
  Dimension fc(0:9),x(0:9),f(-6:6)
  Dimension summax(0:9), summin(0:9), sum(0:9)
  Integer flav
  ! Show initialisation banner only once
  Call SetLHAParm('LOWKEY') ! or SILENT, for no banner at all

  ! You could explicitly set the path to the PDFsets directory
  ! Call setPDFPath("/home/whalley/local/share/lhapdf/PDFsets")

  ! Initialize PDF sets
  Name = "MSTW2008nnlo90cl.LHgrid"
  Call initPDFSetByNameM(1, Name)
  Call initPDFSetByNameM(2, Name)
  Call initPDFSetByNameM(3, Name)

  ! Find the number of eigensets from numberPDF()
  Call numberPDFM(1,neigen)
  neigen = neigen/2
  print *,"Number of eigensets in this fit = ",neigen
  ! Find the min and max values of x and Q2
  Call getXmin(0,xmin)
  Call getXmax(0,xmax)
  print *,"Valid x-range = [",xmin,", ",xmax,"]"
  ! Number of x values to sample
  nx = 10
  ! Set the Q scale and flavour
  q = 10d0
  flav = 4

  ! Get x's and central PDF values
  Call initPDFM(1, 0)
  do ix=0,nx-1
    x(ix) = xlogdist_x(xmin, 0.9*xmax, ix, nx)
    call evolvePDFM(1, x(ix), q, f)
    fc(ix)=f(flav)
    summax(ix) = 0.0d0
    summin(ix) = 0.0d0
    sum(ix) = 0.0d0
  enddo

  ! Sum over error contributions (two ways, depending on how LHDPAF was compiled)
  do ieigen=1,neigen
    call initPDFM(2, 2*ieigen-1)
    call initPDFM(3, 2*ieigen)
    do ix=0,nx-1
      ! Find central and plus/minus values
      call evolvePDFM(2, x(ix), q, f)
      fp = f(flav)
      call evolvePDFM(3, x(ix), q, f)
      fm = f(flav)
      ! Construct shifts
      plus = max(max(fp-fc(ix), fm-fc(ix)),0.0d0)
      minus = min(min(fp-fc(ix), fm-fc(ix)),0.0d0)
      diff = fp-fm
      ! Add it together
      summax(ix) = summax(ix) + plus*plus
      summin(ix) = summin(ix) + minus*minus
      sum(ix) = sum(ix) +  diff*diff
    enddo
  enddo

  print *,"flavour = ",flav,"    Asymmetric (%)   Symmetric (%)"
  print *,"     x    Q**2    xf(x)    plus    minus      +-      "
  do ix=0,nx-1
     eplus = dsqrt(summax(ix))*100/fc(ix)
     eminus=  dsqrt(summin(ix))*100/fc(ix)
     eavge =  0.5*dsqrt(sum(ix))*100/fc(ix)
     print 1000, x(ix), q*q, fc(ix), eplus, eminus, eavge
  enddo
  stop
1000 format(1x,f10.7,f5.0,1pe10.2,0pf8.2,f8.2,f8.2)
  end

 double precision function xlogdist_x(xmin,xmax,ix,nx)
  implicit double precision (a-h,o-z)
 ! print *,xmin,xmax,ix,nx
  xlog10xmin = dlog10(xmin)
  xlog10xmax = dlog10(xmax)
  xlog10x = xlog10xmin + (dfloat(ix)/dfloat(nx-1))*(xlog10xmax-xlog10xmin)
 ! print *,xlog10xmin,xlog10xmax,xlog10x
  xlogdist_x = 10.0d0**xlog10x
  return
 end
