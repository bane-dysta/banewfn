# 3.27 (Hyper)polarizability analysis (24)

## Quick navigation

- path: 3  Functions > 3.27 (Hyper)polarizability analysis (24)
- pdf pages: 350-365
- category: functions
- main menu / option numbers mentioned in title: 24
- direct child sections: 4
- total descendant sections: 6

## When to consult this file

Use this file for the actual Multiwfn function description: menu number, theory, options, inputs, and detailed behavior.

## Keywords and aliases

3.27, Hyper, polarizability, 3.27.1, Parse, output, task, Gaussian, evaluate, relevant, quantities, 3.27.2, sum-over-states, SOS, method, two-, or, three-level, model, analyses, 3.27.3, density, 3.27.5, Visualize, via, unit, sphere, vector, representations

## Direct child sections

- **3.27.1 Parse output of (hyper)polarizability task of Gaussian and evaluate relevant quantities** (pdf pages 350-355)
- **3.27.2 Study (hyper)polarizability by sum-over-states (SOS) method and two- or three-level model analyses** (pdf pages 356-360)
- **3.27.3 Study (hyper)polarizability density** (pdf pages 361-363)
- **3.27.5 Visualize (hyper)polarizability via unit sphere and vector representations** (pdf pages 364-365)

## Extracted manual text

3 Functions
respectively, that means atom A gained 0.3412 electron and atom B lost 0.2053 electron due to the
interaction characterized by NOCV pair 1. Note that diffuse functions should never be used since
SCPA method is incompatible with them. If you really have to use diffuse functions, after
performing ETS-NOCV analysis, you should return to main menu (in this case the orbitals in
memory is NOCV orbitals) and then use main function 8 to compute orbital composition via e.g.
Hirshfeld or Becke method, which works normally when diffuse functions are presented.
Option “-6 Manually define correspondence between NOCV pairs and orbitals” is sometimes
useful. Multiwfn automatically constructs NOCV pairs according to NOCV orbitals sorted by
eigenvalues. For symmetric systems (e.g. linear system Ne...BeO), some NOCV orbitals may be
degenerate, namely having exactly the same eigenvalues; in this case their order (index) is arbitrary,
and the automatically determined correspondence between NOCV pairs and orbitals may be
unexpected. In this case you can use the option -6 to choose a NOCV pair, and then manually input
indices of the two orbitals that the pair should correspond to. This option can use multiple times to
redefine multiple NOCV pairs.
Abundant examples of ETS-NOCV analysis are given in Section 4.23.

3.27 (Hyper)polarizability analysis (24)
Main function 24 is a collection of functions of studying polarizability and hyperpolarizability.
The subfunctions are described in this section. It is noteworthy that atomic polarizability can be
calculated by fuzzy analysis module, which is described in Section 3.18.12.

3.27.1 Parse output of (hyper)polarizability task of Gaussian and
evaluate relevant quantities
The output of (hyper)polarizability task of Gaussian (polar keyword) is difficult to understand,
at least for beginners. This function is used to parse these outputs and then print them in a more
readable format, and at the same time some quantities relating to (hyper)polarizability are outputted.
Currently this function is formally compatible with Gaussian 09 and 16.

Basic concepts and theory backgrounds
Energy of a system can be written as Taylor expansion with respect to uniform external electric
field F

E ( F) = E (0) +

E
1 2E
1 3E
1 4E
2
3
F+
F
+
F
+
F4 + 
2
3
4
F F=0
2 F F=0
6 F F=0
24 F F=0

1
1
1
1
1
 E (0) − μ 0 F − αF 2 − βF 3 −
γF 4 −
δF 5 −
εF 6 
2
6
24
120
720
μ0 = −

E
F F=0

α=−

2E
F 2 F=0

β=−

329

3E
F 3 F=0

γ=−

4E
F 4 F=0

--- page break ---

3 Functions
where 0 is permanent dipole moment, which is a vector;  is polarizability, which is a matrix
(second rank tensor);  is first hyperpolarizability, which is a third rank tensor and known as secondorder nonlinear optical response (NLO) coefficient;  is second hyperpolarizability, which is a fourth
rank tensor and known as third-order NLO coefficient. The higher terms such as  and  are very
unimportant and thus rarely discussed. The (hyper)polarizability tensors are directly correlated to
the frequency of external field F. If F has zero-frequency (static electric field), then the
(hyper)polarizabilities are known as static or frequency-independent ones. The dynamic or
frequency-dependent (hyper)polarizabilities correspond to those at external electromagnetic fields
with non-zero frequency.
• Polarizability ()
Dipole moment of a system in uniform electric field can be written as

μ=−

