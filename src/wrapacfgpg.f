! -*- F90 -*-


      subroutine ACFGPevolvep(xin,qin,p2in,ip2in,pdf) 
      include 'parmsetup.inc' 
      real*8 xin,qin,q2in,p2in,pdf(-6:6),xval(45),qcdl4,qcdl5 
      real*8 upv,dnv,usea,dsea,str,chm,bot,top,glu 
      real*4 par,par2,calc,celc 
      common/acfgp/PAR(30,3),CALC(8,20,32,3),PAR2(30),CELC(8,20,32) 
      character*16 name(nmxset) 
      integer nmem(nmxset),ndef(nmxset),mmem 
      common/NAME/name,nmem,ndef,mmem 
      integer nset 
                                                                        
      save 
                                                                        
      if(imem.eq.1) then 
        call ACFGP1(xin,qin,upv,dnv,usea,dsea,str,chm,glu) 
                                                                        
      elseif(imem.eq.2) then 
        call ACFGP2(xin,qin,upv,dnv,usea,dsea,str,chm,glu) 
                                                                        
      elseif(imem.eq.3.or.imem.eq.0) then 
        call SFAFG1(xin,qin,upv,dnv,usea,dsea,str,chm,glu) 
                                                                        
      else 
        CONTINUE 
      endif 
                                                                        
      pdf(-6)= 0.0d0 
      pdf(6)= 0.0d0 
      pdf(-5)= 0.0d0 
      pdf(5 )= 0.0d0 
      pdf(-4)= chm 
      pdf(4 )= chm 
      pdf(-3)= str 
      pdf(3 )= str 
      pdf(-2)= usea 
      pdf(2 )= upv 
      pdf(-1)= dsea 
      pdf(1 )= dnv 
      pdf(0 )= glu 
                                                                        
      return 
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc 
      entry ACFGPread(nset) 
      read(1,*)nmem(nset),ndef(nset) 
      do iset = 1,3 
        read(1,*)(par(j,iset),j=1,4) 
        read(1,*)(par(j,iset),j=5,8) 
        read(1,*)(par(j,iset),j=9,12) 
        read(1,*)(par(j,iset),j=13,16) 
        read(1,*)(par(j,iset),j=17,20) 
        read(1,*)(par(j,iset),j=21,24) 
        read(1,*)(par(j,iset),j=25,28) 
        read(1,*)(par(j,iset),j=29,30) 
        do j=1,32 
          do k=1,20 
            read(1,*)(calc(i,k,j,iset),i=1,4) 
            read(1,*)(calc(i,k,j,iset),i=5,8) 
          enddo 
        enddo 
      enddo 
! last one                                                              
      read(1,*)(par2(j),j=1,4) 
      read(1,*)(par2(j),j=5,8) 
      read(1,*)(par2(j),j=9,12) 
      read(1,*)(par2(j),j=13,16) 
      read(1,*)(par2(j),j=17,20) 
      read(1,*)(par2(j),j=21,24) 
      read(1,*)(par2(j),j=25,28) 
      read(1,*)(par2(j),j=29,30) 
      do j=1,32 
        do k=1,20 
          read(1,*)(celc(i,k,j),i=1,4) 
          read(1,*)(celc(i,k,j),i=5,8) 
        enddo 
      enddo 
                                                                        
      return 
                                                                        
                                                                        
!                                                                       
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc 
      entry ACFGPalfa(alfas,qalfa) 
        call getnset(iset) 
        call GetOrderAsM(iset,iord) 
        call Getlam4M(iset,imem,qcdl4) 
        call Getlam5M(iset,imem,qcdl5) 
        call aspdflib(alfas,Qalfa,iord,qcdl5) 
                                                                        
      return 
!                                                                       
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc 
      entry ACFGPinit(Eorder,Q2fit) 
      return 
!                                                                       
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc 
      entry ACFGPpdf(mem) 
      imem = mem 
      return 
!                                                                       
 1000 format(5e13.5) 
      END                                           
!cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc 
      SUBROUTINE ACFGP1(DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DGL) 
!                                                                       
!     INTERPOLATION PROGRAM WHICH INTERPOLATES THE GRID "DATAGA" AND GIV
!     QUARK AND GLUON DISTRIBUTIONS IN THE REAL PHOTON, AS FUNCTIONS OF 
!                                                                       
!     THE Q2-EVOLUTION IS PERFORMED WITH BLL AP-EQUATIONS AND NF=4. A MA
!     CHARM DISTRIBUTION (BORROWED FROM GLUCK AND REYA) IS ALSO AVAILABL
!                                                                       
!     THE BOUNDARY CONDITIONS ARE SUCH THAT THE DISTRIBUTION FUNCTIONS A
!     BY A VDM "ANSATZ" AT Q2=.25 GEV**2.                               
!                                                                       
!     THE PROGRAM WORKS FOR  2. GEV**2 < Q2 <5.5E+5   AND .00137 < X < .
!                                                                       
!     THE DISTRIBUTIONS ARE CALCULATED IN THE MSBAR FACTORIZATION SCHEME
!                                                                       
!     THE VALUE OF LAMBDA-MSB IS 200 MEV                                
!                                                                       
!     THE OUTPUT IS WRITTEN IN THE FILE 'FILEOUT':                      
!                                  X*U=X*U(X,Q2)                        
!                                  X*D= ...                             
!                                  X*S= ...                             
!                                  X*C= ...  (MASSLESS CHARM WITH     C(
!                                 X*CM= ...  (MASSIVE CHARM WITH MC=1.5 
!                              X*GLU=GLUON(X,Q2)*X                      
!                                                                       
!                                                                       
!                    F2 = PHOTON STRUCTURE FUNCTION WITHOUT CHARM       
!                    F2C=  "        "         "     WITH MASSIVE CHARM  
!                                                                       
      double precision                                                  &
     &       DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DBB,DGL                      
      REAL    X, Q, UV, DV, UB, DB, SB, CB, BB, GL 
      REAL       Q2 
      common/acfgp/PAR(30,3),CALC(8,20,32,3),PAR2(30),CELC(8,20,32) 
      DIMENSION XPDF(7),CALCO(8,20,32) 
      COMMON/W5051I7/CALCO 
      EXTERNAL AFCPLU 
      DATA ZERO/0.0/ 
!---------------------------------------------------------------------- 
       DATA ISTART/0/ 
       SAVE ISTART,OWLAM2,Q02,FLAV, /W5051I7/ 
!                                                                       
      IF (ISTART.EQ.0) THEN 
        ISTART=1 
        DO 10 K=1,32 
        DO 10 I=1,20 
        DO 10 M=1,8 
   10   CALCO(M,I,K) = CALC(M,I,K,1) 
           OWLAM=PAR(1,1) 
           OWLAM2=OWLAM**2 
           Q02=PAR(30,1) 
           FLAV=PAR(25,1) 
           DELTA=PAR(29,1) 
           CALL WATE32 
         ENDIF 
!                                                                       
      X = DX 
      Q = DQ 
      Q2 = Q*Q 
      IDQ2=2 
      SB=0. 
      IF((Q2-Q02).LE.0) THEN 
        GOTO 1 
      ELSE 
        GOTO 2 
      ENDIF 
    2 IF((IDQ2-1).LE.0) THEN 
        GOTO 1 
      ELSE 
        GOTO 3 
      ENDIF 
    3 SB= LOG( LOG( MAX(Q02,Q2)/OWLAM2)/ LOG(Q02/OWLAM2)) 
    1 CONTINUE 
      CALL AURGAM(8,0,X,SB,XPDF(7)) 
      CALL AURGAM(7,0,X,SB,SING) 
      CALL AURGAM(4,0,X,SB,DPLUSNS) 
      CALL AURGAM(3,0,X,SB,CPLUSNS) 
      CALL AURGAM(5,0,X,SB,UPLUSNS) 
      CALL AURGAM(6,0,X,SB,SPLUSNS) 
      XPDF(3) = CPLUSNS 
      XPDF(4) = DPLUSNS 
      XPDF(5) = UPLUSNS 
      XPDF(6) = SPLUSNS 
      XPDF(1) = SING 
!                                                                       
      ADD = XPDF(1)/FLAV 
      UPLUS=XPDF(5)+ADD 
      DPLUS=-XPDF(4)+ADD 
      SPLUS=-XPDF(6)+ADD 
      CPLUS=-XPDF(3)+ADD 
      UB=UPLUS*0.5 
      UV=UB 
      DB=DPLUS*0.5 
      DV=DB 
      SB=SPLUS*0.5 
      CB=CPLUS*0.5 
      SING=XPDF(1) 
      GLU=XPDF(7) 
      GL=GLU 
!                                                                       
      DUV=MAX(ZERO,UV) 
      DDV=MAX(ZERO,DV) 
      DUB=MAX(ZERO,UB) 
      DDB=MAX(ZERO,DB) 
      DSB=MAX(ZERO,SB) 
      DCB=MAX(ZERO,CB) 
      DGL=MAX(ZERO,GL) 
!                                                                       
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE ACFGP2(DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DGL) 
!                                                                       
!     INTERPOLATION PROGRAM WHICH INTERPOLATES THE GRID "DATAGA" AND GIV
!     QUARK AND GLUON DISTRIBUTIONS IN THE REAL PHOTON, AS FUNCTIONS OF 
!                                                                       
!     THE Q2-EVOLUTION IS PERFORMED WITH BLL AP-EQUATIONS AND NF=4. A MA
!     CHARM DISTRIBUTION (BORROWED FROM GLUCK AND REYA) IS ALSO AVAILABL
!                                                                       
!     THE BOUNDARY CONDITIONS ARE SUCH THAT THE DISTRIBUTION FUNCTIONS A
!     BY A VDM "ANSATZ" AT Q2=.25 GEV**2.                               
!                                                                       
!     THE PROGRAM WORKS FOR  2. GEV**2 < Q2 <5.5E+5   AND .00137 < X < .
!                                                                       
!     THE DISTRIBUTIONS ARE CALCULATED IN THE MSBAR FACTORIZATION SCHEME
!                                                                       
!     THE VALUE OF LAMBDA-MSB IS 200 MEV                                
!                                                                       
!     THE OUTPUT IS WRITTEN IN THE FILE 'FILEOUT':                      
!                                  X*U=X*U(X,Q2)                        
!                                  X*D= ...                             
!                                  X*S= ...                             
!                                  X*C= ...  (MASSLESS CHARM WITH     C(
!                                 X*CM= ...  (MASSIVE CHARM WITH MC=1.5 
!                              X*GLU=GLUON(X,Q2)*X                      
!                                                                       
!                                                                       
!                    F2 = PHOTON STRUCTURE FUNCTION WITHOUT CHARM       
!                    F2C=  "        "         "     WITH MASSIVE CHARM  
!                                                                       
      double precision                                                  &
     &       DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DBB,DGL                      
      REAL    X, Q, UV, DV, UB, DB, SB, CB, BB, GL 
      REAL       Q2 
      common/acfgp/PAR(30,3),CALC(8,20,32,3),PAR2(30),CELC(8,20,32) 
      DIMENSION XPDF(7),CALCO(8,20,32) 
      COMMON/W5051I7/CALCO 
      EXTERNAL AFCPLU 
      DATA ZERO/0.0/ 
!---------------------------------------------------------------------- 
       DATA ISTART/0/ 
       SAVE ISTART,OWLAM2,Q02,FLAV, /W5051I7/ 
!                                                                       
      IF (ISTART.EQ.0) THEN 
        ISTART=1 
        DO 10 K=1,32 
        DO 10 I=1,20 
        DO 10 M=1,8 
   10   CALCO(M,I,K) = CALC(M,I,K,2) 
           OWLAM=PAR(1,2) 
           OWLAM2=OWLAM**2 
           Q02=PAR(30,2) 
           FLAV=PAR(25,2) 
           DELTA=PAR(29,2) 
           CALL WATE32 
         ENDIF 
!                                                                       
      X = DX 
      Q = DQ 
      Q2 = Q*Q 
      IDQ2=2 
      SB=0. 
      IF((Q2-Q02).LE.0) THEN 
        GOTO 1 
      ELSE 
        GOTO 2 
      ENDIF 
    2 IF((IDQ2-1).LE.0) THEN 
        GOTO 1 
      ELSE 
         GOTO 3 
      ENDIF 
    3 SB= LOG( LOG( MAX(Q02,Q2)/OWLAM2)/ LOG(Q02/OWLAM2)) 
    1 CONTINUE 
      CALL AURGAM(8,0,X,SB,XPDF(7)) 
      CALL AURGAM(7,0,X,SB,SING) 
      CALL AURGAM(4,0,X,SB,DPLUSNS) 
      CALL AURGAM(3,0,X,SB,CPLUSNS) 
      CALL AURGAM(5,0,X,SB,UPLUSNS) 
      CALL AURGAM(6,0,X,SB,SPLUSNS) 
      XPDF(3) = CPLUSNS 
      XPDF(4) = DPLUSNS 
      XPDF(5) = UPLUSNS 
      XPDF(6) = SPLUSNS 
      XPDF(1) = SING 
!                                                                       
      ADD = XPDF(1)/FLAV 
      UPLUS=XPDF(5)+ADD 
      DPLUS=-XPDF(4)+ADD 
      SPLUS=-XPDF(6)+ADD 
      CPLUS=-XPDF(3)+ADD 
      UB=UPLUS*0.5 
      UV=UB 
      DB=DPLUS*0.5 
      DV=DB 
      SB=SPLUS*0.5 
      CB=CPLUS*0.5 
      SING=XPDF(1) 
      GLU=XPDF(7) 
      GL=GLU 
!... get parton density with massive charm                              
      CPLUM=AFCPLU(X,Q2) 
      CB=CPLUM*0.5 
!                                                                       
      DUV=MAX(ZERO,UV) 
      DDV=MAX(ZERO,DV) 
      DUB=MAX(ZERO,UB) 
      DDB=MAX(ZERO,DB) 
      DSB=MAX(ZERO,SB) 
      DCB=MAX(ZERO,CB) 
      DGL=MAX(ZERO,GL) 
!                                                                       
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE SFAFG1(DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DGL) 
!                                                                       
!***********************************************************************
!                       ( 1st of February 1994)                         
!     This is an interpolation program which reads the files GRPOL and  
!     GRVDM and gives the quark and gluon distributions in real photon  
!     as functions of x and Q**2.                                       
!                                                                       
!     The Q**2 evolution is a BLL evolution (MSbar scheme) with Nf=4    
!     and LAMBDA(MSbar)=.200 Gev.                                       
!                                                                       
!     A massless charm distribution is generated for Q**2 > 2 Gev**2.   
!                                                                       
!     The distributions are the sum of a pointlike part (PL) and of a   
!     Vdm part (VDM):                                                   
!                     dist=PL + KA*VDM                                  
!     KA is a factor which can be adjusted ( the default value is KA=1.0
!     The file GRPOL contains the pointlike part of the distributions.  
!     The file GRVDM contains the vdm part (A precise definition of this
!     latter is given in the paper "PARTON DISTRIBUTIONS IN THE PHOTON",
!     Preprint LPTHE Orsay 93-37, by P.Aurenche,M.Fontannaz and J.Ph.Gui
!                                                                       
!     The output of the program is written in the file GETOUT with the  
!     following conventions                                             
!                              UPLUS=x(u+ubar)                          
!                              DPLUS=x(d+dbar)                          
!                              SPLUS=x(s+sbar)                          
!                              CPLUS=x(c+cbar)                          
!                              SING =UPLUS+DPLUS+SPLUS+CPLUS            
!                              GLU  =x*g                                
!                                                                       
!      The interpolation is valid for     2. < Q**2 < 5.5E+5 Gev**2,    
!                             and for   .0015<  x   < .99               
!                                                                       
!      The program also gives the structure function F2:                
!                        F2 = q*Cq + g*Cg + Cgam                        
!      Cq and Cg are the Wilson coeficients and Cgam is the direct term.
!                                                                       
!      Although the charm quark evolution is massless, the direct term  
!      Cgam includes the effects due to the charm quark mass. The charm 
!      quark threshold is therefore correctly described at the lowest   
!      ordre in alphastrong (Details are given in the preprint).        
!                                                                       
!                                                                       
!***********************************************************************
!                                                                       
      double precision                                                  &
     &       DX,DQ,DUV,DDV,DUB,DDB,DSB,DCB,DBB,DGL                      
      REAL    X, Q, UV, DV, UB, DB, SB, CB, BB, GL 
      REAL       Q2 
      DIMENSION XPDF(7) 
      common/acfgp/PAR(30,3),CALC(8,20,32,3),PAR2(30),CELC(8,20,32) 
      DIMENSION CALCO(8,20,32) 
      DIMENSION CELCO(8,20,32) 
      COMMON/W5051IA/CALCO 
      COMMON/W5051IB/CELCO 
      EXTERNAL AFCPLU 
      DATA ZERO/0.0/ 
!---------------------------------------------------------------------- 
       DATA ISTART/0/ 
       SAVE ISTART,OWLAM2,Q02,FLAV,KA, /W5051IA/, /W5051IB/ 
!                                                                       
      IF (ISTART.EQ.0) THEN 
        ISTART=1 
        DO 10 K=1,32 
        DO 10 I=1,20 
        DO 10 M=1,8 
        CALCO(M,I,K) = CALC(M,I,K,3) 
   10   CELCO(M,I,K) = CELC(M,I,K) 
           OWLAM=PAR(1,3) 
           OWLAM2=OWLAM**2 
           Q02=PAR(30,3) 
           FLAV=PAR(25,3) 
           DELTA=PAR(29,3) 
           CALL WATE32 
           KA=1.0 
         ENDIF 
!                                                                       
      X = DX 
      Q = DQ 
      Q2 = Q*Q 
      IDQ2=2 
      SB=0. 
      IF((Q2-Q02).LE.0) THEN 
        GOTO 1 
      ELSE 
        GOTO 2 
      ENDIF 
    2 IF((IDQ2-1).LE.0) THEN 
        GOTO 1 
      ELSE 
         GOTO 3 
      ENDIF 
    3 SB= LOG( LOG( MAX(Q02,Q2)/OWLAM2)/ LOG(Q02/OWLAM2)) 
    1 CONTINUE 
      CALL AFGINT(8,0,X,SB,XPDF(7)) 
      CALL AFGINT(7,0,X,SB,SING) 
      CALL AFGINT(4,0,X,SB,DPLUSNS) 
      CALL AFGINT(3,0,X,SB,CPLUSNS) 
      CALL AFGINT(5,0,X,SB,UPLUSNS) 
      CALL AFGINT(6,0,X,SB,SPLUSNS) 
      XPDF(3) = CPLUSNS 
      XPDF(4) = DPLUSNS 
      XPDF(5) = UPLUSNS 
      XPDF(6) = SPLUSNS 
      XPDF(1) = SING 
!                                                                       
      ADD = XPDF(1)/FLAV 
      UPLUS= XPDF(5)+ADD 
      DPLUS=-XPDF(4)+ADD 
      SPLUS=-XPDF(6)+ADD 
      CPLUS=-XPDF(3)+ADD 
      SING=XPDF(1) 
      GLU=XPDF(7) 
      GL=GLU 
!                                                                       
      CALL AFGIN2(8,0,X,SB,XPDF(7)) 
      CALL AFGIN2(7,0,X,SB,SING) 
      CALL AFGIN2(4,0,X,SB,DPLUSNS) 
      CALL AFGIN2(3,0,X,SB,CPLUSNS) 
      CALL AFGIN2(5,0,X,SB,UPLUSNS) 
      CALL AFGIN2(6,0,X,SB,SPLUSNS) 
      XPDF(3) = CPLUSNS 
      XPDF(4) = DPLUSNS 
      XPDF(5) = UPLUSNS 
      XPDF(6) = SPLUSNS 
      XPDF(1) = SING 
!                                                                       
      ADD2 = XPDF(1)/FLAV 
      UPLU2= XPDF(5)+ADD2 
      DPLU2=-XPDF(4)+ADD2 
      SPLU2=-XPDF(6)+ADD2 
      CPLU2=-XPDF(3)+ADD2 
      SING2=XPDF(1) 
      GLU2=XPDF(7) 
      UB=UPLUS+UPLU2*KA 
      UB=UB/2.0
      UV=UB 
      DB=DPLUS+DPLU2*KA 
      DB=DB/2.0
      DV=DB 
      SB=SPLUS+SPLU2*KA 
      SB=SB/2.0
      CB=CPLUS+CPLU2*KA 
      CB=CB/2.0
      SING=SING+SING2*KA 
      GL=GLU+GLU2*KA 
!                                                                       
      DUV=MAX(ZERO,UV) 
      DDV=MAX(ZERO,DV) 
      DUB=MAX(ZERO,UB) 
      DDB=MAX(ZERO,DB) 
      DSB=MAX(ZERO,SB) 
      DCB=MAX(ZERO,CB) 
      DGL=MAX(ZERO,GL) 
!                                                                       
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE WATE32 
!  32 POINT GAUSSIAN QUADRATURE ROUTINE                                 
      double precision                                                  &
     &       X(16),W(16)                                                
      double precision                                                  &
     &               XI(32),WI(32),XX(33)                               
      COMMON/W5051I9/XI,WI,XX,NTERMS 
      NTERMS=32 
      X(1)=0.048307665687738316235D0 
      X(2)=0.144471961582796493485D0 
      X(3)=0.239287362252137074545D0 
      X(4)=0.331868602282127649780D0 
      X(5)=0.421351276130635345364D0 
      X(6)=0.506899908932229390024D0 
      X(7)=0.587715757240762329041D0 
      X(8)=0.663044266930215200975D0 
      X(9)=0.732182118740289680387D0 
      X(10)=0.794483795967942406963D0 
      X(11)=0.849367613732569970134D0 
      X(12)=0.896321155766052123965D0 
      X(13)=0.934906075937739689171D0 
      X(14)=0.964762255587506430774D0 
      X(15)=0.985611511545268335400D0 
      X(16)=0.997263861849481563545D0 
      W(1)=0.096540088514727800567D0 
      W(2)=0.095638720079274859419D0 
      W(3)=0.093844399080804565639D0 
      W(4)=0.091173878695763884713D0 
      W(5)=0.087652093004403811143D0 
      W(6)=0.083311924226946755222D0 
      W(7)=0.078193895787070306472D0 
      W(8)=0.072345794108848506225D0 
      W(9)=0.065822222776361846838D0 
      W(10)=0.058684093478535547145D0 
      W(11)=0.050998059262376176196D0 
      W(12)=0.042835898022226680657D0 
      W(13)=0.034273862913021433103D0 
      W(14)=0.025392065309262059456D0 
      W(15)=0.016274394730905670605D0 
      W(16)=0.007018610009470096600D0 
      NTERMH = NTERMS/2 
      DO 1 I=1,NTERMH 
      XI(I)=-X(17-I) 
      WI(I)=W(17-I) 
      XI(I+16)=X(I) 
      WI(I+16)=W(I) 
    1 END DO 
      DO 2 I=1,NTERMS 
    2 XX(I)=0.5D0*(XI(I)+1.0D0) 
      XX(33)=1.0D0 
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE AURGAM(I,NDRV,X,S,ANS) 
      DIMENSION F1(32),F2(32),F3(32) 
      DIMENSION AF(10),AS(10) 
      DIMENSION CALCO(8,20,32) 
      COMMON/W5051I7/CALCO 
      DATA DELTA/0.8000E-01/ 
      ANS=0. 
      IF(X.GT.0.9985) RETURN 
      N=3 
      IS=S/DELTA+1 
      IF(IS.GE.17) IS=17 
      IS1=IS+1 
      IS2=IS1+1 
      DO 1 L=1,32 
      KL=L+32*NDRV 
      F1(L)=CALCO(I,IS,KL) 
      F2(L)=CALCO(I,IS1,KL) 
      F3(L)=CALCO(I,IS2,KL) 
    1 END DO 
      AF(1)=AFGETFV(X,F1) 
      AF(2)=AFGETFV(X,F2) 
      AF(3)=AFGETFV(X,F3) 
      AS(1)=(IS-1)*DELTA 
      AS(2)=AS(1)+DELTA 
      AS(3)=AS(2)+DELTA 
      CALL AFPOLIN(AS,AF,N,S,AANS,DY) 
      ANS=AANS 
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE AFGINT(I,NDRV,X,S,ANS) 
      DIMENSION F1(32),F2(32),F3(32) 
      DIMENSION AF(10),AS(10) 
      DIMENSION CALCO(8,20,32) 
      COMMON/W5051IA/CALCO 
      DATA DELTA/0.8000E-01/ 
      ANS=0. 
      IF(X.GT.0.9985) RETURN 
      N=3 
      IS=S/DELTA+1 
!     IF(IS.GE.17) IS=17                                                
      IS1=IS+1 
      IS2=IS1+1 
      DO 1 L=1,32 
      KL=L+32*NDRV 
      F1(L)=CALCO(I,IS,KL) 
      F2(L)=CALCO(I,IS1,KL) 
      F3(L)=CALCO(I,IS2,KL) 
    1 END DO 
      AF(1)=AFGETFV(X,F1) 
      AF(2)=AFGETFV(X,F2) 
      AF(3)=AFGETFV(X,F3) 
      AS(1)=(IS-1)*DELTA 
      AS(2)=AS(1)+DELTA 
      AS(3)=AS(2)+DELTA 
      CALL AFPOLIN(AS,AF,N,S,AANS,DY) 
      ANS=AANS 
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE AFGIN2(I,NDRV,X,S,ANS) 
      DIMENSION F1(32),F2(32),F3(32) 
      DIMENSION AF(10),AS(10) 
      DIMENSION CELCO(8,20,32) 
      COMMON/W5051IB/CELCO 
      DATA DELTA/0.8000E-01/ 
      ANS=0. 
      IF(X.GT.0.9985) RETURN 
      N=3 
      IS=S/DELTA+1 
!     IF(IS.GE.17) IS=17                                                
      IS1=IS+1 
      IS2=IS1+1 
      DO 1 L=1,32 
      KL=L+32*NDRV 
      F1(L)=CELCO(I,IS,KL) 
      F2(L)=CELCO(I,IS1,KL) 
      F3(L)=CELCO(I,IS2,KL) 
    1 END DO 
      AF(1)=AFGETFV(X,F1) 
      AF(2)=AFGETFV(X,F2) 
      AF(3)=AFGETFV(X,F3) 
      AS(1)=(IS-1)*DELTA 
      AS(2)=AS(1)+DELTA 
      AS(3)=AS(2)+DELTA 
      CALL AFPOLIN(AS,AF,N,S,AANS,DY) 
      ANS=AANS 
      RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      FUNCTION AFCPLU(X,Q2) 
      CMS=1.5**2 
      BETS=1-4.*CMS*X/(1.-X)/Q2 
      IF(BETS.LE..0) THEN 
         AFCPLU=.0 
         RETURN 
      ENDIF 
      BETA=SQRT(BETS) 
      CPLU=(8.*X*(1.-X)-1.-4.*CMS*X*(1.-X)/Q2)*BETA 
      CAU=X**2+(1.-X)**2+4.*CMS*X*(1.-3.*X)/Q2-8.*CMS**2*X**2/Q2**2 
      CPLU=CPLU+CAU* LOG((1.+BETA)/(1.-BETA)) 
      AFCPLU=3.*(4./9.)*CPLU*X/(3.1415*137.) 
    1 RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
       FUNCTION AFGETFV(X,FVL) 
!  NOUVEAU PROGRAMME D'INTERPOLATION UTILISANT UNE ROUTINE DE MATH. RECI
       DIMENSION FVL(32) 
       double precision                                                 &
     &                XI(32),WI(32),XX(33)                              
       COMMON/W5051I9/XI,WI,XX,NTERMS 
       DIMENSION A(10),B(10) 
       N=4 
       EPS=1.E-7 
       XAM=XX(1)-EPS 
       XAP=XX(1)+EPS 
!      IF(X.LT.XAM) PRINT*,' X = ',X                                    
       IF(X.GT.XAM.AND.X.LT.XAP) GOTO 50 
       GOTO 80 
   50  Y=FVL(1) 
       GOTO 77 
   80  IF(X.LT.XX(2)) GOTO 51 
       IF(X.GT.XX(30)) GOTO 61 
       DO 1 I=3,30 
       IF(X.GT.XX(I)) GOTO 1 
       A(1)=XX(I-2) 
       A(2)=XX(I-1) 
       A(3)=XX(I) 
       A(4)=XX(I+1) 
       B(1)=FVL(I-2) 
       B(2)=FVL(I-1) 
       B(3)=FVL(I) 
       B(4)=FVL(I+1) 
       GOTO 70 
    1  CONTINUE 
   61  A(1)=XX(29) 
       A(2)=XX(30) 
       A(3)=XX(31) 
       A(4)=XX(32) 
       B(1)=FVL(29) 
       B(2)=FVL(30) 
       B(3)=FVL(31) 
       B(4)=FVL(32) 
       GOTO 70 
   51  A(1)=XX(1) 
       A(2)=XX(2) 
       A(3)=XX(3) 
       A(4)=XX(4) 
       B(1)=FVL(1) 
       B(2)=FVL(2) 
       B(3)=FVL(3) 
       B(4)=FVL(4) 
! 70   IF(X.GT..2.AND.X.LT..8) THEN                                     
!            CALL AFPOLIN(A,B,N,X,Y,DY)                                 
!      ELSE                                                             
!            CALL AFRATIN(A,B,N,X,Y,DY)                                 
!      ENDIF                                                            
   70  CONTINUE 
             CALL AFPOLIN(A,B,N,X,Y,DY) 
   77  AFGETFV=Y 
       RETURN 
      END                                           
!ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      SUBROUTINE AFPOLIN(XA,YA,N,X,Y,DY) 
      PARAMETER (NMAX=10) 
      DIMENSION XA(NMAX),YA(NMAX),C(NMAX),D(NMAX) 
      Y=0. 
      IF(N.GT.NMAX) RETURN 
      NS=1 
      DIF=ABS(X-XA(1)) 
      DO 11 I=1,N 
        DIFT=ABS(X-XA(I)) 
        IF (DIFT.LT.DIF) THEN 
          NS=I 
          DIF=DIFT 
        ENDIF 
        C(I)=YA(I) 
        D(I)=YA(I) 
   11 END DO 
      Y=YA(NS) 
      NS=NS-1 
      DO 13 M=1,N-1 
        DO 12 I=1,N-M 
          HO=XA(I)-X 
          HP=XA(I+M)-X 
          W=C(I+1)-D(I) 
          DEN=HO-HP 
!         IF(DEN.EQ.0.)PAUSE                                            
          DEN=W/DEN 
          D(I)=HP*DEN 
          C(I)=HO*DEN 
   12   CONTINUE 
        IF (2*NS.LT.N-M)THEN 
          DY=C(NS+1) 
        ELSE 
          DY=D(NS) 
          NS=NS-1 
        ENDIF 
        Y=Y+DY 
   13 END DO 
      RETURN 
      END                                           
