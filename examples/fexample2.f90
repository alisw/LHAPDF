program example2
  implicit double precision (a-h,o-z)
  double precision f(-6:6)
  integer lhaids(3)
  character*50 setnames(3)
  data lhaids /11000, 25300, 303600/
  data setnames /"CT10nlo", "MMHT2014nnlo68cl", "NNPDF31_nnlo_as_0118"/

  nsets = 3
  nmem = 0
  x = 1e-3
  q = 1000

  ! Modern init by set ID
  write(*,*) "MODERN INIT BY ID"
  do i=1,nsets
     call lhapdf_initpdfset_byid(i, lhaids(i))
  enddo
  do i=1,nsets
     call lhapdf_xfxq_stdpartons(i, nmem, x, q, f)
     write(*,*) i, nmem, f
  enddo
  write(*,*)

  ! Modern init by set name
  write(*,*) "MODERN INIT BY NAME"
  do i=1,nsets
     call lhapdf_initpdfset_byname(i, setnames(i))
  enddo
  do i=1,nsets
     call lhapdf_xfxq_stdpartons(i, nmem, x, q, f)
     write(*,*) i, nmem, f
  enddo
  write(*,*)

  ! Old-style init by name (note no reload messages, since PDFs match)
  write(*,*) "OLD INIT BY NAME"
  do i=1,nsets
     call initpdfsetbynamem(i, setnames(i))
  enddo
  do i=1,nsets
     call lhapdf_xfxq_stdpartons(i, nmem, x, q, f)
     write(*,*) i, nmem, f
  enddo
  write(*,*)

end program example2