E
= μ 0 + αF
1 / 2)βF 2 + (1 / 6) γF 3 + 
 + (

 

F
μ1
μ
μ
2

3

The linear response of dipole moment with respect to F, namely the 1 term, can be explicitly
written as below

  x   xx

μ1 = α  F    y  =  yx
 
  z   zx

 xy
 yy
 zy

 xz   Fx 

 yz   Fy 
 
 zz   Fz 

The polarizability  is a symmetric matrix rather than a scalar, implying the difference of
polarizability in different directions. In order to facilitate comparison of overall polarizability
between various systems, it is convenient to define the isotropic average polarizability

  = Tr( α) / 3 = ( xx +  yy +  zz ) / 3
Anisotropy of polarizability can be defined in various ways:
Definition 1: See Chem. Phys., 410, 90 (2013) for example

 = [( xx −  yy ) 2 + ( xx −  zz ) 2 + ( yy −  zz ) 2 + 6( xy2 +  xz2 +  yz2 )] / 2
Definition 2: This definition is the most commonly use one, see J. Chem. Phys., 98, 3022 (1993)
for example

 = [( xx −  yy ) 2 + ( xx −  zz ) 2 + ( yy −  zz ) 2 ] / 2
Definition 3: {} stand for eigenvalues of  ranking from small to large

 =  3 − ( 1 +  2 ) / 2
The  value along each of the three Cartesian axes can be defined as

 x =  xx +  xy +  xz
 y =  yx +  yy +  yz
 z =  zx +  zy +  zz
• First hyperpolarizability ()
First hyperpolarizability  is a third rank tensor that can be described by a 333 matrix.
Gaussian is capable of calculating both static and dynamic . For the latter case, dc-Pockels form

330

--- page break ---

3 Functions
(-;,0) and SHG form (-2;,) can be evaluated.
It is worth to note that for i,j,k(-;,0), the i and j index can be freely exchanged, while for
i,j,k(-2;,), the j and k index can be freely exchanged, therefore the 3*3*3=27 components can
be reduced to 18 unique ones. For static case, viz. i,j,k(0;0,0), since all of the three indices are
exchangeable (known as Kleinman's symmetry, e.g. xyy=yxy=yyx …), only 10 components are
unique. The Kleinman's symmetry is only approximately applied to dynamic  at low-frequency
external field.
The  value in one of the three Cartesian axes can be calculated by the general equation

i = (1 / 3) (ijj +  jji +  jij )

i, j = {x, y, z}

j

The magnitude of  is defined as

 tot =  x2 +  y2 +  z2
prj (also known as vec) is the projection of  on dipole moment vector , it can be sampled
experimentally (electric field induced second harmonic generation (EFISHG) experiment). || is the
 component in the direction of 


prj =  i i
i | |

|| = (3 / 5)prj

Some people prefer to discuss the perpendicular and parallel components of  with respect to
Z axis, they are defined respectively as

 ⊥( Z ) = (1 / 5) (2 zjj − 3 jzj + 2 jjz )

||( Z ) = (3 / 5) z

j

For static case, we can explicitly write out  in x, y and z directions as

 x =  xxx +  xyy +  xzz
 y =  yxx +  yyy +  yzz
 z =  zxx +  zyy +  zzz
and  ⊥( Z ) = (1 / 5)  z .
• Second hyperpolarizability ()
Second hyperpolarizability  is a fourth rank tensor of 3333 form. Gaussian can calculate
its static limit form (0;0,0,0); while for dynamic case, Gaussian is capable of calculating its EOKO
(Electro-optic Kerr effect) form (-;,0,0) and SHG form (-2;,,0).
The i components of  is defined as

 i = (1 / 15) ( ijji +  ijij +  iijj ) i, j = {x, y, z}
j

The total magnitude of  is measured as  tot =

 x2 +  y2 +  z2 .

There are two definitions of average of , as shown below. Definition 1 is more common, and
it is equivalent to ||

def 1:   =  x +  y +  z
def 2 :   = (1 / 5)( xxxx +  yyyy +  zzzz +  xxyy +  xxzz +  yyzz +  yyxx +  zzxx +  zzyy )

331

--- page break ---

3 Functions
The _|_ is defined as

 ⊥ = (1 / 15) (2 ijji −  iijj ) i, j = {x, y, z}
i

j

Most of above-mentioned equations about  can be found in Chapter 5 of Reviews in
Computational Chemistry, Vol. 12 (1998).
• Hyper-Rayleigh scattering (HRS) and depolarization ratio (DR)
The Hyper-Rayleigh Scattering (HRS) technique was developed as an alternative method to
EFISHG for the measurement of molecular hyperpolarizabilities. HRS can be used to directly
measure  of all molecules, including nonpolar molecules, which cannot be studied by EFISHG.
See Acc. Chem. Res., 31, 675 (1998) for introduction.
According to intensity of incident light at given frequency () and that of scattered light with
doubled frequency (2) detected at 90 angle, the HRS could be determined, which correlates
components of frequency-dependent  tensor as follows. See Phys. Chem. Chem. Phys., 10, 6223
(2008) for more details.

 HRS ( −2; ,  ) =

2
2
 ZZZ
+  XZZ

where (index  cycles {x,y,z} in turn, similar for  and )
2
 ZZZ
=

1
4
2
4
2
2

+  
+    +   

7 
35  
35  
35  
+

4
1
4
1
2
  +  
+
  +


  
35  
35  
105   
105   

+

4
2
4
2
  +

+


  
105   
105   
105   

2
 XZZ
=

1
4
2
8
2
2

+
  −    +




35 
105  
35  
105  
+

3
2
1
2
 −    +
  −


  
35  
35  
35   
105   

−

2
2
2
2
  +

−


  
105   
35   
105   

the  means  and meantime . the brackets indicate an averaging over all possible
molecular orientations. If assume that Kleinman's symmetry is approximately applicable, the above
equations could be reduced to a simpler form, see Eq. 6 of J. Chem. Phys., 136, 024506 (2012).
The associated depolarization ratio (DR) is defined as

DR =

2
 ZZZ
2
 XZZ

Molecules with Td point group have DR exactly equals to 1.5. If the half of wavelength of incident
light is close to absorption band of the current system calculated at same level using TDDFT, the
printed DR may be lower than 1.5 due to SHG resonance.
There are some relevant quantities about HRS could be studied, as shown below, see also J.
Chem. Phys., 136, 024506 (2012) for more information. Note that for consistency, the ZXX in the
equations of this paper have been replaced with XZZ, they are numerically identical in the present

332

--- page break ---

3 Functions
context.
2 〉
2 〉
The 〈𝛽𝑍𝑍𝑍
and 〈𝛽𝑋𝑍𝑍
can be regarded as contributed by two components, dipolar (J=1) and
octupolar (J=3):

9
6
|  J =1 |2 +
|  J =3 |2
45
105
1
4
=
|  J =1 |2 +
|  J =3 |2
45
105

2
 ZZZ
=
2
 XZZ

clearly the two components can be evaluated as follows
2
2
|  J =1 |= 6  ZZZ
− 9  XZZ

|  J =3 |=

1
2

( −7 

2
ZZZ

2
+ 63  XZZ

)

The nonlinear anisotropy parameter is evaluated as  = |J=3| / |J=1|.
The dipolar relative contribution to , namely (J=1), and octupolar relative contribution to ,
namely (J=3), are defined as below. Clearly (J=1)+(J=3)=1

 (  J =1 ) = 1 / (1 +  )
 (  J =3 ) =  / (1 +  )
For small molecules, the one with larger dipole moment tends to have larger (J=1), higher
DR and lower , while the one with smaller dipole moment tends to have larger (J=3), lower DR
and higher .
Assuming a general elliptically polarized incident light propagating along the X direction, with
a state of polarization characterized by two angles (, δ), the intensity of the harmonic light scattered
at 90° along the Y direction and vertically (V) polarized along the Z-axis are given by Bersohn’s
expression (the phase retardation  is assumed to be /2)
2
2
I2V   XZZ
cos4  +  ZZZ
sin 4  + sin 2  cos 2  (  ZXZ +  ZZX )2 − 2  ZZZ  XZZ

where

2
2
(  ZXZ +  ZZX )2 − 2 ZZZ  XZZ = 7  XZZ
−  ZZZ
,  is polarization angle of the

incident light beam.
According to theoretically calculated SHG form of  tensor, all above mentioned quantities
2𝜔
could be readily predicted. The variation of 𝐼V
with respect to  could be scanned and plotted as
curve map.

Input file and usage
In this function, Multiwfn outputs all components of dipole moment, polarizability and 1st/2nd
hyperpolarizability (if available) with explicit labels, as well as all of their relevant quantities
introduced above, such as isotropic polarizability, polarizability anisotropy, hyperpolarizability in
three axes (x, y, z), magnitude of hyperpolarizability (tot) and so on. The  and  components
printed by Multiwfn correspond to standard orientation, while the printed  components correspond
to input orientation.
The polar keyword in Gaussian is specific for calculating ,  and  based on analytic
derivatives (by means of coupled-perturbed SCF equation) or numerical derivatives (by means of

333

--- page break ---

3 Functions
finite field treatment). Notice that in the Gaussian input file you must specify #P, otherwise Multiwfn
cannot properly parse relevant information.
After you enter present function of Multiwfn, you should select actual case of your Gaussian
(hyper)polarizability calculation, so that Multiwfn can successfully parse the outputted information
and show valuable data for you. As can be seen in the menu, there are seven options corresponding
to different situations:
(1) polar keyword + methods supporting analytic 3-order derivatives (HF/DFT/Semi-empirical
methods)
(2) polar keyword + methods supporting analytic 2-order derivatives (e.g. MP2)
(3) polar=Cubic keyword + methods supporting analytic 2-order derivatives
(4) polar keyword + methods supporting analytic 1-order derivatives (CISD, QCISD, CCSD,
MP3, MP4(SDQ), etc.)
(5) polar=DoubleNumer (equivalent to Polar=EnOnly) keyword + methods supporting
analytic 1-order derivatives
(6) polar keyword + methods only supporting energy calculation (CCSD(T), QCISD(T),
MP4(SDTQ), MP5, etc.)
(7) polar=gamma keyword + methods supporting analytic 3-order derivatives (HF/DFT/Semiempirical methods)
All options print polarizability and relevant data, only options (1), (3) and (5) also print first
hyperpolarizability, only (7) also prints second hyperpolarizability.
For case (1), if CPHF=RdFreq is specified along with polar or you used polar=DCSHG, and
meantime the external field frequencies (e.g. 0.05 0.07 0.1 or 532 nm 680 nm) are provided after
molecular geometry with a blank line in front of it, Gaussian will calculate and output frequencydependent (hyper)polarizabilities along with static (hyper)polarizability. The CPHF=RdFreq polar
case only evaluates (-;,0) values, while the polar=DCSHG case evaluates both (-;,0) and
(-2;,) values. For the case of (7), Gaussian calculates (0;0,0,0), (-;,0,0) and (-2;,,0),
and you must provide external field frequencies at the end of input file.
For cases (1) and (7), by default Multiwfn only parses static (hyper)polarizability. If you wish
to parse the frequency-dependent ones instead of the static one, before selecting option 1 or 7 to
start parsing, you should select option “-1 Toggle loading frequency-dependent result for options 1
and 7” first. Then after starting parsing, user can choose the result at which frequency will be parsed.
Note that in the case (1) if you choose to parse (-2;,), you must employ polar=DCSHG
keyword in Gaussian input file.
The quantities related to hyper-Rayleigh scattering (HRS) experiment mentioned above are
also automatically printed when you request Multiwfn to parse frequency-dependent (-2;,)
2𝜔
based on output file of polar=DCSHG. After that, you can also let Multiwfn scan 𝐼V
versus ,
then the generated HRS_angle.txt could be plotted using Origin and so on.
It is noteworthy that, it is well known that the sign of all hyperpolarizability components
outputted by Gaussian are wrong and should be multiplied by -1, Multiwfn automatically accounts
for this problem.
Before carrying out parsing, via option -3 of interface of present function, you can choose the

334

--- page break ---

3 Functions
unit in the output. Atomic unit, SI unit and esu unit can be chosen. The conversion factors are






1 a.u.
1 a.u.
1 a.u.
1 a.u.

SI

esu

8.4783510-30 C m
1.648810-41 C2m2J-1
3.2063610-53 C3m3J-2
6.2353810-65 C4m4J-3

2.54175 10-18 esu
1.481910-25 esu
8.6392210-33 esu
5.0367010-40 esu

The polarizability  is often expressed in terms of "polarizability volume" ('), which has
volume unit.  (1 a.u.)=' (0.14818470 Å3).
An example is given in Section 4.24.1. More discussion and examples about this function can
be found in my blog article "Using Multiwfn to analyze polarizability and hyperpolarizability
outputted by Gaussian" (http://sobereva.com/231, in Chinese)

3.27.2 Study (hyper)polarizability by sum-over-states (SOS) method
and two- or three-level model analyses
This function is used to calculate polarizability, first, second, and third hyperpolarizabilities
based on the well-known sum-over-states (SOS) method, as described below. In addition, the
popular two-level model analysis for the first hyperpolarizability as well as its extension (three-level
model) can also be realized in this module, as introduced in Section 3.27.2.2.

3.27.2.1 Calculation of (hyper)polarizability
A brief survey of the theories for evaluating (hyper)polarizability
Some basic concepts of (hyper)polarizability are introduced in Section 3.27.1. There are a few
different ways to calculate (hyper)polarizability, including derivative method, sum-over-states (SOS)
and response method
(1) Derivative method: This is the most straightforward and commonly used one. The
derivatives needed by static (hyper)polarizability can be evaluated analytically by means of coupledperturbed SCF (CPSCF) equation; specifically, CPHF for HF and CPKS for KS-DFT. These
derivatives can also be evaluated numerically by means of finite difference technique, which is also
known as finite field (FF) method. Evidently FF is much slower and not as accurate as CPSCF,
however it is still useful, because high-order of analytic derivatives, especially the ones at
sophisticated post-HF levels, are not widely supported by many quantum chemistry programs due
to the difficulties in coding. When all requested derivatives are available analytically, derivative
method will be very efficient. The frequency-dependent variant of CPSCF equation enables the
derivative method to evaluate dynamic (hyper)polarizability, but there is no way to evaluate
dynamic (hyper)polarizability in terms of FF treatment. The polar keyword in Gaussian, as
discussed carefully in Section 3.27.1, corresponds to this derivative method.
(2) SOS method: This method for evaluating static and dynamic (hyper)polarizability is
relatively inefficient, because in principle it involves a sum over all excited states (in practical
applications, taking 60-120 lowest states into account are often enough), while determination of a
large number of excited states is usually quite time consuming in ab initio cases (e.g. CIS and
TDDFT), especially for large system (e.g. > 40 atoms). Due to the high computational cost, SOS is

335

--- page break ---

3 Functions
generally not recommended for evaluation of (hyper)polarizability when derivative method can be
carried out analytically. The only advantages of SOS may be that the contribution from different
states can be separated and discussed respectively, and when transition dipole moments between
different excited states are available in hand, the (hyper)polarizability at different frequencies can
be evaluated rather rapidly. It is noteworthy that the SOS based on the cheap semi-empirical ZINDO
calculation (SOS/ZINDO) is very popular for evaluating (hyper)polarizability of large system.
(3) Response method: This method is specific for dynamic (hyper)polarizability and also
known as propagator method. TDHF and TDDFT are its two practical realizations. This method is
not prevalently supported by mainstream quantum chemistry codes.
Working equations of SOS method
The explicit SOS equations for evaluating polarizability and 1st/2nd/3rd hyperpolarizability
can be found in J. Chem. Phys., 99, 3738 (1993), the idea was originally proposed by Orr and Ward
in Mol. Phys., 20, 512 (1971).
The equations for polarizability  and first hyperpolarizability  are (all units are in a.u.)

  0Ai iB0
B A 
 A B
+ 0i i 0  = Pˆ [ A( − ), B ( )] 0i i 0
i +  
i 0   i − 
i 0  i − 

 AB ( − ;  ) =  

0Ai ijB  Cj0
i 0 j 0 (  i −  )(  j − 2 )

 ABC ( − ; 1 , 2 ) = Pˆ [ A( − ), B(1 ), C (2 )]
where

ijA = i ˆ A j

ijA = ijA − 00A  ij  =  i
i

A,B,C... denote one of directions {x,y,z};  is energy of external fields, =0 corresponds to static
electric field; i stands for excitation energy of state i with respect to ground state 0. 𝑃̂ is permutation
𝐴
operator, for  and  evidently there are 2!=2 and 3!=6 permutations, respectively. 𝜇𝑖𝑗
is A
component of transition dipole moment between state i and j; when i=j the term simply corresponds
to electric dipole moment of state i. 𝜇̂ is dipole moment operator, e.g. 𝜇̂ 𝑥 ≡ −𝑥.
The SOS equation for second hyperpolarizability  is

 ABCD ( − ; 1 , 2 , 3 ) = Pˆ [ A( − ), B(1 ), C (2 ), D(3 )]( I −  II )
0Ai ijB  Cjk  kD0
i 0 j 0 k 0 (  i −  )(  j −  2 − 3 )(  k − 3 )

 I = 

0Ai iB0  0Cj  Dj0
i 0 j 0 (  i −  )(  i − 1 )(  j − 3 )

 II = 

The SOS equation for third hyperpolarizability  is

336

--- page break ---

3 Functions

 ABCDE ( − ; 1 , 2 , 3 , 4 ) = Pˆ [ A( − ), B(1 ), C (2 ), D(3 ), E (4 )]( I −  II −  III )
0Ai ijB  Cjk  klD lE0
 = 
l , j ,k ,l (  i −  )(  j −  + 1 )(  k − 3 −  4 )(  l −  4 )
I

( 0 )


0Ai iB0 0Cj  Djk  kE0  1
1 
1
1



 = (1 / 2) 
+
+


(

+

)(

−

)

−


−


−

−


+

+

i , j ,k
j
2
k
4  i

i
1 
j
3
4
k
2
3 
II

( 0 )



III


0Ai iB0 0Cj  Djk  kE0 
1
1
= (1 / 2) 
+


i , j ,k (  i −  )(  i − 1 ) 
 (  j − 3 − 4 )(  k − 4 ) (  j + 2 )(  k + 2 + 3 ) 
( 0 )

Input file
Two kinds of input files could be used:
• Plain text file containing excitation energies and transition dipole moments for all involved
states. Polarizability, first, second and third hyperpolarizabilities can be calculated in this case.
Below format should be satisfied (assume a very simple case, only 2 excited states).
2

// The number of excited states

1

1.1

2

3.2

// Excited state 1, its index and excitation energy (eV)

0 0

0.845 0.2 0.4

// Electric dipole moment of ground in X,Y,Z (a.u.)

0 1

0.231 0.3 0.7

// Transition dipole moment between ground and excited state 1

0 2

0.112 0.564 0.21

1 1

0.021 0.465 0.0

// Electric dipole moment of excited state 1

1 2

0.001 0.3 0.11

// Transition dipole moment between excited states 1 and 2

2 2

0.432 0.14 0.42

You can directly utilize the function introduced in Section 3.21.5 to generate such a plain text
file based on output file of electron excitation task of Gaussian or other codes.
If merely polarizability is the quantity of interest, only the content before the line "1 1" is
needed to be provided, all other contents can be omitted; in this case, the number of excited states
should be written as a negative number (-2 in above case) to tell Multiwfn do not to load them.
• Gaussian output file of common CIS, TDHF, TDDFT or ZINDO task. Since Gaussian does
not output all transition dipole moments needed by SOS hyperpolarizability calculation, only
polarizability will be calculated by Multiwfn in this case. In order to obtain accurate polarizability,
the number of calculated states should be large enough. If nstates keyword is specified to a very
large value, e.g. 1000000, then all states will be calculated. #P is suggested to be used, since the
excitation energy will then be printed in a higher precision format.
Usage
After you entered this function you will see a menu, there are three kinds of functions:
• Options 1~4: Used to calculate , ,  and  at given frequencies, respectively. User needs to
input frequency of each external field. The inputted frequencies may be negative. For example, to
compute hyperpolarizability (-(0.25-0.32);0.25,-0.32), one should input 0.25,-0.32 in option 2. The
default unit is a.u., if you prefer to input the frequencies in nm, you should add corresponding suffix,

337

--- page break ---

3 Functions
for example, 182.25,-142.385 nm.
Since calculation of  and especially  is often time-consuming, in these cases users will be
prompted to input the number of states in consideration, smaller number leads to lower cost, but too
small number may give rise to poor result.
• Options 5~7: Used to study the variation of ,  and  with respect to the number of states in
consideration. User needs to input frequency of each external field. For  and , the number of
states taken into account ranges from 1 to all states loaded, the stepsize is 1. While for , since the
computational cost may be quite high, users are allowed to define the ending value and stepsize.
The result will be outputted to plain text file in current folder, the meaning of each column is clearly
indicated in command-line window.
• Options 15~17: Used to study the variation of the ,  and  with respect to frequency of
external fields. For , users need to input initial value, ending value and stepsize of external field
frequencies. For  and , users should write a plain text file, each row corresponds to a pair of
frequency (in a.u.) to be calculated. Multiwfn will prompt users to input the path of the file. Below
is an example file used to study how (-0;0,,-) varies as  goes from 0 to 0.2 a.u. with stepsize
of 0.02
0.0

0.0

0.0

0.0

0.02

-0.02

0.0

0.04

-0.04

...[ignored]
0.0

0.2

-0.2

Since the computational cost for evaluating  may be quite high, in this case users are allowed to set
the number of states in consideration. The result will be outputted to plain text files in current folder,
the meaning of each column is clearly indicated in command-line window.
• Option 19: This option is used to scan both 1 and 2 of (-(1+2);1,2). You only need
to input initial frequency, ending frequency and number of steps for 1 and 2. Then after a while,
 at different 1 and 2 frequencies will be outputted to plain text files in current folder, the meaning
of each column is clearly indicated in command-line window. Then you can use third-part software
to plot relief map of " vs. 1,2".
Multiwfn not only outputs the tensor of (hyper)polarizability, but also outputs many related
quantities, such as anisotropy, magnitude and the component along Z axis. The quantities involving
,  and  have been introduced in Section 3.27.1.
An example is given in Section 4.24.2.1. More discussion and examples about this function
can be found in my blog article "Using Multiwfn to calculate polarizability and hyperpolarizability
based on sum-over-states (SOS) method" (http://sobereva.com/232, in Chinese)

3.27.2.2 Two-level and three-level model analyses for hyperpolarizability
Theory
From the SOS expression of , it is clear that magnitude of  is completely determined by
character of excited states. Clearly it is a useful idea to interpret the nature of difference in  between
different systems from excited state point of view. Indeed, this analysis has been prevalently
employed in literatures, such my works J. Comput. Chem., 38, 1574 (2017) and Phys. Chem. Chem.

338

--- page break ---

3 Functions
Phys., 27, 11993 (2025). Let us see how to derive such an analysis model.
Recall the SOS formula for 

0Ai ijB  Cj0
i 0 j 0 (  i −  )(  j − 2 )

 ABC ( − ; 1 , 2 ) = Pˆ [ A( − ), B(1 ), C (2 )]

Assume that only the ZZZ component is of our interest and we only focus on static limit case
(=0), the equation simplifies to



SOS
ZZZ

0Zi ijZ  Zj0
= 6
i  j
i 0 j 0

Given that ij = ij − 00 ij , when i=j, this terms corresponds to variation of dipole moment
A

A

A

between excited state i and ground state, namely ii = ii − 00 = i ; while if ij , this term
A

A

A

A

ijA = ijA corresponds to transition dipole moment between excited state i and j.
𝐴
𝐴
SOS
With the fact that 𝜇𝑖𝑗
= 𝜇𝑗𝑖
, the 𝛽𝑍𝑍𝑍
shown above can be written as sum of contribution of

individual excited states and cross term contribution between various excited states:
SOS
 ZZZ
= 6
i

  
( 0Zi ) 2 iZ
+ 12 0i 0 j ij
2
i
i  j
i
j i
Z

Z

Z

The two-level model is very popular, it assumes that the ZZZ is dominated by ground state and
only one excited state:
SOS
 ZZZ
6

( 0Zi )2 iZ
 i2

The excited state i is usually referred to as crucial state and commonly corresponds to the lowestlying one with large oscillator strength (strictly speaking, in the present context, the crucial state
should refer to the lowest-lying one with large Z component of transition dipole moment, however,
the crucial state determined in this way is commonly identical to that determined according to
oscillator strength).
Often the two-level model is equivalently expressed in terms of oscillator strength:
SOS
 ZZZ
= 9iZ f i Z /  i3

where f i Z = (2 / 3) i ( 0Zi ) 2 is Z component of oscillator strength. Furthermore, with assumption
that only Z component of transition dipole moment and variation of dipole moment are relatively
prominent, we have  SOS  i f i /  i3 . Obviously, now one can easily analyze the source of
difference of  between various systems by comparing the ∆𝜇𝑖𝑍 , fi and i terms.
Occasionally, there is no well-defined crucial state. For example, both the 1st and 2nd excited
𝑍
states have large 𝜇0𝑖
, while their energy separation is marginal (nearly degenerate), in this case we
should not simply ignore either one, the two excited states should be simultaneously taken into
account, I define this model as three-level model:

339

--- page break ---

3 Functions

SOS
 ZZZ
=6

0Zi 0Zj ijZ
( 0Zj ) 2  Zj
( 0Zi ) 2 iZ
+
12
+
6
 i2
i  j
 2j

Usage
In the SOS module (subfunction 2 of main function 24), the suboption 20 is used to carry out
the two- and three-level model analyses, all involved terms in the models will be reported so that
you can easily compare them among different systems. The input file of this function is completely
the same as that used for SOS calculation of first hyperpolarizability, as described in the last section.
After entering this option, if you only input index of one excited state, then two-level model
analysis will be carried out, if indices of two excited states are inputted, then three-level model
analysis will be performed. If you input a range, e.g. 1-20, then two-level analysis will be performed
for each excited state in the range.
An example is given in Section 4.24.2.2.

3.27.3 Study (hyper)polarizability density
Blog article introducing (hyper)polarizability density is "Using Multiwfn to calculate (hyper)polarizability
density" (http://sobereva.com/305, in Chinese).

(Hyper)polarizability density can be very easily plotted by Multiwfn as plane map and
isosurface map. This quantity is quite useful in discussing nature of (hyper)polarizability of a given
molecule. If this feature is used in your work, citing my paper J. Comput. Chem., 38, 1574 (2017)
is recommended, in which the (hyper)polarizability density analysis is involved and brief
introduction is given. My other publications also present illustrative applications of this method:
Carbon, 165, 461 (2020), J. Phys. Chem. C, 124, 7353 (2020), J. Phys. Chem. A, 124, 5563 (2020),
J. Phys. Chem. C, 124, 845 (2020).
Theory of (hyper)polarizability density and spatial contribution to (hyper)polarizability
There is a well-known Taylor expansion for (electric) dipole moment

E
= μ 0 + αF + (1 / 2)βF2 + (1 / 6) γF3 + 
F
E
2E
 3E
4E
μ0 = −
α=− 2
β=− 3
γ=− 4
F F = 0
F F = 0
F F = 0
F F = 0

μ( F ) = −

where F is external electric field vector, E is system total energy,  and 0 are current electric dipole
moment and permanent dipole moment, respectively. ,  and  are polarizability, the first and
second hyperpolarizability tensors, respectively.
Similarly, Taylor expansion with respect to F can be applied to electron density

 (r, F) =  ( 0 ) (r ) + ρ(1) (r )F + (1 / 2)ρ( 2 ) (r )F2 + (1 / 6)ρ( 3) (r )F3 + 
 (r )
ρ (r ) =
F F = 0
(1)

 2  (r )
ρ (r ) =
F2 F = 0
( 2)



 3  (r )
ρ (r ) =
F3 F = 0
( 3)

Since μ(F) = −  (r, F)r d r , by comparing above equations, we find

340

--- page break ---

3 Functions

μ0 =  −ρ(0) (r )r d r

α =  −ρ(1) (r )r d r

β =  −ρ(2) (r )r d r

γ =  −ρ(3) (r )r d r

where (1) is known as polarizability density, while (2) and (3) are known as the first and second
hyperpolarizability densities, respectively. Using (hyper)polarizability densities, we can easily
investigate contribution of various spatial regions to total molecular (hyper)polarizabilities.
The second hyperpolarizability density (3) is a third-order tensor function, it can be explicitly
represented as

ijk( 3) (r ) =

 3  (r )
Fi F j Fk

F =0

It is impossible to discuss all of its components, since there are as many as 333=27
components. Assume that the ZZZZ is the most crucial component of , we can simply study ZZZ(r):
(3)
 zzz
(r ) =

 3  (r )
Fz3 F =0
z

which relates to ZZZZ via
(3)
 zzzz =  − z  zzz
(r ) d r

(3)

Clearly, −𝑧𝜌𝑧𝑧𝑧 (𝐫) is the contribution of point r to the ZZZZ. If it is plotted as isosurface map or
(3)
plane map, the source of ZZZZ can be intuitively revealed. However, the disadvantage of −𝑧𝜌𝑧𝑧𝑧 (𝐫)
(3)
is that it depends on the choice origin, which is somewhat arbitrary, therefore 𝜌𝑧𝑧𝑧 has its own value
to study as it is independent of origin.
(3)

The easiest way of obtaining the 𝜌𝑧𝑧𝑧 is using finite difference method (see my article
http://sobereva.com/305 on how to derive it)



( 3)
zzz

=

 ( 2 F z ) − 2  ( F z ) + 2  ( − F z ) −  ( −2 F z )
2( F z )3

where Fz is strength of the external electric field applied along Z axis. The functions such as (Fz)
and (-Fz) denote the electron density distribution yielded when Fz is applied along positive and
negative directions of Z-axis, respectively. The Fz in this case corresponds to finite difference step
size, it should not be too large or too small, otherwise numerical error will be significant. According
my experience, 0.003 a.u. is a good choice of Fz.
Similarly, one can easily derive the equation for polarizability density

 z(1) =

 ( F z ) −  (− F z )
2F z

 zz =  −  z(1) (r ) z d r
and that for first hyperpolarizability density

 zz(2) =

 ( F z ) − 2  (0) +  ( − F z )
( F z )2

 zzz =  −  zz(2) (r ) z d r

341

--- page break ---

3 Functions
Use Multiwfn to study (hyper)polarizability density
Via subfunction 3 of main function 24, one can very conveniently plot plane map and isosurface
map of any kind of (hyper)polarizability density as well as spatial contribution to
(hyper)polarizability. Once grid data of the latter is generated by Multiwfn and exported to .cub file,
one can further evaluate atom or fragment contribution to (hyper)polarizability, as shown in the
example in Section 4.24.3.
This function is used via the following steps
(1) Boot up Multiwfn and load a file containing atom information of the studied system, such
as .xyz, .pdb, .mwfn, .fch and so on, see Section 2.5.
(2) Enter subfunction 3 of main function 24.
(3) Choose the quantity you hope to study
(4) Choose the direction of interest (X or Y or Z)
Assume that you chose “second hyperpolarizability density and spatial contribution to second
(3)

(3)

hyperpolarizability” in step (3) and choose “Z” in step (4), then you can study −𝑧𝜌𝑧𝑧𝑧 and 𝜌𝑧𝑧𝑧 later.
(5) Choose option 1 to generate Gaussian input files of single point calculations under different
external electric fields. You can manually modify the default keywords in these files. By default, the
calculations are conducted at PBE0/aug-cc-pVTZ level.
(6) Run the .gjf files by Gaussian manually, then .wfx files will be generated
(7) Choose option 2 to load the .wfx files
(8) Now you can choose what you want to do. If you choose to calculate grid data of
(hyper)polarizability or spatial contribution to (hyper)polarizability, then you can directly visualize
their isosurface map or export grid data as .cub file. Also you can choose to plot plane map of these
functions.
About molecular orientation
It is important to note that in practice, what we are actually interested in is often the component
along the direction of molecular dipole moment, which is often not parallel to any Cartesian axis.
In this case, before using the present function, you should reorientate the system so that the dipole
moment is exactly parallel to a Cartesian axis, such as Z. Multiwfn can easily realize the
reorientation via the function described in Section 3.300.7. Namely you should load wavefunction
file of the present system after booting up Multiwfn, and then input
300 //Other function (Part 3)
7 //Geometry operation on the present system
7 //Make electric dipole moment parallel to a vector or Cartesian axis
3 //Parallel to Z axis
-1 //Output system to .xyz file
Then you can use the exported .xyz file as input file for studying (hyper)polarizability density.
See Section 4.24.3 for example of studying (hyper)polarizability density and spatial
contribution to (hyper)polarizability.

342

--- page break ---

3 Functions

3.27.5 Visualize (hyper)polarizability via unit sphere and vector
representations
If you are not familiar with (hyper)polarizability, please check Section 3.27.1 first to gain basic
knowledge. In this section, the unit sphere representation will be introduced, it was proposed in J.
Comput. Chem., 32,1128 (2011) to intuitively represent first-order hyperpolarizability tensor, while
I also extended this idea to polarizability and second-order hyperpolarizability.
Theory
Recall the relationship between molecular dipole moment and external field

μ = μ0 + α  F + (1 / 2)β  F  F + (1 / 6) γ  F  F  F +
The  is known as first order hyperpolarizability tensor, the component ABC is proportional to the
magnitude of induced dipole moment in direction A caused by combination of two incident electric
fields respectively in directions B and C.
In the unit sphere representation, effective dipole vector is defined as

βeff ( ,  ) = β  e( ,  )  e( ,  )
where  and  are angles of spherical polar coordinate, e(,) is unit vector normal to the sphere
surface. More specifically, the components of eff can be explicitly written as

ieff =  i , j ,k ek e j
j

i, j, k = {x, y, z}

k

The orientation and length of eff(,) vector respectively reflect the direction and magnitude
of induced dipole moment caused by combination of two incident electric fields exerted in the
direction of (,). If eff is calculated at every vertex of a sphere surface enclosing the molecule, one
can clearly and vividly understand the response of molecular dipole moment with respect to external
electric field exerted in various directions. The original paper only employs this representation to
second harmonic generation (SHG) type of , in fact it can also be applied to other kinds of ,
including both static and dynamic ones (in the latter case, the exerted external field with varying
strength comes from incident electromagnetic wave, and its direction is perpendicular to the
propagation direction of the electromagnetic wave).
Based on the same idea of eff, I defined below quantities

α eff ( ,  ) = α  e( ,  )
γ eff ( ,  ) = γ  e( ,  )  e( ,  )  e( ,  )
The eff(,) vector reflects the direction and magnitude of induced dipole moment caused by
the incident electric field exerted in the direction of (,), while the eff(,) vector reflects the
direction and magnitude of induced dipole moment caused by combination of three incident electric
field simultaneously exerted in the direction of (,). Similarly, by plotting the eff(,) or eff(,)
at every vertex on a sphere, one obtains unit sphere representation map of  and , respectively.
The so-called vector representation of  corresponds to plotting (x, y, z) vector as an arrow,
the components are defined as

343

--- page break ---

3 Functions

i = (1 / 3) (ijj +  jji +  jij ) i, j = {x, y, z}
j

This representation is quite simple, it can approximately represent major character of ,
however, anisotropy character cannot be explicitly exhibited in this representation. For example, x
shows collective effect of xxx, xyy, xzz... components, where the xyy and xzz describe anisotropy
response character since the exerted two electric fields are not collinear with the induced dipole
moment resulting from their combination effect.
I also proposed vector representation for , the situation is very different to the vector
representation of . Double sided arrows are drawn along X, Y and Z axes, and their lengths
respectively represent magnitude of  in the corresponding directions, which are defined as

 x =  xx +  xy +  xz
 y =  yx +  yy +  yz
 z =  zx +  zy +  zz
Similarly, vector representation for  corresponds to drawing double sided arrows along X, Y
and Z axes, and their lengths respectively represent magnitude of  in the corresponding directions
(x, y, z), which are calculated as

 i = (1 / 15) ( ijji +  ijij +  iijj ) i, j = {x, y, z}
j

Usage
Multiwfn is able to perform unit sphere representation analysis for ,  and , namely
generating plotting script of VMD software (http://www.ks.uiuc.edu/Research/vmd/) based on
loaded (hyper)polarizability tensor. In addition, plotting script corresponding to vector
representation can also be generated for .
After booting up Multiwfn, you should load a file containing atom information for the molecule
under study. For example, .xyz, .pdb and .fch can be used, see Section 2.5. The atom information
will be used to determine proper radius of the sphere involved in the unit sphere representation.
After entering present module (subfunction 5 of main function 24), you can use many options
to adjust parameters of unit sphere and vector representations, such as scale factor of arrow length,
radius of arrow and so on, they are fully self-explanatory. By choosing options 1 or 2 or 3, Multiwfn
will respectively load  or  or  tensor from a specific file (see below), then VMD plotting script
corresponding to unit sphere representation will be generated in current folder (alpha.tcl, beta.tcl
and gamma.tcl, respectively), and those corresponding to vector representation will also be
generated (alpha_vec.tcl, beta_vec.tcl and gamma_vec.tcl). Then, using VMD to run the scripts, the
corresponding graph will be immediately obtained.
It is worth to mentioning there is an option "-8 Toggle making longest arrow on sphere has specific length". If
you select it once to switch its status to "Yes", then after selecting option 1 or 2 or 3, you will be asked to input the
expected length of longest arrow on the sphere. Via this option, you can make map plotted by VMD for systems
having very different magnitude of (hyper)polarizability easily comparable.

Preparation of the file containing (hyper)polarizability tensor
The file containing  or  or  tensor can be directly generated by subfunction 1 of main
function 24 by extracting corresponding data from output file of "polar" task of Gaussian. In that
function, you should choose option "-4 Export (hyper)polarizability as .txt file after parsing" once
to switch its status to "Yes", then after parsing data via corresponding option,  will be exported to
alpha.txt,  will be exported to beta.txt, and  will be exported to gamma.txt in current folder, they

344
