//Especificaciones
//luz
ltl pulsando {
	[]( (p_l || PIR_l) -> <>l );
}
ltl no_pulsar {
	[]((!l && !p_l && !PIR_l) -> <>!l);
}
ltl tiempo_encendida {
	[](l->l W T);
}

ltl introducir_codigo {
	[]((estado_alarma==idle && p)-><> (estado_alarma==introducir));
}
ltl infrarrojo {
	[]((estado_alarma==activada && PIR)-> <>a);
}
ltl idle_tran {
	[]((estado_alarma==idle && !t && <>(t))->(estado_alarma==idle U t));
}
ltl introducir_tran {
	[]((estado_alarma==introducir && !t && <>(t))->(estado_alarma==introducir U t));
}
ltl activada_tran {
	[]((estado_alarma==activada && !t && <>(t))->(estado_alarma==activada U t));
}


int T;
int t;

//Preparacion de timers
#define timer int
#define set(tmr,val) tmr=val;
#define expire(tmr) (tmr==0) /*timeout*/
#define tickT(tmr) if :: tmr>0 -> tmr=tmr-1 :: else  T=1  fi
#define tickt(tmr) if :: tmr>0 -> tmr=tmr-1 :: else  t=1  fi

#define minute 60
#define sec 3

#define timeout true

timer tmrT
timer tmrt

active proctype Timers() {
	do
	:: timeout -> atomic {
		tickT(tmrT);
		tickt(tmrt)
	}
	od
}

mtype = {idle, introducir, activada, apagada, encendida};

#define idle 1
#define introducir 2
#define activada 3

#define apagada 4
#define encendida 5

mtype estado_luces = apagada;
mtype estado_alarma = idle;
byte i = 0;
byte q = 0;
int a = 0;
int l = 0;
int PIR = 0;
int p = 0;
int PIR_l = 0;
int p_l = 0;

int codigos[6];

active proctype control_luces () {

		estado_luces = apagada;
	  T = 0;
	  l = 0;
		do
		::	estado_luces == apagada -> atomic {
			if
			::	(p_l || PIR_l) ->
				estado_luces = encendida;
				p_l = 0;
				PIR_l = 0;
				l=1;
				set(tmrT,minute)
			fi;
		}
		::	estado_luces == encendida -> atomic {
			if
	    ::	(p_l || PIR_l) ->
				estado_luces = encendida;
				l=1;
				p_l = 0;
				PIR_l = 0;
				set(tmrT,minute);
			::	(!p && !PIR && !T) ->
	      estado_luces = encendida;
	      l=1;
	    ::  (!p && !PIR && T) ->
	      estado_luces = apagada;
	      l=0;
	      T=0;
	    fi;
		}
	  od;
}

active proctype control_alarma () {
    estado_alarma = idle;
    codigos[0] = 0;
    i = 0;
    q = 0;
		a = 0;
		p = 0;
		PIR = 0;
    do
		:: estado_alarma == idle -> atomic {
			if
			:: (p && i==0) ->
				estado_alarma = introducir;
				p=0;
				set(tmrt,sec);
				q = (q+1)%10;
			:: else ->
				estado_alarma = idle;
			fi;
		}
		:: estado_alarma == introducir -> atomic {
			if
			:: (p && !t) ->
				estado_alarma = introducir;
				p=0;
				set(tmrt,sec);
				q = (q+1)%10;
			:: (t && i<2) ->
				estado_alarma = introducir;
				t = 0;
				codigos[i]=q;
				i = i+1;
				q = codigos[i];
			:: (t && i==2) ->
				estado_alarma = activada;
				t = 0;
				codigos[i]=q;
				i = i+1;
				q = codigos[i];
			:: else ->
				estado_alarma = introducir;
			fi;
		}
		:: estado_alarma == activada -> atomic {
			if
			:: (p && !t) ->
				estado_alarma = activada;
				p=0;
				set(tmrt,sec);
				q = (q+1)%10;
			:: (t && i<5) ->
				estado_alarma = activada;
				t = 0;
				codigos[i]=q;
				i = i+1;
				q = codigos[i];
			:: PIR ->
				a = 1;
			:: (t && i==5) ->
				estado_alarma = idle;
				t = 0;
				a = 0;
				i = 0;
			:: else ->
				estado_alarma = activada;
			fi;
		}
    od
}

active proctype entorno () {
  do
  ::  if
      ::  skip
      ::  true -> p = 1; p_l = 1;
			::  true -> PIR = 1; PIR_l=1;
      fi;
      printf ("pulsado:(%d,%d), PIR:(%d,%d), T:%d, t:%d", p, p_l, PIR,PIR_l, T,t)
			printf ("estado_alarma:%d, estado_luces:%d", estado_alarma, estado_luces)
			printf ("i:%d, q:%d", i,q);
			printf("luz:%d, a:%d, c1:%d, c2:%d, c3:%d\n", l, a, codigos[0], codigos[1], codigos[2])
  od
}
