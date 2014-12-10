#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "pdflib.h"
#include "wishart.h"
#include "rnglib.h"
#include<R.h>
#include<Rinternals.h>
#include<Rmath.h>

SEXP jomo1ranmix(SEXP Y, SEXP Yimp, SEXP Yimp2, SEXP Yimpcat, SEXP X, SEXP Z, SEXP clus, SEXP beta, SEXP u, SEXP betapost, SEXP upost, SEXP omega, SEXP omegapost, SEXP covu, SEXP covupost, SEXP nstep, SEXP Sp, SEXP Sup, SEXP Y_numcat, SEXP num_con, SEXP flagrng){
int indic=0,i,j,k, IY,JY, IX, JX, Io, Jo, Ib, Jb, ns, nmiss=0,t, countm=0, counto=0, countmm=0, countmo=0,countoo=0, jj, tt, kk, ncon,ncat, pos,flag=0,nmaxx,h;
int Iu, Ju, IZ, JZ, nj,c,fl;
SEXP RdimY, RdimX, Rdimo, Rdimb, RdimZ, Rdimu;
double *betaX, *Yobs, *Ymiss, *mumiss, *omegadrawmiss, *betamiss, *betaobs, *omegaoo, *omegamo, *omegamm, *invomega, *invomega2, *help, *help2, *help3, *imp, *zi;
double *sumzy, *incrzz, *incrzy, *mu, *mu2, *newbeta, *newomega, *sumzi, *yi, *invomega3, *help4, *help5, *help6, *missing, *fixomega,meanom,sdom, *resid, logLH, newlogLH,detom;
double maxx,maxim,maxim2, *sumxy, *sumxi, *uj, *xi, *ziu, *incrxx, *incrxy, *newu, *mu3,  *mu4, *help7, *help8, *help9, *invomega4, *newomega2;

/* Protecting R objects from garbage collection and saving matrices dimensions*/ 

RdimY=PROTECT(getAttrib(Yimp,R_DimSymbol));
IY=INTEGER(RdimY)[0];
JY=INTEGER(RdimY)[1];
RdimX=PROTECT(getAttrib(X,R_DimSymbol));
IX=INTEGER(RdimX)[0];
JX=INTEGER(RdimX)[1];
RdimZ=PROTECT(getAttrib(Z,R_DimSymbol));
IZ=INTEGER(RdimZ)[0];
JZ=INTEGER(RdimZ)[1];
Rdimb=PROTECT(getAttrib(beta,R_DimSymbol));
Ib=INTEGER(Rdimb)[0];
Jb=INTEGER(Rdimb)[1];
Rdimo=PROTECT(getAttrib(omega,R_DimSymbol));
Io=INTEGER(Rdimo)[0];
Jo=INTEGER(Rdimo)[1];
Rdimu=PROTECT(getAttrib(u,R_DimSymbol));
Iu=INTEGER(Rdimu)[0];
Ju=INTEGER(Rdimu)[1];
Y=PROTECT(coerceVector(Y,REALSXP));
Yimpcat=PROTECT(coerceVector(Yimpcat,REALSXP));
Y_numcat=PROTECT(coerceVector(Y_numcat,INTSXP));
Yimp=PROTECT(coerceVector(Yimp,REALSXP));
Yimp2=PROTECT(coerceVector(Yimp2,REALSXP));
X=PROTECT(coerceVector(X,REALSXP));
Z=PROTECT(coerceVector(Z,REALSXP));
clus=PROTECT(coerceVector(clus,INTSXP));
beta=PROTECT(coerceVector(beta,REALSXP));
u=PROTECT(coerceVector(u,REALSXP));
upost=PROTECT(coerceVector(upost,REALSXP));
betapost=PROTECT(coerceVector(betapost,REALSXP));
omega=PROTECT(coerceVector(omega,REALSXP));
covu=PROTECT(coerceVector(covu,REALSXP));
omegapost=PROTECT(coerceVector(omegapost,REALSXP));
covupost=PROTECT(coerceVector(covupost,REALSXP));
Sp=PROTECT(coerceVector(Sp,REALSXP));
Sup=PROTECT(coerceVector(Sup,REALSXP));
nstep=PROTECT(coerceVector(nstep,INTSXP));
ns=INTEGER(nstep)[0];
num_con=PROTECT(coerceVector(num_con,INTSXP));
ncon=INTEGER(num_con)[0];
flagrng=PROTECT(coerceVector(flagrng,INTSXP));
fl=INTEGER(flagrng)[0];
ncat=length(Y_numcat);
nj=Iu;

/*Allocating memory for C objects in R*/

help = ( double * ) R_alloc ( JY * JY , sizeof ( double ) );
invomega= (double * ) R_alloc ( JY * JY , sizeof ( double ) );
fixomega = ( double * ) R_alloc ( JY * JY , sizeof ( double ) );
sumzi = ( double * ) R_alloc ( JY * JZ * JY * JZ , sizeof ( double ) );
sumzy = ( double * ) R_alloc ( JY * JZ , sizeof ( double ) );
sumxi = ( double * ) R_alloc ( JY * JX * JY * JX , sizeof ( double ) );
sumxy = ( double * ) R_alloc ( JY * JX , sizeof ( double ) );
zi = ( double * ) R_alloc ( JY * JZ * JY , sizeof ( double ) );
xi = ( double * ) R_alloc ( JY * JX * JY , sizeof ( double ) );
yi = ( double * ) R_alloc ( JY , sizeof ( double ) );
uj = ( double * ) R_alloc ( JY * JZ , sizeof ( double ) );
newu = ( double * ) R_alloc ( JY * JZ , sizeof ( double ) );
ziu = ( double * ) R_alloc ( JY , sizeof ( double ) );
help2 = ( double * ) R_alloc ( JY *JX * JY , sizeof ( double ) );
incrzz = ( double * ) R_alloc ( JY *JZ * JY *JZ , sizeof ( double ) );
incrzy = ( double * ) R_alloc ( JY *JZ , sizeof ( double ) );
incrxx = ( double * ) R_alloc ( JY *JX * JY *JX , sizeof ( double ) );
incrxy = ( double * ) R_alloc ( JY *JX , sizeof ( double ) );
help3 = ( double * ) R_alloc ( JY * JX * JY * JX ,sizeof ( double ) );
invomega2= (double * ) R_alloc ( JY * JX * JY * JX , sizeof ( double ) );
mu = ( double * ) R_alloc ( JY * JX, sizeof ( double ) );
newbeta = ( double * ) R_alloc ( JY * JX ,sizeof ( double ) );
mu2 = ( double * ) R_alloc ( JY * JZ, sizeof ( double ) );
mu3 = ( double * ) R_alloc ( JY * JY * JZ * JZ ,sizeof ( double ) );
mu4 = ( double * ) R_alloc ( JY * JY,sizeof ( double ) );
newomega = ( double * ) R_alloc ( JY * JY , sizeof ( double ) );
newomega2 = ( double * ) R_alloc ( JY * JY * JZ * JZ , sizeof ( double ) );
betaX=( double * ) R_alloc ( JY , sizeof ( double ) );
imp=( double * ) R_alloc ( IY * JY,sizeof ( double ) );
resid=( double * ) R_alloc ( IY * JY,sizeof ( double ) );
Yobs=( double * ) R_alloc ( JY , sizeof ( double ) );
Ymiss=( double * ) R_alloc ( JY , sizeof ( double ) );
mumiss = ( double * ) R_alloc ( JY , sizeof ( double ) );
omegadrawmiss = ( double * ) R_alloc ( JY * JY ,sizeof ( double ) );
betamiss = ( double * ) R_alloc ( JY ,sizeof ( double ) );
betaobs = ( double * ) R_alloc ( JY, sizeof ( double ) );
omegaoo= ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
omegamo= ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
omegamm= ( double * ) R_alloc ( JY*JY , sizeof ( double ) );
invomega3= ( double * ) R_alloc ( JY*JY * JZ * JZ , sizeof ( double ) );
invomega4 = ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
help4 = ( double * ) R_alloc ( JY *JY *JZ*JZ , sizeof ( double ) );
help5 = ( double * ) R_alloc ( JY * JY , sizeof ( double ) );
help6 = ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
help7 = ( double * ) R_alloc ( JY*JY , sizeof ( double ) );
help8 = ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
help9 = ( double * ) R_alloc ( JY *JY , sizeof ( double ) );
missing = ( double * ) R_alloc ( IY , sizeof ( double ) );

/* Some initializations */

for (j=0; j<IY; j++) {
	missing[j]=0;
	for (k=0;k<JY;k++) {
		if (ISNAN(REAL(Yimp)[j+k*IY])) {
			missing[j]++;
		}
	}
}
r8mat_copy_new(IY, JY, REAL(Yimp2), imp);
for (i=0;i<Ib*Jb;i++) REAL(betapost)[i]=0;
for (i=0;i<JY*JY;i++) fixomega[i]=0;
for (i=0;i<JY*JY;i++) newomega[i]=REAL(omega)[i];
pos=ncon;
for (i=0;i<ncat;i++) {
	for (j=0;j<(INTEGER(Y_numcat)[i]-1);j++) {
		for (k=0;k<(INTEGER(Y_numcat)[i]-1);k++) {
			if (j==k) REAL(omega)[(pos+j)+Jo*(pos+k)]=1;
			else REAL(omega)[(pos+j)+Jo*(pos+k)]=0;
			fixomega[(pos+j)+Jo*(pos+k)]=1;
		}
	}
	pos=pos+INTEGER(Y_numcat)[i]-1;
}
GetRNGstate();

/* Running ns iterations of Gibbs sampler*/

for (i=0;i<ns;i++) {

	// Rejection Sampling

	pos=ncon;
	for (j=0;j<ncat;j++) {
		for (t=0;t<IY;t++) {
			if (!ISNAN(REAL(Y)[t+(ncon+j)*IY])) {
				for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) betaX[k]=0;
				for (tt=0;tt<JX;tt++) {
					for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) {
						betaX[k]=betaX[k]+REAL(beta)[tt+(k+pos)*Ib]*REAL(X)[t+tt*IX];
					}
				}
				for (tt=0;tt<JZ;tt++) {
					for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) {
						betaX[k]=betaX[k]+REAL(u)[(INTEGER(clus)[t])+nj*(tt+(k+pos)*JZ)]*REAL(Z)[t+tt*IZ];
					}
				}


				for (k=0;k<pos;k++) help[k]=0;
				for (tt=0;tt<JX;tt++) {
					for (k=0;k<pos;k++) {
						help[k]=help[k]+REAL(beta)[tt+k*Ib]*REAL(X)[t+tt*IX];
					}
				}
				for (tt=0;tt<JZ;tt++) {
					for (k=0;k<pos;k++) {
						help[k]=help[k]+REAL(u)[(INTEGER(clus)[t])+nj*(tt+k*JZ)]*REAL(Z)[t+tt*IZ];
					}
				}

				for (k=0;k<pos;k++) {
					help[k]=imp[t+k*IY]-help[k];
					for (kk=0;kk<pos;kk++) help4[k+pos*kk]=REAL(omega)[k+JY*kk];
					for (kk=0;kk<(INTEGER(Y_numcat)[j]-1);kk++) help5[k+pos*kk]=REAL(omega)[k+JY*(pos+kk)];
				}
				r8mat_pofac(pos,help4, help6,1);
				r8mat_poinv(pos,help6, invomega);
				for (jj=1;jj<pos;jj++) for (tt=0;tt<jj;tt++) invomega[jj+pos*tt]=invomega[tt+pos*jj];
				r8mat_mm_new(pos,pos,(INTEGER(Y_numcat)[j]-1),invomega,help5, help2);
				r8mat_mm_new(1,pos,(INTEGER(Y_numcat)[j]-1),help,help2, mumiss);
				r8mat_add((INTEGER(Y_numcat)[j]-1),1,betaX,mumiss);
				r8mat_mtm_new((INTEGER(Y_numcat)[j]-1),pos,(INTEGER(Y_numcat)[j]-1),help2,help5, omegadrawmiss);
				r8mat_divide((INTEGER(Y_numcat)[j]-1),(INTEGER(Y_numcat)[j]-1),-1,omegadrawmiss);
				for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) omegadrawmiss[k+k*(INTEGER(Y_numcat)[j]-1)]=omegadrawmiss[k+k*(INTEGER(Y_numcat)[j]-1)]+1;
				r8mat_pofac((INTEGER(Y_numcat)[j]-1),omegadrawmiss, omegamm,2);
				flag=0;
				kk=0;
				if (REAL(Y)[t+(ncon+j)*IY]==INTEGER(Y_numcat)[j]) {
					while ((flag==0)&(kk<10000)) {
						r8vec_multinormal_sample((INTEGER(Y_numcat)[j]-1), mumiss,omegamm, newbeta,mu4,fl);
						maxim=maxvec((INTEGER(Y_numcat)[j]-1),newbeta);
						if (maxim<0) {
							for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) imp[t+(k+pos)*IY]=newbeta[k];
							flag=1;
							indic++;
						}
						kk++;
					}
				}
				else {
					while ((flag==0)&(kk<10000)) {
						r8vec_multinormal_sample((INTEGER(Y_numcat)[j]-1), mumiss,omegamm, newbeta,mu4,fl);
						maxim=argmaxvec((INTEGER(Y_numcat)[j]-1),newbeta);
						maxim2=maxvec((INTEGER(Y_numcat)[j]-1),newbeta);
						if (((maxim+1)==REAL(Y)[t+(ncon+j)*IY])&(maxim2>0)) {
							for (k=0;k<(INTEGER(Y_numcat)[j]-1);k++) imp[t+(k+pos)*IY]=newbeta[k];
							flag=1;
							indic++;
						}
						kk++;
					}
				}
				flag=0;

			}
		}
		pos=pos+INTEGER(Y_numcat)[j]-1;
	}
