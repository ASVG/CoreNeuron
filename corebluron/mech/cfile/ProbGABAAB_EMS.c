/* Created by Language version: 6.2.0 */
/* VECTORIZED */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "corebluron/mech/cfile/scoplib.h"
#undef PI
#ifdef _PROF_HPM 
void HPM_Start(const char *); 
void HPM_Stop(const char *); 
#endif 
 
#include "corebluron/nrnoc/md1redef.h"
#include "corebluron/nrnconf.h"
#include "corebluron/nrnoc/multicore.h"

#include "corebluron/utils/randoms/nrnran123.h"

#include "corebluron/nrnoc/md2redef.h"
#if METHOD3
extern int _method3;
#endif

#if !NRNGPU
#undef exp
#define exp hoc_Exp
extern double hoc_Exp(double);
#endif
 
#if !defined(LAYOUT)
/* 1 means AoS, >1 means AoSoA, <= 0 means SOA */
#define LAYOUT 1
#endif
#if LAYOUT >= 1
#define _STRIDE LAYOUT
#else
#define _STRIDE _cntml
#endif
 
#define _nrn_init _nrn_init__ProbGABAAB_EMS
#define _nrn_initial _nrn_initial__ProbGABAAB_EMS
#define _nrn_cur _nrn_cur__ProbGABAAB_EMS
#define _nrn_current _nrn_current__ProbGABAAB_EMS
#define _nrn_jacob _nrn_jacob__ProbGABAAB_EMS
#define _nrn_state _nrn_state__ProbGABAAB_EMS
#define _net_receive _net_receive__ProbGABAAB_EMS 
#define setRNG setRNG__ProbGABAAB_EMS 
#define state state__ProbGABAAB_EMS 
 
#if LAYOUT == 0 /*SoA*/
#define _threadargscomma_ _cntml, _p, _ppvar, _thread, _nt,
#define _threadargsprotocomma_ int _cntml, double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt,
#define _threadargs_ _cntml, _p, _ppvar, _thread, _nt
#define _threadargsproto_ int _cntml, double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt
#else
#define _threadargscomma_ _p, _ppvar, _thread, _nt,
#define _threadargsprotocomma_ double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt,
#define _threadargs_ _p, _ppvar, _thread, _nt
#define _threadargsproto_ double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt
#endif
 	/*SUPPRESS 761*/
	/*SUPPRESS 762*/
	/*SUPPRESS 763*/
	/*SUPPRESS 765*/
	 extern double *getarg();
 /* Thread safe. No static _p or _ppvar. */
 
#define t _nt->_t
#define dt _nt->_dt
#define tau_r_GABAA _p[0*_STRIDE]
#define tau_d_GABAA _p[1*_STRIDE]
#define tau_r_GABAB _p[2*_STRIDE]
#define tau_d_GABAB _p[3*_STRIDE]
#define Use _p[4*_STRIDE]
#define Dep _p[5*_STRIDE]
#define Fac _p[6*_STRIDE]
#define e_GABAA _p[7*_STRIDE]
#define e_GABAB _p[8*_STRIDE]
#define u0 _p[9*_STRIDE]
#define synapseID _p[10*_STRIDE]
#define verboseLevel _p[11*_STRIDE]
#define GABAB_ratio _p[12*_STRIDE]
#define i _p[13*_STRIDE]
#define i_GABAA _p[14*_STRIDE]
#define i_GABAB _p[15*_STRIDE]
#define g_GABAA _p[16*_STRIDE]
#define g_GABAB _p[17*_STRIDE]
#define A_GABAA_step _p[18*_STRIDE]
#define B_GABAA_step _p[19*_STRIDE]
#define A_GABAB_step _p[20*_STRIDE]
#define B_GABAB_step _p[21*_STRIDE]
#define g _p[22*_STRIDE]
#define Rstate _p[23*_STRIDE]
#define tsyn_fac _p[24*_STRIDE]
#define u _p[25*_STRIDE]
#define A_GABAA _p[26*_STRIDE]
#define B_GABAA _p[27*_STRIDE]
#define A_GABAB _p[28*_STRIDE]
#define B_GABAB _p[29*_STRIDE]
#define factor_GABAA _p[30*_STRIDE]
#define factor_GABAB _p[31*_STRIDE]
#define DA_GABAA _p[32*_STRIDE]
#define DB_GABAA _p[33*_STRIDE]
#define DA_GABAB _p[34*_STRIDE]
#define DB_GABAB _p[35*_STRIDE]
#define v _p[36*_STRIDE]
#define _g _p[37*_STRIDE]
#define _tsav _p[38*_STRIDE]
#define _nd_area  _nt->_data[_ppvar[0*_STRIDE]]
#define _p_rng	_nt->_vdata[_ppvar[2]]
 
#if MAC
#if !defined(v)
#define v _mlhv
#endif
#if !defined(h)
#define h _mlhh
#endif
#endif
 
#if defined(__cplusplus)
extern "C" {
#endif
 static int hoc_nrnpointerindex =  2;
 static ThreadDatum* _extcall_thread;
 /* external NEURON variables */
 
#if 0 /*BBCORE*/
 /* declaration of user functions */
 static double _hoc_setRNG();
 static double _hoc_state();
 static double _hoc_toggleVerbose();
 static double _hoc_urand();
 
#endif /*BBCORE*/
 static int _mechtype;
extern int nrn_get_mechtype();
extern void hoc_register_prop_size(int, int, int);
extern Memb_func* memb_func;
 static int _pointtype;
 
#if 0 /*BBCORE*/
 static void* _hoc_create_pnt(_ho) Object* _ho; { void* create_point_process();
 return create_point_process(_pointtype, _ho);
}
 static void _hoc_destroy_pnt();
 static double _hoc_loc_pnt(_vptr) void* _vptr; {double loc_point_process();
 return loc_point_process(_pointtype, _vptr);
}
 static double _hoc_has_loc(_vptr) void* _vptr; {double has_loc_point();
 return has_loc_point(_vptr);
}
 static double _hoc_get_loc_pnt(_vptr)void* _vptr; {
 double get_loc_point_process(); return (get_loc_point_process(_vptr));
}
 
#endif /*BBCORE*/
 
#if 0 /*BBCORE*/
 /* connect user functions to hoc names */
 static VoidFunc hoc_intfunc[] = {
 0,0
};
 static Member_func _member_func[] = {
 "loc", _hoc_loc_pnt,
 "has_loc", _hoc_has_loc,
 "get_loc", _hoc_get_loc_pnt,
 "setRNG", _hoc_setRNG,
 "state", _hoc_state,
 "toggleVerbose", _hoc_toggleVerbose,
 "urand", _hoc_urand,
 0, 0
};
 
#endif /*BBCORE*/
#define toggleVerbose toggleVerbose_ProbGABAAB_EMS
#define urand urand_ProbGABAAB_EMS
 extern double toggleVerbose( _threadargsproto_ );
 extern double urand( _threadargsproto_ );
 /* declare global and static user variables */
#define gmax gmax_ProbGABAAB_EMS
 double gmax = 0.001;
 
#if 0 /*BBCORE*/
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 0,0,0
};
 static HocParmUnits _hoc_parm_units[] = {
 "gmax_ProbGABAAB_EMS", "uS",
 "tau_r_GABAA", "ms",
 "tau_d_GABAA", "ms",
 "tau_r_GABAB", "ms",
 "tau_d_GABAB", "ms",
 "Use", "1",
 "Dep", "ms",
 "Fac", "ms",
 "e_GABAA", "mV",
 "e_GABAB", "mV",
 "GABAB_ratio", "1",
 "i", "nA",
 "i_GABAA", "nA",
 "i_GABAB", "nA",
 "g_GABAA", "uS",
 "g_GABAB", "uS",
 "g", "uS",
 "Rstate", "1",
 "tsyn_fac", "ms",
 "u", "1",
 0,0
};
 
#endif /*BBCORE*/
 static double A_GABAB0 = 0;
 static double A_GABAA0 = 0;
 static double B_GABAB0 = 0;
 static double B_GABAA0 = 0;
 static double delta_t = 0.01;
 