//Rprintf("Fin qui tutto ok, iter= %d \n",i);

	//Updating beta

	r8mat_pofac(JY,REAL(omega), help,3);
	r8mat_poinv(JY,help, invomega);
	for (jj=1;jj<Io;jj++) for (tt=0;tt<jj;tt++) invomega[jj+Io*tt]=invomega[tt+Io*jj];
	for (j=0;j<JY*JY*JX*JX;j++) sumxi[j]=0;	
	for (j=0;j<JY*JX;j++) sumxy[j]=0;
	for (j=0;j<IY;j++) {
		for (t=0;t<JY*JZ*JY;t++) zi[t]=0;
		for (t=0;t<JY*JX*JY;t++) xi[t]=0; 
		for (t=0;t<JY;t++) {
			yi[t]=imp[j+t*IY];
			for (k=0;k<JX;k++) {
				xi[t+(k+t*JX)*JY]=REAL(X)[j+IY*k];
			}
			for (k=0;k<JZ;k++) {
				zi[t+(k+t*JZ)*JY]=REAL(Z)[j+IY*k];
				uj[k+t*JZ]=REAL(u)[(INTEGER(clus)[j])+nj*(k+t*JZ)];
			}
		}
		r8mat_mtm_new(JY*JX,JY,JY,xi,invomega,help2);
		r8mat_mm_new(JY*JX,JY,JX*JY,help2,xi,incrxx);
		r8mat_mm_new(JY,JY*JZ,1,zi,uj,ziu);
		r8mat_divide(JY,1,-1,ziu);
		r8mat_add(JY,1,ziu,yi);
		r8mat_mm_new(JY*JX,JY,1,help2,yi,incrxy);
		r8mat_add(JY*JX,JY*JX,incrxx,sumxi);
		r8mat_add(JY*JX,1,incrxy,sumxy);
	
	}
	
	r8mat_pofac(JY * JX,sumxi,help3,4);
	r8mat_poinv(JY * JX, help3,invomega2);
	for (jj=1;jj<JX*JY;jj++) for (tt=0;tt<jj;tt++) invomega2[jj+JX*JY*tt]=invomega2[tt+JX*JY*jj];
	r8mat_mm_new(JY*JX,JY*JX,1,invomega2,sumxy,mu);
	r8mat_pofac(JY * JX,invomega2,help3,5);
	r8vec_multinormal_sample(JY*JX, mu,help3, REAL(beta),newbeta,fl);
	r8mat_add(Ib,Jb,REAL(beta),REAL(betapost));

	//Updating random effects

	for (c=0;c<nj;c++) {
		r8mat_pofac(JY,REAL(omega),help,6);
		r8mat_poinv(JY,help,invomega);
		for (jj=1;jj<Io;jj++) for (tt=0;tt<jj;tt++) invomega[jj+Io*tt]=invomega[tt+Io*jj];
		for (j=0;j<JY*JY*JZ*JZ;j++) sumzi[j]=0;
		for (j=0;j<JY*JZ;j++) sumzy[j]=0;
		for (j=0;j<IY;j++) {
			if (INTEGER(clus)[j]==c) {
				for (t=0;t<JY*JZ*JY;t++) zi[t]=0;
				for (t=0;t<JY*JX*JY;t++) xi[t]=0; 
				for (t=0;t<JY;t++) {
					yi[t]=imp[j+t*IY];
					for (k=0;k<JX;k++) {
						xi[t+(k+t*JX)*JY]=REAL(X)[j+IX*k];
					}
					for (k=0;k<JZ;k++) {
						zi[t+(k+t*JZ)*JY]=REAL(Z)[j+IZ*k];
					}
				}
				r8mat_mtm_new(JY*JZ,JY,JY,zi,invomega,help4);
				r8mat_mm_new(JY*JZ,JY,JZ*JY,help4,zi,incrzz);
				r8mat_mm_new(JY,JY*JX,1,xi,REAL(beta),ziu);
				r8mat_divide(JY,1,-1,ziu);
				r8mat_add(JY,1,ziu,yi);
				r8mat_mm_new(JY*JZ,JY,1,help4,yi,incrzy);
				r8mat_add(JY*JZ,JY*JZ,incrzz,sumzi);
				r8mat_add(JY*JZ,1,incrzy,sumzy);
				
			}
		}
		
		r8mat_pofac(JY * JZ,REAL(covu),help4,7);
		r8mat_poinv(JY * JZ, help4,invomega3);
		for (jj=1;jj<JZ*JY;jj++) for (tt=0;tt<jj;tt++) invomega3[jj+JZ*JY*tt]=invomega3[tt+JZ*JY*jj];
		r8mat_add(JY*JZ,JY*JZ,invomega3,sumzi);
		
		r8mat_pofac(JY * JZ,sumzi,help4,8);
		r8mat_poinv(JY * JZ, help4,invomega3);
		for (jj=1;jj<JZ*JY;jj++) for (tt=0;tt<jj;tt++) invomega3[jj+JZ*JY*tt]=invomega3[tt+JZ*JY*jj];
		r8mat_mm_new(JY*JZ,JY*JZ,1,invomega3,sumzy,mu2);

		r8mat_pofac(JY * JZ,invomega3,help4,9); 
		r8vec_multinormal_sample(JY*JZ, mu2,help4,newu, incrzy,fl);
		for (t=0;t<JY;t++) for (k=0;k<JZ;k++) REAL(u)[c+nj*(k+t*JZ)] = newu[k+t*JZ];
		
	}
	r8mat_add(Iu,Ju,REAL(u),REAL(upost));

	//Updating level 2 covariance matrix


	for (j=0;j<JY*JY*JZ*JZ;j++) mu3[j]=0;
	for (j=0;j<nj; j++) {
		for (t=0;t<JY;t++) for (k=0;k<JZ;k++) uj[k+t*JZ]=REAL(u)[j+nj*(k+t*JZ)];
		r8mat_mmt_new(JY*JZ,1,JY*JZ,uj,uj,help4);
		r8mat_add(JY*JZ,JY*JZ,help4,mu3);	
	}
	r8mat_add(JY*JZ,JY*JZ,REAL(Sup),mu3);

	r8mat_pofac(JY*JZ,mu3,help4,10);
	r8mat_poinv(JY*JZ, help4, invomega3);
	for (jj=1;jj<(JY*JZ);jj++) for (tt=0;tt<jj;tt++) invomega3[jj+(JZ*JY)*tt]=invomega3[tt+(JZ*JY)*jj];

	wishart_sample(JY*JZ,nj-1,invomega3,newomega2, help4,sumzi,incrzz,mu3,fl);
	
	r8mat_pofac(JY * JZ,newomega2, help4,11);
	r8mat_poinv(JY * JZ, help4,invomega3);
	for (jj=1;jj<(JY*JZ);jj++) for (tt=0;tt<jj;tt++) invomega3[jj+(JZ*JY)*tt]=invomega3[tt+(JZ*JY)*jj];
	for(k=0;k<(JY*JZ);k++)  for(j=0;j<(JY*JZ);j++)  REAL(covu)[j+(JZ*JY)*k]=invomega3[j+(JZ*JY)*k];
	
	r8mat_add(JY*JZ,JY*JZ,REAL(covu),REAL(covupost));


	//Updating residuals

	for (t=0;t<IY;t++) {
		for (j=0; j<JY; j++) {
			resid[t+j*IY]=imp[t+j*IY];
			for (k=0;k<JX;k++) resid[t+j*IY]=resid[t+j*IY]-REAL(beta)[k+j*JX]*REAL(X)[t+k*IX];
			for (k=0;k<JZ;k++) resid[t+j*IY]=resid[t+j*IY]-REAL(u)[INTEGER(clus)[t]+nj*(k+j*JZ)]*REAL(Z)[t+k*IZ];
		}
	}

	//Updating omega
	

	flag=0;
	r8mat_pofac(JY,newomega,help,12);
	detom=r8mat_podet(JY, help);
	r8mat_poinv(JY, help,invomega);
	for (jj=1;jj<Io;jj++) for (tt=0;tt<jj;tt++) invomega[jj+Io*tt]=invomega[tt+Io*jj];
	logLH=0;
	for (t=0;t<IY;t++) {
		for (jj=0;jj<JY;jj++) help4[jj]=resid[t+jj*IY];
		r8mat_mm_new(1,JY,JY,help4,invomega,help5);
		r8mat_mmt_new(1,JY,1,help5,help4,help6);
		logLH=logLH+help6[0];
	}
	logLH=logLH*(-0.5)-IY*detom/2;
	for (j=0;j<JY;j++) {
		for (k=0;k<JY;k++) {
			if ((fixomega[j+JY*k]==0)&(j<=k)) {
				if (j==k) {
					sdom=REAL(omega)[j+JY*k]*sqrt(11.6/IY);
					meanom=REAL(omega)[j+JY*k];
				}
				else {
					sdom=0.1*sqrt(REAL(omega)[j+JY*j]*REAL(omega)[k+JY*k]);
					meanom=REAL(omega)[j+JY*k];
				}
				kk=0;
				while ((flag==0)&(kk<100)) {
					newomega[j+JY*k]=r8_normal_sample(meanom,sdom,fl);
					newomega[k+JY*j]=newomega[j+JY*k];
					flag=checkposdef(JY,newomega,help,help5);
					kk++;
				}
				r8mat_pofac(JY,newomega,help,13);
				detom=r8mat_podet(JY, help);
				r8mat_poinv(JY, help,invomega);
				for (jj=1;jj<Io;jj++) for (tt=0;tt<jj;tt++) invomega[jj+Io*tt]=invomega[tt+Io*jj];
				newlogLH=0;
				for (t=0;t<IY;t++) {
					for (jj=0;jj<JY;jj++) help4[jj]=resid[t+jj*IY];
					r8mat_mm_new(1,JY,JY,help4,invomega,help5);
					r8mat_mmt_new(1,JY,1,help5,help4,help6);
					newlogLH=newlogLH+help6[0];
				}
				newlogLH=newlogLH*(-0.5)-IY*detom/2;
				if (((( double ) unif_rand ( ) / ( double ) RAND_MAX)<exp(newlogLH-logLH))&(flag==1)) {
					REAL(omega)[j+JY*k]=newomega[j+JY*k];
					REAL(omega)[k+JY*j]=newomega[k+JY*j];
					logLH=newlogLH;
				}
				else {
					newomega[j+JY*k]=REAL(omega)[j+JY*k];
					newomega[k+JY*j]=REAL(omega)[k+JY*j];
				}
				flag=0;
			}
		}
	}
	r8mat_add(JY,JY,REAL(omega),REAL(omegapost));
	

	//imputing missing values

	for (j=0; j<IY; j++) {
		for (k=0;k<JY;k++) betaX[k]=0;		
		for (t=0;t<JY*JZ*JY;t++) zi[t]=0;
		for (t=0;t<JY*JX*JY;t++) xi[t]=0; 
		for (t=0;t<JY;t++) {
			yi[t]=imp[j+t*IY];
			for (k=0;k<JX;k++) {
				xi[t+(k+t*JX)*JY]=REAL(X)[j+IY*k];
			}
			for (k=0;k<JZ;k++) {
				zi[t+(k+t*JZ)*JY]=REAL(Z)[j+IY*k];
				uj[k+t*JZ]=REAL(u)[(INTEGER(clus)[j])+nj*(k+t*JZ)];
			}
		}
		r8mat_mm_new(JY,JY*JZ,1,zi,uj,ziu);
		r8mat_mm_new(JY,JY*JX,1,xi,REAL(beta),help6);
		r8mat_add(JY,1,ziu,betaX);
		r8mat_add(JY,1,help6,betaX);
		
		nmiss=missing[j];
		if (nmiss>0) {
			for (k=0;k<JY;k++) {
				if (ISNAN(REAL(Yimp)[j+k*IY])) {
					betamiss[countm]=betaX[k];
					countm++;
				}
				else {
					Yobs[counto]=imp[j+k*IY];
					betaobs[counto]=betaX[k];
					counto++;
				}
				for (t=0;t<JY;t++) {
					if (ISNAN(REAL(Yimp)[j+k*IY])&ISNAN(REAL(Yimp)[j+t*IY])) {
						omegamm[countmm]=REAL(omega)[k+t*Io];
						countmm++;
					}
					else if (ISNAN(REAL(Yimp)[j+t*IY])) {
						omegamo[countmo]=REAL(omega)[k+t*Io];
						countmo++;	
					}
					else if (!ISNAN(REAL(Yimp)[j+k*IY])&!ISNAN(REAL(Yimp)[j+t*IY])){
						omegaoo[countoo]=REAL(omega)[k+t*Io];
						countoo++;	
					}
				}
			}
			r8mat_pofac((JY-nmiss),omegaoo,help7,14);
			r8mat_poinv((JY-nmiss),help7,invomega4);
			for (jj=1;jj<JY-nmiss;jj++) for (tt=0;tt<jj;tt++) invomega4[jj+(JY-nmiss)*tt]=invomega4[tt+(JY-nmiss)*jj];
			r8mat_mmt_new((JY-nmiss),(JY-nmiss),nmiss,invomega4,omegamo,help8);
			r8mat_divide((JY-nmiss),1,-1,betaobs);
			r8mat_add((JY-nmiss),1,betaobs,Yobs);
			r8mat_mtm_new(1,(JY-nmiss),nmiss,Yobs,help8,mumiss);
			r8mat_add(1,nmiss,betamiss,mumiss);
			r8mat_mm_new(nmiss,(JY-nmiss),nmiss,omegamo,help8,omegadrawmiss);
			r8mat_divide(nmiss,nmiss,-1,omegadrawmiss);
			r8mat_add(nmiss,nmiss,omegamm,omegadrawmiss);
			r8mat_pofac(nmiss,omegadrawmiss,help9,15);
			r8vec_multinormal_sample(nmiss,mumiss,help9,Ymiss,help6,fl);
			countm=0;
			for (k=0;k<JY;k++) {
				if (ISNAN(REAL(Yimp)[j+k*IY])) {
					imp[j+k*IY]=Ymiss[countm];
					countm++;	
				}	
			}
			nmiss=0;
			countm=0;
			counto=0;
			countmm=0;
			countmo=0;
			countoo=0;
			
		}
	
	}
	if ((i+1)%10==0) Rprintf("Iteration %d completed\n",i+1);
}
for(i=0;i<IY;i++)  {
	for(j=0;j<JY;j++)  {
		REAL(Yimp2)[i+IY*j]=imp[i+IY*j];
	}
	h=0;
	for (j=0;j<ncat;j++) {
		maxx=imp[i+(ncon+h)*IY];
		nmaxx=0;
		for (k=1;k<(INTEGER(Y_numcat)[j]-1);k++) {
			if (imp[i+(ncon+h+k)*IY]>maxx) {
				maxx=imp[i+(ncon+h+k)*IY];
				nmaxx=k;
			}
		}
		if (maxx>0) REAL(Yimpcat)[i+IY*j]=nmaxx+1;
		else REAL(Yimpcat)[i+IY*j]=INTEGER(Y_numcat)[j];
		h=h+INTEGER(Y_numcat)[j]-1;
	}

}

r8mat_divide(Ib,Jb,ns,REAL(betapost));
r8mat_divide(Iu,Ju,ns,REAL(upost));
r8mat_divide(JY,JY,ns,REAL(omegapost));
r8mat_divide(JY*JZ,JY*JZ,ns,REAL(covupost));
PutRNGstate();
UNPROTECT(27);
return R_NilValue;
}