#if 0 /*BBCORE*/
 /* connect global user variables to hoc */
 static DoubScal hoc_scdoub[] = {
 "gmax_ProbGABAAB_EMS", &gmax_ProbGABAAB_EMS,
 0,0
};
 static DoubVec hoc_vdoub[] = {
 0,0,0
};
 
#endif /*BBCORE*/
 static double _sav_indep;
 static void nrn_alloc(double*, Datum*, int);
static void  nrn_init(_NrnThread*, _Memb_list*, int);
static void nrn_state(_NrnThread*, _Memb_list*, int);
 static void nrn_cur(_NrnThread*, _Memb_list*, int);
static void  nrn_jacob(_NrnThread*, _Memb_list*, int);
 
#if 0 /*BBCORE*/
 static void _hoc_destroy_pnt(_vptr) void* _vptr; {
   destroy_point_process(_vptr);
}
 
#endif /*BBCORE*/
 
static int _ode_count(int);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "6.2.0",
"ProbGABAAB_EMS",
 "tau_r_GABAA",
 "tau_d_GABAA",
 "tau_r_GABAB",
 "tau_d_GABAB",
 "Use",
 "Dep",
 "Fac",
 "e_GABAA",
 "e_GABAB",
 "u0",
 "synapseID",
 "verboseLevel",
 "GABAB_ratio",
 0,
 "i",
 "i_GABAA",
 "i_GABAB",
 "g_GABAA",
 "g_GABAB",
 "A_GABAA_step",
 "B_GABAA_step",
 "A_GABAB_step",
 "B_GABAB_step",
 "g",
 "Rstate",
 "tsyn_fac",
 "u",
 0,
 "A_GABAA",
 "B_GABAA",
 "A_GABAB",
 "B_GABAB",
 0,
 "rng",
 0};
 
static void nrn_alloc(double* _p, Datum* _ppvar, int _type) {
 
#if 0 /*BBCORE*/
 	/*initialize range parameters*/
 	tau_r_GABAA = 0.2;
 	tau_d_GABAA = 8;
 	tau_r_GABAB = 3.5;
 	tau_d_GABAB = 260.9;
 	Use = 1;
 	Dep = 100;
 	Fac = 10;
 	e_GABAA = -80;
 	e_GABAB = -97;
 	u0 = 0;
 	synapseID = 0;
 	verboseLevel = 0;
 	GABAB_ratio = 0;
 
#endif /* BBCORE */
 
}
 static void _initlists();
 static void _net_receive(Point_process*, double*, double);
 static void _net_init(Point_process*, double*, double);
 
#define _psize 39
#define _ppsize 3
 static void bbcore_read(double *, int*, int*, int*, _threadargsproto_);
 extern void hoc_reg_bbcore_read(int, void(*)(double *, int*, int*, int*, _threadargsproto_));
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*f)(Datum*));
extern void _nrn_thread_table_reg(int, void(*)(_threadargsproto_, int));
extern void _cvode_abstol( Symbol**, double*, int);

 void _ProbGABAAB_EMS_reg() {
	int _vectorized = 1;
  _initlists();
 
#if 0 /*BBCORE*/
 
#endif /*BBCORE*/
 	_pointtype = point_register_mech(_mechanism,
	 nrn_alloc,nrn_cur, nrn_jacob, nrn_state, nrn_init,
	 hoc_nrnpointerindex,
	 NULL/*_hoc_create_pnt*/, NULL/*_hoc_destroy_pnt*/, /*_member_func,*/
	 1);
 _mechtype = nrn_get_mechtype(_mechanism[1]);
 _nrn_layout_reg(_mechtype, LAYOUT);
   hoc_reg_bbcore_read(_mechtype, bbcore_read);
  hoc_register_prop_size(_mechtype, _psize, _ppsize);
  hoc_register_dparam_semantics(_mechtype, 0, "area");
  hoc_register_dparam_semantics(_mechtype, 1, "pntproc");
  hoc_register_dparam_semantics(_mechtype, 2, "bbcorepointer");
 pnt_receive[_mechtype] = _net_receive;
 pnt_receive_init[_mechtype] = _net_init;
 pnt_receive_size[_mechtype] = 5;
 }
static int _reset;
static char *modelname = "GABAAB receptor with presynaptic short-term plasticity ";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
static int setRNG(_threadargsproto_);
static int state(_threadargsproto_);
 
/*VERBATIM*/
#include "nrnran123.h"
 
static int  state ( _threadargsproto_ ) {
   A_GABAA = A_GABAA * A_GABAA_step ;
   B_GABAA = B_GABAA * B_GABAA_step ;
   A_GABAB = A_GABAB * A_GABAB_step ;
   B_GABAB = B_GABAB * B_GABAB_step ;
    return 0; }
 
#if 0 /*BBCORE*/
 
static double _hoc_state(void* _vptr) {
 double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   _p = ((Point_process*)_vptr)->_prop->param;
  _ppvar = ((Point_process*)_vptr)->_prop->dparam;
  _thread = _extcall_thread;
  _nt = (_NrnThread*)((Point_process*)_vptr)->_vnt;
 _r = 1.;
 state ( _threadargs_ );
 return(_r);
}
 
#endif /*BBCORE*/
 
static void _net_receive (_pnt, _args, _lflag) Point_process* _pnt; double* _args; double _lflag; 
{  double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   _Memb_list* _ml; int _cntml; int _iml;
 
   _thread = (ThreadDatum*)0; _nt = nrn_threads + _pnt->_tid;
   _ml = memb_list + _pnt->_type;
   _cntml = _ml->_nodecount;
   _iml = _pnt->_i_instance;
#if LAYOUT == 1 /*AoS*/
   _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
   _p = _ml->_data + _iml; _ppvar = _ml->_pdata + _iml;
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
  assert(_tsav <= t); _tsav = t; {
   double _lresult ;
 _args[1] = _args[0] ;
   _args[2] = _args[0] * GABAB_ratio ;
   if ( Fac > 0.0 ) {
     u = u * exp ( - ( t - tsyn_fac ) / Fac ) ;
     }
   else {
     u = Use ;
     }
   if ( Fac > 0.0 ) {
     u = u + Use * ( 1.0 - u ) ;
     }
   tsyn_fac = t ;
   if ( Rstate  == 0.0 ) {
     _args[3] = exp ( - ( t - _args[4] ) / Dep ) ;
     _lresult = urand ( _threadargs_ ) ;
     if ( _lresult > _args[3] ) {
       Rstate = 1.0 ;
       if ( verboseLevel > 0.0 ) {
         printf ( "Recovered! %f at time %g: Psurv = %g, urand=%g\n" , synapseID , t , _args[3] , _lresult ) ;
         }
       }
     else {
       _args[4] = t ;
       if ( verboseLevel > 0.0 ) {
         printf ( "Failed to recover! %f at time %g: Psurv = %g, urand=%g\n" , synapseID , t , _args[3] , _lresult ) ;
         }
       }
     }
   if ( Rstate  == 1.0 ) {
     _lresult = urand ( _threadargs_ ) ;
     if ( _lresult < u ) {
       _args[4] = t ;
       Rstate = 0.0 ;
       A_GABAA = A_GABAA + _args[1] * factor_GABAA ;
       B_GABAA = B_GABAA + _args[1] * factor_GABAA ;
       A_GABAB = A_GABAB + _args[2] * factor_GABAB ;
       B_GABAB = B_GABAB + _args[2] * factor_GABAB ;
       if ( verboseLevel > 0.0 ) {
         printf ( "Release! %f at time %g: vals %g %g %g \n" , synapseID , t , A_GABAA , _args[1] , factor_GABAA ) ;
         }
       }
     else {
       if ( verboseLevel > 0.0 ) {
         printf ( "Failure! %f at time %g: urand = %g\n" , synapseID , t , _lresult ) ;
         }
       }
     }
   } }
 
static void _net_init(Point_process* _pnt, double* _args, double _lflag) {
       double* _p = _pnt->_data;
    Datum* _ppvar = _pnt->_pdata;
    ThreadDatum* _thread = (ThreadDatum*)0;
    _NrnThread* _nt = (_NrnThread*)_pnt->_vnt;
 _args[4] = t ;
   }
 
static int  setRNG ( _threadargsproto_ ) {
   
/*VERBATIM*/
    {
#if !NRNBBCORE
	nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
	if (*pv) {
		nrnran123_deletestream(*pv);
		*pv = (nrnran123_State*)0;
	} 
	if (ifarg(2)) {
		*pv = nrnran123_newstream((uint32_t)*getarg(1), (uint32_t)*getarg(2));
	}
#endif
    }
  return 0; }
 
#if 0 /*BBCORE*/
 
static double _hoc_setRNG(void* _vptr) {
 double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   _p = ((Point_process*)_vptr)->_prop->param;
  _ppvar = ((Point_process*)_vptr)->_prop->dparam;
  _thread = _extcall_thread;
  _nt = (_NrnThread*)((Point_process*)_vptr)->_vnt;
 _r = 1.;
 setRNG ( _threadargs_ );
 return(_r);
}
 
#endif /*BBCORE*/
 
double urand ( _threadargsproto_ ) {
   double _lurand;
 
/*VERBATIM*/
	double value;
	if (_p_rng) {
		/*
		:Supports separate independent but reproducible streams for
		: each instance.
		*/
		value = nrnran123_negexp((nrnran123_State*)_p_rng);
		//printf("random stream for this simulation = %lf\n",value);
		return value;
	}else{
                value = 0.0;
//		assert(0);
	}
	_lurand = value;
 
return _lurand;
 }
 
#if 0 /*BBCORE*/
 
static double _hoc_urand(void* _vptr) {
 double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   _p = ((Point_process*)_vptr)->_prop->param;
  _ppvar = ((Point_process*)_vptr)->_prop->dparam;
  _thread = _extcall_thread;
  _nt = (_NrnThread*)((Point_process*)_vptr)->_vnt;
 _r =  urand ( _threadargs_ );
 return(_r);
}
 
#endif /*BBCORE*/
 
/*VERBATIM*/
static void bbcore_write(double* x, int* d, int* xx, int* offset, _threadargsproto_) {
	if (d) {
		uint32_t* di = ((uint32_t*)d) + *offset;
		nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
		nrnran123_getids(*pv, di, di+1);
//printf("ProbGABAAB_EMS bbcore_write %d %d\n", di[0], di[1]);
	}
        *offset += 2;
}
static void bbcore_read(double* x, int* d, int* xx, int* offset, _threadargsproto_) {
	assert(!_p_rng);
	uint32_t* di = ((uint32_t*)d) + *offset;
	nrnran123_State** pv = (nrnran123_State**)(&_p_rng);
	*pv = nrnran123_newstream(di[0], di[1]);
//printf("ProbGABAAB_EMS bbcore_read %d %d\n", di[0], di[1]);
	*offset += 2;
}
 
double toggleVerbose ( _threadargsproto_ ) {
   double _ltoggleVerbose;
 verboseLevel = 1.0 - verboseLevel ;
   
return _ltoggleVerbose;
 }
 
#if 0 /*BBCORE*/
 
static double _hoc_toggleVerbose(void* _vptr) {
 double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   _p = ((Point_process*)_vptr)->_prop->param;
  _ppvar = ((Point_process*)_vptr)->_prop->dparam;
  _thread = _extcall_thread;
  _nt = (_NrnThread*)((Point_process*)_vptr)->_vnt;
 _r =  toggleVerbose ( _threadargs_ );
 return(_r);
}
 
#endif /*BBCORE*/
 
static int _ode_count(int _type){ hoc_execerror("ProbGABAAB_EMS", "cannot be used with CVODE"); return 0;}

static void initmodel(_threadargsproto_) {
  int _i; double _save;{
  A_GABAB = A_GABAB0;
  A_GABAA = A_GABAA0;
  B_GABAB = B_GABAB0;
  B_GABAA = B_GABAA0;
 {
   double _ltp_GABAA , _ltp_GABAB ;
 
/*VERBATIM*/
          if (_p_rng)
          {
            nrnran123_setseq((nrnran123_State*)_p_rng, 0, 0);
          }
 Rstate = 1.0 ;
   tsyn_fac = 0.0 ;
   u = u0 ;
   A_GABAA = 0.0 ;
   B_GABAA = 0.0 ;
   A_GABAB = 0.0 ;
   B_GABAB = 0.0 ;
   _ltp_GABAA = ( tau_r_GABAA * tau_d_GABAA ) / ( tau_d_GABAA - tau_r_GABAA ) * log ( tau_d_GABAA / tau_r_GABAA ) ;
   _ltp_GABAB = ( tau_r_GABAB * tau_d_GABAB ) / ( tau_d_GABAB - tau_r_GABAB ) * log ( tau_d_GABAB / tau_r_GABAB ) ;
   factor_GABAA = - exp ( - _ltp_GABAA / tau_r_GABAA ) + exp ( - _ltp_GABAA / tau_d_GABAA ) ;
   factor_GABAA = 1.0 / factor_GABAA ;
   factor_GABAB = - exp ( - _ltp_GABAB / tau_r_GABAB ) + exp ( - _ltp_GABAB / tau_d_GABAB ) ;
   factor_GABAB = 1.0 / factor_GABAB ;
   A_GABAA_step = exp ( dt * ( ( - 1.0 ) / tau_r_GABAA ) ) ;
   B_GABAA_step = exp ( dt * ( ( - 1.0 ) / tau_d_GABAA ) ) ;
   A_GABAB_step = exp ( dt * ( ( - 1.0 ) / tau_r_GABAB ) ) ;
   B_GABAB_step = exp ( dt * ( ( - 1.0 ) / tau_d_GABAB ) ) ;
   }
 
}
}

static void nrn_init(_NrnThread* _nt, _Memb_list* _ml, int _type){
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double _v; int* _ni; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml; _ppvar = _ml->_pdata + _iml;
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
 _tsav = -1e20;
    _v = VEC_V(_ni[_iml]);
 v = _v;
 initmodel(_threadargs_);
}
}

static double _nrn_current(_threadargsproto_, double _v){double _current=0.;v=_v;{ {
   g_GABAA = gmax * ( B_GABAA - A_GABAA ) ;
   g_GABAB = gmax * ( B_GABAB - A_GABAB ) ;
   g = g_GABAA + g_GABAB ;
   i_GABAA = g_GABAA * ( v - e_GABAA ) ;
   i_GABAB = g_GABAB * ( v - e_GABAB ) ;
   i = i_GABAA + i_GABAB ;
   }
 _current += i;

} return _current;
}

static void nrn_cur(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
int* _ni; double _rhs, _v; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml; _ppvar = _ml->_pdata + _iml;
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
    _v = VEC_V(_ni[_iml]);
 _g = _nrn_current(_threadargs_, _v + .001);
 	{ _rhs = _nrn_current(_threadargs_, _v);
 	}
 _g = (_g - _rhs)/.001;
 _g *=  1.e2/(_nd_area);
 _rhs *= 1.e2/(_nd_area);
	VEC_RHS(_ni[_iml]) -= _rhs;
 
}
 
}

static void nrn_jacob(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
int* _ni; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml; _ppvar = _ml->_pdata + _iml;
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
	VEC_D(_ni[_iml]) += _g;
 
}
 
}

static void nrn_state(_NrnThread* _nt, _Memb_list* _ml, int _type) {
#ifdef _PROF_HPM 
HPM_Start("nrn_state_ProbGABAAB_EMS"); 
#endif 
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double _v = 0.0; int* _ni; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml; _ppvar = _ml->_pdata + _iml;
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
    _v = VEC_V(_ni[_iml]);
 v=_v;
{
 {  { state(_threadargs_); }
  }}}
#ifdef _PROF_HPM 
HPM_Stop("nrn_state_ProbGABAAB_EMS"); 
#endif 

}

static void terminal(){}

static void _initlists(){
 double _x; double* _p = &_x;
 int _i; static int _first = 1;
 int _cntml=0; assert(0);
  if (!_first) return;
_first = 0;
}

#if defined(__cplusplus)
} /* extern "C" */
#endif
